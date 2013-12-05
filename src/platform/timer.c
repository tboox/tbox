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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		timer.c
 * @ingroup 	platform
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 				"timer"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "platform.h"
#include "../memory/memory.h"
#include "../container/container.h"
#include "../algorithm/algorithm.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the timer task type
typedef struct __tb_timer_task_t
{
	// the func
	tb_timer_task_func_t 		func;

	// the data
	tb_pointer_t 				data;

	// the when
	tb_hong_t 					when;

	// the period
	tb_uint32_t 				period 	: 29;

	// is repeat?
	tb_uint32_t 				repeat 	: 1;

	// the refn, <= 2
	tb_uint32_t 				refn 	: 2;

}tb_timer_task_t;

/// the timer type
typedef struct __tb_timer_t
{
	// the maxn
	tb_size_t 					maxn;

	// is stoped?
	tb_atomic_t 				stop;

	// is worked?
	tb_atomic_t 				work;

	// cache time?
	tb_bool_t 					ctime;

	// the lock
	tb_handle_t 				lock;

	// the pool
	tb_handle_t 				pool;

	// the heap
	tb_heap_t* 					heap;

}tb_timer_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_hong_t tb_timer_now(tb_timer_t* timer)
{
	// using the real time?
	if (!timer->ctime)
	{
		// get the time
		tb_timeval_t tv = {0};
		if (tb_gettimeofday(&tv, tb_null)) return ((tb_hong_t)tv.tv_sec * 1000 + tv.tv_usec / 1000);
	}

	// using cached time
	return tb_ctime_time();
}
static tb_long_t tb_timer_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	// check
	tb_timer_task_t const* ltask = (tb_timer_task_t const*)ldata;
	tb_timer_task_t const* rtask = (tb_timer_task_t const*)rdata;
	tb_assert_and_check_return_val(ltask && rtask, -1);

	// comp
	return (ltask->when > rtask->when? 1 : (ltask->when < rtask->when? -1 : 0));
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_timer_init(tb_size_t maxn, tb_bool_t ctime)
{
	// check
	tb_assert_and_check_return_val(maxn, tb_null);

	// make timer
	tb_timer_t* timer = tb_malloc0(sizeof(tb_timer_t));
	tb_assert_and_check_return_val(timer, tb_null);

	// init func
	tb_item_func_t func = tb_item_func_ptr(tb_null, tb_null); func.comp = tb_timer_comp;

	// init timer
	timer->maxn 		= maxn;
	timer->ctime 		= ctime;

	// init lock
	timer->lock 		= tb_spinlock_init();
	tb_assert_and_check_goto(timer->lock, fail);

	// init pool
	timer->pool 		= tb_rpool_init((maxn >> 2) + 16, sizeof(tb_timer_task_t), 0);
	tb_assert_and_check_goto(timer->pool, fail);
	
	// init heap
	timer->heap 		= tb_heap_init((maxn >> 2) + 16, func);
	tb_assert_and_check_goto(timer->heap, fail);

	// ok
	return (tb_handle_t)timer;
fail:
	if (timer) tb_timer_exit(timer);
	return tb_null;
}
tb_void_t tb_timer_exit(tb_handle_t handle)
{
	tb_timer_t* timer = (tb_timer_t*)handle;
	if (timer)
	{
		// stop it
		tb_atomic_set(&timer->stop, 1);

		// wait loop exit
		tb_size_t tryn = 10;
		while (tb_atomic_get(&timer->work) && tryn--) tb_msleep(500);

		// warning
		if (!tryn && tb_atomic_get(&timer->work)) tb_warning("[timer]: the loop has been not exited now!");

		// enter
		if (timer->lock) tb_spinlock_enter(timer->lock);

		// exit heap
		if (timer->heap) tb_heap_exit(timer->heap);
		timer->heap = tb_null;

		// exit pool
		if (timer->pool) tb_rpool_exit(timer->pool);
		timer->pool = tb_null;

		// leave
		if (timer->lock) tb_spinlock_leave(timer->lock);

		// exit lock
		if (timer->lock) tb_spinlock_exit(timer->lock);
		timer->lock = tb_null;

		// exit it
		tb_free(timer);
	}
}
tb_size_t tb_timer_timeout(tb_handle_t handle)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return_val(timer && timer->heap, 0);

	// stoped?
	tb_assert_and_check_return_val(!tb_atomic_get(&timer->stop), 0);

	// enter
	if (timer->lock) tb_spinlock_enter(timer->lock);

	// done
	tb_size_t timeout = 0; 
	if (tb_heap_size(timer->heap))
	{
		// the task
		tb_timer_task_t const* task = tb_heap_top(timer->heap);
		if (task)
		{
			// the now
			tb_hong_t now = tb_timer_now(timer);

			// the timeout
			timeout = task->when > now? task->when - now : 0;
		}
	}

	// leave
	if (timer->lock) tb_spinlock_leave(timer->lock);

	// ok?
	return timeout;
}
tb_bool_t tb_timer_spak(tb_handle_t handle)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return_val(timer && timer->pool && timer->heap, tb_false);

	// stoped?
	tb_assert_and_check_return_val(!tb_atomic_get(&timer->stop), tb_false);

	// enter
	if (timer->lock) tb_spinlock_enter(timer->lock);

	// done
	tb_bool_t 				ok = tb_false;
	tb_timer_task_func_t 	func = tb_null;
	tb_pointer_t 			data = tb_null;
	do
	{
		// empty? 
		if (!tb_heap_size(timer->heap))
		{
			ok = tb_true;
			break;
		}

		// the top task
		tb_timer_task_t* task = tb_heap_top(timer->heap);
		tb_assert_and_check_break(task);

		// check refn
		tb_assert(task->refn);

		// the now
		tb_hong_t now = tb_timer_now(timer);

		// timeout?
		if (task->when <= now)
		{
			// pop it
			tb_heap_pop(timer->heap);

			// save func and data for calling it later
			func = task->func;
			data = task->data;

			// repeat?
			if (task->repeat)
			{
				// update when
				task->when = now + task->period;

				// continue task
				tb_heap_put(timer->heap, task);
			}
			else 
			{
				// remove it from pool directly
				if (task->refn == 1) tb_rpool_free(timer->pool, task);

				// refn--
				if (task->refn) task->refn--;
			}
		}

		// ok
		ok = tb_true;

	} while (0);

	// leave
	if (timer->lock) tb_spinlock_leave(timer->lock);

	// done func
	if (func) func(data);

	// ok?
	return ok;
}
tb_void_t tb_timer_loop(tb_handle_t handle)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return(timer);

	// work++
	tb_atomic_fetch_and_inc(&timer->work);

	// loop
	while (!tb_atomic_get(&timer->stop))
	{
		// the timeout
		tb_size_t timeout = tb_timer_timeout(handle);
			
		// wait some time
		if (timeout) tb_msleep(timeout);

		// spak ctime
		if (timer->ctime) tb_ctime_spak();

		// spak it
		if (!tb_timer_spak(handle)) break;
	}

	// work--
	tb_atomic_fetch_and_dec(&timer->work);
}
tb_handle_t tb_timer_task_add(tb_handle_t handle, tb_size_t timeout, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return_val(timer && func, tb_null);

	// add task
	return tb_timer_task_add_at(handle, tb_timer_now(timer) + timeout, timeout, repeat, func, data);
}
tb_handle_t tb_timer_task_add_at(tb_handle_t handle, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return_val(timer && timer->pool && timer->heap && func, tb_null);

	// enter
	if (timer->lock) tb_spinlock_enter(timer->lock);

	// make task
	tb_timer_task_t* task = (tb_timer_task_t*)tb_rpool_malloc0(timer->pool);
	if (task)
	{
		// init task
		task->refn 		= 2;
		task->func 		= func;
		task->data 		= data;
		task->when 		= when;
		task->period 	= period;
		task->repeat 	= repeat? 1 : 0;

		// add task
		tb_heap_put(timer->heap, task);
	}

	// leave
	if (timer->lock) tb_spinlock_leave(timer->lock);

	// ok?
	return task;
}
tb_handle_t tb_timer_task_add_after(tb_handle_t handle, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return_val(timer && func, tb_null);

	// add task
	return tb_timer_task_add_at(handle, tb_timer_now(timer) + after, period, repeat, func, data);
}
tb_void_t tb_timer_task_run(tb_handle_t handle, tb_size_t timeout, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return(timer && func);

	// run task
	tb_timer_task_run_at(handle, tb_timer_now(timer) + timeout, timeout, repeat, func, data);
}
tb_void_t tb_timer_task_run_at(tb_handle_t handle, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return(timer && timer->pool && timer->heap && func);

	// enter
	if (timer->lock) tb_spinlock_enter(timer->lock);

	// make task
	tb_timer_task_t* task = (tb_timer_task_t*)tb_rpool_malloc0(timer->pool);
	if (task)
	{
		// init task
		task->refn 		= 1;
		task->func 		= func;
		task->data 		= data;
		task->when 		= when;
		task->period 	= period;
		task->repeat 	= repeat? 1 : 0;

		// add task
		tb_heap_put(timer->heap, task);
	}

	// leave
	if (timer->lock) tb_spinlock_leave(timer->lock);
}
tb_void_t tb_timer_task_run_after(tb_handle_t handle, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return(timer && func);

	// run task
	tb_timer_task_run_at(handle, tb_timer_now(timer) + after, period, repeat, func, data);
}
tb_void_t tb_timer_task_del(tb_handle_t handle, tb_handle_t htask)
{
	// check
	tb_timer_t* 		timer = (tb_timer_t*)handle;
	tb_timer_task_t* 	task = (tb_timer_task_t*)htask;
	tb_assert_and_check_return(timer && timer->pool && task);

	// trace
	tb_trace_impl("del: when: %lld, period: %u, refn: %u", task->when, task->period, task->refn);

	// enter
	if (timer->lock) tb_spinlock_enter(timer->lock);

	// remove it from pool directly if the task have been expired 
	if (task->refn == 1) tb_rpool_free(timer->pool, task);

	// refn--
	if (task->refn) task->refn--;

	// cancel task
	task->func 		= tb_null;
	task->data 		= tb_null;
	task->repeat 	= 0;

	// leave
	if (timer->lock) tb_spinlock_leave(timer->lock);
}

