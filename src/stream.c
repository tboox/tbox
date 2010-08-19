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
 * \file		stream.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "stream.h"
#include "math.h"
/* /////////////////////////////////////////////////////////
 * macros
 */

/* /////////////////////////////////////////////////////////
 * decls
 */

tb_stream_t* tb_stream_open_http(tb_char_t const* url);
tb_stream_t* tb_stream_open_file(tb_char_t const* url);
tb_stream_t* tb_stream_open_data(tb_byte_t const* data, tb_size_t size);

/* /////////////////////////////////////////////////////////
 * details
 */

static tb_int_t tb_stream_read_no_block(tb_stream_t* st, tb_byte_t* data, tb_size_t size)
{
	if (!st || !data) return -1;
	if (!size) return 0;

	// read data from stream data first
	tb_int_t read_n = 0;
	if (st->size > 0)
	{
		// if enough?
		if (st->size > size)
		{
			read_n = size;
			memcpy(data, st->head, read_n);
			st->head += read_n;
			st->size -= read_n;
		}
		else
		{
			read_n = st->size;
			memcpy(data, st->head, read_n);
			st->head = st->data;
			st->size = 0;
		}
	}

	// is enough?
	if (read_n == size) return read_n;

	// read stream directly
	if (!(st->flag & TB_STREAM_FLAG_IS_ZLIB))
	{
		tb_int_t ret = st->read(st, data + read_n, size - read_n);
		if (ret > 0) read_n += ret; 
	}
	else 
	{
		// check
		TB_ASSERT(st->hzlib != TB_INVALID_HANDLE);
		
		// exists zdata?
		if (!tb_zlib_left(st->hzlib))
		{
			// read zdata
			tb_int_t zsize = st->read(st, st->zdata, TB_STREAM_ZDATA_MAX);
			if (zsize < 0) goto fail;
			else if (!zsize) return read_n;

			// attach zdata
			tb_zlib_attach(st->hzlib, st->zdata, zsize);
		}

		// inflate data
		tb_int_t need_n = size - read_n;
		tb_int_t real_n = TB_STREAM_DATA_MAX;
		if (TB_FALSE == tb_zlib_inflate_partial(st->hzlib, st->data, &real_n) || real_n < 0)
			goto fail;

		// read data
		st->size = real_n;
		if (st->size > 0)
		{
			// if enough?
			if (st->size > need_n)
			{
				memcpy(data + read_n, st->data, need_n);
				st->head += need_n;
				st->size -= need_n;
				read_n += need_n;
			}
			else
			{
				memcpy(data + read_n, st->data, st->size);
				read_n += st->size;
				st->head = st->data;
				st->size = 0;
			}
		}
	}

	return read_n;

fail:
	if (read_n) return read_n;
	else return -1;
}
static tb_int_t tb_stream_read_block(tb_stream_t* st, tb_byte_t* data, tb_size_t size)
{
	if (!st || !data) return -1;
	if (!size) return 0;

	tb_int_t try_n = 100;
	tb_int_t read_n = 0;
	while (read_n < size)
	{
		tb_int_t ret = tb_stream_read_no_block(st, data + read_n, size - read_n);
		if (ret < 0) goto fail;
		else if (!ret) 
		{
			if (!try_n--) break;
		}
		read_n += ret;
	}

	return read_n;

fail:
	if (read_n) return read_n;
	else return -1;
}

/* /////////////////////////////////////////////////////////
 * interface
 */

void tb_stream_close(tb_stream_t* st)
{
	if (st)
	{
		// close stream
		if (st->close) st->close(st); 

		// free hzlib
		if (st->hzlib != TB_INVALID_HANDLE) tb_zlib_destroy(st->hzlib);
	}
}

tb_size_t tb_stream_size(tb_stream_t const* st)
{
	if (st && st->ssize) return st->ssize(st);
	else return 0;
}
tb_size_t tb_stream_offset(tb_stream_t const* st)
{
	if (st) return st->offset;
	else return 0;
}
tb_int_t tb_stream_read(tb_stream_t* st, tb_byte_t* data, tb_size_t size)
{
	// read data
	tb_int_t ret = 0;
	if (st->flag & TB_STREAM_FLAG_IS_BLOCK) 
		ret = tb_stream_read_block(st, data, size);
	else ret = tb_stream_read_no_block(st, data, size);

	// update offset
	if (ret > 0) st->offset += ret;
	//TB_DBG("read: %d", ret);
	return ret;
}
tb_int_t tb_stream_write(tb_stream_t* st, tb_byte_t* data, tb_size_t size)
{
	return 0;
}
 
tb_byte_t* tb_stream_need(tb_stream_t* st, tb_size_t size)
{
	if (!st) return TB_NULL;

	//TB_DBG("need: %d %d", size, st->size);

	// hook
	if (st->need)
	{
		tb_byte_t* p = st->need(st, size);
		if (p) return p;
	}

	// check size
	TB_ASSERT(size <= TB_STREAM_DATA_MAX);
	if (size > TB_STREAM_DATA_MAX) return TB_NULL;
	if (!size) return st->head;

	// read data from stream data first
	if (st->size > 0)
	{
		// if enough?
		if (st->size > size) return st->head;
		else
		{
			// move data
			memmove(st->data, st->head, st->size);
			st->head = st->data;
		}
	}

	// read stream directly
	tb_int_t try_n = 100;
	if (!(st->flag & TB_STREAM_FLAG_IS_ZLIB))
	{
		while (st->size < size)
		{
			tb_int_t ret = st->read(st, st->data + st->size, size - st->size);
			if (ret < 0) break;
			else if (!ret)
			{
				if (!try_n--) break;
			}
			st->size += ret;
		}
	}
	else 
	{
		// check
		TB_ASSERT(st->hzlib != TB_INVALID_HANDLE);
		while (st->size < size)
		{
			// exists zdata?
			if (!tb_zlib_left(st->hzlib))
			{
				// read zdata
				tb_int_t zsize = st->read(st, st->zdata, TB_STREAM_ZDATA_MAX);
				if (zsize < 0) return -1;
				else if (!zsize) 
				{
					if (!try_n--) break;
				}

				// attach zdata
				tb_zlib_attach(st->hzlib, st->zdata, zsize);
			}

			// inflate data
			tb_int_t real_n = size - st->size;

			if (TB_FALSE == tb_zlib_inflate_partial(st->hzlib, st->data + st->size, &real_n) || real_n < 0)
				return -1;
			st->size += real_n;
		}
	}

	if (st->size < size) return TB_NULL;
	else return st->head;
}

tb_bool_t tb_stream_seek(tb_stream_t* st, tb_int_t offset, tb_stream_seek_t flag)
{
	// hook
	if (st->seek && TB_TRUE == st->seek(st, offset, flag))
		return TB_TRUE;

	// adjust offset
	if (st->flag & TB_STREAM_FLAG_IS_STREAM)
	{
		if (flag == TB_STREAM_SEEK_BEG && st->offset > offset) return TB_FALSE;
		else if (flag == TB_STREAM_SEEK_END && !offset) return TB_FALSE;
		else if (flag == TB_STREAM_SEEK_CUR) 
		{
			if (offset < 0) return TB_FALSE;
			offset += st->offset;
		}
	}
	else
	{
		if (flag == TB_STREAM_SEEK_CUR) offset += st->offset;
		else if (flag == TB_STREAM_SEEK_END) 
		{
			tb_size_t size = tb_stream_size(st);
			if (!size) return TB_FALSE;
			offset += size;
		}
	}

	// only for backward
	if (st->offset < offset)
	{
		tb_int_t try_n = 100;
		while (st->offset < offset)
		{
			tb_byte_t data[TB_STREAM_DATA_MAX];
			tb_size_t size = TB_MATH_MIN(offset - st->offset, TB_STREAM_DATA_MAX);
			tb_int_t ret = tb_stream_read(st, data, size);
			if (ret != size && st->flag & TB_STREAM_FLAG_IS_BLOCK) break;
			else if (ret < 0) break;
			else if (!ret)
			{
				if (!try_n--) break;
			}
		}
	}

	if (st->offset == offset) return TB_TRUE;
	else return TB_FALSE;
}
tb_stream_flag_t tb_stream_flag(tb_stream_t const* st)
{
	if (st) return st->flag;
}
tb_bool_t tb_stream_switch(tb_stream_t* st, tb_stream_flag_t flag)
{
	// the old flag
	tb_uint32_t oflag = st->flag;

	// is zlib? 0 => 1
	if (!(oflag & TB_STREAM_FLAG_IS_ZLIB) && (flag & TB_STREAM_FLAG_IS_ZLIB))
	{
		// create zlib
		st->hzlib = tb_zlib_create();
		if (st->hzlib == TB_INVALID_HANDLE) return TB_FALSE;

		if (st->size)
		{
			st->size = 0;
			st->head = st->data;
			TB_DBG("[warning]: exists non-zlib data at stream data now!");
		}
	}
	// is zlib? 1 => 0
	else if (!(flag & TB_STREAM_FLAG_IS_ZLIB) && (oflag & TB_STREAM_FLAG_IS_ZLIB))
	{
		// destroy zlib
		if (st->hzlib != TB_INVALID_HANDLE)
			tb_zlib_destroy(st->hzlib);
		st->hzlib = TB_INVALID_HANDLE;
	}

	// update flag
	st->flag = flag;
	return TB_TRUE;
}
