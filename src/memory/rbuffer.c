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
 * \file		rbuffer.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "rbuffer.h"
#include "../libc/libc.h"
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * init & exit
 */
tb_bool_t tb_rbuffer_init(tb_rbuffer_t* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_FALSE);
	tb_memset(buffer, 0, sizeof(tb_rbuffer_t));
	return TB_TRUE;
}
tb_void_t tb_rbuffer_exit(tb_rbuffer_t* buffer)
{
	if (buffer)
	{
		// refn--
		tb_rbuffer_decr(buffer);

		// clear
		tb_memset(buffer, 0, sizeof(tb_rbuffer_t));
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * accessors
 */
tb_byte_t* tb_rbuffer_data(tb_rbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	
	// data
	tb_rbuffer_data_t* data = buffer->data? *(buffer->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// cstr
		return tb_pbuffer_data(&data->pbuf);
	}

	return TB_NULL;
}
tb_size_t tb_rbuffer_size(tb_rbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);
	
	// data
	tb_rbuffer_data_t* data = buffer->data? *(buffer->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// size
		return tb_pbuffer_size(&data->pbuf);
	}

	return 0;
}
tb_size_t tb_rbuffer_maxn(tb_rbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);
	
	// data
	tb_rbuffer_data_t* data = buffer->data? *(buffer->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// maxn
		return tb_pbuffer_maxn(&data->pbuf);
	}

	return 0;
}
tb_size_t tb_rbuffer_refn(tb_rbuffer_t const* buffer)
{
	tb_assert_and_check_return_val(buffer, 0);
	
	// data
	tb_rbuffer_data_t* data = buffer->data? *(buffer->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// refn
		return data->refn;
	}

	return 0;
}

/* ///////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_rbuffer_clear(tb_rbuffer_t* buffer)
{
	tb_assert_and_check_return(buffer);

	// data
	tb_rbuffer_data_t* data = buffer->data? *(buffer->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// clear
		tb_pbuffer_clear(&data->pbuf);
	}
}
tb_byte_t* tb_rbuffer_resize(tb_rbuffer_t* buffer, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);

	// no data? refn++
	if (!buffer->data) tb_rbuffer_incr(buffer);
	tb_assert_and_check_return_val(buffer->data, TB_NULL);

	// data
	tb_rbuffer_data_t* data = *(buffer->data);
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// resize
		return tb_pbuffer_resize(&data->pbuf, n);
	}

	return TB_NULL;
}
tb_size_t tb_rbuffer_incr(tb_rbuffer_t* buffer)
{	
	tb_assert_and_check_return_val(buffer, 0);

	// init
	tb_size_t r = 0;

	// data
	tb_rbuffer_data_t* data = TB_NULL;

	// init
	if (!buffer->data)
	{
		// alloc the shared data pointer
		buffer->data = tb_nalloc0(1, sizeof(tb_rbuffer_data_t*));
		tb_assert_and_check_goto(buffer->data, fail);

		// alloc the shared data
		data = tb_nalloc0(1, sizeof(tb_rbuffer_data_t));
		tb_assert_and_check_goto(data, fail);
	
		// init the shared pointer
		*(buffer->data) = data;

		// init refn
		r = data->refn = 1;

		// init pbuffer
		if (!tb_pbuffer_init(&data->pbuf)) goto fail;
	}
	else
	{
		// data
		data = *(buffer->data);
		if (data)
		{
			// check 
			tb_assert(data->refn);

			// refn++
			r = ++data->refn;
		}
	}

	return r;

fail:
	// free data
	if (data) tb_free(data);

	// free data pointer
	if (buffer->data) tb_free(buffer->data);

	// clear
	tb_memset(buffer, 0, sizeof(tb_rbuffer_t));

	return 0;
}
tb_size_t tb_rbuffer_decr(tb_rbuffer_t* buffer)
{	
	tb_assert_and_check_return_val(buffer, 0);

	// init
	tb_size_t r = 0;

	// data
	tb_rbuffer_data_t* data = buffer->data? *(buffer->data) : TB_NULL;
	if (data)
	{
		// check 
		tb_assert(data->refn);

		// refn--
		r = --data->refn;

		// free it?
		if (!r)
		{
			// exit pbuffer
			tb_pbuffer_exit(&data->pbuf);

			// free data
			tb_free(data);

			// reset pointer
			*buffer->data = TB_NULL;
		}
	}

	return r;
}
/* ///////////////////////////////////////////////////////////////////////
 * memset
 */
tb_byte_t* tb_rbuffer_memset(tb_rbuffer_t* buffer, tb_byte_t b)
{
	return tb_rbuffer_memnsetp(buffer, 0, b, tb_rbuffer_size(buffer));
}
tb_byte_t* tb_rbuffer_memsetp(tb_rbuffer_t* buffer, tb_size_t p, tb_byte_t b)
{
	return tb_rbuffer_memnsetp(buffer, p, b, tb_rbuffer_size(buffer));
}
tb_byte_t* tb_rbuffer_memnset(tb_rbuffer_t* buffer, tb_byte_t b, tb_size_t n)
{
	return tb_rbuffer_memnsetp(buffer, 0, b, n);
}
tb_byte_t* tb_rbuffer_memnsetp(tb_rbuffer_t* buffer, tb_size_t p, tb_byte_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_rbuffer_data(buffer));

	// resize
	tb_byte_t* d = tb_rbuffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, TB_NULL);

	// memset
	tb_memset(d + p, b, n);

	return d;
}
/* ///////////////////////////////////////////////////////////////////////
 * memcpy
 */
tb_byte_t* tb_rbuffer_memcpy(tb_rbuffer_t* buffer, tb_rbuffer_t const* b)
{
	tb_assert_and_check_return_val(buffer && buffer != b, TB_NULL);

	// refn--
	tb_rbuffer_decr(buffer);

	// copy
	tb_memcpy(buffer, b, sizeof(tb_rbuffer_t));

	// refn++
	tb_rbuffer_incr(buffer);

	// ok
	return tb_rbuffer_data(buffer);
}
tb_byte_t* tb_rbuffer_memcpyp(tb_rbuffer_t* buffer, tb_size_t p, tb_rbuffer_t const* b)
{
	if (!p) return tb_rbuffer_memcpy(buffer, b);
	else return tb_rbuffer_memncpyp(buffer, p, tb_rbuffer_data(b), tb_rbuffer_size(b));
}
tb_byte_t* tb_rbuffer_memncpy(tb_rbuffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{
	return tb_rbuffer_memncpyp(buffer, 0, b, n);
}
tb_byte_t* tb_rbuffer_memncpyp(tb_rbuffer_t* buffer, tb_size_t p, tb_byte_t const* b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && b, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_rbuffer_data(buffer));

	// resize
	tb_byte_t* d = tb_rbuffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, TB_NULL);

	// memcpy
	tb_memcpy(d + p, b, n);

	return d;
}
/* ///////////////////////////////////////////////////////////////////////
 * memmov
 */
tb_byte_t* tb_rbuffer_memmov(tb_rbuffer_t* buffer, tb_size_t b)
{
	tb_assert_and_check_return_val(b < tb_rbuffer_size(buffer), TB_NULL);
	return tb_rbuffer_memnmovp(buffer, 0, b, tb_rbuffer_size(buffer) - b);
}
tb_byte_t* tb_rbuffer_memmovp(tb_rbuffer_t* buffer, tb_size_t p, tb_size_t b)
{
	tb_assert_and_check_return_val(b < tb_rbuffer_size(buffer), TB_NULL);
	return tb_rbuffer_memnmovp(buffer, p, b, tb_rbuffer_size(buffer) - b);
}
tb_byte_t* tb_rbuffer_memnmov(tb_rbuffer_t* buffer, tb_size_t b, tb_size_t n)
{
	return tb_rbuffer_memnmovp(buffer, 0, b, n);
}
tb_byte_t* tb_rbuffer_memnmovp(tb_rbuffer_t* buffer, tb_size_t p, tb_size_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(buffer && (b + n) <= tb_rbuffer_size(buffer), TB_NULL);

	// check
	tb_check_return_val(p != b && n, tb_rbuffer_data(buffer));

	// resize
	tb_byte_t* d = tb_rbuffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, TB_NULL);

	// memmov
	tb_memmov(d + p, d + b, n);

	return d;
}

/* ///////////////////////////////////////////////////////////////////////
 * memcat
 */
tb_byte_t* tb_rbuffer_memcat(tb_rbuffer_t* buffer, tb_rbuffer_t const* b)
{
	// copy it?
	if (!tb_rbuffer_size(buffer)) return tb_rbuffer_memcpy(buffer, b);

	// append it
	return tb_rbuffer_memncat(buffer, tb_rbuffer_data(b), tb_rbuffer_size(b));
}
tb_byte_t* tb_rbuffer_memncat(tb_rbuffer_t* buffer, tb_byte_t const* b, tb_size_t n)
{	
	tb_assert_and_check_return_val(buffer && b, TB_NULL);
	
	// check
	tb_check_return_val(n, tb_rbuffer_data(buffer));

	// is null?
	tb_size_t p = tb_rbuffer_size(buffer);
	if (!p) return tb_rbuffer_memncpy(buffer, b, n);

	// resize
	tb_byte_t* d = tb_rbuffer_resize(buffer, p + n);
	tb_assert_and_check_return_val(d, TB_NULL);

	// memcat
	tb_memcpy(d + p, b, n);

	return d;
}

