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
 * \author		ruki
 * \file		event.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../event.h"
#include "windows.h"

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */

tb_handle_t tb_event_init(tb_char_t const* name)
{
	HANDLE handle = CreateEvent(TB_NULL, FALSE, FALSE, name);
	return ((handle != INVALID_HANDLE_VALUE)? handle : TB_NULL);
}
tb_void_t tb_event_exit(tb_handle_t handle)
{
	if (handle) CloseHandle(handle);
}
tb_void_t tb_event_post(tb_handle_t handle)
{
	if (handle) SetEvent(handle);
}
tb_long_t tb_event_wait(tb_handle_t handle, tb_long_t timeout)
{
	tb_assert_and_check_return_val(handle, -1);

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


