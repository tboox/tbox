/*!The Tiny Box Library
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
 * \file		mbuffer.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "mbuffer.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* ////////////////////////////////////////////////////////////////////////
 * atomic
 */
static __tb_inline__ tb_handle_t tb_mbuffer_atomic_mutx_get(tb_mbuffer_t const* buffer)
{
	tb_check_return_val(buffer->mutx, TB_NULL);
	return tb_atomic_get(buffer->mutx);
}
static __tb_inline__ tb_handle_t tb_mbuffer_atomic_mutx_del(tb_mbuffer_t* buffer)
{
	tb_check_return_val(buffer->mutx, TB_NULL);
	return tb_atomic_fetch_and_set0(buffer->mutx);
}

/* ////////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_mbuffer_init(tb_mbuffer_t* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_FALSE);
	tb_memset(buffer, 0, sizeof(tb_mbuffer_t));
	return tb_rbuffer_init(&buffer->rbuf);
}
tb_void_t tb_mbuffer_exit(tb_mbuffer_t* buffer)
{
	if (buffer)
	{
		// refn--
		tb_mbuffer_decr(buffer);

		// clear
		tb_memset(buffer, 0, sizeof(tb_mbuffer_t));
	}
}

/* ////////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_byte_t* tb_mbuffer_data(tb_mbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// cstr
	tb_byte_t* p = tb_rbuffer_data(&buffer->rbuf);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return p;
}
tb_size_t tb_mbuffer_size(tb_mbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);

	// size
	tb_size_t n = tb_rbuffer_size(&buffer->rbuf);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	return n;
}
tb_size_t tb_mbuffer_maxn(tb_mbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);

	// refn
	tb_size_t r = tb_rbuffer_maxn(&buffer->rbuf);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	return r;
}
tb_size_t tb_mbuffer_refn(tb_mbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);

	// refn
	tb_size_t r = tb_rbuffer_refn(&buffer->rbuf);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	return r;
}

/* ////////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_mbuffer_clear(tb_mbuffer_t* buffer)
{
	tb_assert_and_check_return(buffer);

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return(mutx);

	// enter
	tb_check_return(tb_mutex_enter(mutx));

	// clear
	tb_rbuffer_clear(&buffer->rbuf);

	// leave
	tb_check_return(tb_mutex_leave(mutx));
}
tb_byte_t* tb_mbuffer_resize(tb_mbuffer_t* buffer, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// resize
	tb_byte_t* p = tb_rbuffer_resize(&buffer->rbuf, n);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return p;
}
tb_size_t tb_mbuffer_incr(tb_mbuffer_t* buffer)
{	
	tb_assert_and_check_return_val(buffer, 0);

	// init
	tb_size_t r = 0;

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	if (!mutx)
	{
		// init mutx
		mutx = tb_mutex_init("rsting");
		tb_assert_and_check_goto(mutx, fail);

		// alloc the shared mutx 
		buffer->mutx = tb_calloc(1, sizeof(tb_handle_t));
		tb_assert_and_check_goto(buffer->mutx, fail);
	
		// init the shared mutx
		*(buffer->mutx) = mutx;
	
		// refn++
		r = tb_rbuffer_incr(&buffer->rbuf);
	}
	else
	{
		// enter
		tb_check_return_val(tb_mutex_enter(mutx), 0);
		
		// refn++
		r = tb_rbuffer_incr(&buffer->rbuf);

		// leave
		tb_check_return_val(tb_mutex_leave(mutx), 0);
	}

	return r;

fail:

	// free mutx
	if (buffer->mutx) tb_free(buffer->mutx);
	buffer->mutx = TB_NULL;

	// exit mutx
	if (mutx) tb_mutex_exit(mutx);

	return 0;
}
tb_size_t tb_mbuffer_decr(tb_mbuffer_t* buffer)
{	
	tb_assert_and_check_return_val(buffer, 0);

	// init
	tb_size_t r = 0;

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return_val(mutx, 0);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), 0);

	// refn--
	r = tb_rbuffer_decr(&buffer->rbuf);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), 0);

	// free mutx
	if (!r)
	{
		// atomic remove mutx
		mutx = tb_mbuffer_atomic_mutx_del(buffer);
		if (mutx) //!< only one get it
		{
			// exit mutex
			tb_mutex_exit(mutx);

			// free it
			tb_free(buffer->mutx);
		}
	}

	return r;
}
/* ////////////////////////////////////////////////////////////////////////
 * enter & leave
 */
tb_byte_t* tb_mbuffer_enter(tb_mbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return_val(mutx, TB_FALSE);

	// enter
	if (!tb_mutex_enter(mutx)) return TB_NULL;

	return tb_rbuffer_data(&buffer->rbuf);
}
tb_void_t tb_mbuffer_leave(tb_mbuffer_t const* buffer)
{
	tb_assert_and_check_return(buffer);

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return(mutx);

	// leave
	tb_mutex_leave(mutx);
}

/* ////////////////////////////////////////////////////////////////////////
 * memset
 */
tb_byte_t* tb_mbuffer_memset(tb_mbuffer_t* buffer, tb_byte_t b)
{
	return tb_mbuffer_memnsetp(buffer, 0, b, tb_mbuffer_size(buffer));
}
tb_byte_t* tb_mbuffer_memsetp(tb_mbuffer_t* buffer, tb_size_t p, tb_byte_t b)
{
	return tb_mbuffer_memnsetp(buffer, p, b, tb_mbuffer_size(buffer));
}
tb_byte_t* tb_mbuffer_memnset(tb_mbuffer_t* buffer, tb_byte_t b, tb_size_t n)
{
	return tb_mbuffer_memnsetp(buffer, 0, b, n);
}
tb_byte_t* tb_mbuffer_memnsetp(tb_mbuffer_t* buffer, tb_size_t p, tb_byte_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_mbuffer_data(buffer));

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	if (!mutx)
	{
		// refn++
		tb_mbuffer_incr(buffer);

		// mutx
		mutx = tb_mbuffer_atomic_mutx_get(buffer);
		tb_assert_and_check_return_val(mutx, TB_NULL);
	}

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// memnsetp
	tb_byte_t* d = tb_rbuffer_memnsetp(&buffer->rbuf, p, b, n);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return d;
}
/* ////////////////////////////////////////////////////////////////////////
 * memcpy
 */
tb_byte_t* tb_mbuffer_memcpy(tb_mbuffer_t* buffer, tb_mbuffer_t const* b)
{
	tb_assert_and_check_return_val(buffer && buffer != b, TB_NULL);

	// refn--
	tb_mbuffer_decr(buffer);

	// copy
	tb_memcpy(buffer, b, sizeof(tb_mbuffer_t));

	// refn++
	tb_mbuffer_incr(buffer);

	// ok
	return tb_mbuffer_data(buffer);
}
tb_byte_t* tb_mbuffer_memcpyp(tb_mbuffer_t* buffer, tb_size_t p, tb_mbuffer_t const* b)
{
	if (!p) return tb_mbuffer_memcpy(buffer, b);
	else return tb_mbuffer_memncpyp(buffer, p, tb_mbuffer_data(b), tb_mbuffer_size(b));
}
tb_byte_t* tb_mbuffer_memncpy(tb_mbuffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{
	return tb_mbuffer_memncpyp(buffer, 0, b, n);
}
tb_byte_t* tb_mbuffer_memncpyp(tb_mbuffer_t* buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && b, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_mbuffer_data(buffer));

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	if (!mutx)
	{
		// refn++
		tb_mbuffer_incr(buffer);

		// mutx
		mutx = tb_mbuffer_atomic_mutx_get(buffer);
		tb_assert_and_check_return_val(mutx, TB_NULL);
	}

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// memncpyp
	tb_byte_t* d = tb_rbuffer_memncpyp(&buffer->rbuf, p, b, n);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return d;
}
/* ////////////////////////////////////////////////////////////////////////
 * memmov
 */
tb_byte_t* tb_mbuffer_memmov(tb_mbuffer_t* buffer, tb_size_t b)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	
	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// memmov
	tb_byte_t* d = tb_rbuffer_memmov(&buffer->rbuf, b);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return d;
}
tb_byte_t* tb_mbuffer_memmovp(tb_mbuffer_t* buffer, tb_size_t p, tb_size_t b)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	
	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// memmovp
	tb_byte_t* d = tb_rbuffer_memmovp(&buffer->rbuf, p, b);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return d;
}
tb_byte_t* tb_mbuffer_memnmov(tb_mbuffer_t* buffer, tb_size_t b, tb_size_t n)
{
	return tb_mbuffer_memnmovp(buffer, 0, b, n);
}
tb_byte_t* tb_mbuffer_memnmovp(tb_mbuffer_t* buffer, tb_size_t p, tb_size_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_mbuffer_data(buffer));

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// memnmovp
	tb_byte_t* d = tb_rbuffer_memnmovp(&buffer->rbuf, p, b, n);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return d;
}

/* ////////////////////////////////////////////////////////////////////////
 * memcat
 */
tb_byte_t* tb_mbuffer_memcat(tb_mbuffer_t* buffer, tb_mbuffer_t const* b)
{
	tb_assert_and_check_return_val(b, TB_NULL);

	// copy it?
	if (!tb_mbuffer_size(buffer)) return tb_mbuffer_memcpy(buffer, b);

	// append it
	return tb_mbuffer_memncat(buffer, tb_mbuffer_data(b), tb_mbuffer_size(b));
}
tb_byte_t* tb_mbuffer_memncat(tb_mbuffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{	
	tb_assert_and_check_return_val(buffer && b, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_mbuffer_data(buffer));

	// copy it?
	if (!tb_mbuffer_size(buffer)) return tb_mbuffer_memncpy(buffer, b, n);

	// mutx
	tb_handle_t mutx = tb_mbuffer_atomic_mutx_get(buffer);
	tb_check_return_val(mutx, TB_NULL);

	// enter
	tb_check_return_val(tb_mutex_enter(mutx), TB_NULL);

	// memncat
	tb_byte_t* d = tb_rbuffer_memncat(&buffer->rbuf, b, n);

	// leave
	tb_check_return_val(tb_mutex_leave(mutx), TB_NULL);

	return d;
}

