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
 * \file		qbuffer.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "qbuffer.h"
#include "../libc/libc.h"
#include "../utils/utils.h"


/* ///////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_qbuffer_init(tb_qbuffer_t* buffer, tb_size_t maxn)
{
	tb_assert_and_check_return_val(buffer, TB_FALSE);

	// init 
	buffer->data = TB_NULL;
	buffer->head = TB_NULL;
	buffer->size = 0;
	buffer->maxn = maxn;

	return TB_TRUE;
}
tb_void_t tb_qbuffer_exit(tb_qbuffer_t* buffer)
{
	if (buffer)
	{
		if (buffer->data) tb_free(buffer->data);
		tb_memset(buffer, 0, sizeof(tb_qbuffer_t));
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_byte_t* tb_qbuffer_data(tb_qbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	return buffer->data;
}
tb_byte_t* tb_qbuffer_head(tb_qbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	return buffer->head;
}
tb_byte_t* tb_qbuffer_tail(tb_qbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	return buffer->head? buffer->head + buffer->size : TB_NULL;
}
tb_size_t tb_qbuffer_size(tb_qbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);
	return buffer->size;
}
tb_size_t tb_qbuffer_maxn(tb_qbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);
	return buffer->maxn;
}
tb_size_t tb_qbuffer_left(tb_qbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer && buffer->size <= buffer->maxn, 0);
	return buffer->maxn - buffer->size;
}
tb_bool_t tb_qbuffer_full(tb_qbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_FALSE);
	return buffer->size == buffer->maxn? TB_TRUE : TB_FALSE;
}
tb_bool_t tb_qbuffer_null(tb_qbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_FALSE);
	return buffer->size? TB_FALSE : TB_TRUE;
}

/* ///////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_qbuffer_clear(tb_qbuffer_t* buffer)
{
	tb_assert_and_check_return(buffer);
	buffer->size = 0;
	buffer->head = buffer->data;
}
tb_byte_t* tb_qbuffer_resize(tb_qbuffer_t* buffer, tb_size_t maxn)
{
	tb_assert_and_check_return_val(buffer && maxn && maxn >= buffer->size, TB_NULL);

	// has data?
	if (buffer->data)
	{
		// move data to head
		if (buffer->head != buffer->data)
		{
			if (buffer->size) tb_memmov(buffer->data, buffer->head, buffer->size);
			buffer->head = buffer->data;
		}

		// realloc
		if (maxn > buffer->maxn)
		{
			buffer->head = TB_NULL;
			buffer->data = tb_realloc(buffer->data, maxn);
			tb_assert_and_check_return_val(buffer->data, TB_NULL);
			buffer->head = buffer->data;
		}
	}

	// update maxn
	buffer->maxn = maxn;

	// ok
	return buffer->data;
}

/* ///////////////////////////////////////////////////////////////////////
 * read & writ
 */
tb_long_t tb_qbuffer_read(tb_qbuffer_t* buffer, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(buffer && data, -1);

	// no data?
	tb_check_return_val(buffer->data && buffer->size && size, 0);
	tb_assert_and_check_return_val(buffer->head, -1);

	// read data
	tb_long_t read = buffer->size > size? size : buffer->size;
	tb_memcpy(data, buffer->head, read);
	buffer->head += read;
	buffer->size -= read;

	// null? reset head
	if (!buffer->size) buffer->head = buffer->data;

	// ok
	return read;
}
tb_long_t tb_qbuffer_writ(tb_qbuffer_t* buffer, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(buffer && data && buffer->maxn, -1);

	// no data?
	if (!buffer->data)
	{
		// alloc
		buffer->data = tb_malloc(buffer->maxn);

		// init 
		buffer->head = buffer->data;
		buffer->size = 0;
	}
	tb_assert_and_check_return_val(buffer->data && buffer->head, -1);

	// no left?
	tb_size_t left = buffer->maxn - buffer->size;
	tb_check_return_val(left, 0);

	// move data to head
	if (buffer->head != buffer->data)
	{
		if (buffer->size) tb_memmov(buffer->data, buffer->head, buffer->size);
		buffer->head = buffer->data;
	}

	// writ data
	tb_size_t writ = left > size? size : left;
	tb_memcpy(buffer->data + buffer->size, data, writ);
	buffer->size += writ;

	// ok
	return writ;
}

/* ///////////////////////////////////////////////////////////////////////
 * pull & push
 */

tb_byte_t* tb_qbuffer_pull_init(tb_qbuffer_t* buffer, tb_size_t* size)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);

	// no data?
	tb_check_return_val(buffer->data && buffer->size, TB_NULL);
	tb_assert_and_check_return_val(buffer->head, TB_NULL);

	// ok
	if (size) *size = buffer->size;
	return buffer->head;
}
tb_void_t tb_qbuffer_pull_done(tb_qbuffer_t* buffer, tb_size_t size)
{
	tb_assert_and_check_return(buffer && buffer->head && size <= buffer->size);

	// update
	buffer->size -= size;
	buffer->head += size;

	// null? reset head
	if (!buffer->size) buffer->head = buffer->data;
}
tb_byte_t* tb_qbuffer_push_init(tb_qbuffer_t* buffer, tb_size_t* size)
{
	tb_assert_and_check_return_val(buffer && buffer->maxn, TB_NULL);

	// no data?
	if (!buffer->data)
	{
		// alloc
		buffer->data = tb_malloc(buffer->maxn);

		// init 
		buffer->head = buffer->data;
		buffer->size = 0;
	}
	tb_assert_and_check_return_val(buffer->data && buffer->head, TB_NULL);

	// no left?
	tb_size_t left = buffer->maxn - buffer->size;
	tb_check_return_val(left, TB_NULL);

	// move data to head
	if (buffer->head != buffer->data)
	{
		if (buffer->size) tb_memmov(buffer->data, buffer->head, buffer->size);
		buffer->head = buffer->data;
	}

	// ok
	if (size) *size = left;
	return buffer->head + buffer->size;
}
tb_void_t tb_qbuffer_push_done(tb_qbuffer_t* buffer, tb_size_t size)
{
	tb_assert_and_check_return(buffer && buffer->head && buffer->size + size <= buffer->maxn);
	buffer->size += size;
}

