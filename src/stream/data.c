/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		data.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * details
 */

static tb_int_t tb_data_stream_read(tb_stream_t* st, tb_byte_t* data, tb_size_t size)
{
	tb_data_stream_t* dst = st;
	TB_ASSERT(data && size);
	if (!dst) return -1;

	// get data
	tb_byte_t const* p = tb_bits_tell(&(dst->bits));

	// skip size
	tb_size_t left_n = (tb_size_t)(tb_bits_end(&(dst->bits)) - p);
	if (size > left_n) size = left_n;
	tb_bits_skip_bytes(&(dst->bits), size);

	// return data
	memcpy(data, p, size);
	return (tb_int_t)(size);
}
static void tb_data_stream_close(tb_stream_t* st)
{
}
static tb_size_t tb_data_stream_size(tb_stream_t* st)
{
	tb_data_stream_t* dst = st;
	if (dst) return dst->size;
	else return 0;
}
/* /////////////////////////////////////////////////////////
 * interface implemention
 */
tb_stream_t* tb_stream_open_from_data(tb_data_stream_t* st, tb_byte_t const* data, tb_size_t size, tb_stream_flag_t flag)
{
	TB_ASSERT(st && data && size);
	if (st && !data || !size) return TB_NULL;

	// init stream
	memset(st, 0, sizeof(tb_data_stream_t));
	st->base.flag = flag;
	st->base.head = st->base.data;
	st->base.size = 0;
	st->base.offset = 0;

	// init data stream
	st->base.read = tb_data_stream_read;
	st->base.close = tb_data_stream_close;
	st->base.ssize= tb_data_stream_size;
	st->size = size;
	tb_bits_attach(&(st->bits), data, size);

	// is hzlib?
	if (flag & TB_STREAM_FLAG_IS_ZLIB)
	{
		st->base.hzlib = tb_zlib_create();
		if (st->base.hzlib == TB_INVALID_HANDLE) return TB_NULL;
	}

	return ((tb_stream_t*)st);
}
