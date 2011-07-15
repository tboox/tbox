/*!The Tiny Platform Library
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

tb_handle_t tb_thread_create(tb_char_t const* name, void* (*callback)(void*), void* param, tb_size_t stack_size)
{
	pthread_t hthread;
	if (0 != pthread_create(&hthread, NULL, callback, param)) return TB_NULL;
	else return ((tb_handle_t)hthread);
}
void tb_thread_destroy(tb_handle_t hthread)
{
}
tb_bool_t tb_thread_wait(tb_handle_t hthread, tb_int_t timeout)
{
	TB_ASSERT_RETURN_VAL(hthread, TB_FALSE);

	if (0 != pthread_join(((pthread_t)hthread), NULL)) return TB_FALSE;
	else return TB_TRUE;
}
tb_bool_t tb_thread_terminate(tb_handle_t hthread)
{
	return TB_TRUE;
}
void tb_thread_exit(void* retval)
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

