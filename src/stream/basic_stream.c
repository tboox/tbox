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
#define TB_TRACE_MODULE_NAME 			"basic_stream"
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
tb_bool_t tb_basic_stream_init(tb_basic_stream_t* stream, tb_size_t type, tb_size_t cache)
{
	// check
	tb_assert_and_check_return_val(stream, tb_false);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// init mode
		stream->base.mode = TB_STREAM_MODE_AIOO;

		// init type
		stream->base.type = type;

		// init timeout, 10s
		stream->base.timeout = 10000;

		// init internal state?
		stream->base.istate = TB_STATE_CLOSED;

		// init url
		if (!tb_url_init(&stream->base.url)) break;

		// init cache
		if (!tb_queue_buffer_init(&stream->cache, cache)) break;

		// ok
		ok = tb_true;

	} while (0);

	// failed? exit it
	if (!ok) tb_queue_buffer_exit(&stream->cache);

	// ok?
	return ok;
}
tb_basic_stream_t* tb_basic_stream_init_from_url(tb_char_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// the init
	static tb_basic_stream_t* (*s_init[])() = 
	{
		tb_null
	,	tb_basic_stream_init_file
	,	tb_basic_stream_init_sock
	,	tb_basic_stream_init_http
	,	tb_basic_stream_init_data
	};

    // probe protocol
    tb_size_t protocol = tb_url_protocol_probe(url);

    // protocol => type
	tb_size_t type = TB_STREAM_TYPE_NONE;
    switch (protocol)
    {
    case TB_URL_PROTOCOL_FILE: type = TB_STREAM_TYPE_FILE; break;
    case TB_URL_PROTOCOL_HTTP: type = TB_STREAM_TYPE_HTTP; break;
    case TB_URL_PROTOCOL_SOCK: type = TB_STREAM_TYPE_SOCK; break;
    case TB_URL_PROTOCOL_DATA: type = TB_STREAM_TYPE_DATA; break;
    default:
		tb_trace_e("unknown stream for url: %s", url);
        return tb_null;
    }
	tb_assert_and_check_return_val(type && type < tb_arrayn(s_init) && s_init[type], tb_null);

    // done
    tb_bool_t           ok = tb_false;
	tb_basic_stream_t*  stream = tb_null;
    do
    {
        // init stream
        stream = s_init[type]();
        tb_assert_and_check_break(stream);

        // set url
        if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_SET_URL, url)) break;

    } while (0);

    // failed?
    if (!ok)
    {
	    // exit stream
	    if (stream) tb_basic_stream_exit(stream);
        stream = tb_null;
    }

	// ok?
	return stream;
}
tb_void_t tb_basic_stream_exit(tb_basic_stream_t* stream)
{
	if (stream) 
	{
		// close it
		tb_basic_stream_clos(stream);

		// exit it
		if (stream->exit) stream->exit(stream);

		// exit cache
		tb_queue_buffer_exit(&stream->cache);

		// exit url
		tb_url_exit(&stream->base.url);

		// free it
		tb_free(stream);
	}
}
tb_long_t tb_basic_stream_wait(tb_basic_stream_t* stream, tb_size_t wait, tb_long_t timeout)
{
	// check
	tb_assert_and_check_return_val(stream && stream->wait, -1);

	// stoped?
	tb_assert_and_check_return_val(TB_STATE_OPENED == tb_atomic_get(&stream->base.istate), -1);

	// wait it
	tb_long_t ok = stream->wait(stream, wait, timeout);
	
	// wait failed? save state
	if (ok < 0 && !stream->state) stream->state = TB_STATE_WAIT_FAILED;

	// ok?
	tb_check_return_val(!ok, ok);

	// cached?
	if (tb_queue_buffer_maxn(&stream->cache))
	{
		// have read cache?
		if ((wait & TB_BASIC_STREAM_WAIT_READ) && !stream->bwrited && !tb_queue_buffer_null(&stream->cache)) 
			ok |= TB_BASIC_STREAM_WAIT_READ;
		// have writ cache?
		else if ((wait & TB_BASIC_STREAM_WAIT_WRIT) && stream->bwrited && !tb_queue_buffer_full(&stream->cache))
			ok |= TB_BASIC_STREAM_WAIT_WRIT;
	}

	// ok?
	return ok;
}
tb_size_t tb_basic_stream_state(tb_basic_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, TB_STATE_UNKNOWN_ERROR);

	// the stream state
	return stream->state;
}
tb_bool_t tb_basic_stream_open(tb_basic_stream_t* stream)
{
	// check stream
	tb_assert_and_check_return_val(stream && stream->open, tb_false);

	// already been opened?
	tb_check_return_val(!tb_stream_is_opened(stream), tb_true);

	// closed?
	tb_assert_and_check_return_val(TB_STATE_CLOSED == tb_atomic_get(&stream->base.istate), tb_false);

	// init offset
	stream->offset = 0;

	// init state
	stream->state = TB_STATE_OK;

	// open it
	tb_bool_t ok = stream->open(stream);

	// opened
	if (ok) tb_atomic_set(&stream->base.istate, TB_STATE_OPENED);

	// ok?
	return ok;
}
tb_bool_t tb_basic_stream_clos(tb_basic_stream_t* stream)
{
	// check stream
	tb_assert_and_check_return_val(stream, tb_false);

	// already been closed?
	tb_check_return_val(tb_stream_is_opened(stream), tb_true);

	// flush writed data first
	if (stream->bwrited && !tb_basic_stream_sync(stream, tb_true)) return tb_false;

	// has close?
	if (stream->clos && !stream->clos(stream)) return tb_false;

	// clear state
	stream->offset = 0;
	stream->bwrited = 0;
	stream->state = TB_STATE_OK;
	tb_atomic_set(&stream->base.istate, TB_STATE_CLOSED);

	// clear cache
	tb_queue_buffer_clear(&stream->cache);

	// ok
	return tb_true;
}
tb_bool_t tb_basic_stream_need(tb_basic_stream_t* stream, tb_byte_t** data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data && size, tb_false);

	// check stream
	tb_assert_and_check_return_val(stream && tb_stream_is_opened(stream) && stream->read && stream->wait, tb_false);

	// stoped?
	tb_assert_and_check_return_val(TB_STATE_OPENED == tb_atomic_get(&stream->base.istate), tb_false);

	// no cache? enable it first
	if (!tb_queue_buffer_maxn(&stream->cache)) tb_queue_buffer_resize(&stream->cache, size);

	// check
	tb_assert_and_check_return_val(tb_queue_buffer_maxn(&stream->cache) && size <= tb_queue_buffer_maxn(&stream->cache), tb_false);

	// have writed cache? sync first
	if (stream->bwrited && !tb_queue_buffer_null(&stream->cache) && !tb_basic_stream_sync(stream, tb_false)) return tb_false;

	// switch to the read cache mode
	if (stream->bwrited && tb_queue_buffer_null(&stream->cache)) stream->bwrited = 0;

	// check the cache mode, must be read cache
	tb_assert_and_check_return_val(!stream->bwrited, tb_false);

	// enough?
	if (size <= tb_queue_buffer_size(&stream->cache)) 
	{
		// save data
		*data = tb_queue_buffer_head(&stream->cache);

		// ok
		return tb_true;
	}

	// enter cache for push
	tb_size_t 	push = 0;
	tb_size_t 	need = size - tb_queue_buffer_size(&stream->cache);
	tb_byte_t* 	tail = tb_queue_buffer_push_init(&stream->cache, &push);
	tb_assert_and_check_return_val(tail && push, tb_false);
	if (push > need) push = need;

	// fill cache
	tb_size_t read = 0;
	while (read < push && (TB_STATE_OPENED == tb_atomic_get(&stream->base.istate)))
	{
		// read data
		tb_long_t real = stream->read(stream, tail + read, push - read);
		
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
			real = stream->wait(stream, TB_BASIC_STREAM_WAIT_READ, tb_stream_timeout(stream));

			// ok?
			tb_check_break(real > 0);
		}
		else break;
	}
	
	// leave cache for push
	tb_queue_buffer_push_exit(&stream->cache, read);

	// not enough?
	if (size > tb_queue_buffer_size(&stream->cache))
	{
		// killed? save state
		if (!stream->state && (TB_STATE_KILLING == tb_atomic_get(&stream->base.istate)))
			stream->state = TB_STATE_KILLED;

		// failed
		return tb_false;
	}

	// save data
	*data = tb_queue_buffer_head(&stream->cache);

	// ok
	return tb_true;
}
tb_long_t tb_basic_stream_read(tb_basic_stream_t* stream, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);

	// no size?
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(stream && tb_stream_is_opened(stream) && stream->read, -1);

	// done
	tb_long_t read = 0;
	do
	{
		if (tb_queue_buffer_maxn(&stream->cache))
		{
			// switch to the read cache mode
			if (stream->bwrited && tb_queue_buffer_null(&stream->cache)) stream->bwrited = 0;

			// check the cache mode, must be read cache
			tb_assert_and_check_return_val(!stream->bwrited, -1);

			// read data from cache first
			read = tb_queue_buffer_read(&stream->cache, data, size);
			tb_check_return_val(read >= 0, -1);

			// ok?
			tb_check_break(!read);

			// cache is null now.
			tb_assert_and_check_return_val(tb_queue_buffer_null(&stream->cache), -1);

			// enter cache for push
			tb_size_t 	push = 0;
			tb_byte_t* 	tail = tb_queue_buffer_push_init(&stream->cache, &push);
			tb_assert_and_check_return_val(tail && push, -1);

			// push data to cache from stream
			tb_assert(stream->read);
			tb_long_t 	real = stream->read(stream, tail, push);
			tb_check_return_val(real >= 0, -1);

			// read the left data from cache
			if (real > 0) 
			{
				// leave cache for push
				tb_queue_buffer_push_exit(&stream->cache, real);

				// read cache
				real = tb_queue_buffer_read(&stream->cache, data + read, tb_min(real, size - read));
				tb_check_return_val(real >= 0, -1);

				// save read 
				read += real;
			}
		}
		else 
		{
			// read it directly
			read = stream->read(stream, data, size);
			tb_check_return_val(read >= 0, -1);
		}
	}
	while (0);

	// update offset
	stream->offset += read;

//	tb_trace_d("read: %d", read);
	return read;
}
tb_long_t tb_basic_stream_writ(tb_basic_stream_t* stream, tb_byte_t const* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);

	// no size?
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(stream && tb_stream_is_opened(stream) && stream->writ, -1);

	// done
	tb_long_t writ = 0;
	do
	{
		if (tb_queue_buffer_maxn(&stream->cache))
		{
			// switch to the writ cache mode
			if (!stream->bwrited && tb_queue_buffer_null(&stream->cache)) stream->bwrited = 1;

			// check the cache mode, must be writ cache
			tb_assert_and_check_return_val(stream->bwrited, -1);

			// writ data to cache first
			writ = tb_queue_buffer_writ(&stream->cache, data, size);
			tb_check_return_val(writ >= 0, -1);
			
			// ok?
			tb_check_break(!writ);

			// cache is full now.
			tb_assert_and_check_return_val(tb_queue_buffer_full(&stream->cache), -1);

			// enter cache for pull
			tb_size_t 	pull = 0;
			tb_byte_t* 	head = tb_queue_buffer_pull_init(&stream->cache, &pull);
			tb_assert_and_check_return_val(head && pull, -1);

			// pull data to stream from cache
			tb_long_t 	real = stream->writ(stream, head, pull);
			tb_check_return_val(real >= 0, -1);

			// writ the left data to cache
			if (real > 0)
			{
				// leave cache for pull
				tb_queue_buffer_pull_exit(&stream->cache, real);

				// writ cache
				real = tb_queue_buffer_writ(&stream->cache, data + writ, tb_min(real, size - writ));
				tb_check_return_val(real >= 0, -1);

				// save writ 
				writ += real;
			}
		}
		else 
		{
			// writ it directly
			writ = stream->writ(stream, data, size);
			tb_check_return_val(writ >= 0, -1);
		}

	} while (0);

	// update offset
	stream->offset += writ;

//	tb_trace_d("writ: %d", writ);
	return writ;
}
tb_bool_t tb_basic_stream_bread(tb_basic_stream_t* stream, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(stream && data, tb_false);
	tb_check_return_val(size, tb_true);

	// have writed cache? sync first
	if (stream->bwrited && !tb_queue_buffer_null(&stream->cache) && !tb_basic_stream_sync(stream, tb_false))
		return tb_false;

	// check the left
	tb_hize_t left = tb_stream_left(stream);
	tb_check_return_val(size <= left, tb_false);

	// read data from cache
	tb_long_t read = 0;
	while (read < size && (TB_STATE_OPENED == tb_atomic_get(&stream->base.istate)))
	{
		// read data
		tb_long_t real = tb_basic_stream_read(stream, data + read, size - read);	
		if (real > 0) read += real;
		else if (!real)
		{
			// wait
			real = tb_basic_stream_wait(stream, TB_BASIC_STREAM_WAIT_READ, tb_stream_timeout(stream));
			tb_check_break(real > 0);

			// has read?
			tb_assert_and_check_break(real & TB_BASIC_STREAM_WAIT_READ);
		}
		else break;
	}

	// killed? save state
	if (read != size && !stream->state && (TB_STATE_KILLING == tb_atomic_get(&stream->base.istate)))
		stream->state = TB_STATE_KILLED;

	// ok?
	return (read == size? tb_true : tb_false);
}
tb_bool_t tb_basic_stream_bwrit(tb_basic_stream_t* stream, tb_byte_t const* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(stream && data, tb_false);
	tb_check_return_val(size, tb_true);

	// writ data to cache
	tb_long_t writ = 0;
	while (writ < size && (TB_STATE_OPENED == tb_atomic_get(&stream->base.istate)))
	{
		// writ data
		tb_long_t real = tb_basic_stream_writ(stream, data + writ, size - writ);	
		if (real > 0) writ += real;
		else if (!real)
		{
			// wait
			real = tb_basic_stream_wait(stream, TB_BASIC_STREAM_WAIT_WRIT, tb_stream_timeout(stream));
			tb_check_break(real > 0);

			// has writ?
			tb_assert_and_check_break(real & TB_BASIC_STREAM_WAIT_WRIT);
		}
		else break;
	}

	// killed? save state
	if (writ != size && !stream->state && (TB_STATE_KILLING == tb_atomic_get(&stream->base.istate)))
		stream->state = TB_STATE_KILLED;

	// ok?
	return (writ == size? tb_true : tb_false);
}
tb_bool_t tb_basic_stream_sync(tb_basic_stream_t* stream, tb_bool_t bclosing)
{
	// check stream
	tb_assert_and_check_return_val(stream && stream->writ && stream->wait && tb_stream_is_opened(stream), tb_false);

	// stoped?
	tb_assert_and_check_return_val((TB_STATE_OPENED == tb_atomic_get(&stream->base.istate)), tb_false);

	// cached? sync cache first
	if (tb_queue_buffer_maxn(&stream->cache))
	{
		// have data?
		if (!tb_queue_buffer_null(&stream->cache))
		{
			// check: must be writed cache
			tb_assert_and_check_return_val(stream->bwrited, tb_false);

			// enter cache for pull
			tb_size_t 	size = 0;
			tb_byte_t* 	head = tb_queue_buffer_pull_init(&stream->cache, &size);
			tb_assert_and_check_return_val(head && size, tb_false);

			// writ cache data to stream
			tb_size_t 	writ = 0;
			while (writ < size && (TB_STATE_OPENED == tb_atomic_get(&stream->base.istate)))
			{
				// writ
				tb_long_t real = stream->writ(stream, head + writ, size - writ);

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
					real = stream->wait(stream, TB_BASIC_STREAM_WAIT_WRIT, tb_stream_timeout(stream));

					// ok?
					tb_check_break(real > 0);
				}
				// error or end?
				else break;
			}

			// leave cache for pull
			tb_queue_buffer_pull_exit(&stream->cache, writ);

			// cache be not cleared?
			if (!tb_queue_buffer_null(&stream->cache))
			{
				// killed? save state
				if (!stream->state && (TB_STATE_KILLING == tb_atomic_get(&stream->base.istate)))
					stream->state = TB_STATE_KILLED;

				// failed
				return tb_false;
			}
		}
		else stream->bwrited = 1;
	}

	// sync
	return stream->sync? stream->sync(stream, bclosing) : tb_true;
}
tb_bool_t tb_basic_stream_seek(tb_basic_stream_t* stream, tb_hize_t offset)
{
	// check stream
	tb_assert_and_check_return_val(stream && tb_stream_is_opened(stream), tb_false);

	// stoped?
	tb_assert_and_check_return_val((TB_STATE_OPENED == tb_atomic_get(&stream->base.istate)), tb_false);

	// limit offset
	tb_hong_t size = tb_stream_size(stream);
	if (size >= 0 && offset > size) offset = size;

	// the offset be not changed?
	tb_hize_t curt = tb_stream_offset(stream);
	tb_check_return_val(offset != curt, tb_true);

	// for writing
	if (stream->bwrited)
	{
		// sync writed data first
		if (!tb_basic_stream_sync(stream, tb_false)) return tb_false;

		// check cache, must not cache or empty cache
		tb_assert_and_check_return_val(!tb_queue_buffer_maxn(&stream->cache) || tb_queue_buffer_null(&stream->cache), tb_false);

		// seek it
		tb_bool_t ok = stream->seek? stream->seek(stream, offset) : tb_false;

		// save offset
		if (ok) stream->offset = offset;
	}
	// for reading
	else
	{
		// cached? try to seek it at the cache
		tb_bool_t ok = tb_false;
		if (tb_queue_buffer_maxn(&stream->cache))
		{
			tb_size_t 	size = 0;
			tb_byte_t* 	data = tb_queue_buffer_pull_init(&stream->cache, &size);
			if (data && size && offset > curt && offset < curt + size)
			{
				// seek it at the cache
				tb_queue_buffer_pull_exit(&stream->cache, (tb_size_t)(offset - curt));

				// save offset
				stream->offset = offset;
				
				// ok
				ok = tb_true;
			}
		}

		// seek it
		if (!ok)
		{
			// seek it
			ok = stream->seek? stream->seek(stream, offset) : tb_false;

			// ok?
			if (ok)
			{
				// save offset
				stream->offset = offset;
	
				// clear cache
				tb_queue_buffer_clear(&stream->cache);
			}
		}

		// try to read and seek
		if (!ok && offset > curt)
		{
			// read some data for updating offset
			tb_byte_t data[TB_BASIC_STREAM_BLOCK_MAXN];
			while (tb_stream_offset(stream) != offset)
			{
				tb_size_t need = tb_min(offset - curt, TB_BASIC_STREAM_BLOCK_MAXN);
				if (!tb_basic_stream_bread(stream, data, need)) return tb_false;
			}
		}
	}

	// ok?
	return tb_stream_offset(stream) == offset? tb_true : tb_false;
}
tb_bool_t tb_basic_stream_skip(tb_basic_stream_t* stream, tb_hize_t size)
{
	return tb_basic_stream_seek(stream, tb_stream_offset(stream) + size);
}
tb_long_t tb_basic_stream_bread_line(tb_basic_stream_t* stream, tb_char_t* data, tb_size_t size)
{
	// done
	tb_char_t 	ch = 0;
	tb_char_t* 	p = data;
	while ((TB_STATE_OPENED == tb_atomic_get(&stream->base.istate)))
	{
		// read char
		ch = tb_basic_stream_bread_s8(stream);

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
	if ((TB_STATE_KILLING == tb_atomic_get(&stream->base.istate))) return -1;

	// end?
	return tb_stream_beof(stream)? -1 : 0;
}
tb_long_t tb_basic_stream_bwrit_line(tb_basic_stream_t* stream, tb_char_t* data, tb_size_t size)
{
	// writ data
	tb_long_t writ = 0;
	if (size) 
	{
		if (!tb_basic_stream_bwrit(stream, (tb_byte_t*)data, size)) return -1;
	}
	else
	{
		tb_char_t* p = data;
		while (*p)
		{
			if (!tb_basic_stream_bwrit(stream, (tb_byte_t*)p, 1)) return -1;
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
	if (!tb_basic_stream_bwrit(stream, (tb_byte_t*)le, ln)) return -1;
	writ += ln;

	// ok
	return writ;
}
tb_long_t tb_basic_stream_printf(tb_basic_stream_t* stream, tb_char_t const* fmt, ...)
{
	// init data
	tb_char_t data[TB_BASIC_STREAM_BLOCK_MAXN] = {0};
	tb_size_t size = 0;

	// format data
    tb_vsnprintf_format(data, TB_BASIC_STREAM_BLOCK_MAXN, fmt, &size);
	tb_check_return_val(size, 0);

	// writ data
	return tb_basic_stream_bwrit(stream, (tb_byte_t*)data, size)? size : -1;
}
tb_uint8_t tb_basic_stream_bread_u8(tb_basic_stream_t* stream)
{
	tb_byte_t b[1];
	if (!tb_basic_stream_bread(stream, b, 1)) return 0;
	return b[0];
}
tb_sint8_t tb_basic_stream_bread_s8(tb_basic_stream_t* stream)
{
	tb_byte_t b[1];
	if (!tb_basic_stream_bread(stream, b, 1)) return 0;
	return b[0];
}
tb_uint16_t tb_basic_stream_bread_u16_le(tb_basic_stream_t* stream)
{	
	tb_byte_t b[2];
	if (!tb_basic_stream_bread(stream, b, 2)) return 0;
	return tb_bits_get_u16_le(b);
}
tb_sint16_t tb_basic_stream_bread_s16_le(tb_basic_stream_t* stream)
{	
	tb_byte_t b[2];
	if (!tb_basic_stream_bread(stream, b, 2)) return 0;
	return tb_bits_get_s16_le(b);
}
tb_uint32_t tb_basic_stream_bread_u24_le(tb_basic_stream_t* stream)
{	
	tb_byte_t b[3];
	if (!tb_basic_stream_bread(stream, b, 3)) return 0;
	return tb_bits_get_u24_le(b);
}
tb_sint32_t tb_basic_stream_bread_s24_le(tb_basic_stream_t* stream)
{
	tb_byte_t b[3];
	if (!tb_basic_stream_bread(stream, b, 3)) return 0;
	return tb_bits_get_s24_le(b);
}
tb_uint32_t tb_basic_stream_bread_u32_le(tb_basic_stream_t* stream)
{
	tb_byte_t b[4];
	if (!tb_basic_stream_bread(stream, b, 4)) return 0;
	return tb_bits_get_u32_le(b);
}
tb_sint32_t tb_basic_stream_bread_s32_le(tb_basic_stream_t* stream)
{	
	tb_byte_t b[4];
	if (!tb_basic_stream_bread(stream, b, 4)) return 0;
	return tb_bits_get_s32_le(b);
}
tb_uint64_t tb_basic_stream_bread_u64_le(tb_basic_stream_t* stream)
{
	tb_byte_t b[8];
	if (!tb_basic_stream_bread(stream, b, 8)) return 0;
	return tb_bits_get_u64_le(b);
}
tb_sint64_t tb_basic_stream_bread_s64_le(tb_basic_stream_t* stream)
{	
	tb_byte_t b[8];
	if (!tb_basic_stream_bread(stream, b, 8)) return 0;
	return tb_bits_get_s64_le(b);
}
tb_uint16_t tb_basic_stream_bread_u16_be(tb_basic_stream_t* stream)
{	
	tb_byte_t b[2];
	if (!tb_basic_stream_bread(stream, b, 2)) return 0;
	return tb_bits_get_u16_be(b);
}
tb_sint16_t tb_basic_stream_bread_s16_be(tb_basic_stream_t* stream)
{	
	tb_byte_t b[2];
	if (!tb_basic_stream_bread(stream, b, 2)) return 0;
	return tb_bits_get_s16_be(b);
}
tb_uint32_t tb_basic_stream_bread_u24_be(tb_basic_stream_t* stream)
{	
	tb_byte_t b[3];
	if (!tb_basic_stream_bread(stream, b, 3)) return 0;
	return tb_bits_get_u24_be(b);
}
tb_sint32_t tb_basic_stream_bread_s24_be(tb_basic_stream_t* stream)
{
	tb_byte_t b[3];
	if (!tb_basic_stream_bread(stream, b, 3)) return 0;
	return tb_bits_get_s24_be(b);
}
tb_uint32_t tb_basic_stream_bread_u32_be(tb_basic_stream_t* stream)
{
	tb_byte_t b[4];
	if (!tb_basic_stream_bread(stream, b, 4)) return 0;
	return tb_bits_get_u32_be(b);
}
tb_sint32_t tb_basic_stream_bread_s32_be(tb_basic_stream_t* stream)
{	
	tb_byte_t b[4];
	if (!tb_basic_stream_bread(stream, b, 4)) return 0;
	return tb_bits_get_s32_be(b);
}
tb_uint64_t tb_basic_stream_bread_u64_be(tb_basic_stream_t* stream)
{
	tb_byte_t b[8];
	if (!tb_basic_stream_bread(stream, b, 8)) return 0;
	return tb_bits_get_u64_be(b);
}
tb_sint64_t tb_basic_stream_bread_s64_be(tb_basic_stream_t* stream)
{	
	tb_byte_t b[8];
	if (!tb_basic_stream_bread(stream, b, 8)) return 0;
	return tb_bits_get_s64_be(b);
}
tb_bool_t tb_basic_stream_bwrit_u8(tb_basic_stream_t* stream, tb_uint8_t val)
{
	tb_byte_t b[1];
	tb_bits_set_u8(b, val);
	return tb_basic_stream_bwrit(stream, b, 1);
}
tb_bool_t tb_basic_stream_bwrit_s8(tb_basic_stream_t* stream, tb_sint8_t val)
{
	tb_byte_t b[1];
	tb_bits_set_s8(b, val);
	return tb_basic_stream_bwrit(stream, b, 1);
}
tb_bool_t tb_basic_stream_bwrit_u16_le(tb_basic_stream_t* stream, tb_uint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_u16_le(b, val);
	return tb_basic_stream_bwrit(stream, b, 2);
}
tb_bool_t tb_basic_stream_bwrit_s16_le(tb_basic_stream_t* stream, tb_sint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_s16_le(b, val);
	return tb_basic_stream_bwrit(stream, b, 2);
}
tb_bool_t tb_basic_stream_bwrit_u24_le(tb_basic_stream_t* stream, tb_uint32_t val)
{	
	tb_byte_t b[3];
	tb_bits_set_u24_le(b, val);
	return tb_basic_stream_bwrit(stream, b, 3);
}
tb_bool_t tb_basic_stream_bwrit_s24_le(tb_basic_stream_t* stream, tb_sint32_t val)
{
	tb_byte_t b[3];
	tb_bits_set_s24_le(b, val);
	return tb_basic_stream_bwrit(stream, b, 3);
}
tb_bool_t tb_basic_stream_bwrit_u32_le(tb_basic_stream_t* stream, tb_uint32_t val)
{	
	tb_byte_t b[4];
	tb_bits_set_u32_le(b, val);
	return tb_basic_stream_bwrit(stream, b, 4);
}
tb_bool_t tb_basic_stream_bwrit_s32_le(tb_basic_stream_t* stream, tb_sint32_t val)
{
	tb_byte_t b[4];
	tb_bits_set_s32_le(b, val);
	return tb_basic_stream_bwrit(stream, b, 4);
}
tb_bool_t tb_basic_stream_bwrit_u64_le(tb_basic_stream_t* stream, tb_uint64_t val)
{	
	tb_byte_t b[8];
	tb_bits_set_u64_le(b, val);
	return tb_basic_stream_bwrit(stream, b, 8);
}
tb_bool_t tb_basic_stream_bwrit_s64_le(tb_basic_stream_t* stream, tb_sint64_t val)
{
	tb_byte_t b[8];
	tb_bits_set_s64_le(b, val);
	return tb_basic_stream_bwrit(stream, b, 8);
}
tb_bool_t tb_basic_stream_bwrit_u16_be(tb_basic_stream_t* stream, tb_uint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_u16_be(b, val);
	return tb_basic_stream_bwrit(stream, b, 2);
}
tb_bool_t tb_basic_stream_bwrit_s16_be(tb_basic_stream_t* stream, tb_sint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_s16_be(b, val);
	return tb_basic_stream_bwrit(stream, b, 2);
}

tb_bool_t tb_basic_stream_bwrit_u24_be(tb_basic_stream_t* stream, tb_uint32_t val)
{	
	tb_byte_t b[3];
	tb_bits_set_u24_be(b, val);
	return tb_basic_stream_bwrit(stream, b, 3);
}
tb_bool_t tb_basic_stream_bwrit_s24_be(tb_basic_stream_t* stream, tb_sint32_t val)
{
	tb_byte_t b[3];
	tb_bits_set_s24_be(b, val);
	return tb_basic_stream_bwrit(stream, b, 3);
}
tb_bool_t tb_basic_stream_bwrit_u32_be(tb_basic_stream_t* stream, tb_uint32_t val)
{	
	tb_byte_t b[4];
	tb_bits_set_u32_be(b, val);
	return tb_basic_stream_bwrit(stream, b, 4);
}
tb_bool_t tb_basic_stream_bwrit_s32_be(tb_basic_stream_t* stream, tb_sint32_t val)
{
	tb_byte_t b[4];
	tb_bits_set_s32_be(b, val);
	return tb_basic_stream_bwrit(stream, b, 4);
}
tb_bool_t tb_basic_stream_bwrit_u64_be(tb_basic_stream_t* stream, tb_uint64_t val)
{	
	tb_byte_t b[8];
	tb_bits_set_u64_be(b, val);
	return tb_basic_stream_bwrit(stream, b, 8);
}
tb_bool_t tb_basic_stream_bwrit_s64_be(tb_basic_stream_t* stream, tb_sint64_t val)
{
	tb_byte_t b[8];
	tb_bits_set_s64_be(b, val);
	return tb_basic_stream_bwrit(stream, b, 8);
}
#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t tb_basic_stream_bread_float_le(tb_basic_stream_t* stream)
{
	tb_byte_t b[4];
	if (!tb_basic_stream_bread(stream, b, 4)) return 0;
	return tb_bits_get_float_le(b);
}
tb_float_t tb_basic_stream_bread_float_be(tb_basic_stream_t* stream)
{
	tb_byte_t b[4];
	if (!tb_basic_stream_bread(stream, b, 4)) return 0;
	return tb_bits_get_float_be(b);
}
tb_double_t tb_basic_stream_bread_double_ble(tb_basic_stream_t* stream)
{	
	tb_byte_t b[8];
	if (!tb_basic_stream_bread(stream, b, 8)) return 0;
	return tb_bits_get_double_ble(b);
}
tb_double_t tb_basic_stream_bread_double_bbe(tb_basic_stream_t* stream)
{	
	tb_byte_t b[8];
	if (!tb_basic_stream_bread(stream, b, 8)) return 0;
	return tb_bits_get_double_bbe(b);
}
tb_double_t tb_basic_stream_bread_double_lle(tb_basic_stream_t* stream)
{
	tb_byte_t b[8];
	if (!tb_basic_stream_bread(stream, b, 8)) return 0;
	return tb_bits_get_double_lle(b);
}
tb_double_t tb_basic_stream_bread_double_lbe(tb_basic_stream_t* stream)
{
	tb_byte_t b[8];
	if (!tb_basic_stream_bread(stream, b, 8)) return 0;
	return tb_bits_get_double_lbe(b);
}
tb_bool_t tb_basic_stream_bwrit_float_le(tb_basic_stream_t* stream, tb_float_t val)
{
	tb_byte_t b[4];
	tb_bits_set_float_le(b, val);
	return tb_basic_stream_bwrit(stream, b, 4);
}
tb_bool_t tb_basic_stream_bwrit_float_be(tb_basic_stream_t* stream, tb_float_t val)
{
	tb_byte_t b[4];
	tb_bits_set_float_be(b, val);
	return tb_basic_stream_bwrit(stream, b, 4);
}
tb_bool_t tb_basic_stream_bwrit_double_ble(tb_basic_stream_t* stream, tb_double_t val)
{
	tb_byte_t b[8];
	tb_bits_set_double_ble(b, val);
	return tb_basic_stream_bwrit(stream, b, 8);
}
tb_bool_t tb_basic_stream_bwrit_double_bbe(tb_basic_stream_t* stream, tb_double_t val)
{
	tb_byte_t b[8];
	tb_bits_set_double_bbe(b, val);
	return tb_basic_stream_bwrit(stream, b, 8);
}
tb_bool_t tb_basic_stream_bwrit_double_lle(tb_basic_stream_t* stream, tb_double_t val)
{
	tb_byte_t b[8];
	tb_bits_set_double_lle(b, val);
	return tb_basic_stream_bwrit(stream, b, 8);
}
tb_bool_t tb_basic_stream_bwrit_double_lbe(tb_basic_stream_t* stream, tb_double_t val)
{
	tb_byte_t b[8];
	tb_bits_set_double_lbe(b, val);
	return tb_basic_stream_bwrit(stream, b, 8);
}

#endif
