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
 * includes
 */
#include "platform.h"
#include "../container/container.h"
#include "../algorithm/algorithm.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the timer default timeout, 1s
#define TB_TIMER_TIMEOUT_DEFAULT 		(1000)

// the timer time
#define TB_TIMER_NOW(timer) 			(tb_hize_t)((timer)->ctime? ((tb_hong_t)tb_ctime_time() * 1000) : tb_mclock())

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the timer task type
typedef struct __tb_timer_task_t
{
	// the id
	tb_size_t 					id;

	// the func
	tb_timer_task_func_t 		func;

	// the data
	tb_pointer_t 				data;

	// the when
	tb_hize_t 					when;

	// the period
	tb_size_t 					period;

}tb_timer_task_t;

// the timer type
typedef struct __tb_timer_t
{
	// the maxn
	tb_size_t 					maxn;

	// is stop?
	tb_atomic_t 				stop;

	// the lock
	tb_handle_t 				lock;

	// cache time?
	tb_bool_t 					ctime;

	// the tasks
	tb_handle_t 				tasks;

	// the lastid
	tb_atomic_t 				lastid;

}tb_timer_t;

/* ///////////////////////////////////////////////////////////////////////
 * tasks
 */
#if 1
static tb_bool_t tb_timer_tasks_walk(tb_vector_t* vector, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return_val(bdel && data, tb_false);

	// the task
	tb_timer_task_t const* task = item? *((tb_timer_task_t const**)item) : tb_null;

	// is this?
	if (task && task->id == (tb_size_t)data)
	{
		// remove it
		*bdel = tb_true;

		// break;
		return tb_false;
	}

	// continue
	return tb_true;
}
static tb_long_t tb_timer_tasks_comp(tb_iterator_t* iterator, tb_cpointer_t item, tb_cpointer_t task)
{
	// check
	tb_assert_return_val(item && task, 0);

	// the lhs
	tb_hize_t lhs = ((tb_timer_task_t const*)item)->when;

	// the rhs
	tb_hize_t rhs = ((tb_timer_task_t const*)task)->when;

	// comp
	return ((lhs < rhs)? 1 : ((lhs > rhs)? -1 : 0));
}
static __tb_inline__ tb_handle_t tb_timer_tasks_init(tb_size_t maxn)
{
	return tb_vector_init((maxn >> 4) + 16, tb_item_func_ifm(sizeof(tb_timer_task_t), tb_null, tb_null));
}
static __tb_inline__ tb_void_t tb_timer_tasks_exit(tb_handle_t tasks)
{
	tb_vector_exit(tasks);
}
static __tb_inline__ tb_bool_t tb_timer_tasks_add(tb_handle_t tasks, tb_timer_task_t const* task)
{
	// init comp
	((tb_iterator_t*)tasks)->comp = tb_timer_tasks_comp;
	
	// find it by sort
	tb_size_t tail = tb_iterator_tail(tasks);
	tb_size_t prev = tail;
//	tb_size_t itor = tb_pfind_all(tasks, &prev, task);
	tb_size_t itor = tb_binary_pfind_all(tasks, &prev, task);

	// finded? insert to prev
	if (itor != tail) tb_vector_insert_prev(tasks, itor, task);
	else
	{
		// insert to next
		if (prev != tail) tb_vector_insert_next(tasks, prev, task);
		// insert to head
		else tb_vector_insert_head(tasks, task);
	}

#if 0
	{
		tb_print("find: %lu, prev: %lu", task->when, prev);
		tb_size_t itor = tb_iterator_head(tasks);
		tail = tb_iterator_tail(tasks);
		for (; itor != tail; itor = tb_iterator_next(tasks, itor))
		{
			tb_timer_task_t const* item = tb_iterator_item(tasks, itor);
			if (item) tb_print("%llu", item->when);
		}
	}
#endif
	
	// ok
	return tb_true;
}
static __tb_inline__ tb_void_t tb_timer_tasks_del(tb_handle_t tasks, tb_size_t id)
{
	// remove it
	tb_vector_walk(tasks, tb_timer_tasks_walk, (tb_pointer_t)id);
}
static __tb_inline__ tb_bool_t tb_timer_tasks_top(tb_handle_t tasks, tb_timer_task_t* task)
{
	tb_timer_task_t const* item = (tb_timer_task_t const*)tb_vector_last(tasks);
	if (item) 
	{
		*task = *item;
		return tb_true;
	}
	return tb_false;
}
static __tb_inline__ tb_void_t tb_timer_tasks_pop(tb_handle_t tasks)
{
	tb_vector_remove_last(tasks);
}
#else
// TODO: using minheap faster
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_timer_init(tb_size_t maxn, tb_bool_t ctime)
{
	// check
	tb_assert_and_check_return_val(maxn, tb_null);

	// make timer
	tb_timer_t* timer = tb_malloc0(sizeof(tb_timer_t));
	tb_assert_and_check_return_val(timer, tb_null);

	// init timer
	timer->maxn 	= maxn;
	timer->ctime 	= ctime;

	// init lock
	timer->lock 	= tb_spinlock_init();
	tb_assert_and_check_goto(timer->lock, fail);

	// init tasks
	timer->tasks 	= tb_timer_tasks_init(maxn);
	tb_assert_and_check_goto(timer->tasks, fail);

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

		// exit tasks
		if (timer->lock) tb_spinlock_enter(timer->lock);
		if (timer->tasks) tb_timer_tasks_exit(timer->tasks);
		timer->tasks = tb_null;
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
	tb_assert_and_check_return_val(timer && timer->tasks, TB_TIMER_TIMEOUT_DEFAULT);

	// top task
	tb_timer_task_t task = {0};
	if (timer->lock) tb_spinlock_enter(timer->lock);
	tb_bool_t ok = tb_timer_tasks_top(timer->tasks, &task);
	if (timer->lock) tb_spinlock_leave(timer->lock);

	// no task? using the default timeout
	tb_check_return_val(ok, TB_TIMER_TIMEOUT_DEFAULT);

	// the now time
	tb_hize_t now = TB_TIMER_NOW(timer);

	// the timeout
	tb_size_t timeout = task.when > now? (tb_size_t)(task.when - now) : 0;

	// ok?
	return timeout;
}
tb_bool_t tb_timer_spak(tb_handle_t handle)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return_val(timer && timer->tasks, tb_false);

	// stoped?
	tb_assert_and_check_return_val(!tb_atomic_get(&timer->stop), tb_false);

	// top task
	tb_timer_task_t task = {0};
	if (timer->lock) tb_spinlock_enter(timer->lock);
	tb_bool_t ok = tb_timer_tasks_top(timer->tasks, &task);
	if (ok)
	{
		// the now time
		tb_hize_t now = TB_TIMER_NOW(timer);

		// timeout?
		if (task.when <= now)
		{
			// update when
			task.when = now + task.period;

			// pop task, must pop it in the same lock, because the top maybe modified
			tb_timer_tasks_pop(timer->tasks);
		}
		else ok = tb_false;
	}
	if (timer->lock) tb_spinlock_leave(timer->lock);

	// no task or no timeout?
	tb_check_return_val(ok, tb_true);

	// check task
	tb_assert_and_check_return_val(task.func, tb_false);

	// done task
	if (task.func(task.data))
	{
		// add task again
		if (timer->lock) tb_spinlock_enter(timer->lock);
		tb_timer_tasks_add(timer->tasks, &task);
		if (timer->lock) tb_spinlock_leave(timer->lock);
	}

	// ok
	return tb_true;
}
tb_void_t tb_timer_loop(tb_handle_t handle)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return(timer);

	// loop
	while (!tb_atomic_get(&timer->stop))
	{
		// the timeout
		tb_size_t timeout = tb_timer_timeout(handle);
			
		// wait some time
		if (timeout) tb_msleep(timeout);

		// spak ctime
		tb_ctime_spak();

		// spak it
		if (!tb_timer_spak(handle)) break;
	}
}
tb_handle_t tb_timer_task_run(tb_handle_t handle, tb_size_t timeout, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return_val(timer && func, tb_null);

	// run task
	return tb_timer_task_run_at(handle, TB_TIMER_NOW(timer) + timeout, timeout, func, data);
}
tb_handle_t tb_timer_task_run_at(tb_handle_t handle, tb_hize_t when, tb_size_t period, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return_val(timer && timer->tasks && func, tb_null);

	// init task
	tb_timer_task_t task = {0};
	task.id 	= tb_atomic_inc_and_fetch(&timer->lastid);
	task.func 	= func;
	task.data 	= data;
	task.when 	= when;
	task.period = period;

	// add task
	if (timer->lock) tb_spinlock_enter(timer->lock);
	tb_bool_t ok = tb_timer_tasks_add(timer->tasks, &task);
	if (timer->lock) tb_spinlock_leave(timer->lock);

	// ok?
	return ok? task.id : tb_null;
}
tb_handle_t tb_timer_task_run_after(tb_handle_t handle, tb_hize_t after, tb_size_t period, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return_val(timer && func, tb_null);

	// run task
	return tb_timer_task_run_at(handle, TB_TIMER_NOW(timer) + after, period, func, data);
}
tb_void_t tb_timer_task_del(tb_handle_t handle, tb_handle_t task)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return(timer && timer->tasks && task);

	// del task
	if (timer->lock) tb_spinlock_enter(timer->lock);
	tb_timer_tasks_del(timer->tasks, (tb_size_t)task);
	if (timer->lock) tb_spinlock_leave(timer->lock);
}

