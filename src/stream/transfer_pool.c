/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		transfer_pool.c
 * @ingroup 	stream
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 				"transfer_pool"
#define TB_TRACE_MODULE_DEBUG 				(1)
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "transfer.h"
#include "transfer_pool.h"
#include "stream.h"
#include "../network/network.h"
#include "../platform/platform.h"
#include "../container/container.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the task task type
typedef struct __tb_transfer_task_t
{
	// the transfer
	tb_handle_t 					transfer;

	// the pool
	tb_handle_t 					pool;

	// the func
	tb_transfer_save_func_t 		func;

	// the priv
	tb_cpointer_t 					priv;

	// the itor for the working list
	tb_size_t 						itor;

}tb_transfer_task_t;

// the task pool type
typedef struct __tb_transfer_pool_t
{
	// the aicp
	tb_aicp_t* 						aicp;

	// the loop
	tb_handle_t 					loop;

	// the aicp is referneced?
	tb_bool_t 						bref;

	// the task maxn
	tb_size_t 						maxn;

	// the concurrent task count
	tb_size_t 						conc;

	// the lock 
	tb_spinlock_t 					lock;

	// the task pool
	tb_handle_t 					pool;

	// the working list
	tb_list_t* 						working;

	// the waiting list
	tb_single_list_t* 				waiting;

	// the timeout
	tb_long_t 						timeout;

	// is stoped?
	tb_bool_t 						bstoped;

}tb_transfer_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_transfer_task_exit(tb_transfer_task_t* task, tb_bool_t bcalling)
{
	// check
	tb_assert_and_check_return(task);

	// the pool
	tb_transfer_pool_t* pool = (tb_transfer_pool_t*)task->pool;
	tb_assert_and_check_return(pool && pool->pool);

	// trace
	tb_trace_d("task[%p]: exit", task);

    // TODO
	// exit transfer
	if (task->transfer) tb_transfer_exit(task->transfer);
	task->transfer = tb_null;

	// free it
	tb_fixed_pool_free(pool->pool, task);
}
static tb_bool_t tb_transfer_task_save(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
	// the task
	tb_transfer_task_t* task = (tb_transfer_task_t*)priv;
	tb_assert_and_check_return_val(task && task->func && task->transfer, tb_false);

	// the pool
	tb_transfer_pool_t* pool = (tb_transfer_pool_t*)task->pool;
	tb_assert_and_check_return_val(pool, tb_false);

	// trace
	tb_trace_d("task[%p]: save: %llu bytes, rate: %lu bytes/s, state: %s", task, save, rate, tb_state_cstr(state));

	// done func
	tb_bool_t ok = task->func(state, offset, size, save, rate, task->priv);	

	// failed, killed or closed?
	if (state != TB_STATE_OK && state != TB_STATE_PAUSED)
	{
		// enter
		tb_spinlock_enter(&pool->lock);

		// done
		tb_bool_t next_ok = tb_false;
		do
		{
			// check working
			tb_assert_and_check_break(pool->working);

			// remove task from the working list
			tb_list_remove(pool->working, task->itor);
		
			// exit task
			tb_transfer_task_exit(task, tb_true);
			task = tb_null;

			// continue the next waiting task
			tb_check_break(!pool->bstoped);

			// need work it?
			tb_bool_t next = (pool->conc && tb_list_size(pool->working) >= pool->conc)? tb_false : tb_true;
			tb_check_break(next && pool->waiting && tb_single_list_size(pool->waiting));
			
			// get the head task from the waiting list
			task = tb_single_list_head(pool->waiting);
			tb_assert_and_check_break(task && task->transfer);

			// append the task to the working list
			task->itor = tb_list_insert_tail(pool->working, task);
			tb_assert_and_check_break(task->itor != tb_iterator_tail(pool->working));

			// remove the task from the waiting list
			tb_single_list_remove_head(pool->waiting);

			// trace
			tb_trace_d("next: working: %lu, waiting: %lu", tb_list_size(pool->working), pool->waiting? tb_single_list_size(pool->waiting) : 0);

			// ok
			next_ok = tb_true;

		} while (0);

		// leave
		tb_spinlock_leave(&pool->lock);

		// ok and work it?
		if (next_ok && task && task->transfer)
		{
			// done
			if (!tb_transfer_osave(task->transfer, tb_transfer_task_save, task))
			{
				// enter
				tb_spinlock_enter(&pool->lock);

				// remove task from the working list
				tb_list_remove(pool->working, task->itor);

				// exit task
				tb_transfer_task_exit(task, tb_false);

				// leave
				tb_spinlock_leave(&pool->lock);
			}
		}
	}

	// ok?
	return ok;
}
static tb_bool_t tb_transfer_working_kill(tb_iterator_t* iterator, tb_pointer_t item, tb_cpointer_t priv)
{
	// the task 
	tb_transfer_task_t* task = (tb_transfer_task_t*)item;
	tb_check_return_val(task, tb_false);

	// check
	tb_assert_and_check_return_val(task->transfer, tb_false);

	// kill it
	tb_transfer_kill(task->transfer);

	// ok
	return tb_true;
}
static tb_bool_t tb_transfer_working_copy(tb_iterator_t* iterator, tb_pointer_t item, tb_cpointer_t priv)
{
	// the task 
	tb_transfer_task_t* task = (tb_transfer_task_t*)item;
	tb_check_return_val(task && priv, tb_false);

	// save it
	tb_list_insert_tail((tb_list_t*)priv, task);

	// ok
	return tb_true;
}
static tb_bool_t tb_transfer_waiting_exit(tb_iterator_t* iterator, tb_pointer_t item, tb_cpointer_t priv)
{
	// the task 
	tb_transfer_task_t* task = (tb_transfer_task_t*)item;
	tb_check_return_val(task, tb_false);

	// exit it
	tb_transfer_task_exit(task, tb_false);

	// ok
	return tb_true;
}
static tb_pointer_t tb_transfer_pool_loop(tb_cpointer_t priv)
{
	// aicp
	tb_handle_t aicp = (tb_handle_t)priv;

	// trace
	tb_trace_d("loop: init");

	// loop aicp
	if (aicp) tb_aicp_loop(aicp);
	
	// trace
	tb_trace_d("loop: exit");

	// exit
	tb_thread_return(tb_null);
	return tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_handle_t tb_transfer_pool_instance_init(tb_cpointer_t* ppriv)
{
	// init it
	return tb_transfer_pool_init(tb_aicp(), 0, 0, 0);
}
static tb_void_t tb_transfer_pool_instance_exit(tb_handle_t handle, tb_cpointer_t priv)
{
	tb_transfer_pool_exit(handle);
}
static tb_void_t tb_transfer_pool_instance_kill(tb_handle_t handle, tb_cpointer_t priv)
{
	tb_transfer_pool_kill(handle);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_transfer_pool()
{
	return tb_singleton_instance(TB_SINGLETON_TYPE_TRANSFER_POOL, tb_transfer_pool_instance_init, tb_transfer_pool_instance_exit, tb_transfer_pool_instance_kill);
}
tb_handle_t tb_transfer_pool_init(tb_aicp_t* aicp, tb_size_t maxn, tb_size_t conc, tb_long_t timeout)
{
	// done
	tb_bool_t 				ok = tb_false;
	tb_transfer_pool_t* 	pool = tb_null;
	do
	{
		// make pool
		pool = (tb_handle_t)tb_malloc0(sizeof(tb_transfer_pool_t));
		tb_assert_and_check_break(pool);

		// init lock
		if (!tb_spinlock_init(&pool->lock)) break;

		// init pool
		pool->bref 		= aicp? tb_true : tb_false;
		pool->aicp 		= aicp? aicp : tb_aicp_init(conc);
		pool->conc 		= conc;
		pool->maxn 		= maxn;
		pool->timeout 	= timeout;
		pool->bstoped 	= tb_false;
		tb_assert_and_check_break(pool->aicp);

		// init pool
		pool->pool = tb_fixed_pool_init(conc? conc : 16, sizeof(tb_transfer_task_t), 0);
		tb_assert_and_check_break(pool->pool);

		// init working list
		pool->working = tb_list_init(conc? conc : 16, tb_item_func_ptr(tb_null, tb_null));
		tb_assert_and_check_break(pool->working);

		// init waiting list delay, maybe not used if conc be zero
		pool->waiting = tb_null;

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok) 
	{
		// exit it
		if (pool) tb_transfer_pool_exit((tb_handle_t)pool);
		pool = tb_null;
	}

	// ok?
	return (tb_handle_t)pool;
}
tb_void_t tb_transfer_pool_kill(tb_handle_t handle)
{
	// check
	tb_transfer_pool_t* pool = (tb_transfer_pool_t*)handle;
	tb_assert_and_check_return(pool);

	// trace
	tb_trace_d("kill: ..");

	// enter
	tb_spinlock_enter(&pool->lock);

	// kill it if be not stoped
	tb_list_t* working = tb_null;
	if (!pool->bstoped && pool->working && tb_list_size(pool->working))
	{
		// stop it
		pool->bstoped = tb_true;

		// init the working list
		working = tb_list_init(pool->conc? pool->conc : 16, tb_item_func_ptr(tb_null, tb_null));

		// copy it
		tb_walk_all(pool->working, tb_transfer_working_copy, working);
	}

	// leave
	tb_spinlock_leave(&pool->lock);

	// kill it
	if (working)
	{
		tb_walk_all(working, tb_transfer_working_kill, tb_null);
		tb_list_exit(working);
		working = tb_null;
	}
}
tb_void_t tb_transfer_pool_exit(tb_handle_t handle)
{
	// check
	tb_transfer_pool_t* pool = (tb_transfer_pool_t*)handle;
	tb_assert_and_check_return(pool);

	// trace
	tb_trace_d("exit: ..");

	// kill it
	tb_transfer_pool_kill(handle);

	// wait all working transfers exit
	tb_size_t tryn = 10;
	while (tryn--)
	{
		// the working count
		tb_spinlock_enter(&pool->lock);
		tb_size_t nworking = pool->working? tb_list_size(pool->working) : 0;
		tb_spinlock_leave(&pool->lock);

		// ok?
		tb_check_break(nworking);

		// too long? failed
		if (!tryn)
		{
			// trace
			tb_trace_e("exit failed: nworking: %lu", nworking);
			return ;
		}

		// wait some time
		tb_msleep(500);
	}

	// enter
	tb_spinlock_enter(&pool->lock);

	// exit working
	if (pool->working)
	{
		// check
		tb_assert(!tb_list_size(pool->working));

		// exit it
		tb_list_exit(pool->working);
		pool->working = tb_null;
	}

	// exit waiting
	if (pool->waiting) 
	{
		// exit it
		tb_walk_all(pool->waiting, tb_transfer_waiting_exit, tb_null);
		tb_single_list_exit(pool->waiting);
		pool->waiting = tb_null;
	}

	// exit pool
	if (pool->pool) tb_fixed_pool_exit(pool->pool);
	pool->pool = tb_null;

	// leave
	tb_spinlock_leave(&pool->lock);

	// kill aicp
	if (pool->aicp && !pool->bref)
		tb_aicp_kill(pool->aicp);

	// exit loop
	if (pool->loop)
	{
		// wait loop
		tb_long_t wait = 0;
		if ((wait = tb_thread_wait(pool->loop, 5000)) <= 0)
		{
			// trace
			tb_trace_e("loop[%p]: wait failed: %ld!", pool->loop, wait);
		}

		// exit loop
		tb_thread_exit(pool->loop);
		pool->loop = tb_null;
	}

	// exit aicp
	if (pool->aicp && !pool->bref)
		tb_aicp_exit(pool->aicp);
	pool->aicp = tb_null;

	// exit lock
	tb_spinlock_exit(&pool->lock);

	// exit it
	tb_free(pool);

	// trace
	tb_trace_d("exit: ok");
}
tb_size_t tb_transfer_pool_maxn(tb_handle_t handle)
{
	// check
	tb_transfer_pool_t* pool = (tb_transfer_pool_t*)handle;
	tb_assert_and_check_return_val(pool, 0);

	// the maxn
	return pool->maxn;
}
tb_size_t tb_transfer_pool_size(tb_handle_t handle)
{
	// check
	tb_transfer_pool_t* pool = (tb_transfer_pool_t*)handle;
	tb_assert_and_check_return_val(pool, 0);

	// enter
	tb_spinlock_enter(&pool->lock);

	// the size
	tb_size_t size = pool->pool? tb_fixed_pool_size(pool->pool) : 0;

	// leave
	tb_spinlock_leave(&pool->lock);

	// ok?
	return size;
}
tb_bool_t tb_transfer_pool_done(tb_handle_t handle, tb_char_t const* iurl, tb_char_t const* ourl, tb_hize_t offset, tb_transfer_save_func_t save, tb_transfer_ctrl_func_t ctrl, tb_cpointer_t priv)
{
	// check
	tb_transfer_pool_t* pool = (tb_transfer_pool_t*)handle;
	tb_assert_and_check_return_val(pool && pool->aicp && iurl && ourl, tb_false);
	
	// enter
	tb_spinlock_enter(&pool->lock);

	// done
	tb_bool_t 				ok = tb_false;
	tb_bool_t 				bworking = tb_false;
	tb_transfer_task_t* 	task = tb_null;
	do
	{
		// stoped?
		tb_check_break(!pool->bstoped);

		// check
		tb_assert_and_check_break(pool->working && pool->pool);

		// too many tasks?
		tb_check_break(!pool->maxn || tb_fixed_pool_size(pool->pool) < pool->maxn);

		// init loop
		if (!pool->loop && !pool->bref)
		{
			pool->loop = tb_thread_init(tb_null, tb_transfer_pool_loop, pool->aicp, 0);
			tb_assert_and_check_break(pool->loop);
		}

		// make task
		task = tb_fixed_pool_malloc0(pool->pool);
		tb_assert_and_check_break(task);

		// init task
		task->pool 		= pool;
		task->transfer 	= tb_transfer_init_uu(pool->aicp, iurl, ourl, offset);
		task->func 		= save;
		task->priv 		= priv;
		tb_assert_and_check_break(task->transfer);

		// ctrl task
		if (!tb_transfer_ctrl(task->transfer, ctrl, priv)) break;

		// init timeout
		if (pool->timeout) tb_transfer_timeout_set(task->transfer, pool->timeout);

		// working now?
		bworking = (pool->conc && tb_list_size(pool->working) >= pool->conc)? tb_false : tb_true;
		if (bworking)
		{
			// append to the working list
			task->itor = tb_list_insert_tail(pool->working, task);
			tb_assert_and_check_break(task->itor != tb_iterator_tail(pool->working));
		}
		// waiting
		else
		{
			// init waiting list
			if (!pool->waiting)
			{
				pool->waiting = tb_single_list_init(pool->conc? pool->conc : 16, tb_item_func_ptr(tb_null, tb_null));
				tb_assert_and_check_break(pool->waiting);
			}

			// append to the waiting list
			tb_size_t itor = tb_single_list_insert_tail(pool->waiting, task);
			tb_assert_and_check_break(itor != tb_iterator_tail(pool->waiting));
		}

		// ok
		ok = tb_true;

	} while (0);

	// trace
	tb_trace_d("done: %s => %s, working: %lu, waiting: %lu, state: %s", iurl, ourl, tb_list_size(pool->working), pool->waiting? tb_single_list_size(pool->waiting) : 0, ok? "ok" : "no");

	// failed? exit task
	if (!ok && task) tb_transfer_task_exit(task, tb_false);

	// leave
	tb_spinlock_leave(&pool->lock);

	// ok and work it?
	if (ok && bworking && task && task->transfer)
	{
		// done
		if (!tb_transfer_osave(task->transfer, tb_transfer_task_save, task))
		{
			// enter
			tb_spinlock_enter(&pool->lock);

			// remove task from the working list
			tb_list_remove(pool->working, task->itor);

			// exit task
			tb_transfer_task_exit(task, tb_false);

			// leave
			tb_spinlock_leave(&pool->lock);

			// failed
			ok = tb_false;
		}
	}

	// ok?
	return ok;
}
