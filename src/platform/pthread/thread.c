/*!The Treasure Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		thread.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

/* /////////////////////////////////////////////////////////
 * implemention
 */

tb_handle_t tb_thread_open(tb_char_t const* name, tb_pointer_t (*callback)(tb_pointer_t), tb_pointer_t param, tb_size_t stack_size)
{
	pthread_t hthread;
	if (0 != pthread_create(&hthread, NULL, callback, param)) return TB_NULL;
	else return ((tb_handle_t)hthread);
}
tb_void_t tb_thread_close(tb_handle_t hthread)
{
}
tb_bool_t tb_thread_wait(tb_handle_t hthread, tb_int_t timeout)
{
	tb_assert_and_check_return_val(hthread, TB_FALSE);

	if (0 != pthread_join(((pthread_t)hthread), NULL)) return TB_FALSE;
	else return TB_TRUE;
}
tb_bool_t tb_thread_terminate(tb_handle_t hthread)
{
	return TB_TRUE;
}
tb_void_t tb_thread_exit(tb_pointer_t retval)
{
	pthread_exit(retval);
}
tb_bool_t tb_thread_suspend(tb_handle_t hthread)
{
	return TB_TRUE;
}
tb_bool_t tb_thread_resume(tb_handle_t hthread)
{
	return TB_TRUE;
}

