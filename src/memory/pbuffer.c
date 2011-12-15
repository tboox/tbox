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
 * \file		pbuffer.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pbuffer.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
// the maximum grow size of value buffer 
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_PBUFFER_GROW_SIZE 		(64)
# 	define TB_PBUFFER_FMTD_SIZE 		(4096)
#else
# 	define TB_PBUFFER_GROW_SIZE 		(256)
# 	define TB_PBUFFER_FMTD_SIZE 		(8192)
#endif


/* ////////////////////////////////////////////////////////////////////////
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

/* ////////////////////////////////////////////////////////////////////////
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

/* ////////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_pbuffer_clear(tb_pbuffer_t* buffer)
{
	tb_assert_and_check_return(buffer);
	buffer->size = 0;
}
tb_void_t tb_pbuffer_clear0(tb_pbuffer_t* buffer)
{
	tb_assert_and_check_return(buffer);
	buffer->size = 0;
	if (buffer->maxn && buffer->data) tb_memset(buffer->data, 0, buffer->maxn);
}
tb_byte_t* tb_pbuffer_resize(tb_pbuffer_t* buffer, tb_size_t size)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);

	// save it
	tb_pbuffer_t b = *buffer;
	
	// null?
	if (!buffer->data) 
	{
		// check size
		tb_assert(!buffer->size && size);

		// compute size
		buffer->size = size;
		buffer->maxn = tb_align8(size + TB_PBUFFER_GROW_SIZE);
		tb_assert_and_check_goto(size < buffer->maxn, fail);

		// alloc data
		buffer->data = tb_malloc(buffer->maxn);
		tb_assert_and_check_goto(buffer->data, fail);
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
		buffer->maxn = tb_align8(size + TB_PBUFFER_GROW_SIZE);
		tb_assert_and_check_goto(size < buffer->maxn, fail);

		// realloc
		buffer->size = size;
		buffer->data = tb_realloc(buffer->data, buffer->maxn);
		tb_assert_and_check_goto(buffer->data, fail);
	}

	// ok
	return buffer->data;

fail:

	// restore it
	*buffer = b;

	// failed
	tb_trace("failed to resize buffer: %x", b.data? b.data : "");
	return TB_NULL;
}

/* ////////////////////////////////////////////////////////////////////////
 * memset
 */
tb_byte_t* tb_pbuffer_bmemset(tb_pbuffer_t* buffer, tb_byte_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && n, TB_NULL);

	// resize
	if (!tb_pbuffer_resize(buffer, n)) return TB_NULL;

	// check
	tb_assert(buffer->data && buffer->size == n);

	// set data
	tb_memset(buffer->data, b, n);

	// ok
	return buffer->data;
}
/* ////////////////////////////////////////////////////////////////////////
 * memcpy
 */
tb_byte_t* tb_pbuffer_memcpy(tb_pbuffer_t* buffer, tb_pbuffer_t const* b)
{
	tb_assert_and_check_return_val(b, TB_NULL);
	return tb_pbuffer_pmemcpy(buffer, tb_pbuffer_data(b), tb_pbuffer_size(b));
}
tb_byte_t* tb_pbuffer_pmemcpy(tb_pbuffer_t* buffer, tb_byte_t const* p, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && p && n, TB_NULL);

	// resize
	if (!tb_pbuffer_resize(buffer, n)) return TB_NULL;

	// check
	tb_assert(buffer->data && buffer->size == n);

	// copy data
	tb_memcpy(buffer->data, p, n);

	// ok
	return buffer->data;
}

/* ////////////////////////////////////////////////////////////////////////
 * memcat
 */
tb_byte_t* tb_pbuffer_memcat(tb_pbuffer_t* buffer, tb_pbuffer_t const* b)
{
	tb_assert_and_check_return_val(b, TB_NULL);
	return tb_pbuffer_pmemcat(buffer, tb_pbuffer_data(b), tb_pbuffer_size(b));
}
tb_byte_t* tb_pbuffer_pmemcat(tb_pbuffer_t* buffer, tb_byte_t const* p, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && p && n, TB_NULL);

	// copy it if null
	if (!buffer->size) return tb_pbuffer_pmemcpy(buffer, p, n);

	// get old size
	tb_size_t on = buffer->size;

	// resize
	if (!tb_pbuffer_resize(buffer, on + n)) return TB_NULL;

	// check
	tb_assert(buffer->data && buffer->size == on + n);

	// copy data
	tb_memcpy(buffer->data + on, p, n);

	// ok
	return buffer->data;
}
tb_byte_t* tb_pbuffer_bmemcat(tb_pbuffer_t* buffer, tb_byte_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && n, TB_NULL);

	// copy it if null
	if (!buffer->size) return tb_pbuffer_bmemset(buffer, b, n);

	// get old size
	tb_size_t on = buffer->size;

	// resize
	if (!tb_pbuffer_resize(buffer, on + n)) return TB_NULL;

	// check
	tb_assert(buffer->data && buffer->size == on + n);

	// set data
	tb_memset(buffer->data + on, b, n);

	// ok
	return buffer->data;
}
