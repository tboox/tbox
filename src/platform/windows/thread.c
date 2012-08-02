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
#include "../thread.h"
#include <windows.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_handle_t tb_thread_init(tb_char_t const* name, tb_pointer_t (*callback)(tb_pointer_t), tb_pointer_t cb_data, tb_size_t stack_size)
{
	HANDLE handle = CreateThread(NULL, (DWORD)stack_size, (LPTHREAD_START_ROUTINE)callback, (LPVOID)cb_data, 0, NULL);
	return ((handle != INVALID_HANDLE_VALUE)? handle : TB_NULL);
}
tb_void_t tb_thread_exit(tb_handle_t handle)
{
	if (handle) CloseHandle(handle);
}
tb_long_t tb_thread_wait(tb_handle_t handle, tb_long_t timeout)
{
	// wait
	tb_long_t r = WaitForSingleObject(handle, timeout >= 0? timeout : INFINITE);
	tb_assert_and_check_return_val(r != WAIT_FAILED, -1);

	// timeout?
	tb_check_return_val(r != WAIT_TIMEOUT, 0);

	// error?
	tb_check_return_val(r >= WAIT_OBJECT_0, -1);

	// ok
	return 1;
}
tb_bool_t tb_thread_kill(tb_handle_t handle)
{
	if (handle) return TerminateThread(handle, 0)? TB_TRUE : TB_FALSE;
	return TB_FALSE;
}
tb_void_t tb_thread_return(tb_pointer_t value)
{
	ExitThread(0);
}
tb_bool_t tb_thread_suspend(tb_handle_t handle)
{
	if (handle) return ((DWORD)-1 != SuspendThread(handle))? TB_TRUE : TB_FALSE;
	return TB_FALSE;
}
tb_bool_t tb_thread_resume(tb_handle_t handle)
{
	if (handle) return ((DWORD)-1 != ResumeThread(handle))? TB_TRUE : TB_FALSE;
	return TB_FALSE;
}
tb_handle_t tb_thread_self()
{
	return (tb_handle_t)GetCurrentThread();
}
