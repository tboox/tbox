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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		event.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../atomic.h"

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
tb_handle_t tb_event_init(tb_char_t const* name)
{
	// alloc
	tb_size_t* e = tb_calloc(1, sizeof(tb_size_t));
	tb_assert_and_check_return_val(e, TB_NULL);

	// warning
	tb_warning("the event impl maybe not fast");

	// ok
	return (tb_handle_t)e;

fail:
	if (e) tb_free(e);
	return TB_NULL;
}
tb_void_t tb_event_exit(tb_handle_t handle)
{
	tb_assert_and_check_return(handle);
	tb_size_t* e = (tb_size_t*)handle;

	// post first
	tb_event_post(handle);

	// wait some time
	tb_msleep(200);

	// free it
	tb_free(e);
}
tb_void_t tb_event_post(tb_handle_t handle)
{
	tb_assert_and_check_return(handle);
	tb_size_t* e = (tb_size_t*)handle;

	// post signal
	tb_atomic_set(e, 1);
}
tb_long_t tb_event_wait(tb_handle_t handle, tb_long_t timeout)
{
	tb_assert_and_check_return_val(handle, -1);
	tb_size_t* e = (tb_size_t*)handle;

	// init
	tb_long_t 	r = 0;
	tb_int64_t 	base = tb_mclock();

	// wait 
	while (1)
	{
		// get post
		tb_size_t post = tb_atomic_fetch_and_set0(e);

		// has signal?
		if (post == 1) 
		{
			r = 1;
			break;
		}
		// error
		else if (post != 0) 
		{
			r = -1;
			break;
		}
		// no signal?
		else
		{
			// timeout?
			if (timeout >= 0 && tb_mclock() - base >= timeout) break;
			else tb_msleep(200);
		}
	}

	return r;
}

