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
 * @file		basic_stream.c
 * @ingroup 	stream
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 			"bstream"
#define TB_TRACE_MODULE_DEBUG 			(0)

/* //////////////////////////////////////////////////////////////////////////////////////
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

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_basic_stream_t* tb_basic_stream_init_from_url(tb_char_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// the init
	static tb_basic_stream_t* (*g_init[])() = 
	{
		tb_null
	,	tb_basic_stream_init_file
	,	tb_basic_stream_init_sock
	,	tb_basic_stream_init_http
	,	tb_basic_stream_init_data
	};

	// init
	tb_char_t const* 	p = url;
	tb_basic_stream_t* 		bstream = tb_null;
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
		tb_trace_d("[bstream]: unknown prefix for url: %s", url);
		return tb_null;
	}
	tb_assert_and_check_goto(type && type < tb_arrayn(g_init) && g_init[type], fail);

	// init stream
	bstream = g_init[type]();
	tb_assert_and_check_goto(bstream, fail);

	// set url
	if (!tb_stream_ctrl(bstream, TB_STREAM_CTRL_SET_URL, url)) goto fail;

	// ok
	return bstream;

fail:
	
	// exit stream
	if (bstream) tb_basic_stream_exit(bstream);
	return tb_null;
}
tb_void_t tb_basic_stream_exit(tb_basic_stream_t* bstream)
{
	if (bstream) 
	{
		// close it
		tb_basic_stream_clos(bstream);

		// exit it
		if (bstream->exit) bstream->exit(bstream);

		// exit cache
		tb_queue_buffer_exit(&bstream->cache);

		// exit url
		tb_url_exit(&bstream->base.url);

		// free it
		tb_free(bstream);
	}
}
tb_long_t tb_basic_stream_wait(tb_basic_stream_t* bstream, tb_size_t wait, tb_long_t timeout)
{
	// check
	tb_assert_and_check_return_val(bstream && bstream->wait, -1);

	// stoped?
	tb_assert_and_check_return_val(!tb_atomic_get(&bstream->base.bstoped), -1);

	// wait it
	tb_long_t ok = bstream->wait(bstream, wait, timeout);
	
	// wait failed? save state
	if (ok < 0 && !bstream->state) bstream->state = TB_STATE_WAIT_FAILED;

	// ok?
	tb_check_return_val(!ok, ok);

	// cached?
	if (tb_queue_buffer_maxn(&bstream->cache))
	{
		// have read cache?
		if ((wait & TB_BASIC_STREAM_WAIT_READ) && !bstream->bwrited && !tb_queue_buffer_null(&bstream->cache)) 
			ok |= TB_BASIC_STREAM_WAIT_READ;
		// have writ cache?
		else if ((wait & TB_BASIC_STREAM_WAIT_WRIT) && bstream->bwrited && !tb_queue_buffer_full(&bstream->cache))
			ok |= TB_BASIC_STREAM_WAIT_WRIT;
	}

	// ok?
	return ok;
}
tb_size_t tb_basic_stream_state(tb_basic_stream_t* bstream)
{
	// check
	tb_assert_and_check_return_val(bstream, TB_STATE_UNKNOWN_ERROR);

	// the stream state
	return bstream->state;
}
tb_bool_t tb_basic_stream_open(tb_basic_stream_t* bstream)
{
	// check stream
	tb_assert_and_check_return_val(bstream && bstream->open, tb_false);

	// already been opened?
	tb_check_return_val(!tb_stream_is_opened(bstream), tb_true);

	// stoped?
	tb_assert_and_check_return_val(tb_atomic_get(&bstream->base.bstoped), tb_false);

	// init offset
	bstream->offset = 0;

	// init state
	bstream->state = TB_STATE_OK;

	// clear stoped
	tb_atomic_set0(&bstream->base.bstoped);

	// open it
	tb_bool_t ok = bstream->open(bstream);

	// opened
	if (ok) bstream->base.bopened = 1;
	// stoped
	else tb_atomic_set(&bstream->base.bstoped, 1);

	// ok?
	return ok;
}
tb_bool_t tb_basic_stream_clos(tb_basic_stream_t* bstream)
{
	// check stream
	tb_assert_and_check_return_val(bstream, tb_false);

	// already been closed?
	tb_check_return_val(tb_stream_is_opened(bstream), tb_true);

	// flush writed data first
	if (bstream->bwrited && !tb_basic_stream_sync(bstream, tb_true)) return tb_false;

	// has close?
	if (bstream->clos && !bstream->clos(bstream)) return tb_false;

	// clear state
	bstream->offset = 0;
	bstream->bwrited = 0;
	bstream->base.bopened = 0;
	bstream->state = TB_STATE_OK;
	tb_atomic_set(&bstream->base.bstoped, 1);

	// clear cache
	tb_queue_buffer_clear(&bstream->cache);

	// ok
	return tb_true;
}
tb_bool_t tb_basic_stream_need(tb_basic_stream_t* bstream, tb_byte_t** data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data && size, tb_false);

	// check stream
	tb_assert_and_check_return_val(bstream && tb_stream_is_opened(bstream) && bstream->read, tb_false);

	// stoped?
	tb_assert_and_check_return_val(!tb_atomic_get(&bstream->base.bstoped), tb_false);

	// must enable cache
	tb_assert_and_check_return_val(tb_queue_buffer_maxn(&bstream->cache) && size <= tb_queue_buffer_maxn(&bstream->cache), tb_false);

	// have writed cache? sync first
	if (bstream->bwrited && !tb_queue_buffer_null(&bstream->cache) && !tb_basic_stream_sync(bstream, tb_false)) return tb_false;

	// switch to the read cache mode
	if (bstream->bwrited && tb_queue_buffer_null(&bstream->cache)) bstream->bwrited = 0;

	// check the cache mode, must be read cache
	tb_assert_and_check_return_val(!bstream->bwrited, tb_false);

	// enough?
	if (size <= tb_queue_buffer_size(&bstream->cache)) 
	{
		// save data
		*data = tb_queue_buffer_head(&bstream->cache);

		// ok
		return tb_true;
	}

	// enter cache for push
	tb_size_t 	push = 0;
	tb_size_t 	need = size - tb_queue_buffer_size(&bstream->cache);
	tb_byte_t* 	tail = tb_queue_buffer_push_init(&bstream->cache, &push);
	tb_assert_and_check_return_val(tail && push, tb_false);
	if (push > need) push = need;

	// fill cache
	tb_size_t read = 0;
	while (read < push && !tb_atomic_get(&bstream->base.bstoped))
	{
		// read data
		tb_long_t real = bstream->read(bstream, tail + read, push - read);
		
		// ok?
		if (real > 0)
		{
			// save read
			read += real;
		}
		// no data?
		else if (!real)
		{
			// wait
			real = bstream->wait(bstream, TB_BASIC_STREAM_WAIT_READ, tb_stream_timeout(bstream));

			// ok?
			tb_check_break(real > 0);
		}
		else break;
	}
	
	// leave cache for push
	tb_queue_buffer_push_exit(&bstream->cache, read);

	// not enough?
	if (size > tb_queue_buffer_size(&bstream->cache))
	{
		// killed? save state
		if (!bstream->state && tb_atomic_get(&bstream->base.bstoped))
			bstream->state = TB_STATE_KILLED;

		// failed
		return tb_false;
	}

	// save data
	*data = tb_queue_buffer_head(&bstream->cache);

	// ok
	return tb_true;
}
tb_long_t tb_basic_stream_aread(tb_basic_stream_t* bstream, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);

	// no size?
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(bstream && tb_stream_is_opened(bstream) && bstream->read, -1);

	// done
	tb_long_t read = 0;
	do
	{
		if (tb_queue_buffer_maxn(&bstream->cache))
		{
			// switch to the read cache mode
			if (bstream->bwrited && tb_queue_buffer_null(&bstream->cache)) bstream->bwrited = 0;

			// check the cache mode, must be read cache
			tb_assert_and_check_return_val(!bstream->bwrited, -1);

			// read data from cache first
			read = tb_queue_buffer_read(&bstream->cache, data, size);
			tb_check_return_val(read >= 0, -1);

			// ok?
			tb_check_break(!read);

			// cache is null now.
			tb_assert_and_check_return_val(tb_queue_buffer_null(&bstream->cache), -1);

			// enter cache for push
			tb_size_t 	push = 0;
			tb_byte_t* 	tail = tb_queue_buffer_push_init(&bstream->cache, &push);
			tb_assert_and_check_return_val(tail && push, -1);

			// push data to cache from stream
			tb_assert(bstream->read);
			tb_long_t 	real = bstream->read(bstream, tail, push);
			tb_check_return_val(real >= 0, -1);

			// read the left data from cache
			if (real > 0) 
			{
				// leave cache for push
				tb_queue_buffer_push_exit(&bstream->cache, real);

				// read cache
				real = tb_queue_buffer_read(&bstream->cache, data + read, tb_min(real, size - read));
				tb_check_return_val(real >= 0, -1);

				// save read 
				read += real;
			}
		}
		else 
		{
			// read it directly
			read = bstream->read(bstream, data, size);
			tb_check_return_val(read >= 0, -1);
		}
	}
	while (0);

	// update offset
	bstream->offset += read;

//	tb_trace_d("read: %d", read);
	return read;
}
tb_long_t tb_basic_stream_awrit(tb_basic_stream_t* bstream, tb_byte_t const* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);

	// no size?
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(bstream && tb_stream_is_opened(bstream) && bstream->writ, -1);

	// done
	tb_long_t writ = 0;
	do
	{
		if (tb_queue_buffer_maxn(&bstream->cache))
		{
			// switch to the writ cache mode
			if (!bstream->bwrited && tb_queue_buffer_null(&bstream->cache)) bstream->bwrited = 1;

			// check the cache mode, must be writ cache
			tb_assert_and_check_return_val(bstream->bwrited, -1);

			// writ data to cache first
			writ = tb_queue_buffer_writ(&bstream->cache, data, size);
			tb_check_return_val(writ >= 0, -1);
			
			// ok?
			tb_check_break(!writ);

			// cache is full now.
			tb_assert_and_check_return_val(tb_queue_buffer_full(&bstream->cache), -1);

			// enter cache for pull
			tb_size_t 	pull = 0;
			tb_byte_t* 	head = tb_queue_buffer_pull_init(&bstream->cache, &pull);
			tb_assert_and_check_return_val(head && pull, -1);

			// pull data to stream from cache
			tb_long_t 	real = bstream->writ(bstream, head, pull);
			tb_check_return_val(real >= 0, -1);

			// writ the left data to cache
			if (real > 0)
			{
				// leave cache for pull
				tb_queue_buffer_pull_exit(&bstream->cache, real);

				// writ cache
				real = tb_queue_buffer_writ(&bstream->cache, data + writ, tb_min(real, size - writ));
				tb_check_return_val(real >= 0, -1);

				// save writ 
				writ += real;
			}
		}
		else 
		{
			// writ it directly
			writ = bstream->writ(bstream, data, size);
			tb_check_return_val(writ >= 0, -1);
		}

	} while (0);

	// update offset
	bstream->offset += writ;

//	tb_trace_d("writ: %d", writ);
	return writ;
}
tb_bool_t tb_basic_stream_bread(tb_basic_stream_t* bstream, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(bstream && data, tb_false);
	tb_check_return_val(size, tb_true);

	// have writed cache? sync first
	if (bstream->bwrited && !tb_queue_buffer_null(&bstream->cache) && !tb_basic_stream_sync(bstream, tb_false))
		return tb_false;

	// check the left
	tb_hize_t left = tb_stream_left(bstream);
	tb_check_return_val(size <= left, tb_false);

	// read data from cache
	tb_long_t read = 0;
	while (read < size && !tb_atomic_get(&bstream->base.bstoped))
	{
		// read data
		tb_long_t real = tb_basic_stream_aread(bstream, data + read, size - read);	
		if (real > 0) read += real;
		else if (!real)
		{
			// wait
			real = tb_basic_stream_wait(bstream, TB_BASIC_STREAM_WAIT_READ, tb_stream_timeout(bstream));
			tb_check_break(real > 0);

			// has read?
			tb_assert_and_check_break(real & TB_BASIC_STREAM_WAIT_READ);
		}
		else break;
	}

	// killed? save state
	if (read != size && !bstream->state && tb_atomic_get(&bstream->base.bstoped))
		bstream->state = TB_STATE_KILLED;

	// ok?
	return (read == size? tb_true : tb_false);
}
tb_bool_t tb_basic_stream_bwrit(tb_basic_stream_t* bstream, tb_byte_t const* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(bstream && data, tb_false);
	tb_check_return_val(size, tb_true);

	// writ data to cache
	tb_long_t writ = 0;
	while (writ < size && !tb_atomic_get(&bstream->base.bstoped))
	{
		// writ data
		tb_long_t real = tb_basic_stream_awrit(bstream, data + writ, size - writ);	
		if (real > 0) writ += real;
		else if (!real)
		{
			// wait
			real = tb_basic_stream_wait(bstream, TB_BASIC_STREAM_WAIT_WRIT, tb_stream_timeout(bstream));
			tb_check_break(real > 0);

			// has writ?
			tb_assert_and_check_break(real & TB_BASIC_STREAM_WAIT_WRIT);
		}
		else break;
	}

	// killed? save state
	if (writ != size && !bstream->state && tb_atomic_get(&bstream->base.bstoped))
		bstream->state = TB_STATE_KILLED;

	// ok?
	return (writ == size? tb_true : tb_false);
}
tb_bool_t tb_basic_stream_sync(tb_basic_stream_t* bstream, tb_bool_t bclosing)
{
	// check stream
	tb_assert_and_check_return_val(bstream && bstream->writ && tb_stream_is_opened(bstream), tb_false);

	// stoped?
	tb_assert_and_check_return_val(!tb_atomic_get(&bstream->base.bstoped), tb_false);

	// cached? sync cache first
	if (tb_queue_buffer_maxn(&bstream->cache))
	{
		// have data?
		if (!tb_queue_buffer_null(&bstream->cache))
		{
			// check: must be writed cache
			tb_assert_and_check_return_val(bstream->bwrited, tb_false);

			// enter cache for pull
			tb_size_t 	size = 0;
			tb_byte_t* 	head = tb_queue_buffer_pull_init(&bstream->cache, &size);
			tb_assert_and_check_return_val(head && size, tb_false);

			// writ cache data to stream
			tb_size_t 	writ = 0;
			while (writ < size && !tb_atomic_get(&bstream->base.bstoped))
			{
				// writ
				tb_long_t real = bstream->writ(bstream, head + writ, size - writ);

				// ok?
				if (real > 0)
				{
					// save writ
					writ += real;
				}
				// no data?
				else if (!real)
				{
					// wait
					real = bstream->wait(bstream, TB_BASIC_STREAM_WAIT_WRIT, tb_stream_timeout(bstream));

					// ok?
					tb_check_break(real > 0);
				}
				// error or end?
				else break;
			}

			// leave cache for pull
			tb_queue_buffer_pull_exit(&bstream->cache, writ);

			// cache be not cleared?
			if (!tb_queue_buffer_null(&bstream->cache))
			{
				// killed? save state
				if (!bstream->state && tb_atomic_get(&bstream->base.bstoped))
					bstream->state = TB_STATE_KILLED;

				// failed
				return tb_false;
			}
		}
		else bstream->bwrited = 1;
	}

	// sync
	return bstream->sync? bstream->sync(bstream, bclosing) : tb_true;
}
tb_bool_t tb_basic_stream_seek(tb_basic_stream_t* bstream, tb_hize_t offset)
{
	// check stream
	tb_assert_and_check_return_val(bstream && tb_stream_is_opened(bstream), tb_false);

	// stoped?
	tb_assert_and_check_return_val(!tb_atomic_get(&bstream->base.bstoped), tb_false);

	// limit offset
	tb_hong_t size = tb_stream_size(bstream);
	if (size >= 0 && offset > size) offset = size;

	// the offset be not changed?
	tb_hize_t curt = tb_stream_offset(bstream);
	tb_check_return_val(offset != curt, tb_true);

	// for writing
	if (bstream->bwrited)
	{
		// sync writed data first
		if (!tb_basic_stream_sync(bstream, tb_false)) return tb_false;

		// check cache, must not cache or empty cache
		tb_assert_and_check_return_val(!tb_queue_buffer_maxn(&bstream->cache) || tb_queue_buffer_null(&bstream->cache), tb_false);

		// seek it
		tb_bool_t ok = bstream->seek? bstream->seek(bstream, offset) : tb_false;

		// save offset
		if (ok) bstream->offset = offset;
	}
	// for reading
	else
	{
		// cached? try to seek it at the cache
		tb_bool_t ok = tb_false;
		if (tb_queue_buffer_maxn(&bstream->cache))
		{
			tb_size_t 	size = 0;
			tb_byte_t* 	data = tb_queue_buffer_pull_init(&bstream->cache, &size);
			if (data && size && offset > curt && offset < curt + size)
			{
				// seek it at the cache
				tb_queue_buffer_pull_exit(&bstream->cache, (tb_size_t)(offset - curt));

				// save offset
				bstream->offset = offset;
				
				// ok
				ok = tb_true;
			}
		}

		// seek it
		if (!ok)
		{
			// seek it
			ok = bstream->seek? bstream->seek(bstream, offset) : tb_false;

			// ok?
			if (ok)
			{
				// save offset
				bstream->offset = offset;
	
				// clear cache
				tb_queue_buffer_clear(&bstream->cache);
			}
		}

		// try to read and seek
		if (!ok && offset > curt)
		{
			// read some data for updating offset
			tb_byte_t data[TB_BASIC_STREAM_BLOCK_MAXN];
			while (tb_stream_offset(bstream) != offset)
			{
				tb_size_t need = tb_min(offset - curt, TB_BASIC_STREAM_BLOCK_MAXN);
				if (!tb_basic_stream_bread(bstream, data, need)) return tb_false;
			}
		}
	}

	// ok?
	return tb_stream_offset(bstream) == offset? tb_true : tb_false;
}
tb_bool_t tb_basic_stream_skip(tb_basic_stream_t* bstream, tb_hize_t size)
{
	return tb_basic_stream_seek(bstream, tb_stream_offset(bstream) + size);
}
tb_long_t tb_basic_stream_bread_line(tb_basic_stream_t* bstream, tb_char_t* data, tb_size_t size)
{
	// done
	tb_char_t 	ch = 0;
	tb_char_t* 	p = data;
	while (!tb_atomic_get(&bstream->base.bstoped))
	{
		// read char
		ch = tb_basic_stream_bread_s8(bstream);

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

	// killed?
	if (tb_atomic_get(&bstream->base.bstoped)) return -1;

	// end?
	return tb_stream_beof(bstream)? -1 : 0;
}
tb_long_t tb_basic_stream_bwrit_line(tb_basic_stream_t* bstream, tb_char_t* data, tb_size_t size)
{
	// writ data
	tb_long_t writ = 0;
	if (size) 
	{
		if (!tb_basic_stream_bwrit(bstream, (tb_byte_t*)data, size)) return -1;
	}
	else
	{
		tb_char_t* p = data;
		while (*p)
		{
			if (!tb_basic_stream_bwrit(bstream, (tb_byte_t*)p, 1)) return -1;
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
	if (!tb_basic_stream_bwrit(bstream, (tb_byte_t*)le, ln)) return -1;
	writ += ln;

	// ok
	return writ;
}
tb_long_t tb_basic_stream_printf(tb_basic_stream_t* bstream, tb_char_t const* fmt, ...)
{
	// init data
	tb_char_t data[TB_BASIC_STREAM_BLOCK_MAXN] = {0};
	tb_size_t size = 0;

	// format data
    tb_vsnprintf_format(data, TB_BASIC_STREAM_BLOCK_MAXN, fmt, &size);
	tb_check_return_val(size, 0);

	// writ data
	return tb_basic_stream_bwrit(bstream, (tb_byte_t*)data, size)? size : -1;
}
tb_uint8_t tb_basic_stream_bread_u8(tb_basic_stream_t* bstream)
{
	tb_byte_t b[1];
	if (!tb_basic_stream_bread(bstream, b, 1)) return 0;
	return b[0];
}
tb_sint8_t tb_basic_stream_bread_s8(tb_basic_stream_t* bstream)
{
	tb_byte_t b[1];
	if (!tb_basic_stream_bread(bstream, b, 1)) return 0;
	return b[0];
}

tb_uint16_t tb_basic_stream_bread_u16_le(tb_basic_stream_t* bstream)
{	
	tb_byte_t b[2];
	if (!tb_basic_stream_bread(bstream, b, 2)) return 0;
	return tb_bits_get_u16_le(b);
}
tb_sint16_t tb_basic_stream_bread_s16_le(tb_basic_stream_t* bstream)
{	
	tb_byte_t b[2];
	if (!tb_basic_stream_bread(bstream, b, 2)) return 0;
	return tb_bits_get_s16_le(b);
}
tb_uint32_t tb_basic_stream_bread_u24_le(tb_basic_stream_t* bstream)
{	
	tb_byte_t b[3];
	if (!tb_basic_stream_bread(bstream, b, 3)) return 0;
	return tb_bits_get_u24_le(b);
}
tb_sint32_t tb_basic_stream_bread_s24_le(tb_basic_stream_t* bstream)
{
	tb_byte_t b[3];
	if (!tb_basic_stream_bread(bstream, b, 3)) return 0;
	return tb_bits_get_s24_le(b);
}
tb_uint32_t tb_basic_stream_bread_u32_le(tb_basic_stream_t* bstream)
{
	tb_byte_t b[4];
	if (!tb_basic_stream_bread(bstream, b, 4)) return 0;
	return tb_bits_get_u32_le(b);
}
tb_sint32_t tb_basic_stream_bread_s32_le(tb_basic_stream_t* bstream)
{	
	tb_byte_t b[4];
	if (!tb_basic_stream_bread(bstream, b, 4)) return 0;
	return tb_bits_get_s32_le(b);
}
tb_uint64_t tb_basic_stream_bread_u64_le(tb_basic_stream_t* bstream)
{
	tb_byte_t b[8];
	if (!tb_basic_stream_bread(bstream, b, 8)) return 0;
	return tb_bits_get_u64_le(b);
}
tb_sint64_t tb_basic_stream_bread_s64_le(tb_basic_stream_t* bstream)
{	
	tb_byte_t b[8];
	if (!tb_basic_stream_bread(bstream, b, 8)) return 0;
	return tb_bits_get_s64_le(b);
}
tb_uint16_t tb_basic_stream_bread_u16_be(tb_basic_stream_t* bstream)
{	
	tb_byte_t b[2];
	if (!tb_basic_stream_bread(bstream, b, 2)) return 0;
	return tb_bits_get_u16_be(b);
}
tb_sint16_t tb_basic_stream_bread_s16_be(tb_basic_stream_t* bstream)
{	
	tb_byte_t b[2];
	if (!tb_basic_stream_bread(bstream, b, 2)) return 0;
	return tb_bits_get_s16_be(b);
}
tb_uint32_t tb_basic_stream_bread_u24_be(tb_basic_stream_t* bstream)
{	
	tb_byte_t b[3];
	if (!tb_basic_stream_bread(bstream, b, 3)) return 0;
	return tb_bits_get_u24_be(b);
}
tb_sint32_t tb_basic_stream_bread_s24_be(tb_basic_stream_t* bstream)
{
	tb_byte_t b[3];
	if (!tb_basic_stream_bread(bstream, b, 3)) return 0;
	return tb_bits_get_s24_be(b);
}
tb_uint32_t tb_basic_stream_bread_u32_be(tb_basic_stream_t* bstream)
{
	tb_byte_t b[4];
	if (!tb_basic_stream_bread(bstream, b, 4)) return 0;
	return tb_bits_get_u32_be(b);
}
tb_sint32_t tb_basic_stream_bread_s32_be(tb_basic_stream_t* bstream)
{	
	tb_byte_t b[4];
	if (!tb_basic_stream_bread(bstream, b, 4)) return 0;
	return tb_bits_get_s32_be(b);
}
tb_uint64_t tb_basic_stream_bread_u64_be(tb_basic_stream_t* bstream)
{
	tb_byte_t b[8];
	if (!tb_basic_stream_bread(bstream, b, 8)) return 0;
	return tb_bits_get_u64_be(b);
}
tb_sint64_t tb_basic_stream_bread_s64_be(tb_basic_stream_t* bstream)
{	
	tb_byte_t b[8];
	if (!tb_basic_stream_bread(bstream, b, 8)) return 0;
	return tb_bits_get_s64_be(b);
}
tb_bool_t tb_basic_stream_bwrit_u8(tb_basic_stream_t* bstream, tb_uint8_t val)
{
	tb_byte_t b[1];
	tb_bits_set_u8(b, val);
	return tb_basic_stream_bwrit(bstream, b, 1);
}
tb_bool_t tb_basic_stream_bwrit_s8(tb_basic_stream_t* bstream, tb_sint8_t val)
{
	tb_byte_t b[1];
	tb_bits_set_s8(b, val);
	return tb_basic_stream_bwrit(bstream, b, 1);
}

tb_bool_t tb_basic_stream_bwrit_u16_le(tb_basic_stream_t* bstream, tb_uint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_u16_le(b, val);
	return tb_basic_stream_bwrit(bstream, b, 2);
}
tb_bool_t tb_basic_stream_bwrit_s16_le(tb_basic_stream_t* bstream, tb_sint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_s16_le(b, val);
	return tb_basic_stream_bwrit(bstream, b, 2);
}

tb_bool_t tb_basic_stream_bwrit_u24_le(tb_basic_stream_t* bstream, tb_uint32_t val)
{	
	tb_byte_t b[3];
	tb_bits_set_u24_le(b, val);
	return tb_basic_stream_bwrit(bstream, b, 3);
}
tb_bool_t tb_basic_stream_bwrit_s24_le(tb_basic_stream_t* bstream, tb_sint32_t val)
{
	tb_byte_t b[3];
	tb_bits_set_s24_le(b, val);
	return tb_basic_stream_bwrit(bstream, b, 3);
}

tb_bool_t tb_basic_stream_bwrit_u32_le(tb_basic_stream_t* bstream, tb_uint32_t val)
{	
	tb_byte_t b[4];
	tb_bits_set_u32_le(b, val);
	return tb_basic_stream_bwrit(bstream, b, 4);
}
tb_bool_t tb_basic_stream_bwrit_s32_le(tb_basic_stream_t* bstream, tb_sint32_t val)
{
	tb_byte_t b[4];
	tb_bits_set_s32_le(b, val);
	return tb_basic_stream_bwrit(bstream, b, 4);
}
tb_bool_t tb_basic_stream_bwrit_u64_le(tb_basic_stream_t* bstream, tb_uint64_t val)
{	
	tb_byte_t b[8];
	tb_bits_set_u64_le(b, val);
	return tb_basic_stream_bwrit(bstream, b, 8);
}
tb_bool_t tb_basic_stream_bwrit_s64_le(tb_basic_stream_t* bstream, tb_sint64_t val)
{
	tb_byte_t b[8];
	tb_bits_set_s64_le(b, val);
	return tb_basic_stream_bwrit(bstream, b, 8);
}
tb_bool_t tb_basic_stream_bwrit_u16_be(tb_basic_stream_t* bstream, tb_uint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_u16_be(b, val);
	return tb_basic_stream_bwrit(bstream, b, 2);
}
tb_bool_t tb_basic_stream_bwrit_s16_be(tb_basic_stream_t* bstream, tb_sint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_s16_be(b, val);
	return tb_basic_stream_bwrit(bstream, b, 2);
}

tb_bool_t tb_basic_stream_bwrit_u24_be(tb_basic_stream_t* bstream, tb_uint32_t val)
{	
	tb_byte_t b[3];
	tb_bits_set_u24_be(b, val);
	return tb_basic_stream_bwrit(bstream, b, 3);
}
tb_bool_t tb_basic_stream_bwrit_s24_be(tb_basic_stream_t* bstream, tb_sint32_t val)
{
	tb_byte_t b[3];
	tb_bits_set_s24_be(b, val);
	return tb_basic_stream_bwrit(bstream, b, 3);
}
tb_bool_t tb_basic_stream_bwrit_u32_be(tb_basic_stream_t* bstream, tb_uint32_t val)
{	
	tb_byte_t b[4];
	tb_bits_set_u32_be(b, val);
	return tb_basic_stream_bwrit(bstream, b, 4);
}
tb_bool_t tb_basic_stream_bwrit_s32_be(tb_basic_stream_t* bstream, tb_sint32_t val)
{
	tb_byte_t b[4];
	tb_bits_set_s32_be(b, val);
	return tb_basic_stream_bwrit(bstream, b, 4);
}
tb_bool_t tb_basic_stream_bwrit_u64_be(tb_basic_stream_t* bstream, tb_uint64_t val)
{	
	tb_byte_t b[8];
	tb_bits_set_u64_be(b, val);
	return tb_basic_stream_bwrit(bstream, b, 8);
}
tb_bool_t tb_basic_stream_bwrit_s64_be(tb_basic_stream_t* bstream, tb_sint64_t val)
{
	tb_byte_t b[8];
	tb_bits_set_s64_be(b, val);
	return tb_basic_stream_bwrit(bstream, b, 8);
}

