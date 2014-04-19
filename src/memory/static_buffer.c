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
 * @file		static_buffer.c
 * @ingroup 	memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "memory.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
// the maximum grow size of value buffer 
#ifdef __tb_small__
# 	define TB_STATIC_BUFFER_GROW_SIZE 		(64)
#else
# 	define TB_STATIC_BUFFER_GROW_SIZE 		(256)
#endif


/* //////////////////////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_static_buffer_init(tb_static_buffer_t* buffer, tb_byte_t* data, tb_size_t maxn)
{
	tb_assert_and_check_return_val(buffer, tb_false);
	buffer->size = 0;
	buffer->data = data;
	buffer->maxn = maxn;
	return tb_true;
}
tb_void_t tb_static_buffer_exit(tb_static_buffer_t* buffer)
{
	if (buffer) tb_memset(buffer, 0, sizeof(tb_static_buffer_t));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_byte_t* tb_static_buffer_data(tb_static_buffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, tb_null);
	return buffer->data;
}
tb_size_t tb_static_buffer_size(tb_static_buffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);
	return buffer->size;
}
tb_size_t tb_static_buffer_maxn(tb_static_buffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);
	return buffer->maxn;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_static_buffer_clear(tb_static_buffer_t* buffer)
{
	tb_assert_and_check_return(buffer);
	buffer->size = 0;
}
tb_byte_t* tb_static_buffer_resize(tb_static_buffer_t* buffer, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && buffer->data && n <= buffer->maxn, tb_null);

	// resize
	buffer->size = n;

	// ok
	return buffer->data;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * memset
 */
tb_byte_t* tb_static_buffer_memset(tb_static_buffer_t* buffer, tb_byte_t b)
{
	return tb_static_buffer_memnsetp(buffer, 0, b, tb_static_buffer_size(buffer));
}
tb_byte_t* tb_static_buffer_memsetp(tb_static_buffer_t* buffer, tb_size_t p, tb_byte_t b)
{
	return tb_static_buffer_memnsetp(buffer, p, b, tb_static_buffer_size(buffer));
}
tb_byte_t* tb_static_buffer_memnset(tb_static_buffer_t* buffer, tb_byte_t b, tb_size_t n)
{
	return tb_static_buffer_memnsetp(buffer, 0, b, n);
}
tb_byte_t* tb_static_buffer_memnsetp(tb_static_buffer_t* buffer, tb_size_t p, tb_byte_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer, tb_null);
	
	// check
	tb_check_return_val(n, tb_static_buffer_data(buffer));

	// resize
	tb_byte_t* d = tb_static_buffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, tb_null);

	// memset
	tb_memset(d + p, b, n);

	return d;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * memcpy
 */
tb_byte_t* tb_static_buffer_memcpy(tb_static_buffer_t* buffer, tb_static_buffer_t const* b)
{
	return tb_static_buffer_memncpyp(buffer, 0, tb_static_buffer_data(b), tb_static_buffer_size(b));
}
tb_byte_t* tb_static_buffer_memcpyp(tb_static_buffer_t* buffer, tb_size_t p, tb_static_buffer_t const* b)
{
	return tb_static_buffer_memncpyp(buffer, p, tb_static_buffer_data(b), tb_static_buffer_size(b));
}
tb_byte_t* tb_static_buffer_memncpy(tb_static_buffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{
	return tb_static_buffer_memncpyp(buffer, 0, b, n);
}
tb_byte_t* tb_static_buffer_memncpyp(tb_static_buffer_t* buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && b, tb_null);
	
	// check
	tb_check_return_val(n, tb_static_buffer_data(buffer));

	// resize
	tb_byte_t* d = tb_static_buffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, tb_null);

	// memcpy
	tb_memcpy(d + p, b, n);

	return d;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * memmov
 */
tb_byte_t* tb_static_buffer_memmov(tb_static_buffer_t* buffer, tb_size_t b)
{
	tb_assert_and_check_return_val(b <= tb_static_buffer_size(buffer), tb_null);
	return tb_static_buffer_memnmovp(buffer, 0, b, tb_static_buffer_size(buffer) - b);
}
tb_byte_t* tb_static_buffer_memmovp(tb_static_buffer_t* buffer, tb_size_t p, tb_size_t b)
{
	tb_assert_and_check_return_val(b <= tb_static_buffer_size(buffer), tb_null);
	return tb_static_buffer_memnmovp(buffer, p, b, tb_static_buffer_size(buffer) - b);
}
tb_byte_t* tb_static_buffer_memnmov(tb_static_buffer_t* buffer, tb_size_t b, tb_size_t n)
{
	return tb_static_buffer_memnmovp(buffer, 0, b, n);
}
tb_byte_t* tb_static_buffer_memnmovp(tb_static_buffer_t* buffer, tb_size_t p, tb_size_t b, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(buffer && (b + n) <= tb_static_buffer_size(buffer), tb_null);

	// clear?
	if (b == tb_static_buffer_size(buffer)) 
	{
		tb_static_buffer_clear(buffer);
		return tb_static_buffer_data(buffer);
	}

	// check
	tb_check_return_val(p != b && n, tb_static_buffer_data(buffer));

	// resize
	tb_byte_t* d = tb_static_buffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, tb_null);

	// memmov
	tb_memmov(d + p, d + b, n);
	return d;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * memcat
 */
tb_byte_t* tb_static_buffer_memcat(tb_static_buffer_t* buffer, tb_static_buffer_t const* b)
{
	return tb_static_buffer_memncat(buffer, tb_static_buffer_data(b), tb_static_buffer_size(b));
}
tb_byte_t* tb_static_buffer_memncat(tb_static_buffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{	
	tb_assert_and_check_return_val(buffer && b, tb_null);
	
	// check
	tb_check_return_val(n, tb_static_buffer_data(buffer));

	// is null?
	tb_size_t p = tb_static_buffer_size(buffer);
	if (!p) return tb_static_buffer_memncpy(buffer, b, n);

	// resize
	tb_byte_t* d = tb_static_buffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, tb_null);

	// memcat
	tb_memcpy(d + p, b, n);

	return d;
}

