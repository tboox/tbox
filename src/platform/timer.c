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

	// the mutx
	tb_handle_t 				mutx;

	// cache time?
	tb_bool_t 					ctime;

	// the tasks
	tb_handle_t 				tasks;

}tb_timer_t;

/* ///////////////////////////////////////////////////////////////////////
 * tasks
 */
static __tb_inline__ tb_handle_t tb_timer_tasks_init(tb_size_t maxn)
{
	return tb_null;
}
static __tb_inline__ tb_void_t tb_timer_tasks_exit(tb_handle_t tasks)
{
}
static __tb_inline__ tb_handle_t tb_timer_tasks_add(tb_handle_t tasks, tb_timer_task_t const* task)
{
	return task;
}
static __tb_inline__ tb_bool_t tb_timer_tasks_del(tb_handle_t tasks, tb_handle_t task)
{
	return tb_false;
}
static __tb_inline__ tb_void_t tb_timer_tasks_pop(tb_handle_t tasks)
{
}
static __tb_inline__ tb_handle_t tb_timer_tasks_top(tb_handle_t tasks)
{
	return tb_null;
}
static __tb_inline__ tb_timer_task_t* tb_timer_tasks_get(tb_handle_t tasks, tb_handle_t task)
{
	return tb_null;
}

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

	// init mutx
	timer->mutx 	= tb_mutex_init();
	tb_assert_and_check_goto(timer->mutx, fail);

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
		if (timer->mutx) tb_mutex_enter(timer->mutx);
		if (timer->tasks) tb_timer_tasks_exit(timer->tasks);
		timer->tasks = tb_null;
		if (timer->mutx) tb_mutex_leave(timer->mutx);

		// exit mutx
		if (timer->mutx) tb_mutex_exit(timer->mutx);
		timer->mutx = tb_null;

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
	if (timer->mutx) tb_mutex_enter(timer->mutx);
	tb_handle_t itor = tb_timer_tasks_top(timer->tasks);
	if (itor)
	{
		tb_timer_task_t* item = tb_timer_tasks_get(timer->tasks, itor);
		if (item) task = *item;
	}
	if (timer->mutx) tb_mutex_leave(timer->mutx);

	// no task? using the default timeout
	tb_check_return_val(itor, TB_TIMER_TIMEOUT_DEFAULT);

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
	if (timer->mutx) tb_mutex_enter(timer->mutx);
	tb_handle_t itor = tb_timer_tasks_top(timer->tasks);
	if (itor)
	{
		tb_timer_task_t* item = tb_timer_tasks_get(timer->tasks, itor);
		if (item) task = *item;
	}
	if (timer->mutx) tb_mutex_leave(timer->mutx);

	// no task? 
	tb_check_return_val(itor, tb_true);

	// check task
	tb_assert_and_check_return_val(task.func, tb_false);

	// the now time
	tb_hize_t now = TB_TIMER_NOW(timer);

	// timeout?
	tb_check_return_val(task.when <= now, tb_true);

	// done task
	if (!task.func(task.data))
	{
		// remove task
		if (timer->mutx) tb_mutex_enter(timer->mutx);
		tb_timer_tasks_pop(timer->tasks);
		if (timer->mutx) tb_mutex_leave(timer->mutx);
	}
	else
	{
		// update task
		if (timer->mutx) tb_mutex_enter(timer->mutx);
		tb_timer_task_t* item = tb_timer_tasks_get(timer->tasks, itor);
		if (item) item->when = now + item->period;
		if (timer->mutx) tb_mutex_leave(timer->mutx);
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
		tb_long_t timeout = tb_timer_timeout(handle);

		// wait some time
		if (timeout) tb_msleep((tb_size_t)timeout);

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
	task.func 	= func;
	task.data 	= data;
	task.when 	= when;
	task.period = period;

	// add task
	if (timer->mutx) tb_mutex_enter(timer->mutx);
	tb_handle_t itor = tb_timer_tasks_add(timer->tasks, &task);
	if (timer->mutx) tb_mutex_leave(timer->mutx);

	// ok?
	return itor;
}
tb_handle_t tb_timer_task_run_after(tb_handle_t handle, tb_hize_t after, tb_size_t period, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return_val(timer && func, tb_null);

	// run task
	return tb_timer_task_run_at(handle, TB_TIMER_NOW(timer) + after, period, func, data);
}
tb_bool_t tb_timer_task_del(tb_handle_t handle, tb_handle_t task)
{
	// check
	tb_timer_t* timer = (tb_timer_t*)handle;
	tb_assert_and_check_return_val(timer && timer->tasks && task, tb_false);

	// del task
	if (timer->mutx) tb_mutex_enter(timer->mutx);
	tb_handle_t ok = tb_timer_tasks_del(timer->tasks, &task);
	if (timer->mutx) tb_mutex_leave(timer->mutx);

	// ok?
	return ok;
}

