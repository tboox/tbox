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
 * \file		posix.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include <time.h>
#include <pthread.h>

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the event type
typedef struct __tb_event_t
{
	// mutx
	pthread_mutex_t mutx;

	// cond
	pthread_cond_t 	cond; 

}tb_event_t;

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
tb_handle_t tb_event_init(tb_char_t const* name)
{
	// alloc
	tb_event_t* e = tb_calloc(1, sizeof(tb_event_t));
	tb_assert_and_check_return_val(e, TB_NULL);

	// init mutx
	if (!pthread_mutex_init(&e->mutx, TB_NULL))
	{
		// init cond
		if (!pthread_cond_init(&e->cond, TB_NULL))
			return (tb_handle_t)e;

		// free mutx
		pthread_mutex_destroy(&e->mutx);
	}

	// free it
	tb_free(e);

	// fail
	return TB_NULL;
}
tb_void_t tb_event_exit(tb_handle_t handle)
{
	tb_assert_and_check_return(handle);
	tb_event_t* e = (tb_event_t*)handle;

	// post first
	tb_event_post(handle);

	// free cond
	pthread_cond_destroy(&e->cond);

	// free mutx
	pthread_mutex_destroy(&e->mutx);

	// free it
	tb_free(e);
}
tb_void_t tb_event_post(tb_handle_t handle)
{
	tb_assert_and_check_return(handle);
	tb_event_t* e = (tb_event_t*)handle;

	// post signal
	tb_long_t r = pthread_cond_signal(&e->cond);
	tb_assert(!r);
}
tb_long_t tb_event_wait(tb_handle_t handle, tb_long_t timeout)
{
	tb_assert_and_check_return_val(handle, -1);
	tb_event_t* e = (tb_event_t*)handle;

	// init time
	struct timespec t = {0};
	t.tv_sec = time(TB_NULL);
	if (timeout > 0)
	{
		t.tv_sec += timeout / 1000;
		t.tv_nsec += (timeout % 1000) * 1000000;
	}
	else if (timeout < 0) t.tv_sec += 12 * 30 * 24 * 3600; // infinity: one year

	// wait 
	pthread_mutex_lock(&e->mutx);
	tb_long_t r = pthread_cond_timedwait(&e->cond, &e->mutx, &t);
	pthread_mutex_unlock(&e->mutx);

	// ok?
    tb_check_return_val(r, 1);

	// timeout?
    tb_check_return_val(r != ETIMEDOUT, 0);

	// error
	return -1;
}

