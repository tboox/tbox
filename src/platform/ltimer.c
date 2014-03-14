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
 * @file		ltimer.c
 * @ingroup 	platform
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_MODULE_NAME 				"ltimer"

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

// the timer wheel maxn
#ifdef __tb_small__
# 	define TB_LTIMER_WHEEL_MAXN 			(4096)
#else
# 	define TB_LTIMER_WHEEL_MAXN 			(8192)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the timer task type
typedef struct __tb_ltimer_task_t
{
	// the func
	tb_timer_task_func_t 		func;

	// the data
	tb_pointer_t 				data;

	// the when
	tb_hong_t 					when;

	// the period
	tb_uint32_t 				period 	: 28;

	// is repeat?
	tb_uint32_t 				repeat 	: 1;

	// is killed?
	tb_uint32_t 				killed 	: 1;

	// the refn, <= 2
	tb_uint32_t 				refn 	: 2;

	// the wheel index
	tb_uint32_t 				windx;

}tb_ltimer_task_t;

/*! the timer type
 *
 * <pre>
 *
 * tick: 1s
 *
 *           1s    1s   ..
 * wheel: |-----|-----|-----|-----|-----|-----|---- ... -----|  <= circle queue
 *                             timers
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

	// is stoped?
	tb_atomic_t 				stop;

	// is worked?
	tb_atomic_t 				work;

	// the base time
	tb_hong_t 					btime;

	// cache time?
	tb_bool_t 					ctime;

	// the tick
	tb_size_t 					tick;

	// the lock
	tb_spinlock_t 				lock;

	// the pool
	tb_handle_t 				pool;

	// the expired tasks
	tb_vector_t* 				expired;

	// the wheel
	tb_vector_t* 				wheel[TB_LTIMER_WHEEL_MAXN];

	// the wheel base
	tb_size_t 					wbase;

}tb_ltimer_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
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
static tb_bool_t tb_ltimer_add_task(tb_ltimer_t* timer, tb_ltimer_task_t* task)
{
	// check
	tb_assert_and_check_return_val(timer && timer->pool && timer->tick, tb_false);
	tb_assert_and_check_return_val(task && task->func && task->refn && task->when, tb_false);

	// trace
	tb_trace_d("add: when: %lld, period: %u, refn: %u", task->when, task->period, task->refn);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// empty? move to the wheel head
		if (!tb_rpool_size(timer->pool)) 
		{
			timer->btime = tb_ltimer_now(timer);
			timer->wbase = 0;
		}
		tb_trace_d("add: btime: %lld, wbase: %lu", timer->btime, timer->wbase);

		// the timer difference
		tb_hong_t tdiff = task->when - timer->btime;
		tb_assert_and_check_break(tdiff >= 0);
		tb_trace_d("add: tdiff: %lld", tdiff);

		// the wheel difference
		tb_size_t wdiff = (tb_size_t)(tdiff / timer->tick);
		tb_assert_and_check_break(wdiff < TB_LTIMER_WHEEL_MAXN);
		tb_trace_d("add: wdiff: %lu", wdiff);

		// the wheel index
		tb_size_t windx = (timer->wbase + wdiff) & (TB_LTIMER_WHEEL_MAXN - 1);
		tb_trace_d("add: windx: %lu", windx);

		// the wheel list
		tb_vector_t* wlist = timer->wheel[windx];
		if (!wlist) wlist = timer->wheel[windx] = tb_vector_init((timer->maxn / TB_LTIMER_WHEEL_MAXN) + 8, tb_item_func_ptr(tb_null, tb_null));
		tb_assert_and_check_break(wlist);

		// save the wheel index
		task->windx = windx;

		// add task to the wheel list
		tb_vector_insert_tail(wlist, task);

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
static tb_bool_t tb_ltimer_del_task(tb_ltimer_t* timer, tb_ltimer_task_t* task)
{
	// check
	tb_assert_and_check_return_val(timer && timer->pool && timer->tick, tb_false);
	tb_assert_and_check_return_val(task && task->func && task->refn && task->when, tb_false);

	// trace
	tb_trace_d("del: when: %lld, period: %u, refn: %u", task->when, task->period, task->refn);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// check
		tb_assert_and_check_break(task->windx != -1 && task->windx < tb_arrayn(timer->wheel));

		// the wheel list
		tb_vector_t* wlist = timer->wheel[task->windx];
		tb_assert_and_check_break(wlist);

		// find the task from the wheel list
		tb_size_t itor = tb_find_all(wlist, task, tb_null);
		tb_assert_and_check_break(itor != tb_iterator_tail(wlist));

		// del the task from the wheel list
		tb_vector_remove(wlist, itor);

		// clear the wheel index
		task->windx = -1;

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
static tb_bool_t tb_ltimer_expired_init(tb_vector_t* vector, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	// the timer
	tb_ltimer_t* timer = (tb_ltimer_t*)data;
	tb_assert_and_check_return_val(timer && timer->expired, tb_false);

	// the task
	tb_ltimer_task_t const* task = item? *((tb_ltimer_task_t const**)item) : tb_null;
	if (task)
	{
		// trace
		tb_trace_d("spak: when: %lld, period: %u, refn: %u", task->when, task->period, task->refn);

		// check refn
		tb_assert(task->refn);

		// expired 
		tb_vector_insert_tail(timer->expired, task);
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_ltimer_expired_done(tb_vector_t* vector, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	// the task
	tb_ltimer_task_t const* task = item? *((tb_ltimer_task_t const**)item) : tb_null;
	
	// done func
	if (task && task->func) task->func(task->killed? tb_true : tb_false, task->data);

	// ok
	return tb_true;
}
static tb_bool_t tb_ltimer_expired_exit(tb_vector_t* vector, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	// the timer
	tb_ltimer_t* 	timer = data? (tb_ltimer_t*)(((tb_pointer_t*)data)[0]) : tb_null;
	tb_hong_t* 		now = data? (tb_hong_t*)(((tb_pointer_t*)data)[1]) : tb_null;
	tb_assert_and_check_return_val(timer && now, tb_false);

	// the task
	tb_ltimer_task_t* task = item? *((tb_ltimer_task_t**)item) : tb_null;
	if (task)
	{
		// repeat?
		if (task->repeat)
		{
			// update when
			task->when = *now + task->period;

			// continue the task
			if (!tb_ltimer_add_task(timer, task))
			{
				// trace
				tb_trace_d("[ltimer]: continue to add task failed");
			}
		}
		else
		{
			// refn--
			if (task->refn > 1) task->refn--;
			// remove it from pool directly
			else tb_rpool_free(timer->pool, task);
		}
	}

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_ltimer_init(tb_size_t maxn, tb_size_t tick, tb_bool_t ctime)
{
	// check
	tb_assert_and_check_return_val(tick >= TB_LTIMER_TICK_100MS, tb_null);

	// make timer
	tb_ltimer_t* timer = tb_malloc0(sizeof(tb_ltimer_t));
	tb_assert_and_check_return_val(timer, tb_null);

	// init timer
	timer->maxn 		= tb_max(maxn, 16);
	timer->ctime 		= ctime;
	timer->tick 		= tick;
	timer->btime 		= tb_ltimer_now(timer);

	// init lock
	if (!tb_spinlock_init(&timer->lock)) goto fail;

	// init pool
	timer->pool 		= tb_rpool_init((maxn >> 2) + 16, sizeof(tb_ltimer_task_t), 0);
	tb_assert_and_check_goto(timer->pool, fail);

	// init the expired tasks
	timer->expired 		= tb_vector_init((maxn / TB_LTIMER_WHEEL_MAXN) + 8, tb_item_func_ptr(tb_null, tb_null));
	tb_assert_and_check_goto(timer->expired, fail);

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

		// wait loop exit
		tb_size_t tryn = 10;
		while (tb_atomic_get(&timer->work) && tryn--) tb_msleep(500);

		// warning
		if (!tryn && tb_atomic_get(&timer->work)) tb_trace_w("[ltimer]: the loop has been not exited now!");

		// enter
		tb_spinlock_enter(&timer->lock);

		// exit wheel
		{
			tb_size_t i = 0;
			for (i = 0; i < TB_LTIMER_WHEEL_MAXN; i++)
			{
				if (timer->wheel[i]) tb_vector_exit(timer->wheel[i]);
				timer->wheel[i] = tb_null;
			}
		}

		// exit pool
		if (timer->pool) tb_rpool_exit(timer->pool);
		timer->pool = tb_null;

		// leave
		tb_spinlock_leave(&timer->lock);

		// exit the expired tasks
		if (timer->expired) tb_vector_exit(timer->expired);
		timer->expired = tb_null;

		// exit lock
		tb_spinlock_exit(&timer->lock);

		// exit it
		tb_free(timer);
	}
}
tb_void_t tb_ltimer_clear(tb_handle_t handle)
{
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	if (timer)
	{
		// enter
		tb_spinlock_enter(&timer->lock);

		// move to the wheel head
		timer->btime = tb_ltimer_now(timer);
		timer->wbase = 0;

		// clear wheel
		{
			tb_size_t i = 0;
			for (i = 0; i < TB_LTIMER_WHEEL_MAXN; i++)
			{
				if (timer->wheel[i]) tb_vector_clear(timer->wheel[i]);
			}
		}

		// clear pool
		if (timer->pool) tb_rpool_clear(timer->pool);

		// leave
		tb_spinlock_leave(&timer->lock);
	}
}
tb_size_t tb_ltimer_limit(tb_handle_t handle)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer, 0);

	// the timer limit
	return (TB_LTIMER_WHEEL_MAXN * timer->tick);
}
tb_size_t tb_ltimer_delay(tb_handle_t handle)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer, -1);

	// ok?
	return timer->tick;
}
tb_bool_t tb_ltimer_spak(tb_handle_t handle)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer && timer->pool && timer->tick && timer->expired, tb_false);

	// stoped?
	tb_check_return_val(!tb_atomic_get(&timer->stop), tb_false);

	// the now time
	tb_hong_t now = tb_ltimer_now(timer);

	// clear expired
	tb_vector_clear(timer->expired);

	// enter
	tb_spinlock_enter(&timer->lock);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// empty? move to the wheel head
		if (!tb_rpool_size(timer->pool))
		{
			timer->btime = now;
			timer->wbase = 0;
			ok = tb_true;
			break;
		}

		// the diff
		tb_size_t diff = (tb_size_t)((now - timer->btime) / timer->tick);

		// trace
		tb_trace_d("spak: btime: %lld, wbase: %lu, now: %lld, diff: %lu", timer->btime, timer->wbase, now, diff);

		// walk the expired lists
		tb_size_t i = 0;
		tb_size_t n = tb_min(diff, TB_LTIMER_WHEEL_MAXN - 1);
		for (i = 0; i <= n; i++)
		{
			// the wheel index
			tb_size_t indx = (timer->wbase + i) & (TB_LTIMER_WHEEL_MAXN - 1);

			// the wheel list
			tb_vector_t* list = timer->wheel[indx];
			tb_check_continue(list && tb_vector_size(list));

			// init the expired task 
			tb_vector_walk(list, tb_ltimer_expired_init, timer);

			// clear the wheel list
			tb_vector_clear(list);
		}

		// update the base time
		timer->btime = now;

		// update the wheel base
		timer->wbase = (timer->wbase + diff) & (TB_LTIMER_WHEEL_MAXN - 1);
		
		// ok
		ok = tb_true;

	} while (0);

	// leave
	tb_spinlock_leave(&timer->lock);

	// ok? and exists expired task?
	if (ok && tb_vector_size(timer->expired))
	{
		// done the expired task 
		tb_vector_walk(timer->expired, tb_ltimer_expired_done, tb_null);

		// enter
		tb_spinlock_enter(&timer->lock);

		// exit the expired task
		tb_pointer_t data[2]; data[0] = timer; data[1] = &now;
		tb_vector_walk(timer->expired, tb_ltimer_expired_exit, data);

		// leave
		tb_spinlock_leave(&timer->lock);
	}

	// ok?
	return ok;
}
tb_void_t tb_ltimer_loop(tb_handle_t handle)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return(timer);

	// work++
	tb_atomic_fetch_and_inc(&timer->work);

	// loop
	while (!tb_atomic_get(&timer->stop))
	{
		// the delay
		tb_size_t delay = tb_ltimer_delay(handle);
		tb_assert_and_check_break(delay != -1);

		// wait some time
		if (delay) tb_msleep(delay);

		// spak ctime
		if (timer->ctime) tb_ctime_spak();

		// spak it
		if (!tb_ltimer_spak(handle)) break;
	}

	// work--
	tb_atomic_fetch_and_dec(&timer->work);
}
tb_handle_t tb_ltimer_task_add(tb_handle_t handle, tb_size_t delay, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer && func, tb_null);

	// add task
	return tb_ltimer_task_add_at(handle, tb_ltimer_now(timer) + delay, delay, repeat, func, data);
}
tb_handle_t tb_ltimer_task_add_at(tb_handle_t handle, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer && timer->pool && func, tb_null);

	// stoped?
	tb_assert_and_check_return_val(!tb_atomic_get(&timer->stop), tb_null);

	// enter
	tb_spinlock_enter(&timer->lock);

	// make task
	tb_ltimer_task_t* task = (tb_ltimer_task_t*)tb_rpool_malloc0(timer->pool);
	if (task)
	{
		// init task
		task->refn 		= 2;
		task->func 		= func;
		task->data 		= data;
		task->when 		= when;
		task->period 	= period;
		task->repeat 	= repeat? 1 : 0;
		task->killed 	= 0;
		task->windx 	= -1;

		// add task
		if (!tb_ltimer_add_task(timer, task))
		{
			tb_rpool_free(timer->pool, task);
			task = tb_null;
		}
	}

	// leave
	tb_spinlock_leave(&timer->lock);

	// ok?
	return task;
}
tb_handle_t tb_ltimer_task_add_after(tb_handle_t handle, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return_val(timer && func, tb_null);

	// add task
	return tb_ltimer_task_add_at(handle, tb_ltimer_now(timer) + after, period, repeat, func, data);
}
tb_void_t tb_ltimer_task_run(tb_handle_t handle, tb_size_t delay, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return(timer && func);

	// run task
	tb_ltimer_task_run_at(handle, tb_ltimer_now(timer) + delay, delay, repeat, func, data);
}
tb_void_t tb_ltimer_task_run_at(tb_handle_t handle, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return(timer && timer->pool && func);

	// stoped?
	tb_assert_and_check_return(!tb_atomic_get(&timer->stop));

	// enter
	tb_spinlock_enter(&timer->lock);

	// make task
	tb_ltimer_task_t* task = (tb_ltimer_task_t*)tb_rpool_malloc0(timer->pool);
	if (task)
	{
		// init task
		task->refn 		= 1;
		task->func 		= func;
		task->data 		= data;
		task->when 		= when;
		task->period 	= period;
		task->repeat 	= repeat? 1 : 0;
		task->killed 	= 0;
		task->windx 	= -1;

		// add task
		if (!tb_ltimer_add_task(timer, task))
			tb_rpool_free(timer->pool, task);
	}

	// leave
	tb_spinlock_leave(&timer->lock);
}
tb_void_t tb_ltimer_task_run_after(tb_handle_t handle, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_ltimer_t* timer = (tb_ltimer_t*)handle;
	tb_assert_and_check_return(timer && func);

	// run task
	tb_ltimer_task_run_at(handle, tb_ltimer_now(timer) + after, period, repeat, func, data);
}
tb_void_t tb_ltimer_task_del(tb_handle_t handle, tb_handle_t htask)
{
	// check
	tb_ltimer_t* 		timer = (tb_ltimer_t*)handle;
	tb_ltimer_task_t* 	task = (tb_ltimer_task_t*)htask;
	tb_assert_and_check_return(timer && timer->pool && task);

	// trace
	tb_trace_d("del: when: %lld, period: %u, refn: %u", task->when, task->period, task->refn);

	// enter
	tb_spinlock_enter(&timer->lock);

	if (task->refn > 1)
	{
		// refn--
		task->refn--;

		// cancel task
		task->func 		= tb_null;
		task->data 		= tb_null;
		task->repeat 	= 0;
	}
	// remove it from pool directly if the task have been expired 
	else tb_rpool_free(timer->pool, task);

	// leave
	tb_spinlock_leave(&timer->lock);
}
tb_void_t tb_ltimer_task_kil(tb_handle_t handle, tb_handle_t htask)
{
	// check
	tb_ltimer_t* 		timer = (tb_ltimer_t*)handle;
	tb_ltimer_task_t* 	task = (tb_ltimer_task_t*)htask;
	tb_assert_and_check_return(timer && timer->pool && task);

	// trace
	tb_trace_d("kil: when: %lld, period: %u, refn: %u", task->when, task->period, task->refn);

	// enter
	tb_spinlock_enter(&timer->lock);

	// done
	do
	{
		// expired or removed?
		tb_check_break(task->refn == 2);

		// del the task first
		if (!tb_ltimer_del_task(timer, task))
		{
			// trace
			tb_trace_d("[ltimer]: del task failed");
			break;
		}

		// killed
		task->killed = 1;

		// no repeat
		task->repeat = 0;
				
		// modify when => now
		task->when = tb_ltimer_now(timer);

		// re-add task
		if (!tb_ltimer_add_task(timer, task))
		{
			// trace
			tb_trace_d("[ltimer]: re-add task failed");
			break;
		}

	} while (0);

	// leave
	tb_spinlock_leave(&timer->lock);
}

