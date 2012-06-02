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
 * @file		sbuffer.c
 * @ingroup 	memory
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sbuffer.h"
#include "malloc.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
// the maximum grow size of value buffer 
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_SBUFFER_GROW_SIZE 		(64)
#else
# 	define TB_SBUFFER_GROW_SIZE 		(256)
#endif


/* ///////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_sbuffer_init(tb_sbuffer_t* buffer, tb_byte_t* data, tb_size_t maxn)
{
	tb_assert_and_check_return_val(buffer, TB_FALSE);
	buffer->size = 0;
	buffer->data = data;
	buffer->maxn = maxn;
	return TB_TRUE;
}
tb_void_t tb_sbuffer_exit(tb_sbuffer_t* buffer)
{
	if (buffer) tb_memset(buffer, 0, sizeof(tb_sbuffer_t));
}

/* ///////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_byte_t* tb_sbuffer_data(tb_sbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	return buffer->data;
}
tb_size_t tb_sbuffer_size(tb_sbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);
	return buffer->size;
}
tb_size_t tb_sbuffer_maxn(tb_sbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);
	return buffer->maxn;
}

/* ///////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_sbuffer_clear(tb_sbuffer_t* buffer)
{
	tb_assert_and_check_return(buffer);
	buffer->size = 0;
}
tb_byte_t* tb_sbuffer_resize(tb_sbuffer_t* buffer, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && buffer->data && n <= buffer->maxn, TB_NULL);

	// resize
	buffer->size = n;

	// ok
	return buffer->data;
}

/* ///////////////////////////////////////////////////////////////////////
 * memset
 */
tb_byte_t* tb_sbuffer_memset(tb_sbuffer_t* buffer, tb_byte_t b)
{
	return tb_sbuffer_memnsetp(buffer, 0, b, tb_sbuffer_size(buffer));
}
tb_byte_t* tb_sbuffer_memsetp(tb_sbuffer_t* buffer, tb_size_t p, tb_byte_t b)
{
	return tb_sbuffer_memnsetp(buffer, p, b, tb_sbuffer_size(buffer));
}
tb_byte_t* tb_sbuffer_memnset(tb_sbuffer_t* buffer, tb_byte_t b, tb_size_t n)
{
	return tb_sbuffer_memnsetp(buffer, 0, b, n);
}
tb_byte_t* tb_sbuffer_memnsetp(tb_sbuffer_t* buffer, tb_size_t p, tb_byte_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_sbuffer_data(buffer));

	// resize
	tb_byte_t* d = tb_sbuffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, TB_NULL);

	// memset
	tb_memset(d + p, b, n);

	return d;
}
/* ///////////////////////////////////////////////////////////////////////
 * memcpy
 */
tb_byte_t* tb_sbuffer_memcpy(tb_sbuffer_t* buffer, tb_sbuffer_t const* b)
{
	return tb_sbuffer_memncpyp(buffer, 0, tb_sbuffer_data(b), tb_sbuffer_size(b));
}
tb_byte_t* tb_sbuffer_memcpyp(tb_sbuffer_t* buffer, tb_size_t p, tb_sbuffer_t const* b)
{
	return tb_sbuffer_memncpyp(buffer, p, tb_sbuffer_data(b), tb_sbuffer_size(b));
}
tb_byte_t* tb_sbuffer_memncpy(tb_sbuffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{
	return tb_sbuffer_memncpyp(buffer, 0, b, n);
}
tb_byte_t* tb_sbuffer_memncpyp(tb_sbuffer_t* buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && b, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_sbuffer_data(buffer));

	// resize
	tb_byte_t* d = tb_sbuffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, TB_NULL);

	// memcpy
	tb_memcpy(d + p, b, n);

	return d;
}
/* ///////////////////////////////////////////////////////////////////////
 * memmov
 */
tb_byte_t* tb_sbuffer_memmov(tb_sbuffer_t* buffer, tb_size_t b)
{
	tb_assert_and_check_return_val(b < tb_sbuffer_size(buffer), TB_NULL);
	return tb_sbuffer_memnmovp(buffer, 0, b, tb_sbuffer_size(buffer) - b);
}
tb_byte_t* tb_sbuffer_memmovp(tb_sbuffer_t* buffer, tb_size_t p, tb_size_t b)
{
	tb_assert_and_check_return_val(b < tb_sbuffer_size(buffer), TB_NULL);
	return tb_sbuffer_memnmovp(buffer, p, b, tb_sbuffer_size(buffer) - b);
}
tb_byte_t* tb_sbuffer_memnmov(tb_sbuffer_t* buffer, tb_size_t b, tb_size_t n)
{
	return tb_sbuffer_memnmovp(buffer, 0, b, n);
}
tb_byte_t* tb_sbuffer_memnmovp(tb_sbuffer_t* buffer, tb_size_t p, tb_size_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && (b + n) <= tb_sbuffer_size(buffer), TB_NULL);

	// check
	tb_check_return_val(p != b && n, tb_sbuffer_data(buffer));

	// resize
	tb_byte_t* d = tb_sbuffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, TB_NULL);

	// memmov
	tb_memmov(d + p, d + b, n);

	return d;
}

/* ///////////////////////////////////////////////////////////////////////
 * memcat
 */
tb_byte_t* tb_sbuffer_memcat(tb_sbuffer_t* buffer, tb_sbuffer_t const* b)
{
	return tb_sbuffer_memncat(buffer, tb_sbuffer_data(b), tb_sbuffer_size(b));
}
tb_byte_t* tb_sbuffer_memncat(tb_sbuffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{	
	tb_assert_and_check_return_val(buffer && b, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_sbuffer_data(buffer));

	// is null?
	tb_size_t p = tb_sbuffer_size(buffer);
	if (!p) return tb_sbuffer_memncpy(buffer, b, n);

	// resize
	tb_byte_t* d = tb_sbuffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, TB_NULL);

	// memcat
	tb_memcpy(d + p, b, n);

	return d;
}

