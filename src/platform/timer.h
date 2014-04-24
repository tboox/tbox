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
 * @file		timer.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_TIMER_H
#define TB_PLATFORM_TIMER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the timer task func type
 *
 * @param killed 	is killed?
 * @param data 		the timer data
 */
typedef tb_void_t 	(*tb_timer_task_func_t)(tb_bool_t killed, tb_cpointer_t data);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init timer
 *
 * lower tick and limit range, but faster
 * 
 * @param maxn		the timer maxn
 * @param ctime		using ctime?
 *
 * @return 			the timer
 */
tb_handle_t 		tb_timer_init(tb_size_t maxn, tb_bool_t ctime);

/*! exit timer
 *
 * @param handle	the timer handle
 */
tb_void_t 			tb_timer_exit(tb_handle_t handle);

/*! clear timer
 *
 * @param handle	the timer handle
 */
tb_void_t 			tb_timer_clear(tb_handle_t handle);

/*! the timer delay for spak 
 *
 * @param handle	the timer handle
 *
 * @return 			the timer delay, (tb_size_t)-1: error or no task
 */
tb_size_t 			tb_timer_delay(tb_handle_t handle);

/*! the timer top when
 *
 * @param handle	the timer handle
 *
 * @return 			the top when, -1: no task
 */
tb_hize_t 			tb_timer_top(tb_handle_t handle);

/*! spak timer for the external loop at the single thread
 *
 * @code
 * tb_void_t tb_timer_loop()
 * {
 * 		while (1)
 * 		{
 * 			// wait
 * 			wait(tb_timer_delay(timer))
 *
 * 			// spak timer
 * 			tb_timer_spak(timer);
 *     	}
 * }
 * @endcode
 *
 * @param handle	the timer handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_timer_spak(tb_handle_t handle);

/*! loop timer for the external thread
 *
 * @code
 * tb_void_t tb_timer_thread(tb_pointer_t)
 * {
 * 		tb_timer_loop(timer);
 * }
 * @endcode
 *
 * @param handle	the timer handle
 *
 * @return 			tb_true or tb_false
 */
tb_void_t 			tb_timer_loop(tb_handle_t handle);

/*! post timer task after delay and will be auto-remove it after be expired
 *
 * @param handle	the timer handle
 * @param delay 	the delay time, ms
 * @param repeat 	is repeat?
 * @param func		the timer func
 * @param data		the timer data
 *
 */
tb_void_t 			tb_timer_task_post(tb_handle_t handle, tb_size_t delay, tb_bool_t repeat, tb_timer_task_func_t func, tb_cpointer_t data);

/*! post timer task at the absolute time and will be auto-remove it after be expired
 *
 * @param handle	the timer handle
 * @param when 		the absolute time, ms
 * @param period 	the period time, ms
 * @param repeat 	is repeat?
 * @param func		the timer func
 * @param data		the timer data
 *
 */
tb_void_t 			tb_timer_task_post_at(tb_handle_t handle, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_cpointer_t data);

/*! post timer task after the relative time and will be auto-remove it after be expired
 *
 * @param handle	the timer handle
 * @param after 	the after time, ms
 * @param period 	the period time, ms
 * @param repeat 	is repeat?
 * @param func		the timer func
 * @param data		the timer data
 *
 */
tb_void_t 			tb_timer_task_post_after(tb_handle_t handle, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_cpointer_t data);

/*! init and post timer task after delay and need remove it manually
 *
 * @param handle	the timer handle
 * @param delay 	the delay time, ms
 * @param repeat 	is repeat?
 * @param func		the timer func
 * @param data		the timer data
 *
 * @return 			the timer task
 */
tb_handle_t 		tb_timer_task_init(tb_handle_t handle, tb_size_t delay, tb_bool_t repeat, tb_timer_task_func_t func, tb_cpointer_t data);

/*! init and post timer task at the absolute time and need remove it manually
 *
 * @param handle	the timer handle
 * @param when 		the absolute time, ms
 * @param period 	the period time, ms
 * @param repeat 	is repeat?
 * @param func		the timer func
 * @param data		the timer data
 *
 * @return 			the timer task
 */
tb_handle_t 		tb_timer_task_init_at(tb_handle_t handle, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_cpointer_t data);

/*! init and post timer task after the relative time and need remove it manually
 *
 * @param handle	the timer handle
 * @param after 	the after time, ms
 * @param period 	the period time, ms
 * @param repeat 	is repeat?
 * @param func		the timer func
 * @param data		the timer data
 *
 * @return 			the timer task
 */
tb_handle_t 		tb_timer_task_init_after(tb_handle_t handle, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_cpointer_t data);

/*! exit timer task, the task will be not called if have been not called
 *
 * @param handle	the timer handle
 * @param task		the timer task
 */
tb_void_t 			tb_timer_task_exit(tb_handle_t handle, tb_handle_t task);

/*! kill timer task, the task will be called immediately if have been not called
 *
 * @param handle	the timer handle
 * @param task		the timer task
 */
tb_void_t 			tb_timer_task_kill(tb_handle_t handle, tb_handle_t task);

#endif
