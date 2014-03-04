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
 * @file		gstream.c
 * @ingroup 	stream
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 			"gstream"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stream.h"
#include "../asio/asio.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../string/string.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_long_t tb_gstream_cache_aread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size)
{
	// done
	tb_long_t read = 0;
	do
	{
		if (gstream->bcached)
		{
			// read data from cache first
			read = tb_qbuffer_read(&gstream->cache, data, size);
			tb_check_return_val(read >= 0, -1);

			// empty? full it from stream
			tb_check_break(!read);

			// cache is null now.
			tb_assert_and_check_return_val(tb_qbuffer_null(&gstream->cache), -1);

			// enter cache for push
			tb_size_t 	push = 0;
			tb_byte_t* 	tail = tb_qbuffer_push_init(&gstream->cache, &push);
			tb_assert_and_check_return_val(tail && push, -1);

			// push data to cache from stream
			tb_assert(gstream->read);
			tb_long_t 	real = gstream->read(gstream, tail, push);
			tb_check_return_val(real >= 0, -1);

			// read the left data from cache
			if (real > 0) 
			{
				// leave cache for push
				tb_qbuffer_push_exit(&gstream->cache, real);

				// read cache
				real = tb_qbuffer_read(&gstream->cache, data + read, tb_min(real, size - read));
				tb_check_return_val(real >= 0, -1);

				// update read 
				read += real;
			}
		}
		else 
		{
			// read it directly
			read = gstream->read(gstream, data, size);
			tb_check_return_val(read >= 0, -1);
		}
	}
	while (0);

	// update offset
	gstream->offset += read;

	// update status
	gstream->bwrited = 0;

//	tb_trace_impl("read: %d", read);
	return read;
}
static tb_long_t tb_gstream_cache_awrit(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size)
{
	// done
	tb_long_t writ = 0;
	do
	{
		if (gstream->bcached)
		{
			// writ data to cache first
			writ = tb_qbuffer_writ(&gstream->cache, data, size);
			tb_check_return_val(writ >= 0, -1);
			
			// full? writ data to stream from cache
			tb_check_break(!writ);

			// cache is full now.
			tb_assert_and_check_return_val(tb_qbuffer_full(&gstream->cache), -1);

			// enter cache for pull
			tb_size_t 	pull = 0;
			tb_byte_t* 	head = tb_qbuffer_pull_init(&gstream->cache, &pull);
			tb_assert_and_check_return_val(head && pull, -1);

			// pull data to stream from cache
			tb_assert(gstream->writ);
			tb_long_t 	real = gstream->writ(gstream, head, pull);
			tb_check_return_val(real >= 0, -1);

			// writ the left data to cache
			if (real > 0)
			{
				// leave cache for pull
				tb_qbuffer_pull_exit(&gstream->cache, real);

				// writ cache
				real = tb_qbuffer_writ(&gstream->cache, data + writ, tb_min(real, size - writ));
				tb_check_return_val(real >= 0, -1);

				// update writ 
				writ += real;
			}
		}
		else 
		{
			// writ it directly
			writ = gstream->writ(gstream, data, size);
			tb_check_return_val(writ >= 0, -1);
		}

	} while (0);

	// update offset
	gstream->offset += writ;

	// update status
	gstream->bwrited = 1;

//	tb_trace_impl("writ: %d", writ);
	return writ;
}
static tb_bool_t tb_gstream_cache_sync(tb_gstream_t* gstream, tb_bool_t bclosing)
{
	// TODO: gstream->bcached
	return tb_false;
#if 0
	// init 
	tb_long_t r = -1;

	// flush cache first if has cache
	if (!tb_qbuffer_null(&gstream->cache)) 
	{
		// enter cache for pull
		tb_size_t 	pull = 0;
		tb_byte_t* 	head = tb_qbuffer_pull_init(&gstream->cache, &pull);
		tb_assert_and_check_return_val(head && pull, -1);

		// writ data to stream
		tb_long_t 	writ = gstream->writ(gstream, head, pull);
		tb_check_goto(writ >= 0, end);
		tb_assert_and_check_return_val(writ <= pull, -1);

		// leave cache for pull
		tb_qbuffer_pull_exit(&gstream->cache, writ);
	}

	// flush data if no cache
	if (tb_qbuffer_null(&gstream->cache))
	{
		// flush writ data
		r = gstream->writ(gstream, data, size);
	}
	else 
	{
		// continue 
		r = 0;
	}

end:
	// update status if end
	if (r < 0) gstream->bwrited = 0;

	// ok?
	return r;
#endif
}
static tb_bool_t tb_gstream_cache_seek(tb_gstream_t* gstream, tb_hize_t offset)
{
	// flush writed data first
	if (gstream->bwrited && !tb_gstream_sync(gstream, tb_false)) return tb_false;

	// cached?
	if (gstream->bcached)
	{
		tb_size_t 	size = 0;
		tb_hize_t 	curt = tb_stream_offset(gstream);
		tb_byte_t* 	data = tb_qbuffer_pull_init(&gstream->cache, &size);
		if (data && size && offset >= curt && offset <= curt + size)
		{
			// seek it at the cache
			tb_qbuffer_pull_exit(&gstream->cache, (tb_size_t)(offset - curt));

			// save offset
			gstream->offset = offset;

			// ok
			return tb_true;
		}
	}

	// seek it
	tb_check_return_val(gstream->seek, tb_false);
	tb_bool_t ok = gstream->seek(gstream, offset);

	// ok? 
	if (ok)
	{
		// clear cache
		tb_qbuffer_clear(&gstream->cache);

		// save offset
		gstream->offset = offset;
	}
	return ok;
}
static tb_bool_t tb_gstream_cache_need(tb_gstream_t* gstream, tb_byte_t** data, tb_size_t size)
{
	// TODO: gstream->bcached
	return tb_false;
#if 0
	// enough?
	if (size <= tb_qbuffer_size(&gstream->cache)) 
	{
		// update status
		gstream->bwrited = 0;

		// save data
		*data = tb_qbuffer_head(&gstream->cache);

		// ok
		return size;
	}

	// enter cache for push
	tb_size_t 	push = 0;
	tb_byte_t* 	tail = tb_qbuffer_push_init(&gstream->cache, &push);
	tb_assert_and_check_return_val(tail && push && size <= push, tb_false); // size <= push?

	// fill cache
	if (need < size)
	{
		// read data
		tb_long_t real = gstream->read(gstream, tail + need, push - need, tb_false);
		tb_check_return_val(real >= 0, -1);

		// update need
		need += real;
	}
	
	// leave cache for push
	tb_qbuffer_push_exit(&gstream->cache, need);

	// update status
	gstream->bwrited = 0;

	// too much?
	if (need > size) need = size;

	// save data
	if (need > 0) *data = tb_qbuffer_head(&gstream->cache);

	// ok?
//	tb_trace_impl("need: %u size: %u", need, size);
	return need;
#endif
}
static tb_long_t tb_gstream_cache_wait(tb_gstream_t* gstream, tb_size_t wait, tb_long_t timeout)
{
	tb_assert_and_check_return_val(gstream, -1);

	// wait the native event first
	tb_long_t e = gstream->wait(gstream, wait, timeout);
	tb_assert_and_check_return_val(e >= 0, -1);
	
	// no event?
	tb_check_return_val(!e, e);

	// wait for cache
	if ((wait & TB_GSTREAM_WAIT_READ) && !tb_qbuffer_null(&gstream->cache)) e |= TB_GSTREAM_WAIT_READ;
	if ((wait & TB_GSTREAM_WAIT_WRIT) && !tb_qbuffer_full(&gstream->cache)) e |= TB_GSTREAM_WAIT_WRIT;

	return e;
}
/* ///////////////////////////////////////////////////////////////////////
 * interface
 */
tb_gstream_t* tb_gstream_init_from_url(tb_char_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// the init
	static tb_gstream_t* (*g_init[])() = 
	{
		tb_null
	,	tb_gstream_init_file
	,	tb_gstream_init_sock
	,	tb_gstream_init_http
	,	tb_gstream_init_data
	};

	// init
	tb_char_t const* 	p = url;
	tb_gstream_t* 		gstream = tb_null;
	tb_size_t 			type = TB_STREAM_TYPE_NONE;
	if (!tb_strnicmp(p, "http://", 7)) 			type = TB_STREAM_TYPE_HTTP;
	else if (!tb_strnicmp(p, "sock://", 7)) 	type = TB_STREAM_TYPE_SOCK;
	else if (!tb_strnicmp(p, "file://", 7)) 	type = TB_STREAM_TYPE_FILE;
	else if (!tb_strnicmp(p, "data://", 7)) 	type = TB_STREAM_TYPE_DATA;
	else if (!tb_strnicmp(p, "https://", 8)) 	type = TB_STREAM_TYPE_HTTP;
	else if (!tb_strnicmp(p, "socks://", 8)) 	type = TB_STREAM_TYPE_SOCK;
	else if (!tb_strstr(p, "://")) 				type = TB_STREAM_TYPE_FILE;
	else 
	{
		tb_trace("[gstream]: unknown prefix for url: %s", url);
		return tb_null;
	}
	tb_assert_and_check_goto(type && type < tb_arrayn(g_init) && g_init[type], fail);

	// init stream
	gstream = g_init[type]();
	tb_assert_and_check_goto(gstream, fail);

	// set url
	if (!tb_stream_ctrl(gstream, TB_STREAM_CTRL_SET_URL, url)) goto fail;

	// ok
	return gstream;

fail:
	
	// exit stream
	if (gstream) tb_gstream_exit(gstream);
	return tb_null;
}
tb_void_t tb_gstream_exit(tb_gstream_t* gstream)
{
	if (gstream) 
	{
		// close it
		tb_gstream_clos(gstream);

		// exit it
		if (gstream->exit) gstream->exit(gstream);

		// exit cache
		tb_qbuffer_exit(&gstream->cache);

		// exit url
		tb_url_exit(&gstream->base.url);

		// free it
		tb_free(gstream);
	}
}
tb_long_t tb_gstream_wait(tb_gstream_t* gstream, tb_size_t wait, tb_long_t timeout)
{
	// check
	tb_assert_and_check_return_val(gstream && gstream->wait, -1);
	
	// have readed event for cache?
	if (wait & TB_GSTREAM_WAIT_READ)
	{
		// the cache is writed-cache now, need call afwrit or bfwrit first.
		tb_assert_and_check_return_val(!gstream->bwrited || tb_qbuffer_null(&gstream->cache), -1);
	}

	// have writed event for cache?
	if (wait & TB_GSTREAM_WAIT_WRIT)
	{
		// the cache is readed-cache now, need call afread or bfread first.
		tb_assert_and_check_return_val(gstream->bwrited || tb_qbuffer_null(&gstream->cache), -1);
	}

	// wait
	tb_long_t ok = tb_gstream_cache_wait(gstream, wait, timeout);

	// wait failed
	if (ok < 0 && !gstream->state) gstream->state = TB_STREAM_STATE_WAIT_FAILED;

	// ok?
	return ok;
}
tb_size_t tb_gstream_state(tb_gstream_t* gstream)
{
	// check
	tb_assert_and_check_return_val(gstream, TB_STREAM_STATE_UNKNOWN_ERROR);

	// the stream state
	return gstream->state;
}
tb_bool_t tb_gstream_beof(tb_gstream_t* gstream)
{
	// already been opened?
	tb_assert_and_check_return_val(gstream && tb_stream_is_opened(gstream), tb_true);

	// size
	tb_hong_t size = tb_stream_size(gstream);
	tb_hize_t offt = tb_stream_offset(gstream);

	// eof?
	return (size > 0 && offt >= size)? tb_true : tb_false;
}
tb_bool_t tb_gstream_open(tb_gstream_t* gstream)
{
	// check stream
	tb_assert_and_check_return_val(gstream && gstream->open, tb_false);

	// already been opened?
	tb_check_return_val(!tb_stream_is_opened(gstream), tb_true);

	// check cache
	tb_assert_and_check_return_val(!gstream->bcached || tb_qbuffer_maxn(&gstream->cache), tb_false);

	// init offset
	gstream->offset = 0;

	// init state
	gstream->state = TB_STREAM_STATE_OK;

	// open it
	tb_bool_t ok = gstream->open(gstream);

	// opened
	if (ok) gstream->base.bopened = 1;

	// ok?
	return ok;
}
tb_void_t tb_gstream_kill(tb_gstream_t* gstream)
{
	// TODO
	tb_trace_noimpl();
}
tb_bool_t tb_gstream_clos(tb_gstream_t* gstream)
{
	// check stream
	tb_assert_and_check_return_val(gstream, tb_false);

	// already been closed?
	tb_check_return_val(tb_stream_is_opened(gstream), tb_true);

	// flush writed data first
	if (gstream->bwrited && !tb_gstream_sync(gstream, tb_true)) return tb_false;

	// has close?
	if (gstream->clos && !gstream->clos(gstream)) return tb_false;

	// clear state
	gstream->offset = 0;
	gstream->bwrited = 0;
	gstream->base.bopened = 0;
	gstream->state = TB_STREAM_STATE_OK;

	// clear cache
	tb_qbuffer_clear(&gstream->cache);

	// ok
	return tb_true;
}
tb_bool_t tb_gstream_need(tb_gstream_t* gstream, tb_byte_t** data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data && size, tb_false);

	// check stream
	tb_assert_and_check_return_val(gstream && tb_stream_is_opened(gstream) && gstream->read, tb_false);

	// check cache
	tb_assert_and_check_return_val(gstream->bcached && tb_qbuffer_maxn(&gstream->cache), tb_false);

	// the cache is writed-cache now, need call afwrit or bfwrit first.
	tb_assert_and_check_return_val(!gstream->bwrited || tb_qbuffer_null(&gstream->cache), tb_false); 

	// need data from cache
	return tb_gstream_cache_need(gstream, data, size);
}
tb_long_t tb_gstream_aread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(gstream && tb_stream_is_opened(gstream) && gstream->read, -1);

	// check cache
	tb_assert_and_check_return_val(!gstream->bcached || tb_qbuffer_maxn(&gstream->cache), -1);

	// the cache is writed-cache now, need call afwrit or bfwrit first.
	tb_assert_and_check_return_val(!gstream->bwrited || tb_qbuffer_null(&gstream->cache), -1); 

	// read data from cache
	return tb_gstream_cache_aread(gstream, data, size);
}

tb_long_t tb_gstream_awrit(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(gstream && tb_stream_is_opened(gstream) && gstream->writ, -1);

	// check cache
	tb_assert_and_check_return_val(!gstream->bcached || tb_qbuffer_maxn(&gstream->cache), -1);

	// the cache is readed-cache now, need call afread or bfread first.
	tb_assert_and_check_return_val(gstream->bwrited || tb_qbuffer_null(&gstream->cache), -1); 

	// writ data to cache
	return tb_gstream_cache_awrit(gstream, data, size);
}
tb_bool_t tb_gstream_bread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(gstream && data, tb_false);
	tb_check_return_val(size, tb_true);

	// check the left
	tb_hize_t left = tb_stream_left(gstream);
	tb_check_return_val(size <= left, tb_false);

	// read data from cache
	tb_long_t read = 0;
	while (read < size)
	{
		// read data
		tb_long_t real = tb_gstream_aread(gstream, data + read, size - read);	
		if (real > 0) read += real;
		else if (!real)
		{
			// wait
			tb_long_t e = tb_gstream_wait(gstream, TB_GSTREAM_WAIT_READ, tb_stream_timeout(gstream));
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_GSTREAM_WAIT_READ);
		}
		else break;
	}

	// ok?
	return (read == size? tb_true : tb_false);
}
tb_bool_t tb_gstream_bwrit(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(gstream && data, tb_false);
	tb_check_return_val(size, tb_true);

	// writ data to cache
	tb_long_t writ = 0;
	while (writ < size)
	{
		// writ data
		tb_long_t real = tb_gstream_awrit(gstream, data + writ, size - writ);	
		if (real > 0) writ += real;
		else if (!real)
		{
			// wait
			tb_long_t e = tb_gstream_wait(gstream, TB_GSTREAM_WAIT_WRIT, tb_stream_timeout(gstream));
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has writ?
			tb_assert_and_check_break(e & TB_GSTREAM_WAIT_WRIT);
		}
		else break;
	}

	// ok?
	return (writ == size? tb_true : tb_false);
}
tb_bool_t tb_gstream_sync(tb_gstream_t* gstream, tb_bool_t bclosing)
{
	// check stream
	tb_assert_and_check_return_val(gstream && tb_stream_is_opened(gstream), tb_false);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gstream->cache), tb_false);

	// the cache is readed-cache now, need call afread or bfread first.
	tb_assert_and_check_return_val(gstream->bwrited || tb_qbuffer_null(&gstream->cache), tb_false); 

	// sync it
	return tb_gstream_cache_sync(gstream, bclosing);
}
tb_long_t tb_gstream_bread_line(tb_gstream_t* gstream, tb_char_t* data, tb_size_t size)
{
	tb_char_t 	ch = 0;
	tb_char_t* 	p = data;
	while (1)
	{
		// read char
		ch = tb_gstream_bread_s8(gstream);

		// is line?
		if (ch == '\n') 
		{
			// finish line
			if (p > data && p[-1] == '\r')
				p--;
			*p = '\0';
	
			// ok
			return p - data;
		}
		// append char to line
		else 
		{
			if ((p - data) < size - 1)
			*p++ = ch;

			// no data?
			if (!ch) break;
		}
	}

	// end?
	return tb_gstream_beof(gstream)? -1 : 0;
}
tb_long_t tb_gstream_bwrit_line(tb_gstream_t* gstream, tb_char_t* data, tb_size_t size)
{
	// writ data
	tb_long_t writ = 0;
	if (size) 
	{
		if (!tb_gstream_bwrit(gstream, data, size)) return -1;
	}
	else
	{
		tb_char_t* p = data;
		while (*p)
		{
			if (!tb_gstream_bwrit(gstream, p, 1)) return -1;
			p++;
		}
	
		writ = p - data;
	}

	// writ "\r\n" or "\n"
#ifdef TB_CONFIG_OS_WINDOWS
	tb_char_t le[] = "\r\n";
	tb_size_t ln = 2;
#else
	tb_char_t le[] = "\n";
	tb_size_t ln = 1;
#endif
	if (!tb_gstream_bwrit(gstream, le, ln)) return -1;
	writ += ln;

	// ok
	return writ;
}

tb_long_t tb_gstream_printf(tb_gstream_t* gstream, tb_char_t const* fmt, ...)
{
	// init data
	tb_char_t data[TB_GSTREAM_BLOCK_MAXN] = {0};
	tb_size_t size = 0;

	// format data
    tb_vsnprintf_format(data, TB_GSTREAM_BLOCK_MAXN, fmt, &size);
	tb_check_return_val(size, 0);

	// writ data
	return tb_gstream_bwrit(gstream, data, size)? size : -1;
}
tb_bool_t tb_gstream_seek(tb_gstream_t* gstream, tb_hize_t offset)
{
	// check stream
	tb_assert_and_check_return_val(gstream && tb_stream_is_opened(gstream), tb_false);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gstream->cache), tb_false);

	// need seek?
	tb_hize_t curt = tb_stream_offset(gstream);
	tb_check_return_val(offset != curt, tb_true);

	// try seek to cache
	tb_bool_t ok = tb_gstream_cache_seek(gstream, offset);

	// failed? try to read and seek
	if (!ok)
	{
		// must be read mode
		tb_assert_and_check_return_val(!gstream->bwrited, tb_false);

		// limit offset
		tb_hong_t size = tb_stream_size(gstream);
		if (size >= 0 && offset > size) offset = size;

		// forward it?
		tb_assert_and_check_return_val(offset > curt, tb_false);

		// read some data for updating offset
		tb_byte_t data[TB_GSTREAM_BLOCK_MAXN];
		tb_size_t need = tb_min(offset - curt, TB_GSTREAM_BLOCK_MAXN);
		if (!tb_gstream_bread(gstream, data, need)) return tb_false;
	}

	// ok?
	return tb_stream_offset(gstream) == offset? tb_true : tb_false;
}
tb_bool_t tb_gstream_skip(tb_gstream_t* gstream, tb_hize_t size)
{
	return tb_gstream_seek(gstream, tb_stream_offset(gstream) + size);
}
tb_uint8_t tb_gstream_bread_u8(tb_gstream_t* gstream)
{
	tb_byte_t b[1];
	if (!tb_gstream_bread(gstream, b, 1)) return 0;
	return b[0];
}
tb_sint8_t tb_gstream_bread_s8(tb_gstream_t* gstream)
{
	tb_byte_t b[1];
	if (!tb_gstream_bread(gstream, b, 1)) return 0;
	return b[0];
}

tb_uint16_t tb_gstream_bread_u16_le(tb_gstream_t* gstream)
{	
	tb_byte_t b[2];
	if (!tb_gstream_bread(gstream, b, 2)) return 0;
	return tb_bits_get_u16_le(b);
}
tb_sint16_t tb_gstream_bread_s16_le(tb_gstream_t* gstream)
{	
	tb_byte_t b[2];
	if (!tb_gstream_bread(gstream, b, 2)) return 0;
	return tb_bits_get_s16_le(b);
}
tb_uint32_t tb_gstream_bread_u24_le(tb_gstream_t* gstream)
{	
	tb_byte_t b[3];
	if (!tb_gstream_bread(gstream, b, 3)) return 0;
	return tb_bits_get_u24_le(b);
}
tb_sint32_t tb_gstream_bread_s24_le(tb_gstream_t* gstream)
{
	tb_byte_t b[3];
	if (!tb_gstream_bread(gstream, b, 3)) return 0;
	return tb_bits_get_s24_le(b);
}
tb_uint32_t tb_gstream_bread_u32_le(tb_gstream_t* gstream)
{
	tb_byte_t b[4];
	if (!tb_gstream_bread(gstream, b, 4)) return 0;
	return tb_bits_get_u32_le(b);
}
tb_sint32_t tb_gstream_bread_s32_le(tb_gstream_t* gstream)
{	
	tb_byte_t b[4];
	if (!tb_gstream_bread(gstream, b, 4)) return 0;
	return tb_bits_get_s32_le(b);
}
tb_uint64_t tb_gstream_bread_u64_le(tb_gstream_t* gstream)
{
	tb_byte_t b[8];
	if (!tb_gstream_bread(gstream, b, 8)) return 0;
	return tb_bits_get_u64_le(b);
}
tb_sint64_t tb_gstream_bread_s64_le(tb_gstream_t* gstream)
{	
	tb_byte_t b[8];
	if (!tb_gstream_bread(gstream, b, 8)) return 0;
	return tb_bits_get_s64_le(b);
}
tb_uint16_t tb_gstream_bread_u16_be(tb_gstream_t* gstream)
{	
	tb_byte_t b[2];
	if (!tb_gstream_bread(gstream, b, 2)) return 0;
	return tb_bits_get_u16_be(b);
}
tb_sint16_t tb_gstream_bread_s16_be(tb_gstream_t* gstream)
{	
	tb_byte_t b[2];
	if (!tb_gstream_bread(gstream, b, 2)) return 0;
	return tb_bits_get_s16_be(b);
}
tb_uint32_t tb_gstream_bread_u24_be(tb_gstream_t* gstream)
{	
	tb_byte_t b[3];
	if (!tb_gstream_bread(gstream, b, 3)) return 0;
	return tb_bits_get_u24_be(b);
}
tb_sint32_t tb_gstream_bread_s24_be(tb_gstream_t* gstream)
{
	tb_byte_t b[3];
	if (!tb_gstream_bread(gstream, b, 3)) return 0;
	return tb_bits_get_s24_be(b);
}
tb_uint32_t tb_gstream_bread_u32_be(tb_gstream_t* gstream)
{
	tb_byte_t b[4];
	if (!tb_gstream_bread(gstream, b, 4)) return 0;
	return tb_bits_get_u32_be(b);
}
tb_sint32_t tb_gstream_bread_s32_be(tb_gstream_t* gstream)
{	
	tb_byte_t b[4];
	if (!tb_gstream_bread(gstream, b, 4)) return 0;
	return tb_bits_get_s32_be(b);
}
tb_uint64_t tb_gstream_bread_u64_be(tb_gstream_t* gstream)
{
	tb_byte_t b[8];
	if (!tb_gstream_bread(gstream, b, 8)) return 0;
	return tb_bits_get_u64_be(b);
}
tb_sint64_t tb_gstream_bread_s64_be(tb_gstream_t* gstream)
{	
	tb_byte_t b[8];
	if (!tb_gstream_bread(gstream, b, 8)) return 0;
	return tb_bits_get_s64_be(b);
}
tb_bool_t tb_gstream_bwrit_u8(tb_gstream_t* gstream, tb_uint8_t val)
{
	tb_byte_t b[1];
	tb_bits_set_u8(b, val);
	return tb_gstream_bwrit(gstream, b, 1);
}
tb_bool_t tb_gstream_bwrit_s8(tb_gstream_t* gstream, tb_sint8_t val)
{
	tb_byte_t b[1];
	tb_bits_set_s8(b, val);
	return tb_gstream_bwrit(gstream, b, 1);
}

tb_bool_t tb_gstream_bwrit_u16_le(tb_gstream_t* gstream, tb_uint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_u16_le(b, val);
	return tb_gstream_bwrit(gstream, b, 2);
}
tb_bool_t tb_gstream_bwrit_s16_le(tb_gstream_t* gstream, tb_sint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_s16_le(b, val);
	return tb_gstream_bwrit(gstream, b, 2);
}

tb_bool_t tb_gstream_bwrit_u24_le(tb_gstream_t* gstream, tb_uint32_t val)
{	
	tb_byte_t b[3];
	tb_bits_set_u24_le(b, val);
	return tb_gstream_bwrit(gstream, b, 3);
}
tb_bool_t tb_gstream_bwrit_s24_le(tb_gstream_t* gstream, tb_sint32_t val)
{
	tb_byte_t b[3];
	tb_bits_set_s24_le(b, val);
	return tb_gstream_bwrit(gstream, b, 3);
}

tb_bool_t tb_gstream_bwrit_u32_le(tb_gstream_t* gstream, tb_uint32_t val)
{	
	tb_byte_t b[4];
	tb_bits_set_u32_le(b, val);
	return tb_gstream_bwrit(gstream, b, 4);
}
tb_bool_t tb_gstream_bwrit_s32_le(tb_gstream_t* gstream, tb_sint32_t val)
{
	tb_byte_t b[4];
	tb_bits_set_s32_le(b, val);
	return tb_gstream_bwrit(gstream, b, 4);
}
tb_bool_t tb_gstream_bwrit_u64_le(tb_gstream_t* gstream, tb_uint64_t val)
{	
	tb_byte_t b[8];
	tb_bits_set_u64_le(b, val);
	return tb_gstream_bwrit(gstream, b, 8);
}
tb_bool_t tb_gstream_bwrit_s64_le(tb_gstream_t* gstream, tb_sint64_t val)
{
	tb_byte_t b[8];
	tb_bits_set_s64_le(b, val);
	return tb_gstream_bwrit(gstream, b, 8);
}
tb_bool_t tb_gstream_bwrit_u16_be(tb_gstream_t* gstream, tb_uint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_u16_be(b, val);
	return tb_gstream_bwrit(gstream, b, 2);
}
tb_bool_t tb_gstream_bwrit_s16_be(tb_gstream_t* gstream, tb_sint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_s16_be(b, val);
	return tb_gstream_bwrit(gstream, b, 2);
}

tb_bool_t tb_gstream_bwrit_u24_be(tb_gstream_t* gstream, tb_uint32_t val)
{	
	tb_byte_t b[3];
	tb_bits_set_u24_be(b, val);
	return tb_gstream_bwrit(gstream, b, 3);
}
tb_bool_t tb_gstream_bwrit_s24_be(tb_gstream_t* gstream, tb_sint32_t val)
{
	tb_byte_t b[3];
	tb_bits_set_s24_be(b, val);
	return tb_gstream_bwrit(gstream, b, 3);
}
tb_bool_t tb_gstream_bwrit_u32_be(tb_gstream_t* gstream, tb_uint32_t val)
{	
	tb_byte_t b[4];
	tb_bits_set_u32_be(b, val);
	return tb_gstream_bwrit(gstream, b, 4);
}
tb_bool_t tb_gstream_bwrit_s32_be(tb_gstream_t* gstream, tb_sint32_t val)
{
	tb_byte_t b[4];
	tb_bits_set_s32_be(b, val);
	return tb_gstream_bwrit(gstream, b, 4);
}
tb_bool_t tb_gstream_bwrit_u64_be(tb_gstream_t* gstream, tb_uint64_t val)
{	
	tb_byte_t b[8];
	tb_bits_set_u64_be(b, val);
	return tb_gstream_bwrit(gstream, b, 8);
}
tb_bool_t tb_gstream_bwrit_s64_be(tb_gstream_t* gstream, tb_sint64_t val)
{
	tb_byte_t b[8];
	tb_bits_set_s64_be(b, val);
	return tb_gstream_bwrit(gstream, b, 8);
}

