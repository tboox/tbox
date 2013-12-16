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
 * @file		mutex.c
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../mutex.h"
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_mutex_init()
{
	pthread_mutex_t* pmutex = tb_malloc0(sizeof(pthread_mutex_t));
	if (!pmutex || pthread_mutex_init(pmutex, tb_null)) return tb_null;
	else return ((tb_handle_t)pmutex);
}
tb_void_t tb_mutex_exit(tb_handle_t handle)
{
	tb_assert_and_check_return(handle);
	pthread_mutex_t* pmutex = (pthread_mutex_t*)handle;
	if (pmutex)
	{
		pthread_mutex_destroy(pmutex);
		tb_free((tb_pointer_t)pmutex);
	}
}
tb_bool_t tb_mutex_enter(tb_handle_t handle)
{
	tb_assert_and_check_return_val(handle, tb_false);
	if (pthread_mutex_lock((pthread_mutex_t*)handle)) return tb_false;
	else return tb_true;
}
tb_bool_t tb_mutex_enter_try(tb_handle_t handle)
{
	tb_assert_and_check_return_val(handle, tb_false);
	if (pthread_mutex_trylock((pthread_mutex_t*)handle)) return tb_false;
	else return tb_true;
}
tb_bool_t tb_mutex_leave(tb_handle_t handle)
{
	tb_assert_and_check_return_val(handle, tb_false);
	if (pthread_mutex_unlock((pthread_mutex_t*)handle)) return tb_false;
	else return tb_true;
}
