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
 * @file		aicp.c
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "timer.h"
#include "../platform/ltimer.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_aicp_timer_timeout(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->timer, 0);
 
	// the timeout
	return tb_ltimer_timeout(aicp->timer);
}
tb_bool_t tb_aicp_timer_spak(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->timer, tb_false);
 
	// spak it
	return tb_ltimer_spak(aicp->timer);
}
tb_handle_t tb_aicp_timer_add(tb_aicp_t* aicp, tb_size_t timeout, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->timer && func, tb_null);
 
	// add task
	return tb_ltimer_task_add(aicp->timer, timeout, repeat, func, data);
}
tb_handle_t tb_aicp_timer_add_at(tb_aicp_t* aicp, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->timer && func, tb_null);

	// add task
	return tb_ltimer_task_add_at(aicp->timer, when, period, repeat, func, data);
}
tb_handle_t tb_aicp_timer_add_after(tb_aicp_t* aicp, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->timer && func, tb_null);
 
	// add task
	return tb_ltimer_task_add_after(aicp->timer, after, period, repeat, func, data);
}
tb_void_t tb_aicp_timer_run(tb_aicp_t* aicp, tb_size_t timeout, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(aicp && aicp->timer && func);

	// run task
	tb_ltimer_task_run(aicp->timer, timeout, repeat, func, data);
}
tb_void_t tb_aicp_timer_run_at(tb_aicp_t* aicp, tb_hize_t when, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(aicp && aicp->timer && func);
 
	// run task
	tb_ltimer_task_run_at(aicp->timer, when, period, repeat, func, data);
}
tb_void_t tb_aicp_timer_run_after(tb_aicp_t* aicp, tb_hize_t after, tb_size_t period, tb_bool_t repeat, tb_timer_task_func_t func, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(aicp && aicp->timer && func);

	// run task
	tb_ltimer_task_run_after(aicp->timer, after, period, repeat, func, data);
}
tb_void_t tb_aicp_timer_del(tb_aicp_t* aicp, tb_handle_t task)
{
	// check
	tb_assert_and_check_return(aicp && aicp->timer && task);
 
	// del task
	tb_ltimer_task_del(aicp->timer, task);
}

