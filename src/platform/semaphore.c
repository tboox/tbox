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
#include "time.h"
#include "atomic.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_semaphore_init(tb_size_t init)
{
	// make
	tb_atomic_t* semaphore = tb_malloc0(sizeof(tb_atomic_t));
	tb_assert_and_check_return_val(semaphore, tb_null);

	// init
	*semaphore = init;

	// ok
	return (tb_handle_t)semaphore;

fail:
	if (semaphore) tb_free(semaphore);
	return tb_null;
}
tb_void_t tb_semaphore_exit(tb_handle_t handle)
{
	// check
	tb_atomic_t* semaphore = (tb_atomic_t*)handle;
	tb_assert_and_check_return(handle);

	// free it
	tb_free(semaphore);
}
tb_bool_t tb_semaphore_post(tb_handle_t handle, tb_size_t post)
{
	// check
	tb_atomic_t* semaphore = (tb_atomic_t*)handle;
	tb_assert_and_check_return_val(semaphore && post, tb_false);

	// post it
	tb_long_t value = tb_atomic_fetch_and_add(semaphore, post);

	// ok
	return value >= 0? tb_true : tb_false;
}
tb_long_t tb_semaphore_value(tb_handle_t handle)
{
	// check
	tb_atomic_t* semaphore = (tb_atomic_t*)handle;
	tb_assert_and_check_return_val(semaphore, tb_false);

	// get value
	return (tb_long_t)tb_atomic_get(semaphore);
}
tb_long_t tb_semaphore_wait(tb_handle_t handle, tb_long_t timeout)
{
	// check
	tb_atomic_t* semaphore = (tb_atomic_t*)handle;
	tb_assert_and_check_return_val(semaphore, -1);

	// init
	tb_long_t 	r = 0;
	tb_hong_t 	base = tb_mclock();

	// wait 
	while (1)
	{
		// get post
		tb_long_t post = (tb_long_t)tb_atomic_get(semaphore);

		// has signal?
		if (post > 0) 
		{
			// semaphore--
			tb_atomic_fetch_and_dec(semaphore);

			// ok
			r = post;
			break;
		}
		// no signal?
		else if (!post)
		{
			// timeout?
			if (timeout >= 0 && tb_mclock() - base >= timeout) break;
			else tb_msleep(200);
		}
		// error
		else
		{
			r = -1;
			break;
		}
	}

	return r;
}

