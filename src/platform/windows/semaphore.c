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
 * @file		semaphore.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../platform.h"
#include "windows.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_SEMAPHORE_VALUE_MAXN 			(65536)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the semaphore type
typedef struct __tb_semaphore_t
{
	// the semaphore handle
	HANDLE 			handle;

	// the semaphore value
	tb_atomic_t 	value;

}tb_semaphore_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_handle_t tb_semaphore_init(tb_size_t init)
{
	// check
	tb_assert_and_check_return_val(init <= TB_SEMAPHORE_VALUE_MAXN, tb_null);

	// make semaphore
	tb_semaphore_t* semaphore = tb_malloc0(sizeof(tb_semaphore_t));
	tb_assert_and_check_return_val(semaphore, tb_null);

	// init semaphore 
	semaphore->handle = CreateSemaphoreA(tb_null, (DWORD)init, TB_SEMAPHORE_VALUE_MAXN, tb_null);
	tb_assert_and_check_goto(semaphore->handle && semaphore->handle != INVALID_HANDLE_VALUE, fail);

	// init value
	semaphore->value = init;

	// ok
	return semaphore;

fail:
	if (semaphore) tb_semaphore_exit(semaphore);
	return tb_null;
}
tb_void_t tb_semaphore_exit(tb_handle_t handle)
{
	tb_semaphore_t* semaphore = (tb_semaphore_t*)handle;
	if (semaphore) 
	{
		// exit semaphore
		if (semaphore->handle && semaphore->handle != INVALID_HANDLE_VALUE) CloseHandle(semaphore->handle);
		semaphore->handle = INVALID_HANDLE_VALUE;

		// exit it
		tb_free(semaphore);
	}
}
tb_bool_t tb_semaphore_post(tb_handle_t handle, tb_size_t post)
{
	// check
	tb_semaphore_t* semaphore = (tb_semaphore_t*)handle;
	tb_assert_and_check_return_val(semaphore && semaphore->handle && semaphore->handle != INVALID_HANDLE_VALUE && post, tb_false);
	
	// += post
	tb_atomic_fetch_and_add(&semaphore->value, post);
	
	// post
	LONG prev = 0;
	if (!ReleaseSemaphore(semaphore->handle, (LONG)post, &prev) && prev >= 0) 
	{
		// restore
		tb_atomic_fetch_and_sub(&semaphore->value, post);
		return tb_false;
	}

	// check
	tb_assert_and_check_return_val(prev + post <= TB_SEMAPHORE_VALUE_MAXN, tb_false);
	
	// save value
	tb_atomic_set(&semaphore->value, prev + post);
	
	// ok
	return tb_true;
}
tb_long_t tb_semaphore_value(tb_handle_t handle)
{
	// check
	tb_semaphore_t* semaphore = (tb_semaphore_t*)handle;
	tb_assert_and_check_return_val(semaphore, -1);

	// get value
	return (tb_long_t)tb_atomic_get(&semaphore->value);
}
tb_long_t tb_semaphore_wait(tb_handle_t handle, tb_long_t timeout)
{
	// check
	tb_semaphore_t* semaphore = (tb_semaphore_t*)handle;
	tb_assert_and_check_return_val(semaphore && semaphore->handle && semaphore->handle != INVALID_HANDLE_VALUE, -1);

	// wait
	tb_long_t r = WaitForSingleObject(semaphore->handle, timeout >= 0? timeout : INFINITE);
	tb_assert_and_check_return_val(r != WAIT_FAILED, -1);

	// timeout?
	tb_check_return_val(r != WAIT_TIMEOUT, 0);

	// error?
	tb_check_return_val(r >= WAIT_OBJECT_0, -1);

	// check value
	tb_assert_and_check_return_val((tb_long_t)tb_atomic_get(&semaphore->value) > 0, -1);
	
	// value--
	tb_atomic_fetch_and_dec(&semaphore->value);
	
	// ok
	return 1;
}


