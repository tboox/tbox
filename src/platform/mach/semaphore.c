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
 * @file		semaphore.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <time.h>
#include <errno.h>
#include <mach/semaphore.h>
#include <mach/task.h>
#include <mach/mach.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the semaphore type
typedef struct __tb_semaphore_t
{
	// the handle
    semaphore_t 		handle;

	// the value
    tb_atomic_t 		value;

}tb_semaphore_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_semaphore_init(tb_size_t init)
{
	// make semaphore
	tb_semaphore_t* semaphore = tb_malloc0(sizeof(tb_semaphore_t));
	tb_assert_and_check_return_val(semaphore, tb_null);

	// init semaphore 
	if (KERN_SUCCESS != semaphore_create(mach_task_self(), &(semaphore->handle), SYNC_POLICY_FIFO, init)) goto fail;

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
		semaphore_destroy(mach_task_self(), semaphore->handle);

		// exit it
		tb_free(semaphore);
	}
}
tb_bool_t tb_semaphore_post(tb_handle_t handle, tb_size_t post)
{
	// check
	tb_semaphore_t* semaphore = (tb_semaphore_t*)handle;
	tb_assert_and_check_return_val(semaphore && post, tb_false);

	// post
	while (post--)
	{
		// +2 first
		tb_atomic_fetch_and_add(&semaphore->value, 2);

		// signal
		if (KERN_SUCCESS != semaphore_signal(semaphore->handle)) 
		{
			// restore
			tb_atomic_fetch_and_sub(&semaphore->value, 2);
			return tb_false;
		}

		// -1
		tb_atomic_fetch_and_dec(&semaphore->value);
	}

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
	tb_assert_and_check_return_val(semaphore, -1);

	// init timespec
	mach_timespec_t spec = {0};
	if (timeout > 0)
	{
		spec.tv_sec += timeout / 1000;
		spec.tv_nsec += (timeout % 1000) * 1000000;
	}
	else if (timeout < 0) spec.tv_sec += 12 * 30 * 24 * 3600; // infinity: one year

	// wait
	tb_long_t ok = semaphore_timedwait(semaphore->handle, spec);

	// timeout?
	tb_check_return_val(ok != KERN_OPERATION_TIMED_OUT, 0);

	// ok?
	tb_check_return_val(ok == KERN_SUCCESS, -1);

	// check value
	tb_assert_and_check_return_val((tb_long_t)tb_atomic_get(&semaphore->value) > 0, -1);
	
	// value--
	tb_atomic_fetch_and_dec(&semaphore->value);
	
	// ok
	return 1;
}


