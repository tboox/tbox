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
 * @file		ltimer.c
 * @ingroup 	platform
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "platform.h"
#include "../memory/memory.h"
#include "../container/container.h"
#include "../algorithm/algorithm.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// FIXME: the timer default timeout, 1s
#define TB_TIMER_TIMEOUT_DEFAULT 		(1000)

// the timer wheel maxn
#ifdef __tb_small__
# 	define TB_TIMER_WHEEL_MAXN 			(4096)
#else
# 	define TB_TIMER_WHEEL_MAXN 			(8192)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the timer task type
typedef struct __tb_ltimer_task_t
{
	// the func
	tb_ltimer_task_func_t 		func;

	// the data
	tb_pointer_t 				data;

	// the when, -1: canceled
	tb_atomic_t 				when;

	// the period
	tb_size_t 					period;

}tb_ltimer_task_t;

/*! the timer type
 *
 * <pre>
 *
 * precision: 1ms
 *
 *          1ms   1ms   ..
 * wheel: |-----|-----|-----|-----|-----|-----|---- ... -----|
 *                           timeout
 *                 btime ==================> now
 *                       |     |     |
 *                       |     |     |
 *                       |     |     | => the same timeout task list (vector)
 *                                   |
 *
 * </pre>
 */
typedef struct __tb_ltimer_t
{
	// the maxn
	tb_size_t 					maxn;

	// is stop?
	tb_atomic_t 				stop;

	// the base time
	tb_hong_t 					btime;

	// cache time?
	tb_bool_t 					ctime;

	// the precision
	tb_size_t 					precision;

	// the lock
	tb_handle_t 				lock;

	// the pool
	tb_handle_t 				pool;

	// the wheel
	tb_vector_t* 				wheel[TB_TIMER_WHEEL_MAXN];

}tb_ltimer_t;

/* ///////////////////////////////////////////////////////////////////////
 * now
 */
static __tb_inline__ tb_hong_t tb_ltimer_now(tb_ltimer_t* timer)
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

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_ltimer_init(tb_size_t maxn, tb_size_t precision, tb_bool_t ctime)
{
	// check
	tb_assert_and_check_return_val(maxn && precision && tb_ispow2(precision), tb_null);

	// make timer
	tb_ltimer_t* timer = tb_malloc0(sizeof(tb_ltimer_t));
	tb_assert_and_check_return_val(timer, tb_null);

	// init timer
	timer->maxn 		= maxn;
	timer->ctime 		= ctime;
	timer->precision 	= precision;
	timer->btime 		= tb_ltimer_now(timer);

	// init lock
	timer->lock 		= tb_spinlock_init();
	tb_assert_and_check_goto(timer->lock, fail);

	// init pool
	timer->pool 		= tb_rpool_init((maxn >> 2) + 16, sizeof(tb_ltimer_task_t), 0);
	tb_assert_and_check_goto(timer->pool, fail);

	// ok
	return (tb_handle_t)timer;
fail:
	if (timer) tb_ltimer_exit(timer);
	return tb_null;
}
tb_void_t tb_ltimer_exit(tb_handle_t handle)
{
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	if (timer)
	{
		// stop it
		tb_atomic_set(&timer->stop, 1);

		// enter
		if (timer->lock) tb_spinlock_enter(timer->lock);

		// exit wheel
		{
			tb_size_t i = 0;
			for (i = 0; i < TB_TIMER_WHEEL_MAXN; i++)
			{
				if (timer->wheel[i]) tb_vector_exit(timer->wheel[i]);
				timer->wheel[i] = tb_null;
			}
		}

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
tb_size_t tb_ltimer_precision(tb_handle_t handle)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer, TB_LTIMER_PRECISION_MS);

	// the timer precision
	return timer->precision;
}
tb_hong_t tb_ltimer_limit(tb_handle_t handle)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer, -1);

	// the timer limit
	return timer->btime + (timer->precision * (TB_TIMER_WHEEL_MAXN - 1));
}
tb_size_t tb_ltimer_timeout(tb_handle_t handle)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer, TB_TIMER_TIMEOUT_DEFAULT);

	// ok?
	return 0;
}
tb_bool_t tb_ltimer_spak(tb_handle_t handle)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer, tb_false);

	// stoped?
	tb_assert_and_check_return_val(!tb_atomic_get(&timer->stop), tb_false);


	// ok
	return tb_true;
}
tb_void_t tb_ltimer_loop(tb_handle_t handle)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return(timer);

	// loop
	while (!tb_atomic_get(&timer->stop))
	{
		// the timeout
		tb_size_t timeout = tb_ltimer_timeout(handle);
			
		// wait some time
		if (timeout) tb_msleep(timeout);

		// spak ctime
		if (timer->ctime) tb_ctime_spak();

		// spak it
		if (!tb_ltimer_spak(handle)) break;
	}
}
tb_handle_t tb_ltimer_task_run(tb_handle_t handle, tb_size_t timeout, tb_ltimer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer && func, tb_null);

	// run task
	return tb_ltimer_task_run_at(handle, tb_ltimer_now(timer) + timeout, timeout, func, data);
}
tb_handle_t tb_ltimer_task_run_at(tb_handle_t handle, tb_hize_t when, tb_size_t period, tb_ltimer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer && func, tb_null);

	return tb_null;
}
tb_handle_t tb_ltimer_task_run_after(tb_handle_t handle, tb_hize_t after, tb_size_t period, tb_ltimer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer && func, tb_null);

	// run task
	return tb_ltimer_task_run_at(handle, tb_ltimer_now(timer) + after, period, func, data);
}
tb_void_t tb_ltimer_task_del(tb_handle_t handle, tb_handle_t task)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return(timer && task);
}

