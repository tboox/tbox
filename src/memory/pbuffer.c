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
 * \file		pbuffer.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pbuffer.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
// the maximum grow size of value buffer 
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_PBUFFER_GROW_SIZE 		(64)
#else
# 	define TB_PBUFFER_GROW_SIZE 		(256)
#endif


/* ///////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_pbuffer_init(tb_pbuffer_t* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_FALSE);
	tb_memset(buffer, 0, sizeof(tb_pbuffer_t));
	return TB_TRUE;
}
tb_void_t tb_pbuffer_exit(tb_pbuffer_t* buffer)
{
	if (buffer)
	{
		if (buffer->data) tb_free(buffer->data);
		tb_memset(buffer, 0, sizeof(tb_pbuffer_t));
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_byte_t* tb_pbuffer_data(tb_pbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	return buffer->data;
}
tb_size_t tb_pbuffer_size(tb_pbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);
	return buffer->size;
}
tb_size_t tb_pbuffer_maxn(tb_pbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);
	return buffer->maxn;
}

/* ///////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_pbuffer_clear(tb_pbuffer_t* buffer)
{
	tb_assert_and_check_return(buffer);
	buffer->size = 0;
}
tb_byte_t* tb_pbuffer_resize(tb_pbuffer_t* buffer, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);

	// save it
	tb_pbuffer_t b = *buffer;
	
	// null?
	if (!buffer->data) 
	{
		// check size
		tb_assert(!buffer->size && n);

		// compute size
		buffer->size = n;
		buffer->maxn = tb_align8(n + TB_PBUFFER_GROW_SIZE);
		tb_assert_and_check_goto(n < buffer->maxn, fail);

		// alloc data
		buffer->data = tb_malloc(buffer->maxn);
		tb_assert_and_check_goto(buffer->data, fail);
	}
	// decrease
	else if (n < buffer->maxn)
	{
		buffer->size = n;
	}
	// increase
	else
	{
		// compute size
		buffer->maxn = tb_align8(n + TB_PBUFFER_GROW_SIZE);
		tb_assert_and_check_goto(n < buffer->maxn, fail);

		// realloc
		buffer->size = n;
		buffer->data = tb_ralloc(buffer->data, buffer->maxn);
		tb_assert_and_check_goto(buffer->data, fail);
	}

	// ok
	return buffer->data;

fail:

	// restore it
	*buffer = b;

	return TB_NULL;
}

/* ///////////////////////////////////////////////////////////////////////
 * memset
 */
tb_byte_t* tb_pbuffer_memset(tb_pbuffer_t* buffer, tb_byte_t b)
{
	return tb_pbuffer_memnsetp(buffer, 0, b, tb_pbuffer_size(buffer));
}
tb_byte_t* tb_pbuffer_memsetp(tb_pbuffer_t* buffer, tb_size_t p, tb_byte_t b)
{
	return tb_pbuffer_memnsetp(buffer, p, b, tb_pbuffer_size(buffer));
}
tb_byte_t* tb_pbuffer_memnset(tb_pbuffer_t* buffer, tb_byte_t b, tb_size_t n)
{
	return tb_pbuffer_memnsetp(buffer, 0, b, n);
}
tb_byte_t* tb_pbuffer_memnsetp(tb_pbuffer_t* buffer, tb_size_t p, tb_byte_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_pbuffer_data(buffer));

	// resize
	tb_byte_t* d = tb_pbuffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, TB_NULL);

	// memset
	tb_memset(d + p, b, n);

	return d;
}
/* ///////////////////////////////////////////////////////////////////////
 * memcpy
 */
tb_byte_t* tb_pbuffer_memcpy(tb_pbuffer_t* buffer, tb_pbuffer_t const* b)
{
	return tb_pbuffer_memncpyp(buffer, 0, tb_pbuffer_data(b), tb_pbuffer_size(b));
}
tb_byte_t* tb_pbuffer_memcpyp(tb_pbuffer_t* buffer, tb_size_t p, tb_pbuffer_t const* b)
{
	return tb_pbuffer_memncpyp(buffer, p, tb_pbuffer_data(b), tb_pbuffer_size(b));
}
tb_byte_t* tb_pbuffer_memncpy(tb_pbuffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{
	return tb_pbuffer_memncpyp(buffer, 0, b, n);
}
tb_byte_t* tb_pbuffer_memncpyp(tb_pbuffer_t* buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && b, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_pbuffer_data(buffer));

	// resize
	tb_byte_t* d = tb_pbuffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, TB_NULL);

	// memcpy
	tb_memcpy(d + p, b, n);

	return d;
}
/* ///////////////////////////////////////////////////////////////////////
 * memmov
 */
tb_byte_t* tb_pbuffer_memmov(tb_pbuffer_t* buffer, tb_size_t b)
{
	tb_assert_and_check_return_val(b < tb_pbuffer_size(buffer), TB_NULL);
	return tb_pbuffer_memnmovp(buffer, 0, b, tb_pbuffer_size(buffer) - b);
}
tb_byte_t* tb_pbuffer_memmovp(tb_pbuffer_t* buffer, tb_size_t p, tb_size_t b)
{
	tb_assert_and_check_return_val(b < tb_pbuffer_size(buffer), TB_NULL);
	return tb_pbuffer_memnmovp(buffer, p, b, tb_pbuffer_size(buffer) - b);
}
tb_byte_t* tb_pbuffer_memnmov(tb_pbuffer_t* buffer, tb_size_t b, tb_size_t n)
{
	return tb_pbuffer_memnmovp(buffer, 0, b, n);
}
tb_byte_t* tb_pbuffer_memnmovp(tb_pbuffer_t* buffer, tb_size_t p, tb_size_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && (b + n) <= tb_pbuffer_size(buffer), TB_NULL);

	// check
	tb_check_return_val(p != b && n, tb_pbuffer_data(buffer));

	// resize
	tb_byte_t* d = tb_pbuffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, TB_NULL);

	// memmov
	tb_memmov(d + p, d + b, n);

	return d;
}

/* ///////////////////////////////////////////////////////////////////////
 * memcat
 */
tb_byte_t* tb_pbuffer_memcat(tb_pbuffer_t* buffer, tb_pbuffer_t const* b)
{
	return tb_pbuffer_memncat(buffer, tb_pbuffer_data(b), tb_pbuffer_size(b));
}
tb_byte_t* tb_pbuffer_memncat(tb_pbuffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{	
	tb_assert_and_check_return_val(buffer && b, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_pbuffer_data(buffer));

	// is null?
	tb_size_t p = tb_pbuffer_size(buffer);
	if (!p) return tb_pbuffer_memncpy(buffer, b, n);

	// resize
	tb_byte_t* d = tb_pbuffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, TB_NULL);

	// memcat
	tb_memcpy(d + p, b, n);

	return d;
}

