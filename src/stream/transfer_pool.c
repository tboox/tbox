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
 * @author      ruki
 * @file        transfer_pool.c
 * @ingroup     stream
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "transfer_pool"
#define TB_TRACE_MODULE_DEBUG               (1)
 
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
    tb_handle_t                     transfer;

    // the pool
    tb_handle_t                     pool;

    // the func
    tb_async_transfer_done_func_t   func;

    // the priv
    tb_cpointer_t                   priv;

    // the itor for the work list
    tb_size_t                       itor;

}tb_transfer_task_t;

// the task pool type
typedef struct __tb_transfer_pool_t
{
    // the aicp
    tb_aicp_t*                      aicp;

    // the task maxn
    tb_size_t                       maxn;

    // the lock 
    tb_spinlock_t                   lock;

    // the task pool
    tb_handle_t                     pool;

    // the idle task list
    tb_vector_t*                    idle;

    // the work task list
    tb_list_t*                      work;

    /* the state
     *
     * TB_STATE_OK
     * TB_STATE_KILLING
     */
    tb_atomic_t                     state;

}tb_transfer_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_transfer_task_exit(tb_transfer_pool_t* pool, tb_transfer_task_t* task)
{
    // check
    tb_assert_and_check_return(pool && pool->pool && pool->idle && task);

    // trace
    tb_trace_d("task[%p]: exit", task);

    // append task to the idle list
    tb_vector_insert_tail(pool->idle, task);
}
static tb_transfer_task_t* tb_transfer_task_init(tb_transfer_pool_t* pool, tb_async_transfer_done_func_t done, tb_async_transfer_ctrl_func_t ctrl, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(pool && done, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_transfer_task_t* task = tb_null;
    do
    {
        // init task pool
        if (!pool->pool) pool->pool = tb_fixed_pool_init((pool->maxn >> 2) + 16, sizeof(tb_transfer_task_t), 0);
        tb_assert_and_check_break(pool->pool);

        // init idle task list
        if (!pool->idle) pool->idle = tb_vector_init((pool->maxn >> 2) + 16, tb_item_func_ptr(tb_null, tb_null));
        tb_assert_and_check_break(pool->idle);

        // init work task list
        if (!pool->work) pool->work = tb_list_init((pool->maxn >> 2) + 16, tb_item_func_ptr(tb_null, tb_null));
        tb_assert_and_check_break(pool->work);

        // init task from the idle list first
        if (tb_vector_size(pool->idle))
        {
            // get the last task
            task = (tb_transfer_task_t*)tb_vector_last(pool->idle);
            tb_assert_and_check_break(task);

            // remove the last task
            tb_vector_remove_last(pool->idle);

            // check
            tb_assert_and_check_break(task->transfer);
        }
        // init task from the task pool
        else
        {
            // make task
            task = (tb_transfer_task_t*)tb_fixed_pool_malloc0(pool->pool);
            tb_assert_and_check_break(task);

            // init transfer
            task->transfer = tb_async_transfer_init(pool->aicp, tb_true);
            tb_assert_and_check_break(task->transfer);
        }

        // init ctrl
        if (ctrl && !tb_async_transfer_ctrl(task->transfer, ctrl, priv)) break;

        // init task
        task->func = done;
        task->priv = priv;
        task->itor = tb_iterator_tail(pool->work);
        task->pool = pool;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (task) tb_transfer_task_exit(pool, task);
        task = tb_null;
    }

    // ok?
    return task;
}
static tb_bool_t tb_transfer_task_done(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
    // the task
    tb_transfer_task_t* task = (tb_transfer_task_t*)priv;
    tb_assert_and_check_return_val(task && task->func && task->transfer, tb_false);

    // the pool
    tb_transfer_pool_t* pool = (tb_transfer_pool_t*)task->pool;
    tb_assert_and_check_return_val(pool, tb_false);

    // trace
    tb_trace_d("task[%p]: done: %llu bytes, rate: %lu bytes/s, state: %s", task, save, rate, tb_state_cstr(state));

    // done func
    tb_bool_t ok = task->func(state, offset, size, save, rate, task->priv); 

    // failed, killed or closed?
    if (state != TB_STATE_OK && state != TB_STATE_PAUSED)
    {
        // enter
        tb_spinlock_enter(&pool->lock);

        // check 
        tb_assert(pool->work && task->itor != tb_iterator_tail(pool->work));

        // remove task from the work list
        if (pool->work) tb_list_remove(pool->work, task->itor);
    
        // exit task
        tb_transfer_task_exit(pool, task);
        task = tb_null;

        // leave
        tb_spinlock_leave(&pool->lock);
    }

    // ok?
    return ok;
}
static tb_bool_t tb_transfer_pool_work_kill(tb_iterator_t* iterator, tb_pointer_t item, tb_cpointer_t priv)
{
    // the task 
    tb_transfer_task_t* task = (tb_transfer_task_t*)item;
    tb_check_return_val(task, tb_false);

    // check
    tb_assert_and_check_return_val(task->transfer, tb_false);

    // kill it
    tb_async_transfer_kill(task->transfer);

    // ok
    return tb_true;
}
static tb_bool_t tb_transfer_pool_work_copy(tb_iterator_t* iterator, tb_pointer_t item, tb_cpointer_t priv)
{
    // the task 
    tb_transfer_task_t* task = (tb_transfer_task_t*)item;
    tb_check_return_val(task && priv, tb_false);

    // save it
    tb_list_insert_tail((tb_list_t*)priv, task);

    // ok
    return tb_true;
}
#ifdef __tb_debug__
static tb_bool_t tb_transfer_pool_work_wait(tb_iterator_t* iterator, tb_pointer_t item, tb_cpointer_t priv)
{
    // check
    tb_transfer_task_t* task = (tb_transfer_task_t*)item;
    tb_assert_and_check_return_val(task, tb_false);

    // trace
    tb_trace_d("wait: task: %p: ..", task);

    // ok
    return tb_true;
}
#endif
static tb_bool_t tb_transfer_pool_walk_exit(tb_pointer_t item, tb_cpointer_t priv)
{
    // check
    tb_transfer_task_t* task = (tb_transfer_task_t*)item;
    tb_assert_and_check_return_val(task, tb_false);

    // exit transfer
    if (task->transfer) tb_async_transfer_exit(task->transfer);
    task->transfer = tb_null;

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_handle_t tb_transfer_pool_instance_init(tb_cpointer_t* ppriv)
{
    // init it
    return tb_transfer_pool_init(tb_null, 0);
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
tb_handle_t tb_transfer_pool_init(tb_aicp_t* aicp, tb_size_t maxn)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_transfer_pool_t*     pool = tb_null;
    do
    {
        // make pool
        pool = (tb_handle_t)tb_malloc0(sizeof(tb_transfer_pool_t));
        tb_assert_and_check_break(pool);

        // init lock
        if (!tb_spinlock_init(&pool->lock)) break;

        // init pool
        pool->aicp      = aicp? aicp : tb_aicp();
        pool->maxn      = maxn? maxn : (1 << 16);
        pool->state     = TB_STATE_OK;
        tb_assert_and_check_break(pool->aicp);

        // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
        tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&pool->lock, TB_TRACE_MODULE_NAME);
#endif

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

    // kill it
    if (TB_STATE_OK == tb_atomic_fetch_and_set(&pool->state, TB_STATE_KILLING))
        tb_transfer_pool_kill_all(handle);
}
tb_void_t tb_transfer_pool_kill_all(tb_handle_t handle)
{
    // check
    tb_transfer_pool_t* pool = (tb_transfer_pool_t*)handle;
    tb_assert_and_check_return(pool);

    // check 
    tb_check_return(TB_STATE_OK == tb_atomic_get(&pool->state));

    // enter
    tb_spinlock_enter(&pool->lock);

    // trace
    tb_trace_d("kill_all: %lu, ..", pool->work? tb_list_size(pool->work) : 0);

    // kill it
    tb_list_t* copy = tb_null;
    if (pool->work && tb_list_size(pool->work))
    {
        // init the work list
        copy = tb_list_init((pool->maxn >> 2) + 16, tb_item_func_ptr(tb_null, tb_null));

        // copy it
        if (copy) tb_walk_all(pool->work, tb_transfer_pool_work_copy, copy);
    }

    // leave
    tb_spinlock_leave(&pool->lock);

    // kill it
    if (copy)
    {
        tb_walk_all(copy, tb_transfer_pool_work_kill, tb_null);
        tb_list_exit(copy);
        copy = tb_null;
    }
}
tb_long_t tb_transfer_pool_wait_all(tb_handle_t handle, tb_long_t timeout)
{
    // check
    tb_transfer_pool_t* pool = (tb_transfer_pool_t*)handle;
    tb_assert_and_check_return_val(pool, -1);

    // wait it
    tb_size_t size = 0;
    tb_hong_t time = tb_cache_time_spak();
    while ((timeout < 0 || tb_cache_time_spak() < time + timeout))
    {
        // enter
        tb_spinlock_enter(&pool->lock);

        // the size
        tb_size_t size = pool->work? tb_list_size(pool->work) : 0;

        // trace
        tb_trace_d("wait: %lu: ..", size);

        // trace work
#ifdef __tb_debug__
        if (size) tb_walk_all(pool->work, tb_transfer_pool_work_wait, tb_null);
#endif

        // leave
        tb_spinlock_leave(&pool->lock);

        // ok?
        tb_check_break(size);

        // wait some time
        tb_msleep(200);
    }

    // ok?
    return !size? 1 : 0;
}
tb_bool_t tb_transfer_pool_exit(tb_handle_t handle)
{
    // check
    tb_transfer_pool_t* pool = (tb_transfer_pool_t*)handle;
    tb_assert_and_check_return_val(pool, tb_false);

    // trace
    tb_trace_d("exit: ..");

    // kill it first
    tb_transfer_pool_kill(handle);

    // wait all
    if (tb_transfer_pool_wait_all(handle, 5000) <= 0)
    {
        // trace
        tb_trace_e("exit: wait failed!");
        return tb_false;
    }

    // enter
    tb_spinlock_enter(&pool->lock);

    // exit the work list
    if (pool->work)
    {
        // check
        tb_assert(!tb_list_size(pool->work));

        // exit it
        tb_list_exit(pool->work);
        pool->work = tb_null;
    }

    // exit the idle list
    if (pool->idle) 
    {
        tb_vector_exit(pool->idle);
        pool->idle = tb_null;
    }

    // exit pool
    if (pool->pool) 
    {
        // exit all task
        tb_fixed_pool_walk(pool->pool, tb_transfer_pool_walk_exit, tb_null);

        // exit it
        tb_fixed_pool_exit(pool->pool);
        pool->pool = tb_null;
    }

    // leave
    tb_spinlock_leave(&pool->lock);

    // exit lock
    tb_spinlock_exit(&pool->lock);

    // exit it
    tb_free(pool);

    // trace
    tb_trace_d("exit: ok");
    
    // ok
    return tb_true;
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
    tb_size_t size = pool->work? tb_list_size(pool->work) : 0;

    // leave
    tb_spinlock_leave(&pool->lock);

    // ok?
    return size;
}
tb_bool_t tb_transfer_pool_done(tb_handle_t handle, tb_char_t const* iurl, tb_char_t const* ourl, tb_hize_t offset, tb_size_t rate, tb_async_transfer_done_func_t done, tb_async_transfer_ctrl_func_t ctrl, tb_cpointer_t priv)
{
    // check
    tb_transfer_pool_t* pool = (tb_transfer_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->aicp && iurl && ourl, tb_false);
    
    // enter
    tb_spinlock_enter(&pool->lock);

    // done
    tb_bool_t               ok = tb_false;
    tb_transfer_task_t*     task = tb_null;
    do
    {
        // check 
        tb_check_break(TB_STATE_OK == tb_atomic_get(&pool->state));

        // too many tasks?
        if (pool->work && tb_list_size(pool->work) >= pool->maxn)
        {
            // trace
            tb_trace_e("too many tasks, done task: %s => %s failed!", iurl, ourl);
            break;
        }

        // init task
        task = tb_transfer_task_init(pool, done, ctrl, priv);
        tb_assert_and_check_break(task && task->transfer);

        // init transfer stream
        if (!tb_async_transfer_init_istream_from_url(task->transfer, iurl)) break;
        if (!tb_async_transfer_init_ostream_from_url(task->transfer, ourl)) break;

        // init transfer rate
        tb_async_transfer_limitrate(task->transfer, rate);

        // check
        tb_assert_and_check_break(pool->pool && pool->idle && pool->work);

        // append to the work list
        task->itor = tb_list_insert_tail(pool->work, task);
        tb_assert_and_check_break(task->itor != tb_iterator_tail(pool->work));

        // ok
        ok = tb_true;

    } while (0);

    // trace
    tb_trace_d("done: task: %p, %s => %s, work: %lu, idle: %lu, state: %s", task, iurl, ourl, tb_list_size(pool->work), tb_vector_size(pool->idle), ok? "ok" : "no");

    // failed?
    if (!ok) 
    {
        // exit it
        if (task) tb_transfer_task_exit(pool, task);
        task = tb_null;
    }

    // leave
    tb_spinlock_leave(&pool->lock);

    // ok? done it
    if (ok && task && task->transfer)
    {
        // done
        if (!tb_async_transfer_open_done(task->transfer, 0, tb_transfer_task_done, task))
        {
            // enter
            tb_spinlock_enter(&pool->lock);

            // remove task from the work list
            tb_list_remove(pool->work, task->itor);

            // exit task
            tb_transfer_task_exit(pool, task);

            // leave
            tb_spinlock_leave(&pool->lock);

            // failed
            ok = tb_false;
        }
    }

    // ok?
    return ok;
}
