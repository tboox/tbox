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
 * @file		spinlock.c
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../spinlock.h"
#include <windows.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_spinlock_init()
{
	// make critical section
	LPCRITICAL_SECTION pcs = tb_malloc0(sizeof(CRITICAL_SECTION));
	tb_assert_and_check_return_val(pcs, tb_null);

	// init critical section
	InitializeCriticalSection(pcs);
	
	// ok?
	return ((tb_handle_t)pcs);
}
tb_void_t tb_spinlock_exit(tb_handle_t handle)
{
	LPCRITICAL_SECTION pcs = (LPCRITICAL_SECTION)handle;
	if (pcs)
	{
		DeleteCriticalSection(pcs);
		tb_free((tb_pointer_t)pcs);
	}
}
tb_bool_t tb_spinlock_enter(tb_handle_t handle)
{
	// check
	LPCRITICAL_SECTION pcs = (LPCRITICAL_SECTION)handle;
	tb_assert_and_check_return_val(pcs, tb_false);

	// enter
	EnterCriticalSection(pcs);

	// ok
	return tb_true;
}
tb_bool_t tb_spinlock_enter_try(tb_handle_t handle)
{
	// check
	LPCRITICAL_SECTION pcs = (LPCRITICAL_SECTION)handle;
	tb_assert_and_check_return_val(pcs, tb_false);

	// try to enter
	return TryEnterCriticalSection(pcs)? tb_true : tb_false;
}
tb_bool_t tb_spinlock_leave(tb_handle_t handle)
{
	// check
	LPCRITICAL_SECTION pcs = (LPCRITICAL_SECTION)handle;
	tb_assert_and_check_return_val(pcs, tb_false);

	// leave
	LeaveCriticalSection(pcs);

	// ok
	return tb_true;
}
