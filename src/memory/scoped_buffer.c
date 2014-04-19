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
 * @file		scoped_buffer.c
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
# 	define TB_SCOPED_BUFFER_GROW_SIZE 		(64)
#else
# 	define TB_SCOPED_BUFFER_GROW_SIZE 		(256)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_scoped_buffer_init(tb_scoped_buffer_t* buffer)
{
	// check
	tb_assert_and_check_return_val(buffer, tb_false);

	// init
	tb_memset(buffer, 0, sizeof(tb_scoped_buffer_t));

	// ok
	return tb_true;
}
tb_void_t tb_scoped_buffer_exit(tb_scoped_buffer_t* buffer)
{
	if (buffer)
	{
		// exit data
		if (buffer->data) tb_free(buffer->data);
		buffer->data = tb_null;

		// clear size
		buffer->size = 0;
		buffer->maxn = 0;
	}
}
tb_byte_t* tb_scoped_buffer_data(tb_scoped_buffer_t* buffer)
{
	// check
	tb_assert_and_check_return_val(buffer, tb_null);

	// the buffer data
	return buffer->data;
}
tb_size_t tb_scoped_buffer_size(tb_scoped_buffer_t const* buffer)
{ 
	// check
	tb_assert_and_check_return_val(buffer, 0);

	// the buffer size
	return buffer->size;
}
tb_size_t tb_scoped_buffer_maxn(tb_scoped_buffer_t const* buffer)
{
	// check
	tb_assert_and_check_return_val(buffer, 0);

	// the buffer maxn
	return buffer->maxn;
}
tb_void_t tb_scoped_buffer_clear(tb_scoped_buffer_t* buffer)
{
	// check
	tb_assert_and_check_return(buffer);

	// clear it
	buffer->size = 0;
}
tb_byte_t* tb_scoped_buffer_resize(tb_scoped_buffer_t* buffer, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(buffer, tb_null);

	// save it
	tb_scoped_buffer_t b = *buffer;
	
	// done
	tb_bool_t ok = tb_false;
	do
	{
		// null?
		if (!buffer->data) 
		{
			// check size
			tb_assert_and_check_break(!buffer->size && size);

			// compute size
			buffer->size = size;
			buffer->maxn = tb_align8(size + TB_SCOPED_BUFFER_GROW_SIZE);
			tb_assert_and_check_break(size < buffer->maxn);

			// alloc data
			buffer->data = tb_malloc(buffer->maxn);
			tb_assert_and_check_break(buffer->data);
		}
		// decrease
		else if (size < buffer->maxn)
		{
			buffer->size = size;
		}
		// increase
		else
		{
			// compute size
			buffer->maxn = tb_align8(size + TB_SCOPED_BUFFER_GROW_SIZE);
			tb_assert_and_check_break(size < buffer->maxn);

			// realloc
			buffer->size = size;
			buffer->data = tb_ralloc(buffer->data, buffer->maxn);
			tb_assert_and_check_break(buffer->data);
		}

		// ok 
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		// restore it
		*buffer = b;
		return tb_null;
	}

	// ok
	return (tb_byte_t*)buffer->data;
}
tb_byte_t* tb_scoped_buffer_memset(tb_scoped_buffer_t* buffer, tb_byte_t b)
{
	return tb_scoped_buffer_memnsetp(buffer, 0, b, tb_scoped_buffer_size(buffer));
}
tb_byte_t* tb_scoped_buffer_memsetp(tb_scoped_buffer_t* buffer, tb_size_t p, tb_byte_t b)
{
	return tb_scoped_buffer_memnsetp(buffer, p, b, tb_scoped_buffer_size(buffer));
}
tb_byte_t* tb_scoped_buffer_memnset(tb_scoped_buffer_t* buffer, tb_byte_t b, tb_size_t n)
{
	return tb_scoped_buffer_memnsetp(buffer, 0, b, n);
}
tb_byte_t* tb_scoped_buffer_memnsetp(tb_scoped_buffer_t* buffer, tb_size_t p, tb_byte_t b, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(buffer, tb_null);
	
	// check
	tb_check_return_val(n, tb_scoped_buffer_data(buffer));

	// resize
	tb_byte_t* d = tb_scoped_buffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, tb_null);

	// memset
	tb_memset(d + p, b, n);

	// ok?
	return d;
}
tb_byte_t* tb_scoped_buffer_memcpy(tb_scoped_buffer_t* buffer, tb_scoped_buffer_t* b)
{
	return tb_scoped_buffer_memncpyp(buffer, 0, tb_scoped_buffer_data(b), tb_scoped_buffer_size(b));
}
tb_byte_t* tb_scoped_buffer_memcpyp(tb_scoped_buffer_t* buffer, tb_size_t p, tb_scoped_buffer_t* b)
{
	return tb_scoped_buffer_memncpyp(buffer, p, tb_scoped_buffer_data(b), tb_scoped_buffer_size(b));
}
tb_byte_t* tb_scoped_buffer_memncpy(tb_scoped_buffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{
	return tb_scoped_buffer_memncpyp(buffer, 0, b, n);
}
tb_byte_t* tb_scoped_buffer_memncpyp(tb_scoped_buffer_t* buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(buffer && b, tb_null);
	
	// check
	tb_check_return_val(n, tb_scoped_buffer_data(buffer));

	// resize
	tb_byte_t* d = tb_scoped_buffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, tb_null);

	// safer than memcpy, buffer maybe overlap
	tb_memmov(d + p, b, n);

	// ok
	return d;
}
tb_byte_t* tb_scoped_buffer_memmov(tb_scoped_buffer_t* buffer, tb_size_t b)
{
	// check
	tb_assert_and_check_return_val(b <= tb_scoped_buffer_size(buffer), tb_null);
	return tb_scoped_buffer_memnmovp(buffer, 0, b, tb_scoped_buffer_size(buffer) - b);
}
tb_byte_t* tb_scoped_buffer_memmovp(tb_scoped_buffer_t* buffer, tb_size_t p, tb_size_t b)
{
	// check
	tb_assert_and_check_return_val(b <= tb_scoped_buffer_size(buffer), tb_null);
	return tb_scoped_buffer_memnmovp(buffer, p, b, tb_scoped_buffer_size(buffer) - b);
}
tb_byte_t* tb_scoped_buffer_memnmov(tb_scoped_buffer_t* buffer, tb_size_t b, tb_size_t n)
{
	return tb_scoped_buffer_memnmovp(buffer, 0, b, n);
}
tb_byte_t* tb_scoped_buffer_memnmovp(tb_scoped_buffer_t* buffer, tb_size_t p, tb_size_t b, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(buffer && (b + n) <= tb_scoped_buffer_size(buffer), tb_null);

	// clear?
	if (b == tb_scoped_buffer_size(buffer)) 
	{
		tb_scoped_buffer_clear(buffer);
		return tb_scoped_buffer_data(buffer);
	}

	// check
	tb_check_return_val(p != b && n, tb_scoped_buffer_data(buffer));

	// resize
	tb_byte_t* d = tb_scoped_buffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, tb_null);

	// memmov
	tb_memmov(d + p, d + b, n);
	return d;
}
tb_byte_t* tb_scoped_buffer_memcat(tb_scoped_buffer_t* buffer, tb_scoped_buffer_t* b)
{
	return tb_scoped_buffer_memncat(buffer, tb_scoped_buffer_data(b), tb_scoped_buffer_size(b));
}
tb_byte_t* tb_scoped_buffer_memncat(tb_scoped_buffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{	
	// check
	tb_assert_and_check_return_val(buffer && b, tb_null);
	
	// check
	tb_check_return_val(n, tb_scoped_buffer_data(buffer));

	// is null?
	tb_size_t p = tb_scoped_buffer_size(buffer);
	if (!p) return tb_scoped_buffer_memncpy(buffer, b, n);

	// resize
	tb_byte_t* d = tb_scoped_buffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, tb_null);

	// memcat
	tb_memcpy(d + p, b, n);

	return d;
}

