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
 * @file		timer.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_AICP_TIMER_H
#define TB_ASIO_AICP_TIMER_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "aicp.h"
#include "../platform/timer.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the timer timeout for spak 
 *
 * @param handle	the timer handle
 *
 * @return 			the timer timeout
 */
tb_size_t 			tb_aicp_timer_timeout(tb_aicp_t* aicp);

/*! spak timer for the external loop at the single thread
 *
 * @code
 * tb_void_t tb_aicp_timer_loop()
 * {
 * 		while (1)
 * 		{
 * 			// wait
 * 			wait(tb_aicp_timer_timeout(timer))
 *
 * 			// spak timer
 * 			tb_aicp_timer_spak(timer);
 *     	}
 * }
 * @endcode
 *
 * @param handle	the timer handle
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_aicp_timer_spak(tb_aicp_t* aicp);

/*! run timer task after timeout and will be auto-remove it after be expired
 *
 * @param handle	the timer handle
 * @param period 	the period time, ms
 * @param repeat 	is repeat?
 * @param func		the timer func
 * @param data		the timer data
 *
 */
tb_void_t 			tb_aicp_timer_run(tb_aicp_t* aicp, tb_size_t timeout, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data);

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
tb_void_t 			tb_aicp_timer_run_at(tb_aicp_t* aicp, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data);

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
tb_void_t 			tb_aicp_timer_run_after(tb_aicp_t* aicp, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data);

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
tb_handle_t 		tb_aicp_timer_add(tb_aicp_t* aicp, tb_size_t timeout, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data);

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
tb_handle_t 		tb_aicp_timer_add_at(tb_aicp_t* aicp, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data);

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
tb_handle_t 		tb_aicp_timer_add_after(tb_aicp_t* aicp, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data);

/*! del timer task
 *
 * @param handle	the timer handle
 * @param task		the timer task
 */
tb_void_t 			tb_aicp_timer_del(tb_aicp_t* aicp, tb_handle_t task);

#endif
