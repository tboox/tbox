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
#include <windows.h>

/* /////////////////////////////////////////////////////////
 * implemention
 */

tb_handle_t tb_thread_open(tb_char_t const* name, tb_pointer_t (*callback)(tb_pointer_t), tb_pointer_t param, tb_size_t stack_size)
{
	HANDLE hthread = CreateThread(NULL, (DWORD)stack_size, (LPTHREAD_START_ROUTINE)callback, (LPVOID)param, 0, NULL);
	return ((hthread != INVALID_HANDLE_VALUE)? hthread : TB_NULL);
}
tb_void_t tb_thread_close(tb_handle_t hthread)
{
	if (hthread) CloseHandle(hthread);
	hthread = TB_NULL;
}
tb_bool_t tb_thread_wait(tb_handle_t hthread, tb_int_t timeout)
{
	if (hthread && WAIT_OBJECT_0 == WaitForSingleObject(hthread, timeout)) return TB_TRUE;
	return TB_FALSE;
}
tb_bool_t tb_thread_terminate(tb_handle_t hthread)
{
	if (hthread) return TerminateThread(hthread, 0)? TB_TRUE : TB_FALSE;
	return TB_FALSE;
}
tb_void_t tb_thread_exit(tb_pointer_t retval)
{
	ExitThread(0);
}
tb_bool_t tb_thread_suspend(tb_handle_t hthread)
{
	if (hthread) return ((DWORD)-1 != SuspendThread(hthread))? TB_TRUE : TB_FALSE;
	return TB_FALSE;
}
tb_bool_t tb_thread_resume(tb_handle_t hthread)
{
	if (hthread) return ((DWORD)-1 != ResumeThread(hthread))? TB_TRUE : TB_FALSE;
	return TB_FALSE;
}

