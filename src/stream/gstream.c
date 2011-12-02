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
 * \file		stream.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "gstream.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../string/string.h"
#include "../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * types
 */
 
// the stream table item type
typedef struct __tb_gstream_item_t
{
	// the stream type
	tb_size_t 			type;

	// the stream name
	tb_char_t const* 	name;
	
	// the stream creator
	tb_gstream_t* 		(*create)();

}tb_gstream_item_t;

/* /////////////////////////////////////////////////////////
 * globals
 */

// the stream table
static tb_gstream_item_t g_gstream_table[] = 
{
	{TB_GSTREAM_TYPE_HTTP, "http", tb_gstream_create_http}
,	{TB_GSTREAM_TYPE_HTTP, "https", tb_gstream_create_http}
,	{TB_GSTREAM_TYPE_FILE, "file", tb_gstream_create_file}
,	{TB_GSTREAM_TYPE_DATA, "data", tb_gstream_create_data}
};


/* /////////////////////////////////////////////////////////
 * details
 */
static tb_byte_t* tb_gstream_cache_need(tb_gstream_t* gst, tb_size_t size)
{
#if 0
	// hook, .e.g the data stream
	if (gst->need) return gst->need(gst, size);

	// check 
	tb_assert_and_check_return_val(size <= TB_GSTREAM_CACHE_MAXN, TB_NULL);
	if (!size) return gst->cache_head;

	// read it if exists
	if (gst->cache_size > 0)
	{
		// if enough?
		if (gst->cache_size > size) return gst->cache_head;
		else
		{
			// move data
			tb_memmov(gst->cache_data, gst->cache_head, gst->size);
			gst->cache_head = gst->cache_data;
		}
	}

	// fill the left data
	tb_long_t ret = tb_gstream_block_read(gst, gst->cache_data + gst->cache_size, size - gst->cache_size);
	if (ret < 0) return TB_NULL;

	// update size
	gst->cache_size += ret;
	tb_assert_and_check_return_val(gst->cache_size == size, TB_NULL);
#endif
	tb_trace_noimpl();
	return TB_NULL;
}
static tb_long_t tb_gstream_cache_read(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_long_t read = 0;

	// read data from cache first
	tb_size_t cache_size = gst->cache_size;
	if (cache_size)
	{
		// update read
		read = cache_size > size? size : cache_size;

		// read data
		tb_memcpy(data, gst->cache_head, read);
		gst->cache_head += read;
		gst->cache_size -= read;
	}
	
	// has data?
	tb_check_return_val(!read, read);

	// cache is null now.
	tb_assert_and_check_return_val(!gst->cache_size, -1);

	// fill cache
	tb_assert(gst->read);
	read = gst->read(gst, gst->cache_data, TB_GSTREAM_CACHE_MAXN);
	if (read > 0) 
	{
		gst->cache_head = gst->cache_data;
		gst->cache_size = read;
	}

	return read;
}
static tb_long_t tb_gstream_cache_writ(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_long_t writ = 0;

	// writ data to cache first
	tb_size_t cache_left = TB_GSTREAM_CACHE_MAXN - gst->cache_size;
	if (cache_left)
	{
		// update writ
		writ = cache_left > size? size : cache_left;

		// writ data
		tb_memcpy(gst->cache_data + gst->cache_size, data, writ);
		gst->cache_size += writ;
	}
	
	// has data?
	tb_check_return_val(!writ, writ);

	// cache is full now.
	tb_assert_and_check_return_val(gst->cache_size == TB_GSTREAM_CACHE_MAXN, -1);

	// fill cache
	tb_assert(gst->writ);
	writ = gst->writ(gst, gst->cache_data, TB_GSTREAM_CACHE_MAXN);
	if (writ > 0)
	{
		if (writ < TB_GSTREAM_CACHE_MAXN) 
			tb_memmov(gst->cache_data, gst->cache_data + writ, TB_GSTREAM_CACHE_MAXN - writ);
		gst->cache_size -= writ;
	}

	tb_trace("writ: %d", writ);
	return writ;
}
static tb_void_t tb_gstream_cache_flush(tb_gstream_t* gst)
{
	tb_trace_noimpl();
}

/* /////////////////////////////////////////////////////////
 * interface
 */
tb_gstream_t* tb_gstream_create_from_url(tb_char_t const* url)
{
	tb_assert_and_check_return_val(url, TB_NULL);

	// get proto name
	tb_char_t 			proto[32];
	tb_char_t* 			p = proto;
	tb_char_t const* 	e = p + 32;
	tb_char_t const* 	u = url;
	for (; p < e && *u && *u != ':'; p++, u++) *p = *u;
	*p = '\0';

	// find stream
	tb_size_t 		i = 0;
	tb_size_t 		n = tb_arrayn(g_gstream_table);
	tb_gstream_t* 	gst = TB_NULL;
	for (; i < n; ++i)
	{
		if (!tb_strcmp(g_gstream_table[i].name, proto))
		{
			gst = g_gstream_table[i].create();
			break;
		}
	}

	// \note: prehandle for file
	if (gst && gst->type == TB_GSTREAM_TYPE_FILE) url = url + 7; 	// file:///home/file => /home/file
	else if (!gst && url[0] == '/') gst = tb_gstream_create_file(); // is /home/file?

	// check
	tb_assert_and_check_return_val(gst, TB_NULL);

	// set url
	if (!tb_gstream_ioctl1(gst, TB_GSTREAM_CMD_SET_URL, url)) goto fail;

	// ok
	return gst;

fail:
	if (gst) tb_gstream_destroy(gst);
	return TB_NULL;
}

tb_void_t tb_gstream_destroy(tb_gstream_t* gst)
{
	if (gst) 
	{
		// close it
		tb_gstream_close(gst);

		// free cache
		if (gst->cache_data) tb_free(gst->cache_data);
		gst->cache_data = TB_NULL;

		// free url
		if (gst->url) tb_free(gst->url);
		gst->url = TB_NULL;

		// free it
		if (gst->free) gst->free(gst);
		tb_free(gst);
	}
}

tb_bool_t tb_gstream_open(tb_gstream_t* gst)
{
	// check stream
	tb_assert_and_check_return_val(gst && gst->open, TB_FALSE);

	// already been opened?
	tb_assert_and_check_return_val(!gst->bopened, TB_TRUE);

	// set open state first, avoid occupy
	gst->bopened = 1;

	// init timeout
	if (!gst->timeout) gst->timeout = TB_GSTREAM_TIMEOUT_DEFAULT;
	
	// init cache
	if (!gst->cache_data) gst->cache_data = tb_malloc(TB_GSTREAM_CACHE_MAXN);
	tb_assert_and_check_return_val(gst->cache_data, TB_FALSE);
	gst->cache_head = gst->cache_data;
	gst->cache_size = 0;

	// open it
	if (!gst->open(gst)) gst->bopened = 0;

	// ok?
	return gst->bopened? TB_TRUE : TB_FALSE;
}
tb_void_t tb_gstream_close(tb_gstream_t* gst)
{
	// check stream
	tb_assert_and_check_return(gst);

	// already been closed?
	tb_assert_and_check_return(gst->bopened);

	// set close state first, avoid occupy
	gst->bopened = 0;

	// flush cache
	tb_gstream_cache_flush(gst);

	// close it
	if (gst->close) gst->close(gst);	

	// exit cache
	gst->cache_head = gst->cache_data;
	gst->cache_size = 0;
}
tb_byte_t* tb_gstream_need(tb_gstream_t* gst, tb_size_t size)
{
	// check size
	tb_check_return_val(size, TB_NULL);

	// check stream
	tb_assert_and_check_return_val(gst && gst->read, TB_NULL);

	// check cache
	tb_assert_and_check_return_val(gst->cache_data && gst->cache_head, TB_NULL);

	// need data from cache
	return tb_gstream_cache_need(gst, size);
}

tb_long_t tb_gstream_read(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(gst && gst->bopened && gst->read, -1);

	// check cache
	tb_assert_and_check_return_val(gst->cache_data && gst->cache_head, -1);

	// read data from cache
	return tb_gstream_cache_read(gst, data, size);
}

tb_long_t tb_gstream_writ(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(gst && gst->bopened && gst->writ, -1);

	// check cache
	tb_assert_and_check_return_val(gst->cache_data && gst->cache_head, -1);

	// writ data to cache
	return tb_gstream_cache_writ(gst, data, size);
}
tb_long_t tb_gstream_bread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(gst && gst->bopened && gst->read, -1);

	// check cache
	tb_assert_and_check_return_val(gst->cache_data && gst->cache_head, -1);

	// read data from cache
	tb_long_t 	read = 0;
	tb_int64_t 	time = tb_mclock();
	while (read < size)
	{
		tb_long_t ret = tb_gstream_cache_read(gst, data + read, size - read);	
		if (ret > 0)
		{
			read += ret;
			time = tb_mclock();
		}
		else if (!ret)
		{
			// timeout?
			if (tb_mclock() - time > gst->timeout) break;

			// sleep some time
			tb_usleep(100);
		}
		else return -1;
	}
	return read;
}

tb_long_t tb_gstream_bwrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(gst && gst->bopened && gst->writ, -1);

	// check cache
	tb_assert_and_check_return_val(gst->cache_data && gst->cache_head, -1);

	// writ data to cache
	tb_long_t 	writ = 0;
	tb_int64_t 	time = tb_mclock();
	while (writ < size)
	{
		tb_long_t ret = tb_gstream_cache_writ(gst, data + writ, size - writ);	
		if (ret > 0)
		{
			writ += ret;
			time = tb_mclock();
		}
		else if (!ret)
		{
			// timeout?
			if (tb_mclock() - time > gst->timeout) break;

			// sleep some time
			tb_usleep(100);
		}
		else return -1;
	}
	return writ;
}
tb_long_t tb_gstream_read_line(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_trace_noimpl();
	return 0;
}
tb_long_t tb_gstream_writ_line(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	tb_trace_noimpl();
	return 0;
}

tb_long_t tb_gstream_printf(tb_gstream_t* gst, tb_char_t const* fmt, ...)
{
	// init data
	tb_char_t data[TB_GSTREAM_CACHE_MAXN] = {0};
	tb_size_t size = 0;

	// format data
    TB_VA_FMT(data, TB_GSTREAM_CACHE_MAXN, fmt, &size);
	tb_check_return_val(size, 0);

	// writ data
	return tb_gstream_bwrit(gst, data, size);
}

tb_bool_t tb_gstream_seek(tb_gstream_t* gst, tb_int64_t offset, tb_gstream_seek_t flag)
{
#if 0
	tb_assert_and_check_return_val(gst, TB_FALSE);

	// not support for cache now
	tb_assert_and_check_return_val(!gst->cache_size, TB_FALSE);

	// hook
	if (gst->seek && gst->seek(gst, offset, flag))
		return TB_TRUE;

	// compute the real offset
	tb_uint64_t size = tb_gstream_size(gst);
	tb_uint64_t curt = tb_gstream_offset(gst);
	if (flag == TB_GSTREAM_SEEK_CUR) offset += curt;
	else if (flag == TB_GSTREAM_SEEK_END)
	{
		tb_assert_and_check_return_val(size && offset <= 0, TB_FALSE);
		offset += size;
	}

	// forward only
	tb_assert_and_check_return_val(offset >= 0 && (!size || offset <= size), TB_FALSE);
	if (curt < offset)
	{
		tb_int64_t time = tb_mclock();
		while (tb_gstream_offset(gst) < offset)
		{
			tb_byte_t data[TB_GSTREAM_CACHE_MAXN];
			tb_size_t need = tb_min(offset - tb_gstream_offset(gst), TB_GSTREAM_CACHE_MAXN);
			tb_long_t ret = tb_gstream_read(gst, data, need);
			if (ret > 0) time = tb_mclock();
			else if (!ret)
			{
				// timeout?
				if (tb_mclock() - time > gst->timeout) break;
			}
			else break;
		}
	}

	// ok?
	return (tb_gstream_offset(gst) == offset)? TB_TRUE : TB_FALSE;
#else
	tb_trace_noimpl();
	return TB_FALSE;
#endif
}
tb_uint64_t tb_gstream_size(tb_gstream_t const* gst)
{
	tb_assert_and_check_return_val(gst, 0);
	return gst->size? gst->size(gst) : 0;
}
tb_uint64_t tb_gstream_offset(tb_gstream_t const* gst)
{
	tb_assert_and_check_return_val(gst && gst->offset, 0);

	// get offset
	tb_uint64_t offset = gst->offset(gst);
	tb_assert_and_check_return_val(offset >= gst->cache_size, 0);

	// adjust offset
	return offset - gst->cache_size;
}
tb_uint64_t tb_gstream_left(tb_gstream_t const* gst)
{
	tb_uint64_t size = tb_gstream_size(gst);
	tb_uint64_t offset = tb_gstream_offset(gst);
	return (size > offset? (size - offset) : 0);
}
tb_size_t tb_gstream_timeout(tb_gstream_t const* gst)
{	
	tb_assert_and_check_return_val(gst, 0);
	return gst->timeout;
}
tb_bool_t tb_gstream_ioctl0(tb_gstream_t* gst, tb_size_t cmd)
{
	tb_assert_and_check_return_val(gst && gst->ioctl0, TB_FALSE);
	return gst->ioctl0(gst, cmd);
}
tb_bool_t tb_gstream_ioctl1(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1)
{	
	tb_assert_and_check_return_val(gst && gst->ioctl1, TB_FALSE);

	tb_bool_t ret = TB_FALSE;
	switch (cmd)
	{
	case TB_GSTREAM_CMD_SET_URL:
		{
			if (arg1)
			{
				if (!gst->url) gst->url = tb_calloc(1, TB_GSTREAM_URL_MAXN);
				if (gst->url)
				{
					tb_strncpy(gst->url, (tb_char_t const*)arg1, TB_GSTREAM_URL_MAXN);
					gst->url[TB_GSTREAM_URL_MAXN - 1] = '\0';
					ret = TB_TRUE;
				}
			}
		}
		break;
	case TB_GSTREAM_CMD_GET_URL:
		{
			if (arg1)
			{
				*((tb_char_t const**)arg1) = gst->url;
				ret = TB_TRUE;
			}
		}
		break;
	case TB_GSTREAM_CMD_SET_TIMEOUT:
		{
			gst->timeout = (tb_size_t)arg1;
			ret = TB_TRUE;
		}
		break;
	default:
		break;
	}
	return (gst->ioctl1(gst, cmd, arg1) || ret)? TB_TRUE : TB_FALSE;
}
tb_bool_t tb_gstream_ioctl2(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1, tb_pointer_t arg2)
{
	tb_assert_and_check_return_val(gst && gst->ioctl2, TB_FALSE);
	return gst->ioctl2(gst, cmd, arg1, arg2);
}
tb_uint8_t tb_gstream_read_u8(tb_gstream_t* gst)
{
	tb_byte_t b[1];
	if (1 != tb_gstream_bread(gst, b, 1)) return 0;
	return b[0];
}
tb_sint8_t tb_gstream_read_s8(tb_gstream_t* gst)
{
	tb_byte_t b[1];
	if (1 != tb_gstream_bread(gst, b, 1)) return 0;
	return b[0];
}

tb_uint16_t tb_gstream_read_u16_le(tb_gstream_t* gst)
{	
	tb_byte_t b[2];
	if (2 != tb_gstream_bread(gst, b, 2)) return 0;
	return tb_bits_get_u16_le(b);
}
tb_sint16_t tb_gstream_read_s16_le(tb_gstream_t* gst)
{	
	tb_byte_t b[2];
	if (2 != tb_gstream_bread(gst, b, 2)) return 0;
	return tb_bits_get_s16_le(b);
}
tb_uint32_t tb_gstream_read_u24_le(tb_gstream_t* gst)
{	
	tb_byte_t b[3];
	if (3 != tb_gstream_bread(gst, b, 3)) return 0;
	return tb_bits_get_u24_le(b);
}
tb_sint32_t tb_gstream_read_s24_le(tb_gstream_t* gst)
{
	tb_byte_t b[3];
	if (3 != tb_gstream_bread(gst, b, 3)) return 0;
	return tb_bits_get_s24_le(b);
}
tb_uint32_t tb_gstream_read_u32_le(tb_gstream_t* gst)
{
	tb_byte_t b[4];
	if (4 != tb_gstream_bread(gst, b, 4)) return 0;
	return tb_bits_get_u32_le(b);
}
tb_sint32_t tb_gstream_read_s32_le(tb_gstream_t* gst)
{	
	tb_byte_t b[4];
	if (4 != tb_gstream_bread(gst, b, 4)) return 0;
	return tb_bits_get_s32_le(b);
}
tb_uint16_t tb_gstream_read_u16_be(tb_gstream_t* gst)
{	
	tb_byte_t b[2];
	if (2 != tb_gstream_bread(gst, b, 2)) return 0;
	return tb_bits_get_u16_be(b);
}
tb_sint16_t tb_gstream_read_s16_be(tb_gstream_t* gst)
{	
	tb_byte_t b[2];
	if (2 != tb_gstream_bread(gst, b, 2)) return 0;
	return tb_bits_get_s16_be(b);
}
tb_uint32_t tb_gstream_read_u24_be(tb_gstream_t* gst)
{	
	tb_byte_t b[3];
	if (3 != tb_gstream_bread(gst, b, 3)) return 0;
	return tb_bits_get_u24_be(b);
}
tb_sint32_t tb_gstream_read_s24_be(tb_gstream_t* gst)
{
	tb_byte_t b[3];
	if (3 != tb_gstream_bread(gst, b, 3)) return 0;
	return tb_bits_get_s24_be(b);
}
tb_uint32_t tb_gstream_read_u32_be(tb_gstream_t* gst)
{
	tb_byte_t b[4];
	if (4 != tb_gstream_bread(gst, b, 4)) return 0;
	return tb_bits_get_u32_be(b);
}
tb_sint32_t tb_gstream_read_s32_be(tb_gstream_t* gst)
{	
	tb_byte_t b[4];
	if (4 != tb_gstream_bread(gst, b, 4)) return 0;
	return tb_bits_get_s32_be(b);
}
tb_bool_t tb_gstream_writ_u8(tb_gstream_t* gst, tb_uint8_t val)
{
	tb_byte_t b[1];
	tb_bits_set_u8(b, val);
	if (1 != tb_gstream_bwrit(gst, b, 1)) return TB_FALSE;
	return TB_TRUE;
}
tb_bool_t tb_gstream_writ_s8(tb_gstream_t* gst, tb_sint8_t val)
{
	tb_byte_t b[1];
	tb_bits_set_s8(b, val);
	if (1 != tb_gstream_bwrit(gst, b, 1)) return TB_FALSE;
	return TB_TRUE;
}

tb_bool_t tb_gstream_writ_u16_le(tb_gstream_t* gst, tb_uint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_u16_le(b, val);
	if (2 != tb_gstream_bwrit(gst, b, 2)) return TB_FALSE;
	return TB_TRUE;
}
tb_bool_t tb_gstream_writ_s16_le(tb_gstream_t* gst, tb_sint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_s16_le(b, val);
	if (2 != tb_gstream_bwrit(gst, b, 2)) return TB_FALSE;
	return TB_TRUE;
}

tb_bool_t tb_gstream_writ_u24_le(tb_gstream_t* gst, tb_uint32_t val)
{	
	tb_byte_t b[3];
	tb_bits_set_u24_le(b, val);
	if (3 != tb_gstream_bwrit(gst, b, 3)) return TB_FALSE;
	return TB_TRUE;
}
tb_bool_t tb_gstream_writ_s24_le(tb_gstream_t* gst, tb_sint32_t val)
{
	tb_byte_t b[3];
	tb_bits_set_s24_le(b, val);
	if (3 != tb_gstream_bwrit(gst, b, 3)) return TB_FALSE;
	return TB_TRUE;
}

tb_bool_t tb_gstream_writ_u32_le(tb_gstream_t* gst, tb_uint32_t val)
{	
	tb_byte_t b[4];
	tb_bits_set_u32_le(b, val);
	if (4 != tb_gstream_bwrit(gst, b, 4)) return TB_FALSE;
	return TB_TRUE;
}
tb_bool_t tb_gstream_writ_s32_le(tb_gstream_t* gst, tb_sint32_t val)
{
	tb_byte_t b[4];
	tb_bits_set_s32_le(b, val);
	if (4 != tb_gstream_bwrit(gst, b, 4)) return TB_FALSE;
	return TB_TRUE;
}

tb_bool_t tb_gstream_writ_u16_be(tb_gstream_t* gst, tb_uint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_u16_be(b, val);
	if (2 != tb_gstream_bwrit(gst, b, 2)) return TB_FALSE;
	return TB_TRUE;
}
tb_bool_t tb_gstream_writ_s16_be(tb_gstream_t* gst, tb_sint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_s16_be(b, val);
	if (2 != tb_gstream_bwrit(gst, b, 2)) return TB_FALSE;
	return TB_TRUE;
}

tb_bool_t tb_gstream_writ_u24_be(tb_gstream_t* gst, tb_uint32_t val)
{	
	tb_byte_t b[3];
	tb_bits_set_u24_be(b, val);
	if (3 != tb_gstream_bwrit(gst, b, 3)) return TB_FALSE;
	return TB_TRUE;
}
tb_bool_t tb_gstream_writ_s24_be(tb_gstream_t* gst, tb_sint32_t val)
{
	tb_byte_t b[3];
	tb_bits_set_s24_be(b, val);
	if (3 != tb_gstream_bwrit(gst, b, 3)) return TB_FALSE;
	return TB_TRUE;
}

tb_bool_t tb_gstream_writ_u32_be(tb_gstream_t* gst, tb_uint32_t val)
{	
	tb_byte_t b[4];
	tb_bits_set_u32_be(b, val);
	if (4 != tb_gstream_bwrit(gst, b, 4)) return TB_FALSE;
	return TB_TRUE;
}
tb_bool_t tb_gstream_writ_s32_be(tb_gstream_t* gst, tb_sint32_t val)
{
	tb_byte_t b[4];
	tb_bits_set_s32_be(b, val);
	if (4 != tb_gstream_bwrit(gst, b, 4)) return TB_FALSE;
	return TB_TRUE;
}
tb_uint64_t tb_gstream_load(tb_gstream_t* gst, tb_gstream_t* ist)
{
	tb_assert_and_check_return_val(gst && ist, 0);	

	// read data
	tb_byte_t 		data[TB_GSTREAM_CACHE_MAXN];
	tb_uint64_t 	read = 0;
	tb_uint64_t 	left = tb_gstream_left(ist);
	tb_int64_t 		time = tb_mclock();
	do
	{
		// read data
		tb_long_t n = tb_gstream_read(ist, data, TB_GSTREAM_CACHE_MAXN);
		tb_trace("read: %d, offset: %llu, left: %llu, size: %llu", n, tb_gstream_offset(ist), tb_gstream_left(ist), tb_gstream_size(ist));
		if (n > 0)
		{
			// update clock
			time = tb_mclock();

			// writ data
			if (n != tb_gstream_bwrit(gst, data, n)) break;

			// update read
			read += n;
		}
		else if (!n) 
		{
			// timeout?
			if (tb_mclock() - time > gst->timeout) break;

			// sleep some time
			tb_usleep(100);
		}
		else break;

		// is end?
		if (left && read >= left) break;

	} while(1);

	return read;
}
tb_uint64_t tb_gstream_save(tb_gstream_t* gst, tb_gstream_t* ost)
{
	tb_assert_and_check_return_val(gst && ost, 0);
	return tb_gstream_load(ost, gst);
}

