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
 * @file		mutex.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../mutex.h"
#include "../../utils/utils.h"
#include <windows.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_handle_t tb_mutex_init()
{
	HANDLE handle = CreateMutex(tb_null, FALSE, tb_null);
	return ((handle != INVALID_HANDLE_VALUE)? handle : tb_null);
}
tb_void_t tb_mutex_exit(tb_handle_t handle)
{
	if (handle) CloseHandle(handle);
}
tb_bool_t tb_mutex_enter(tb_handle_t handle)
{
	// try to enter for profiler
#ifdef TB_LOCK_PROFILER_ENABLE
	if (tb_mutex_enter_try(handle)) return tb_true;
#endif
	
	// enter
	if (handle && WAIT_OBJECT_0 == WaitForSingleObject(handle, INFINITE)) return tb_true;

	// failed
	return tb_false;
}
tb_bool_t tb_mutex_enter_try(tb_handle_t handle)
{
	// try to enter
	if (handle && WAIT_OBJECT_0 == WaitForSingleObject(handle, 0)) return tb_true;
	
	// occupied
#ifdef TB_LOCK_PROFILER_ENABLE
	tb_lock_profiler_occupied(tb_lock_profiler(), handle);
#endif

	// failed
	return tb_false;
}
tb_bool_t tb_mutex_leave(tb_handle_t handle)
{
	if (handle) return ReleaseMutex(handle)? tb_true : tb_false;
	return tb_false;
}
