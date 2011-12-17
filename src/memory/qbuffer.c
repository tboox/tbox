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
 * \file		qbuffer.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "qbuffer.h"
#include "../libc/libc.h"
#include "../utils/utils.h"


/* ////////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_qbuffer_init(tb_qbuffer_t* buffer, tb_size_t maxn)
{
	tb_assert_and_check_return_val(buffer, TB_FALSE);

	// alloc
	buffer->data = tb_malloc(maxn);
	tb_assert_and_check_return_val(buffer->data, TB_FALSE);

	// init 
	buffer->head = buffer->data;
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

/* ////////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_byte_t* tb_qbuffer_data(tb_qbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	return buffer->data;
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

/* ////////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_qbuffer_clear(tb_qbuffer_t* buffer)
{
	tb_assert_and_check_return(buffer);
	buffer->size = 0;
	buffer->head = buffer->data;
}
tb_size_t tb_qbuffer_read(tb_qbuffer_t* buffer, tb_byte_t* data, tb_size_t size)
{
	tb_trace_noimpl();
	return 0;
}
tb_size_t tb_qbuffer_writ(tb_qbuffer_t* buffer, tb_byte_t* data, tb_size_t size)
{
	tb_trace_noimpl();
	return 0;
}
