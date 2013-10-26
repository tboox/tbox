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
 * @file		thread.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../thread.h"
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_handle_t tb_thread_init(tb_char_t const* name, tb_pointer_t (*func)(tb_pointer_t), tb_pointer_t data, tb_size_t stack_size)
{
	pthread_t handle;
	if (0 != pthread_create(&handle, tb_null, func, data)) return tb_null;
	else return ((tb_handle_t)handle);
}
tb_void_t tb_thread_exit(tb_handle_t handle)
{
}
tb_long_t tb_thread_wait(tb_handle_t handle, tb_long_t timeout)
{
	tb_assert_and_check_return_val(handle, -1);

	// wait
	if (0 != pthread_join(((pthread_t)handle), tb_null)) return -1;
	
	// ok
	return 1;
}
tb_bool_t tb_thread_kill(tb_handle_t handle)
{
	return tb_true;
}
tb_void_t tb_thread_return(tb_pointer_t value)
{
	pthread_exit(value);
}
tb_bool_t tb_thread_suspend(tb_handle_t handle)
{
	tb_trace_noimpl();
	return tb_false;
}
tb_bool_t tb_thread_resume(tb_handle_t handle)
{
	tb_trace_noimpl();
	return tb_false;
}
tb_size_t tb_thread_self()
{
	return (tb_size_t)pthread_self();
}

