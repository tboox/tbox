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
 * along with TBox; 
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

static tb_int_t tb_dstream_read(tb_gstream_t* st, tb_byte_t* data, tb_size_t size)
{
	tb_dstream_t* dst = (tb_dstream_t*)st;
	TB_ASSERT(data && size);
	if (dst && data)
	{
		// adjust size
		tb_size_t left = dst->data + dst->size - dst->head;
		if (size > left) size = left;

		// read data
		memcpy(data, dst->head, size);
		dst->head += size;
		return (tb_int_t)(size);
	}
	else return -1;
}
static tb_int_t tb_dstream_write(tb_gstream_t* st, tb_byte_t* data, tb_size_t size)
{
	tb_dstream_t* dst = (tb_dstream_t*)st;
	TB_ASSERT(data && size);
	if (dst && data)
	{
		// adjust size
		tb_size_t left = dst->data + dst->size - dst->head;
		if (size > left) size = left;

		// write data
		memcpy(dst->head, data, size);
		dst->head += size;
		return (tb_int_t)(size);
	}
	else return -1;
}
static void tb_dstream_close(tb_gstream_t* st)
{
}
static tb_size_t tb_dstream_size(tb_gstream_t* st)
{
	tb_dstream_t* dst = st;
	if (dst && !(st->flag & TB_GSTREAM_FLAG_ZLIB)) return dst->size;
	else return 0;
}
static tb_byte_t* tb_dstream_need(tb_gstream_t* st, tb_size_t size)
{
	tb_dstream_t* dst = st;
	if (dst && !(st->flag & TB_GSTREAM_FLAG_ZLIB))
	{
		// is out?
		TB_ASSERT(dst->head + size <= dst->data + dst->size);
		if (dst->head + size > dst->data + dst->size) return TB_NULL;

		return dst->head;
	}
	else return TB_NULL;
}
static tb_bool_t tb_dstream_seek(tb_gstream_t* st, tb_int_t offset, tb_gstream_seek_t flag)
{
	tb_dstream_t* dst = st;
	if (dst && !(st->flag & TB_GSTREAM_FLAG_ZLIB))
	{
		// seek
		if (flag == TB_GSTREAM_SEEK_BEG) dst->head = dst->data + offset;
		else if (flag == TB_GSTREAM_SEEK_CUR) dst->head += offset;
		else if (flag == TB_GSTREAM_SEEK_END) dst->head = dst->data + dst->size - offset;

		// is out?
		if (dst->head < dst->data) dst->head = dst->data;
		else if (dst->head > dst->data + dst->size) dst->head = dst->data + dst->size;

		// update offset
		st->offset = dst->head - dst->data;

		return TB_TRUE;
	}
	else return TB_FALSE;
}
/* /////////////////////////////////////////////////////////
 * interface implemention
 */
tb_gstream_t* tb_gstream_open_from_data(tb_dstream_t* st, tb_byte_t const* data, tb_size_t size, tb_gstream_flag_t flag)
{
	TB_ASSERT(st && data && size);
	if (st && !data || !size) return TB_NULL;

	// init stream
	memset(st, 0, sizeof(tb_dstream_t));
	st->base.flag = flag;
	st->base.head = st->base.data;
	st->base.size = 0;
	st->base.offset = 0;

	// init data stream
	st->base.read = tb_dstream_read;
	st->base.write = tb_dstream_write;
	st->base.close = tb_dstream_close;
	st->base.ssize= tb_dstream_size;
	st->base.need = tb_dstream_need;
	st->base.seek = tb_dstream_seek;
	st->data = data;
	st->head = data;
	st->size = size;

	// init url
	tb_string_init(&st->base.url);

#ifdef TB_CONFIG_ZLIB
	// is hzlib?
	if (flag & TB_GSTREAM_FLAG_ZLIB)
	{
		st->base.hzlib = tb_zlib_create();
		if (st->base.hzlib == TB_INVALID_HANDLE) return TB_NULL;
	}
#endif
	return ((tb_gstream_t*)st);
}
