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
 * @file		ltimer.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_LTIMER_H
#define TB_PLATFORM_LTIMER_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the timer tick enum
typedef enum __tb_ltimer_tick_e
{
	TB_LTIMER_TICK_100MS 	= 100
,	TB_LTIMER_TICK_S 		= 1000
,	TB_LTIMER_TICK_M 		= 60000
,	TB_LTIMER_TICK_H 		= 3600000

}tb_ltimer_tick_e;

/*! the timer task func type
 *
 * @param data 		the timer data
 */
typedef tb_void_t 	(*tb_ltimer_task_func_t)(tb_pointer_t data);

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init timer
 *
 * lower tick and limit range, but faster
 * 
 * @param maxn		the timer maxn
 * @param tick		the timer tick
 * @param ctime		using ctime?
 *
 * @return 			the timer
 */
tb_handle_t 		tb_ltimer_init(tb_size_t maxn, tb_size_t tick, tb_bool_t ctime);

/*! exit timer
 *
 * @param handle	the timer handle
 */
tb_void_t 			tb_ltimer_exit(tb_handle_t handle);

/*! the timer limit
 *
 * @param handle	the timer handle
 *
 * @return 			the timer limit range: [now, now + limit)
 */
tb_size_t 			tb_ltimer_limit(tb_handle_t handle);

/*! the timer timeout for spak 
 *
 * @param handle	the timer handle
 *
 * @return 			the timer timeout
 */
tb_size_t 			tb_ltimer_timeout(tb_handle_t handle);

/*! spak timer for the external loop at the single thread
 *
 * @code
 * tb_void_t tb_ltimer_loop()
 * {
 * 		while (1)
 * 		{
 * 			// wait
 * 			wait(tb_ltimer_timeout(timer))
 *
 * 			// spak timer
 * 			tb_ltimer_spak(timer);
 *     	}
 * }
 * @endcode
 *
 * @param handle	the timer handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_ltimer_spak(tb_handle_t handle);

/*! loop timer for the external thread
 *
 * @code
 * tb_void_t tb_ltimer_thread(tb_pointer_t)
 * {
 * 		tb_ltimer_loop(timer);
 * }
 * @endcode
 *
 * @param handle	the timer handle
 *
 * @return 			tb_true or tb_false
 */
tb_void_t 			tb_ltimer_loop(tb_handle_t handle);

/*! run timer task after timeout and will be auto-remove it after be expired
 *
 * @param handle	the timer handle
 * @param period 	the period time, ms
 * @param repeat 	is repeat?
 * @param func		the timer func
 * @param data		the timer data
 *
 */
tb_void_t 			tb_ltimer_task_run(tb_handle_t handle, tb_size_t timeout, tb_bool_t repeat, tb_ltimer_task_func_t func, tb_pointer_t data);

/*! run timer task at the absolute time and will be auto-remove it after be expired
 *
 * @param handle	the timer handle
 * @param when 		the absolute time, ms
 * @param period 	the period time, ms
 * @param repeat 	is repeat?
 * @param func		the timer func
 * @param data		the timer data
 *
 */
tb_void_t 			tb_ltimer_task_run_at(tb_handle_t handle, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_ltimer_task_func_t func, tb_pointer_t data);

/*! run timer task after the relative time and will be auto-remove it after be expired
 *
 * @param handle	the timer handle
 * @param after 	the after time, ms
 * @param period 	the period time, ms
 * @param repeat 	is repeat?
 * @param func		the timer func
 * @param data		the timer data
 *
 */
tb_void_t 			tb_ltimer_task_run_after(tb_handle_t handle, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_ltimer_task_func_t func, tb_pointer_t data);

/*! add and run timer task after timeout and need remove it manually
 *
 * @param handle	the timer handle
 * @param period 	the period time, ms
 * @param repeat 	is repeat?
 * @param func		the timer func
 * @param data		the timer data
 *
 * @return 			the timer task
 */
tb_handle_t 		tb_ltimer_task_add(tb_handle_t handle, tb_size_t timeout, tb_bool_t repeat, tb_ltimer_task_func_t func, tb_pointer_t data);

/*! add and run timer task at the absolute time and need remove it manually
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
tb_handle_t 		tb_ltimer_task_add_at(tb_handle_t handle, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_ltimer_task_func_t func, tb_pointer_t data);

/*! add and run timer task after the relative time and need remove it manually
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
tb_handle_t 		tb_ltimer_task_add_after(tb_handle_t handle, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_ltimer_task_func_t func, tb_pointer_t data);

/*! del timer task
 *
 * @param handle	the timer handle
 * @param task		the timer task
 */
tb_void_t 			tb_ltimer_task_del(tb_handle_t handle, tb_handle_t task);

#endif
