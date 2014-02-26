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
static tb_long_t tb_gstream_cache_aneed(tb_gstream_t* gstream, tb_byte_t** data, tb_size_t size)
{
	tb_long_t need = 0;

	// enough?
	if (size <= tb_qbuffer_size(&gstream->cache)) 
	{
		need = size;
		goto end;
	}

	// enter cache for push
	tb_long_t 	push = 0;
	tb_byte_t* 	tail = tb_qbuffer_push_init(&gstream->cache, &push);
	tb_assert_and_check_return_val(tail && push > 0, -1);
	tb_assert_and_check_return_val(size <= push, -1);

	// fill cache
	tb_assert(gstream->read);
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

end:

	// update status
	gstream->bwrited = 0;

	// too much?
	if (need > size) need = size;

	// ok
	if (need > 0) *data = tb_qbuffer_head(&gstream->cache);

//	tb_trace_impl("need: %u size: %u", need, size);
	return need;
}
static tb_long_t tb_gstream_cache_aread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size)
{
	tb_long_t read = 0;
	if (gstream->bcached)
	{
		// read data from cache first
		read = tb_qbuffer_read(&gstream->cache, data, size);
		tb_check_return_val(read >= 0, -1);

		// empty? full it from stream
		tb_check_goto(!read, end);

		// cache is null now.
		tb_assert_and_check_return_val(tb_qbuffer_null(&gstream->cache), -1);

		// enter cache for push
		tb_size_t 	push = 0;
		tb_byte_t* 	tail = tb_qbuffer_push_init(&gstream->cache, &push);
		tb_assert_and_check_return_val(tail && push, -1);

		// push data to cache from stream
		tb_assert(gstream->read);
		tb_long_t 	real = gstream->read(gstream, tail, push, tb_false);
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
		read = gstream->read(gstream, data, size, tb_false);
		tb_check_return_val(read >= 0, -1);
	}

end:
	// update offset
	gstream->offset += read;

	// update status
	gstream->bwrited = 0;

//	tb_trace_impl("read: %d", read);
	return read;
}
static tb_long_t tb_gstream_cache_awrit(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size)
{
	tb_long_t writ = 0;
	if (gstream->bcached)
	{
		// writ data to cache first
		writ = tb_qbuffer_writ(&gstream->cache, data, size);
		tb_check_return_val(writ >= 0, -1);
		
		// full? writ data to stream from cache
		tb_check_goto(!writ, end);

		// cache is full now.
		tb_assert_and_check_return_val(tb_qbuffer_full(&gstream->cache), -1);

		// enter cache for pull
		tb_size_t 	pull = 0;
		tb_byte_t* 	head = tb_qbuffer_pull_init(&gstream->cache, &pull);
		tb_assert_and_check_return_val(head && pull, -1);

		// pull data to stream from cache
		tb_assert(gstream->writ);
		tb_long_t 	real = gstream->writ(gstream, head, pull, tb_false);
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
		writ = gstream->writ(gstream, data, size, tb_false);
		tb_check_return_val(writ >= 0, -1);
	}

end:
	// update offset
	gstream->offset += writ;

	// update status
	gstream->bwrited = 1;

//	tb_trace_impl("writ: %d", writ);
	return writ;
}
static tb_long_t tb_gstream_cache_afread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size)
{
	// init
	tb_long_t r = 1;

	// has data?
	if (data && size)
	{
		// read data from cache first
		r = tb_gstream_cache_aread(gstream, data, size);
		tb_check_return_val(r <= 0, r);
	}
	else
	{
		// clear cache
		tb_qbuffer_clear(&gstream->cache);
	}

	// flush read data
	r = gstream->read(gstream, data, size, tb_true);

	// ok?
	return r;
}
static tb_long_t tb_gstream_cache_afwrit(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size)
{
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
		tb_long_t 	writ = gstream->writ(gstream, head, pull, tb_false);
		tb_check_goto(writ >= 0, end);
		tb_assert_and_check_return_val(writ <= pull, -1);

		// leave cache for pull
		tb_qbuffer_pull_exit(&gstream->cache, writ);
	}

	// flush data if no cache
	if (tb_qbuffer_null(&gstream->cache))
	{
		// flush writ data
		r = gstream->writ(gstream, data, size, tb_true);
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
}
static tb_long_t tb_gstream_cache_seek(tb_gstream_t* gstream, tb_hize_t offset)
{
	// init
	tb_long_t r = -1;

	// flush writed data first
	if (gstream->bwrited) 
	{
		// flush it
		r = tb_gstream_cache_afwrit(gstream, tb_null, 0);

		// continue it if has data
		tb_check_return_val(r < 0, 0);

		// bwrited will be clear
		tb_assert_and_check_return_val(!gstream->bwrited, -1);
	}
	else if (gstream->bcached)
	{
		tb_size_t 	size = 0;
		tb_hize_t 	curt = tb_gstream_offset(gstream);
		tb_byte_t* 	data = tb_qbuffer_pull_init(&gstream->cache, &size);
		if (data && size && offset >= curt && offset <= curt + size)
		{
			// seek it at the cache
			tb_qbuffer_pull_exit(&gstream->cache, (tb_size_t)(offset - curt));

			// ok
			goto ok;
		}
	}

	// seek it
	tb_check_return_val(gstream->seek, -1);
	r = gstream->seek(gstream, offset);

	// support the native seek? clear cache
	if (r >= 0) tb_qbuffer_clear(&gstream->cache);

	// ok?
	tb_check_return_val(r > 0, r);

ok:
	gstream->offset = offset;
	return 1;
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
	if (!tb_gstream_ctrl(gstream, TB_STREAM_CTRL_SET_URL, url)) goto fail;

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
		tb_gstream_bclos(gstream);

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
	tb_assert_and_check_return_val(gstream && gstream->base.bopened, tb_true);

	// size
	tb_hong_t size = tb_gstream_size(gstream);
	tb_hong_t offt = tb_gstream_offset(gstream);

	// eof?
	return size && offt >= size? tb_true : tb_false;
}
tb_long_t tb_gstream_aopen(tb_gstream_t* gstream)
{
	// check stream
	tb_assert_and_check_return_val(gstream && gstream->open, -1);

	// already been opened?
	tb_check_return_val(!gstream->base.bopened, 1);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gstream->cache), -1);

	// init offset
	gstream->offset = 0;

	// init state
	gstream->state = TB_STREAM_STATE_OK;

	// open it
	tb_long_t r = gstream->open(gstream);
	
	// ok?
	if (r > 0) gstream->base.bopened = 1;
	return r;
}
tb_bool_t tb_gstream_bopen(tb_gstream_t* gstream)
{
	tb_assert_and_check_return_val(gstream, tb_false);

	// try opening it
	tb_long_t 	r = 0;
	while (!(r = tb_gstream_aopen(gstream)))
	{
		// wait
		r = tb_gstream_wait(gstream, TB_GSTREAM_WAIT_EALL, tb_stream_timeout(gstream));

		// fail or timeout?
		tb_check_break(r > 0);
	}

	// ok?
	return r > 0? tb_true : tb_false;
}
tb_long_t tb_gstream_aclos(tb_gstream_t* gstream)
{
	// check stream
	tb_assert_and_check_return_val(gstream, -1);

	// already been closed?
	tb_check_return_val(gstream->base.bopened, 1);

	// flush writed data first
	if (gstream->bwrited) 
	{
		// flush it
		tb_long_t r = tb_gstream_cache_afwrit(gstream, tb_null, 0);

		// continue it if has data
		tb_check_return_val(r < 0, 0);

		// bwrited will be clear
		tb_assert_and_check_return_val(!gstream->bwrited, -1);
	}

	// has close?
	if (gstream->clos) 
	{
		// close it
		tb_long_t r = gstream->clos(gstream);	

		// continue?
		tb_check_return_val(r, r);
	}

	// reset offset
	gstream->offset = 0;

	// clear cache
	tb_qbuffer_clear(&gstream->cache);

	// update status
	gstream->base.bopened = 0;

	// ok
	return 1;
}
tb_bool_t tb_gstream_bclos(tb_gstream_t* gstream)
{
	tb_assert_and_check_return_val(gstream, tb_false);

	// try opening it
	tb_long_t 	r = 0;
	tb_hong_t 	t = tb_mclock();
	tb_long_t 	timeout = tb_stream_timeout(gstream);
	while (!(r = tb_gstream_aclos(gstream)))
	{
		// timeout?
		if (timeout >= 0 && tb_mclock() - t > timeout) break;

		// sleep some time
		tb_usleep(100);
	}

	// ok?
	return r > 0? tb_true : tb_false;
}
tb_long_t tb_gstream_aneed(tb_gstream_t* gstream, tb_byte_t** data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data && size, -1);

	// check stream
	tb_assert_and_check_return_val(gstream && gstream->base.bopened && gstream->read, -1);

	// check cache
	tb_assert_and_check_return_val(gstream->bcached && tb_qbuffer_maxn(&gstream->cache), -1);

	// the cache is writed-cache now, need call afwrit or bfwrit first.
	tb_assert_and_check_return_val(!gstream->bwrited || tb_qbuffer_null(&gstream->cache), -1); 

	// need data from cache
	return tb_gstream_cache_aneed(gstream, data, size);
}
tb_bool_t tb_gstream_bneed(tb_gstream_t* gstream, tb_byte_t** data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(gstream && data, tb_false);

	// need data from cache
	tb_long_t prev = 0;
	tb_long_t need = 0;
	while ((need = tb_gstream_aneed(gstream, data, size)) < size)
	{
		// error?
		tb_check_break(need >= 0 && need >= prev);
		
		// no data?
		if (!(need - prev))
		{
			// wait
			tb_long_t e = tb_gstream_wait(gstream, TB_GSTREAM_WAIT_READ, tb_stream_timeout(gstream));
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_GSTREAM_WAIT_READ);
		}
		else prev = need;
	}

	// ok?
	return (need == size? tb_true : tb_false);
}
tb_long_t tb_gstream_aread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(gstream && gstream->base.bopened && gstream->read, -1);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gstream->cache), -1);

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
	tb_assert_and_check_return_val(gstream && gstream->base.bopened && gstream->writ, -1);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gstream->cache), -1);

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
	tb_hize_t left = tb_gstream_left(gstream);
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
	tb_assert_and_check_abort(gstream && data);
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

tb_long_t tb_gstream_afread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size)
{
	// check stream
	tb_assert_and_check_return_val(gstream && gstream->base.bopened, -1);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gstream->cache), -1);

	// the cache is writed-cache now, need call afwrit or bfwrit first.
	tb_assert_and_check_return_val(!gstream->bwrited || tb_qbuffer_null(&gstream->cache), -1); 

	// flush it
	return tb_gstream_cache_afread(gstream, data, size);
}
tb_long_t tb_gstream_afwrit(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size)
{
	// check stream
	tb_assert_and_check_return_val(gstream && gstream->base.bopened, -1);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gstream->cache), -1);

	// the cache is readed-cache now, need call afread or bfread first.
	tb_assert_and_check_return_val(gstream->bwrited || tb_qbuffer_null(&gstream->cache), -1); 

	// flush it
	return tb_gstream_cache_afwrit(gstream, data, size);
}
tb_bool_t tb_gstream_bfread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(gstream, tb_false);

	// has data
	if (data && size)
	{
		tb_long_t read = 0;
		while (read < size)
		{
			// read data
			tb_long_t real = tb_gstream_afread(gstream, data + read, size - read);	
			// has data?
			if (real > 0) read += real;
			// no data?
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
	// only flush the cache data
	else
	{
		while (!tb_gstream_afread(gstream, tb_null, 0))
		{
			// wait
			tb_long_t e = tb_gstream_wait(gstream, TB_GSTREAM_WAIT_READ, tb_stream_timeout(gstream));
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_GSTREAM_WAIT_READ);
		}
	}

	// ok
	return tb_true;
}
tb_bool_t tb_gstream_bfwrit(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(gstream, tb_false);

	// has data
	if (data && size)
	{
		// writ data to cache
		tb_long_t writ = 0;
		while (writ < size)
		{
			// writ data
			tb_long_t real = tb_gstream_afwrit(gstream, data + writ, size - writ);	

			// has data?
			if (real > 0) writ += real;
			// no data?
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
	// only flush the cache data
	else
	{
		while (!tb_gstream_afwrit(gstream, tb_null, 0))
		{
			// wait
			tb_long_t e = tb_gstream_wait(gstream, TB_GSTREAM_WAIT_WRIT, tb_stream_timeout(gstream));
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has writ?
			tb_assert_and_check_break(e & TB_GSTREAM_WAIT_WRIT);
		}
	}

	// ok
	return tb_true;
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
tb_long_t tb_gstream_aseek(tb_gstream_t* gstream, tb_hize_t offset)
{
	// check stream
	tb_assert_and_check_return_val(gstream && gstream->base.bopened, -1);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gstream->cache), -1);

	// need seek?
	tb_hize_t curt = tb_gstream_offset(gstream);
	tb_check_return_val(offset != curt, 1);

	// try seek to cache
	tb_long_t r = tb_gstream_cache_seek(gstream, offset);
	// ok?
	if (r > 0) goto end;
	// continue?
	else if (!r) return 0;
	else
	{
		// must be read mode
		tb_assert_and_check_return_val(!gstream->bwrited, -1);

		// limit offset
		tb_hize_t size = tb_gstream_size(gstream);
		if (size && offset > size) offset = size;

		// forward it?
		tb_assert_and_check_return_val(offset > curt, -1);

		// read some data for updating offset
		tb_byte_t data[TB_GSTREAM_BLOCK_MAXN];
		tb_size_t need = tb_min(offset - curt, TB_GSTREAM_BLOCK_MAXN);
		r = tb_gstream_aread(gstream, data, need);

		// no data? continue it
		tb_check_return_val(r, 0);

		// no finished? continue it
		if (r > 0 && tb_gstream_offset(gstream) < offset) return 0;
	}

end:
	// ok?
	return tb_gstream_offset(gstream) == offset? 1 : -1;
}
tb_bool_t tb_gstream_bseek(tb_gstream_t* gstream, tb_hize_t offset)
{
	tb_assert_and_check_return_val(gstream, tb_false);

	// try opening it
	tb_long_t r = 0;
	while (!(r = tb_gstream_aseek(gstream, offset)))
	{
		// wait
		r = tb_gstream_wait(gstream, TB_GSTREAM_WAIT_EALL, tb_stream_timeout(gstream));

		// fail or timeout?
		tb_check_break(r > 0);
	}

	// ok?
	return r > 0? tb_true : tb_false;
}
tb_long_t tb_gstream_askip(tb_gstream_t* gstream, tb_hize_t size)
{
	tb_assert_and_check_return_val(gstream, -1);
	return tb_gstream_aseek(gstream, tb_gstream_offset(gstream) + size);
}
tb_bool_t tb_gstream_bskip(tb_gstream_t* gstream, tb_hize_t size)
{
	return tb_gstream_bseek(gstream, tb_gstream_offset(gstream) + size);
}
tb_hize_t tb_gstream_size(tb_gstream_t const* gstream)
{
	// check
	tb_assert_and_check_return_val(gstream, 0);
 
	// get the size
	tb_hize_t size = 0;
	return tb_gstream_ctrl((tb_gstream_t*)gstream, TB_STREAM_CTRL_GET_SIZE, &size)? size : 0;
}
tb_hize_t tb_gstream_offset(tb_gstream_t const* gstream)
{
	tb_assert_and_check_return_val(gstream, 0);
	return gstream->offset;
}
tb_hize_t tb_gstream_left(tb_gstream_t const* gstream)
{
	tb_hize_t size = tb_gstream_size(gstream);
	return ((size && size >= gstream->offset)? (size - gstream->offset) : -1);
}
tb_size_t tb_gstream_timeout(tb_gstream_t const* gstream)
{	
	tb_assert_and_check_return_val(gstream, 0);
	return gstream->base.timeout;
}
tb_bool_t tb_gstream_ctrl(tb_gstream_t* gstream, tb_size_t ctrl, ...)
{	
	// check
	tb_assert_and_check_return_val(gstream && gstream->ctrl, tb_false);

	// init args
	tb_va_list_t args;
    tb_va_start(args, ctrl);

	// ctrl
	tb_bool_t ok = tb_false;
	switch (ctrl)
	{
	case TB_STREAM_CTRL_SET_URL:
		{
			tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			if (url && tb_url_set(&gstream->base.url, url)) ok = tb_true;
		}
		break;
	case TB_STREAM_CTRL_GET_URL:
		{
			tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			if (purl)
			{
				tb_char_t const* url = tb_url_get(&gstream->base.url);
				if (url)
				{
					*purl = url;
					ok = tb_true;
				}
			}
		}
		break;
	case TB_STREAM_CTRL_SET_HOST:
		{
			tb_char_t const* host = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			if (host)
			{
				tb_url_host_set(&gstream->base.url, host);
				ok = tb_true;
			}
		}
		break;
	case TB_STREAM_CTRL_GET_HOST:
		{
			tb_char_t const** phost = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			if (phost)
			{
				tb_char_t const* host = tb_url_host_get(&gstream->base.url);
				if (host)
				{
					*phost = host;
					ok = tb_true;
				}
			}
		}
		break;
	case TB_STREAM_CTRL_SET_PORT:
		{
			tb_size_t port = (tb_size_t)tb_va_arg(args, tb_size_t);
			if (port)
			{
				tb_url_port_set(&gstream->base.url, port);
				ok = tb_true;
			}
		}
		break;
	case TB_STREAM_CTRL_GET_PORT:
		{
			tb_size_t* pport = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			if (pport)
			{
				*pport = tb_url_port_get(&gstream->base.url);
				ok = tb_true;
			}
		}
		break;
	case TB_STREAM_CTRL_SET_PATH:
		{
			tb_char_t const* path = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			if (path)
			{
				tb_url_path_set(&gstream->base.url, path);
				ok = tb_true;
			}
		}
		break;
	case TB_STREAM_CTRL_GET_PATH:
		{
			tb_char_t const** ppath = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			if (ppath)
			{
				tb_char_t const* path = tb_url_path_get(&gstream->base.url);
				if (path)
				{
					*ppath = path;
					ok = tb_true;
				}
			}
		}
		break;
	case TB_STREAM_CTRL_SET_SSL:
		{
			tb_bool_t bssl = (tb_bool_t)tb_va_arg(args, tb_bool_t);
			tb_url_ssl_set(&gstream->base.url, bssl);
			ok = tb_true;
		}
		break;
	case TB_STREAM_CTRL_GET_SSL:
		{
			tb_bool_t* pssl = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
			if (pssl)
			{
				*pssl = tb_url_ssl_get(&gstream->base.url);
				ok = tb_true;
			}
		}
		break;
	case TB_STREAM_CTRL_SET_CACHE:
		{
			tb_size_t cache = (tb_size_t)tb_va_arg(args, tb_size_t);
			if (cache)
			{
				tb_qbuffer_resize(&gstream->cache, cache);
				if (tb_qbuffer_maxn(&gstream->cache)) ok = tb_true;
				gstream->bcached = 1;
			}
			else gstream->bcached = 0;
		}
		break;
	case TB_STREAM_CTRL_GET_CACHE:
		{
			tb_size_t* pcache = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			if (pcache)
			{
				*pcache = tb_qbuffer_maxn(&gstream->cache);
				ok = tb_true;
			}
		}
		break;
	case TB_STREAM_CTRL_SET_TIMEOUT:
		{
			gstream->base.timeout = (tb_size_t)tb_va_arg(args, tb_size_t);
			ok = tb_true;
		}
		break;
	case TB_STREAM_CTRL_GET_TIMEOUT:
		{
			tb_size_t* ptimeout = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			if (ptimeout)
			{
				*ptimeout = gstream->base.timeout;
				ok = tb_true;
			}
		}
		break;
	default:
		break;
	}

	// reset args
	tb_va_end(args);
    tb_va_start(args, ctrl);

	// ctrl for native stream
	ok = (gstream->ctrl(gstream, ctrl, args) || ok)? tb_true : tb_false;

	// exit args
	tb_va_end(args);

	// ok?
	return ok;
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

