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
 * \file		gstream.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 			"gst"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "gstream.h"
#include "../aio/aio.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../string/string.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
 
// the default stream cache maxn
#define TB_GSTREAM_MCACHE_DEFAULT 					(8192)

// the default stream timeout
#define TB_GSTREAM_TIMEOUT_DEFAULT 					(10000)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */
 
// the stream table item type
typedef struct __tb_gstream_item_t
{
	// the stream type
	tb_size_t 			type;

	// the stream initor
	tb_gstream_t* 		(*init)();

}tb_gstream_item_t;

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the stream table
static tb_gstream_item_t g_gstream_table[] = 
{
	{TB_GSTREAM_TYPE_NULL, TB_NULL}
,	{TB_GSTREAM_TYPE_FILE, tb_gstream_init_file}
,	{TB_GSTREAM_TYPE_SOCK, tb_gstream_init_sock}
,	{TB_GSTREAM_TYPE_HTTP, tb_gstream_init_http}
,	{TB_GSTREAM_TYPE_DATA, tb_gstream_init_data}
};


/* ///////////////////////////////////////////////////////////////////////
 * details
 */

static tb_long_t tb_gstream_cache_aneed(tb_gstream_t* gst, tb_byte_t** data, tb_size_t size)
{
	tb_long_t need = 0;

	// enough?
	if (size <= tb_qbuffer_size(&gst->cache)) 
	{
		need = size;
		goto end;
	}

	// enter cache for push
	tb_long_t 	push = 0;
	tb_byte_t* 	tail = tb_qbuffer_push_init(&gst->cache, &push);
	tb_assert_and_check_return_val(tail && push > 0, -1);
	tb_assert_and_check_return_val(size <= push, -1);

	// fill cache
	tb_assert(gst->aread);
	if (need < size)
	{
		// read data
		tb_long_t real = gst->aread(gst, tail + need, push - need);
		tb_assert_and_check_return_val(real >= 0, -1);

		// update need
		need += real;
	}
	
	// leave cache for push
	tb_qbuffer_push_done(&gst->cache, need);

end:

	// update status
	gst->bwrited = 0;

	// too much?
	if (need > size) need = size;

	// ok
	if (need > 0) *data = tb_qbuffer_head(&gst->cache);

//	tb_trace_impl("need: %u size: %u", need, size);
	return need;
}
static tb_long_t tb_gstream_cache_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	// read data from cache first
	tb_long_t read = tb_qbuffer_read(&gst->cache, data, size);
	tb_check_return_val(read >= 0, -1);

	// enough?
	tb_check_goto(read < size, end);

	// cache is null now.
	tb_assert_and_check_return_val(tb_qbuffer_null(&gst->cache), -1);

	// enter cache for push
	tb_size_t 	push = 0;
	tb_byte_t* 	tail = tb_qbuffer_push_init(&gst->cache, &push);
	tb_assert_and_check_return_val(tail && push, -1);

	// push data to cache from stream
	tb_assert(gst->aread);
	tb_long_t 	real = gst->aread(gst, tail, push);
	tb_check_return_val(real >= 0, -1);

	// read the left data from cache
	if (real > 0) 
	{
		// leave cache for push
		tb_qbuffer_push_done(&gst->cache, real);

		// read cache
		real = tb_qbuffer_read(&gst->cache, data + read, tb_min(real, size - read));
		tb_check_return_val(real >= 0, -1);

		// update read 
		read += real;
	}

end:
	// update offset
	gst->offset += read;

	// update status
	gst->bwrited = 0;

//	tb_trace_impl("read: %d", read);
	return read;
}
static tb_long_t tb_gstream_cache_awrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	// writ data to cache first
	tb_long_t writ = tb_qbuffer_writ(&gst->cache, data, size);
	tb_check_return_val(writ >= 0, -1);
	
	// enough?
	tb_check_goto(writ < size, end);

	// cache is full now.
	tb_assert_and_check_return_val(tb_qbuffer_full(&gst->cache), -1);

	// enter cache for pull
	tb_size_t 	pull = 0;
	tb_byte_t* 	head = tb_qbuffer_pull_init(&gst->cache, &pull);
	tb_assert_and_check_return_val(head && pull, -1);

	// pull data to stream from cache
	tb_assert(gst->awrit);
	tb_long_t 	real = gst->awrit(gst, head, pull);
	tb_check_return_val(real >= 0, -1);

	// writ the left data to cache
	if (real > 0)
	{
		// leave cache for pull
		tb_qbuffer_pull_done(&gst->cache, real);

		// writ cache
		real = tb_qbuffer_writ(&gst->cache, data + writ, tb_min(real, size - writ));
		tb_check_return_val(real >= 0, -1);

		// update writ 
		writ += real;
	}

end:
	// update offset
	gst->offset += writ;

	// update status
	gst->bwrited = 1;

//	tb_trace_impl("writ: %d", writ);
	return writ;
}
static tb_long_t tb_gstream_cache_afread(tb_gstream_t* gst)
{
	// last operaton is read
	tb_assert_and_check_return_val(!gst->bwrited, -1);

	// init
	tb_long_t r = 1;

	// clear cache
	tb_qbuffer_clear(&gst->cache);

	// flush read data
	if (gst->afread) r = gst->afread(gst);

	return r;
}
static tb_long_t tb_gstream_cache_afwrit(tb_gstream_t* gst)
{
	// last operaton is writ
	tb_assert_and_check_return_val(gst->bwrited, -1);

	// init 
	tb_long_t r = 1;

	// only flush writ data if no cache
	if (tb_qbuffer_null(&gst->cache)) 
	{
		if (gst->afwrit) r = gst->afwrit(gst);
		goto end;
	}

	// enter cache for pull
	tb_size_t 	pull = 0;
	tb_byte_t* 	head = tb_qbuffer_pull_init(&gst->cache, &pull);
	tb_assert_and_check_return_val(head && pull, -1);

	// writ data to stream
	tb_long_t 	writ = gst->awrit(gst, head, pull);
	tb_assert_and_check_return_val(writ >= 0 && writ <= pull, -1);

	// leave cache for pull
	tb_qbuffer_pull_done(&gst->cache, writ);

	// ok?
	if (tb_qbuffer_null(&gst->cache))
	{
		// flush writ data
		if (gst->afwrit) r = gst->afwrit(gst);
	}
	else 
	{
		// continue 
		r = 0;
	}

end:
	// update status if ok
	if (r > 0) gst->bwrited = 0;

	return r;
}
static tb_bool_t tb_gstream_cache_seek(tb_gstream_t* gst, tb_int64_t offset)
{
	// seek to cache
	if (offset >= gst->offset && offset <= gst->offset + tb_qbuffer_size(&gst->cache))
	{
#if 0
		// update cache
		gst->cache_head += offset - gst->offset;
		gst->cache_size -= offset - gst->offset;
		if (!gst->cache_size) gst->cache_head = gst->cache_data;
#else
		// FIXME
		tb_trace_noimpl();
		return TB_FALSE;
#endif
		// ok
		return TB_TRUE;
	}

	// seek to stream
	if (gst->seek && gst->seek(gst, offset))
	{
		// clear cache
		tb_qbuffer_clear(&gst->cache);

		// ok
		return TB_TRUE;
	}

	return TB_FALSE;
}
tb_long_t tb_gstream_cache_wait(tb_gstream_t* gst, tb_size_t etype, tb_long_t timeout)
{
	tb_assert_and_check_return_val(gst, -1);

	// wait the native event first
	tb_long_t e = gst->wait(gst, etype, timeout);
	tb_assert_and_check_return_val(e >= 0, -1);
	
	// no event?
	tb_check_return_val(!e, e);

	// wait for cache
	if ((etype & TB_AIOO_ETYPE_READ) && !tb_qbuffer_null(&gst->cache)) e |= TB_AIOO_ETYPE_READ;
	if ((etype & TB_AIOO_ETYPE_WRIT) && !tb_qbuffer_full(&gst->cache)) e |= TB_AIOO_ETYPE_WRIT;

	return e;
}
/* ///////////////////////////////////////////////////////////////////////
 * interface
 */
tb_gstream_t* tb_gstream_init_from_url(tb_char_t const* url)
{
	tb_assert_and_check_return_val(url, TB_NULL);

	// init
	tb_gstream_t* 		gst = TB_NULL;
	tb_size_t 			t = TB_GSTREAM_TYPE_NULL;
	tb_char_t const* 	p = url;
	if (!tb_strnicmp(p, "http://", 7)) 
		t = TB_GSTREAM_TYPE_HTTP;
	else if ((*p == '/') || (!tb_strnicmp(p, "file://", 7))) 
		t = TB_GSTREAM_TYPE_FILE;
	else if (!tb_strnicmp(p, "sock://", 7))
		t = TB_GSTREAM_TYPE_SOCK;
	else if (!tb_strnicmp(p, "https://", 8))
		t = TB_GSTREAM_TYPE_HTTP;
	else if (!tb_strnicmp(p, "files://", 8))
		t = TB_GSTREAM_TYPE_FILE;
	else if (!tb_strnicmp(p, "socks://", 8))
		t = TB_GSTREAM_TYPE_SOCK;
	else return TB_NULL;
	tb_assert_and_check_goto(t && t < tb_arrayn(g_gstream_table), fail);

	// init stream
	gst = g_gstream_table[t].init();
	tb_assert_and_check_goto(gst, fail);

	// set url
	if (!tb_gstream_ctrl1(gst, TB_GSTREAM_CMD_SET_URL, url)) goto fail;

	// ok
	return gst;

fail:
	
	// exit stream
	if (gst) tb_gstream_exit(gst);

	return TB_NULL;
}

tb_bool_t tb_gstream_init(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst, TB_FALSE);

	// clear
	tb_memset(gst, 0, sizeof(tb_gstream_t));

	// init timeout
	gst->timeout = TB_GSTREAM_TIMEOUT_DEFAULT;

	// init url
	if (!tb_url_init(&gst->url)) return TB_FALSE;

	// init cache
	if (!tb_qbuffer_init(&gst->cache, TB_GSTREAM_MCACHE_DEFAULT)) goto fail;

	// ok
	return TB_TRUE;

fail:
	tb_qbuffer_exit(&gst->cache);
	return TB_FALSE;
}
tb_void_t tb_gstream_exit(tb_gstream_t* gst)
{
	if (gst) 
	{
		// close it
		tb_gstream_bclose(gst);

		// exit cache
		tb_qbuffer_exit(&gst->cache);

		// exit url
		tb_url_exit(&gst->url);

		// free native
		if (gst->free) gst->free(gst);

		// free it
		tb_free(gst);
	}
}
tb_handle_t tb_gstream_bare(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst && gst->bare, TB_NULL);
	return gst->bare(gst);
}
tb_long_t tb_gstream_wait(tb_gstream_t* gst, tb_size_t etype, tb_long_t timeout)
{
	tb_assert_and_check_return_val(gst && gst->wait, -1);
	
	// have readed event for cache?
	if (etype & TB_AIOO_ETYPE_READ)
	{
		// the cache is writed-cache now, need call afwrit or bfwrit first.
		tb_assert_and_check_return_val(!gst->bwrited || tb_qbuffer_null(&gst->cache), -1);
	}

	// have writed event for cache?
	if (etype & TB_AIOO_ETYPE_WRIT)
	{
		// the cache is readed-cache now, need call afread or bfread first.
		tb_assert_and_check_return_val(gst->bwrited || tb_qbuffer_null(&gst->cache), -1);
	}

	// wait
	return tb_gstream_cache_wait(gst, etype, timeout);
}
tb_void_t tb_gstream_clear(tb_gstream_t* gst)
{
	// check stream
	tb_assert_and_check_return(gst);

	// check cache
	tb_assert_and_check_return(tb_qbuffer_maxn(&gst->cache));

	// reset offset
	gst->offset = 0;

	// reset to readed-mode
	gst->bwrited = 0;

	// clear cache
	tb_qbuffer_clear(&gst->cache);
}
tb_long_t tb_gstream_aopen(tb_gstream_t* gst)
{
	// check stream
	tb_assert_and_check_return_val(gst && gst->aopen, -1);

	// already been opened?
	tb_check_return_val(!gst->bopened, 1);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gst->cache), -1);

	// init offset
	gst->offset = 0;

	// open it
	tb_long_t r = gst->aopen(gst);
	
	// ok?
	if (r > 0) gst->bopened = 1;
	return r;
}
tb_bool_t tb_gstream_bopen(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst, TB_FALSE);

	// try opening it
	tb_long_t 	r = 0;
	tb_int64_t 	t = tb_mclock();
	while (!(r = tb_gstream_aopen(gst)))
	{
		// wait
		r = tb_gstream_wait(gst, TB_AIOO_ETYPE_EALL, gst->timeout);
		tb_assert_and_check_return_val(r >= 0, TB_FALSE);

		// timeout?
		tb_check_return_val(r, TB_FALSE);
	}

	// ok?
	return r > 0? TB_TRUE : TB_FALSE;
}
tb_long_t tb_gstream_aclose(tb_gstream_t* gst)
{
	// check stream
	tb_assert_and_check_return_val(gst, -1);

	// already been closed?
	tb_check_return_val(gst->bopened, 1);

	// init 
	tb_long_t r1 = 1;
	tb_long_t r2 = 1;

	// flush writed data first
	if (gst->bwrited) 
	{
		// flush it
		r1 = tb_gstream_cache_afwrit(gst);

		// continue?
		tb_check_return_val(r1, r1);

		// bwrited will be clear
		tb_assert_and_check_return_val(!gst->bwrited, -1);
	}

	// has close?
	if (gst->aclose) 
	{
		// close it
		r2 = gst->aclose(gst);	

		// continue?
		tb_check_return_val(r2, r2);
	}

	// reset offset
	gst->offset = 0;

	// clear cache
	tb_qbuffer_clear(&gst->cache);

	// update status
	gst->bopened = 0;

	// ok?
	return (r1 > 0 && r2 > 0)? 1 : -1;
}
tb_bool_t tb_gstream_bclose(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst, TB_FALSE);

	// try opening it
	tb_long_t 	r = 0;
	tb_int64_t 	t = tb_mclock();
	while (!(r = tb_gstream_aclose(gst)))
	{
		// timeout?
		if (tb_mclock() - t > gst->timeout) break;

		// sleep some time
		tb_usleep(100);
	}

	// ok?
	return r > 0? TB_TRUE : TB_FALSE;
}
tb_long_t tb_gstream_aneed(tb_gstream_t* gst, tb_byte_t** data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data && size, -1);

	// check stream
	tb_assert_and_check_return_val(gst && gst->bopened && gst->aread, -1);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gst->cache), -1);

	// the cache is writed-cache now, need call afwrit or bfwrit first.
	tb_assert_and_check_return_val(!gst->bwrited || tb_qbuffer_null(&gst->cache), -1); 

	// need data from cache
	return tb_gstream_cache_aneed(gst, data, size);
}
tb_bool_t tb_gstream_bneed(tb_gstream_t* gst, tb_byte_t** data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(gst && data, TB_FALSE);

	// need data from cache
	tb_long_t prev = 0;
	tb_long_t need = 0;
	while ((need = tb_gstream_aneed(gst, data, size)) < size)
	{
		// error?
		tb_check_break(need >= 0 && need >= prev);
		
		// no data?
		if (!(need - prev))
		{
			// wait
			tb_long_t e = tb_gstream_wait(gst, TB_AIOO_ETYPE_READ, gst->timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOO_ETYPE_READ);
		}
		else prev = need;
	}

	// ok?
	return (need == size? TB_TRUE : TB_FALSE);
}
tb_long_t tb_gstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(gst && gst->bopened && gst->aread, -1);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gst->cache), -1);

	// the cache is writed-cache now, need call afwrit or bfwrit first.
	tb_assert_and_check_return_val(!gst->bwrited || tb_qbuffer_null(&gst->cache), -1); 

	// read data from cache
	return tb_gstream_cache_aread(gst, data, size);
}

tb_long_t tb_gstream_awrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(data, -1);
	tb_check_return_val(size, 0);

	// check stream
	tb_assert_and_check_return_val(gst && gst->bopened && gst->awrit, -1);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gst->cache), -1);

	// the cache is readed-cache now, need call afread or bfread first.
	tb_assert_and_check_return_val(gst->bwrited || tb_qbuffer_null(&gst->cache), -1); 

	// writ data to cache
	return tb_gstream_cache_awrit(gst, data, size);
}
tb_bool_t tb_gstream_bread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(gst && data, TB_FALSE);
	tb_check_return_val(size, TB_TRUE);

	// read data from cache
	tb_long_t read = 0;
	while (read < size)
	{
		// read data
		tb_long_t n = tb_gstream_aread(gst, data + read, size - read);	
		if (n > 0) read += n;
		else if (!n)
		{
			// wait
			tb_long_t e = tb_gstream_wait(gst, TB_AIOO_ETYPE_READ, gst->timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOO_ETYPE_READ);
		}
		else break;
	}

	// ok?
	return (read == size? TB_TRUE : TB_FALSE);
}

tb_bool_t tb_gstream_bwrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size)
{
	// check data
	tb_assert_and_check_return_val(gst && data, TB_FALSE);
	tb_check_return_val(size, TB_TRUE);

	// writ data to cache
	tb_long_t writ = 0;
	while (writ < size)
	{
		// writ data
		tb_long_t n = tb_gstream_awrit(gst, data + writ, size - writ);	
		if (n > 0) writ += n;
		else if (!n)
		{
			// wait
			tb_long_t e = tb_gstream_wait(gst, TB_AIOO_ETYPE_WRIT, gst->timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has writ?
			tb_assert_and_check_break(e & TB_AIOO_ETYPE_WRIT);
		}
		else break;
	}

	// ok?
	return (writ == size? TB_TRUE : TB_FALSE);
}

tb_long_t tb_gstream_afread(tb_gstream_t* gst)
{
	// check stream
	tb_assert_and_check_return_val(gst, -1);

	// flush readed cache data if the last operaton is read
	tb_check_return_val(!gst->bwrited, 1);

	// flush it
	return tb_gstream_cache_afread(gst);
}
tb_long_t tb_gstream_afwrit(tb_gstream_t* gst)
{
	// check stream
	tb_assert_and_check_return_val(gst, -1);

	// flush writed cache data if the last operaton is writ
	tb_check_return_val(gst->bwrited, 1);

	// flush it
	return tb_gstream_cache_afwrit(gst);
}
tb_bool_t tb_gstream_bfread(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst, TB_FALSE);

	// try opening it
	tb_long_t r = 0;
	while (!(r = tb_gstream_afread(gst)))
	{
		// wait
		tb_long_t e = tb_gstream_wait(gst, TB_AIOO_ETYPE_READ, gst->timeout);
		tb_assert_and_check_break(e >= 0);

		// timeout?
		tb_check_break(e);

		// has read?
		tb_assert_and_check_break(e & TB_AIOO_ETYPE_READ);
	}

	// ok?
	return r > 0? TB_TRUE : TB_FALSE;
}
tb_bool_t tb_gstream_bfwrit(tb_gstream_t* gst)
{
	tb_assert_and_check_return_val(gst, TB_FALSE);

	// try opening it
	tb_long_t r = 0;
	while (!(r = tb_gstream_afwrit(gst)))
	{
		// wait
		tb_long_t e = tb_gstream_wait(gst, TB_AIOO_ETYPE_WRIT, gst->timeout);
		tb_assert_and_check_break(e >= 0);

		// timeout?
		tb_check_break(e);

		// has writ?
		tb_assert_and_check_break(e & TB_AIOO_ETYPE_WRIT);
	}

	// ok?
	return r > 0? TB_TRUE : TB_FALSE;
}

tb_long_t tb_gstream_bread_line(tb_gstream_t* gst, tb_char_t* data, tb_size_t size)
{
	tb_char_t 	ch = 0;
	tb_char_t* 	p = data;
	while (1)
	{
		// read char
		ch = tb_gstream_bread_s8(gst);

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
	return 0;
}
tb_long_t tb_gstream_bwrit_line(tb_gstream_t* gst, tb_char_t* data, tb_size_t size)
{
	// writ data
	tb_long_t writ = 0;
	if (size) 
	{
		if (!tb_gstream_bwrit(gst, data, size)) return -1;
	}
	else
	{
		tb_char_t* p = data;
		while (*p)
		{
			if (!tb_gstream_bwrit(gst, p, 1)) return -1;
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
	if (!tb_gstream_bwrit(gst, le, ln)) return -1;
	writ += ln;

	// ok
	return writ;
}

tb_long_t tb_gstream_printf(tb_gstream_t* gst, tb_char_t const* fmt, ...)
{
	// init data
	tb_char_t data[TB_GSTREAM_BLOCK_MAXN] = {0};
	tb_size_t size = 0;

	// format data
    tb_va_format(data, TB_GSTREAM_BLOCK_MAXN, fmt, &size);
	tb_check_return_val(size, 0);

	// writ data
	return tb_gstream_bwrit(gst, data, size)? size : -1;
}

tb_bool_t tb_gstream_seek(tb_gstream_t* gst, tb_int64_t offset, tb_size_t flag)
{
	// check stream
	tb_assert_and_check_return_val(gst && gst->bopened, TB_FALSE);

	// check cache
	tb_assert_and_check_return_val(tb_qbuffer_maxn(&gst->cache), TB_FALSE);

	// the absolute offset
	tb_uint64_t size = tb_gstream_size(gst);
	if (flag == TB_GSTREAM_SEEK_CUR) offset += gst->offset;
	else if (flag == TB_GSTREAM_SEEK_END)
	{
		tb_assert_and_check_return_val(size && offset <= 0, TB_FALSE);
		offset += size;
	}
	tb_assert_and_check_return_val(offset >= 0 && (!size || offset <= size), TB_FALSE);

	// need not seek
	if (offset == gst->offset) return TB_TRUE;

	// seek to cache
	if (tb_gstream_cache_seek(gst, offset))
		return TB_TRUE;

	// forward only
	if (gst->offset < offset)
	{
		tb_int64_t time = tb_mclock();
		while (gst->offset < offset)
		{
			tb_byte_t data[TB_GSTREAM_BLOCK_MAXN];
			tb_size_t need = tb_min(offset - gst->offset, TB_GSTREAM_BLOCK_MAXN);
			tb_long_t n = tb_gstream_aread(gst, data, need);
			if (n > 0) 
			{
				// update clock
				time = tb_mclock();
			}
			else if (!n)
			{
				// timeout?
				if (tb_mclock() - time > gst->timeout) break;

				// sleep some time
				tb_usleep(100);
			}
			else break;
		}
	}

	// ok?
	return (gst->offset == offset)? TB_TRUE : TB_FALSE;
}
tb_bool_t tb_gstream_skip(tb_gstream_t* gst, tb_size_t size)
{
	return tb_gstream_seek(gst, size, TB_GSTREAM_SEEK_CUR);
}
tb_size_t tb_gstream_type(tb_gstream_t const* gst)
{
	tb_assert_and_check_return_val(gst, TB_GSTREAM_TYPE_NULL);
	return gst->type;
}
tb_uint64_t tb_gstream_size(tb_gstream_t const* gst)
{
	tb_assert_and_check_return_val(gst, 0);
	return gst->size? gst->size(gst) : 0;
}
tb_uint64_t tb_gstream_offset(tb_gstream_t const* gst)
{
	tb_assert_and_check_return_val(gst, 0);
	return gst->offset;
}
tb_uint64_t tb_gstream_left(tb_gstream_t const* gst)
{
	tb_uint64_t size = tb_gstream_size(gst);
	return (size > gst->offset? (size - gst->offset) : 0);
}
tb_size_t tb_gstream_timeout(tb_gstream_t const* gst)
{	
	tb_assert_and_check_return_val(gst, 0);
	return gst->timeout;
}
tb_bool_t tb_gstream_ctrl0(tb_gstream_t* gst, tb_size_t cmd)
{
	tb_assert_and_check_return_val(gst && gst->ctrl0, TB_FALSE);
	return gst->ctrl0(gst, cmd);
}
tb_bool_t tb_gstream_ctrl1(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1)
{	
	tb_assert_and_check_return_val(gst && gst->ctrl1, TB_FALSE);

	tb_bool_t ret = TB_FALSE;
	switch (cmd)
	{
	case TB_GSTREAM_CMD_SET_URL:
		{
			if (arg1)
			{
				if (tb_url_set(&gst->url, (tb_char_t const*)arg1)) ret = TB_TRUE;
			}
		}
		break;
	case TB_GSTREAM_CMD_GET_URL:
		{
			if (arg1)
			{
				tb_char_t const* url = tb_url_get(&gst->url);
				if (url)
				{
					*((tb_char_t const**)arg1) = url;
					ret = TB_TRUE;
				}
			}
		}
		break;
	case TB_GSTREAM_CMD_SET_HOST:
		{
			if (arg1)
			{
				tb_url_host_set(&gst->url, (tb_char_t const*)arg1);
				ret = TB_TRUE;
			}
		}
		break;
	case TB_GSTREAM_CMD_GET_HOST:
		{
			if (arg1)
			{
				tb_char_t const* host = tb_url_host_get(&gst->url);
				if (host)
				{
					*((tb_char_t const**)arg1) = host;
					ret = TB_TRUE;
				}
			}
		}
		break;
	case TB_GSTREAM_CMD_SET_PORT:
		{
			if (arg1)
			{
				tb_url_port_set(&gst->url, (tb_size_t)arg1);
				ret = TB_TRUE;
			}
		}
		break;
	case TB_GSTREAM_CMD_GET_PORT:
		{
			if (arg1)
			{
				*((tb_size_t*)arg1) = tb_url_port_get(&gst->url);
				ret = TB_TRUE;
			}
		}
		break;
	case TB_GSTREAM_CMD_SET_PATH:
		{
			if (arg1)
			{
				tb_url_path_set(&gst->url, (tb_char_t const*)arg1);
				ret = TB_TRUE;
			}
		}
		break;
	case TB_GSTREAM_CMD_GET_PATH:
		{
			if (arg1)
			{
				tb_char_t const* path = tb_url_path_get(&gst->url);
				if (path)
				{
					*((tb_char_t const**)arg1) = path;
					ret = TB_TRUE;
				}
			}
		}
		break;
	case TB_GSTREAM_CMD_SET_SSL:
		{
			if (arg1)
			{
				tb_url_ssl_set(&gst->url, (tb_bool_t)arg1);
				ret = TB_TRUE;
			}
		}
		break;
	case TB_GSTREAM_CMD_GET_SSL:
		{
			if (arg1)
			{
				*((tb_bool_t*)arg1) = tb_url_ssl_get(&gst->url);
				ret = TB_TRUE;
			}
		}
		break;
	case TB_GSTREAM_CMD_SET_CACHE:
		{
			if (arg1)
			{
				tb_qbuffer_resize(&gst->cache, (tb_size_t)arg1);
				if (tb_qbuffer_maxn(&gst->cache)) ret = TB_TRUE;
			}
		}
		break;
	case TB_GSTREAM_CMD_GET_CACHE:
		{
			if (arg1)
			{
				*((tb_size_t*)arg1) = tb_qbuffer_maxn(&gst->cache);
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
	case TB_GSTREAM_CMD_GET_TIMEOUT:
		{
			if (arg1)
			{
				*((tb_size_t*)arg1) = gst->timeout;
				ret = TB_TRUE;
			}
		}
		break;
	default:
		break;
	}
	return (gst->ctrl1(gst, cmd, arg1) || ret)? TB_TRUE : TB_FALSE;
}
tb_bool_t tb_gstream_ctrl2(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1, tb_pointer_t arg2)
{
	tb_assert_and_check_return_val(gst && gst->ctrl2, TB_FALSE);
	return gst->ctrl2(gst, cmd, arg1, arg2);
}
tb_uint8_t tb_gstream_bread_u8(tb_gstream_t* gst)
{
	tb_byte_t b[1];
	if (!tb_gstream_bread(gst, b, 1)) return 0;
	return b[0];
}
tb_sint8_t tb_gstream_bread_s8(tb_gstream_t* gst)
{
	tb_byte_t b[1];
	if (!tb_gstream_bread(gst, b, 1)) return 0;
	return b[0];
}

tb_uint16_t tb_gstream_bread_u16_le(tb_gstream_t* gst)
{	
	tb_byte_t b[2];
	if (!tb_gstream_bread(gst, b, 2)) return 0;
	return tb_bits_get_u16_le(b);
}
tb_sint16_t tb_gstream_bread_s16_le(tb_gstream_t* gst)
{	
	tb_byte_t b[2];
	if (!tb_gstream_bread(gst, b, 2)) return 0;
	return tb_bits_get_s16_le(b);
}
tb_uint32_t tb_gstream_bread_u24_le(tb_gstream_t* gst)
{	
	tb_byte_t b[3];
	if (!tb_gstream_bread(gst, b, 3)) return 0;
	return tb_bits_get_u24_le(b);
}
tb_sint32_t tb_gstream_bread_s24_le(tb_gstream_t* gst)
{
	tb_byte_t b[3];
	if (!tb_gstream_bread(gst, b, 3)) return 0;
	return tb_bits_get_s24_le(b);
}
tb_uint32_t tb_gstream_bread_u32_le(tb_gstream_t* gst)
{
	tb_byte_t b[4];
	if (!tb_gstream_bread(gst, b, 4)) return 0;
	return tb_bits_get_u32_le(b);
}
tb_sint32_t tb_gstream_bread_s32_le(tb_gstream_t* gst)
{	
	tb_byte_t b[4];
	if (!tb_gstream_bread(gst, b, 4)) return 0;
	return tb_bits_get_s32_le(b);
}
tb_uint16_t tb_gstream_bread_u16_be(tb_gstream_t* gst)
{	
	tb_byte_t b[2];
	if (!tb_gstream_bread(gst, b, 2)) return 0;
	return tb_bits_get_u16_be(b);
}
tb_sint16_t tb_gstream_bread_s16_be(tb_gstream_t* gst)
{	
	tb_byte_t b[2];
	if (!tb_gstream_bread(gst, b, 2)) return 0;
	return tb_bits_get_s16_be(b);
}
tb_uint32_t tb_gstream_bread_u24_be(tb_gstream_t* gst)
{	
	tb_byte_t b[3];
	if (!tb_gstream_bread(gst, b, 3)) return 0;
	return tb_bits_get_u24_be(b);
}
tb_sint32_t tb_gstream_bread_s24_be(tb_gstream_t* gst)
{
	tb_byte_t b[3];
	if (!tb_gstream_bread(gst, b, 3)) return 0;
	return tb_bits_get_s24_be(b);
}
tb_uint32_t tb_gstream_bread_u32_be(tb_gstream_t* gst)
{
	tb_byte_t b[4];
	if (!tb_gstream_bread(gst, b, 4)) return 0;
	return tb_bits_get_u32_be(b);
}
tb_sint32_t tb_gstream_bread_s32_be(tb_gstream_t* gst)
{	
	tb_byte_t b[4];
	if (!tb_gstream_bread(gst, b, 4)) return 0;
	return tb_bits_get_s32_be(b);
}
tb_bool_t tb_gstream_bwrit_u8(tb_gstream_t* gst, tb_uint8_t val)
{
	tb_byte_t b[1];
	tb_bits_set_u8(b, val);
	return tb_gstream_bwrit(gst, b, 1);
}
tb_bool_t tb_gstream_bwrit_s8(tb_gstream_t* gst, tb_sint8_t val)
{
	tb_byte_t b[1];
	tb_bits_set_s8(b, val);
	return tb_gstream_bwrit(gst, b, 1);
}

tb_bool_t tb_gstream_bwrit_u16_le(tb_gstream_t* gst, tb_uint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_u16_le(b, val);
	return tb_gstream_bwrit(gst, b, 2);
}
tb_bool_t tb_gstream_bwrit_s16_le(tb_gstream_t* gst, tb_sint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_s16_le(b, val);
	return tb_gstream_bwrit(gst, b, 2);
}

tb_bool_t tb_gstream_bwrit_u24_le(tb_gstream_t* gst, tb_uint32_t val)
{	
	tb_byte_t b[3];
	tb_bits_set_u24_le(b, val);
	return tb_gstream_bwrit(gst, b, 3);
}
tb_bool_t tb_gstream_bwrit_s24_le(tb_gstream_t* gst, tb_sint32_t val)
{
	tb_byte_t b[3];
	tb_bits_set_s24_le(b, val);
	return tb_gstream_bwrit(gst, b, 3);
}

tb_bool_t tb_gstream_bwrit_u32_le(tb_gstream_t* gst, tb_uint32_t val)
{	
	tb_byte_t b[4];
	tb_bits_set_u32_le(b, val);
	return tb_gstream_bwrit(gst, b, 4);
}
tb_bool_t tb_gstream_bwrit_s32_le(tb_gstream_t* gst, tb_sint32_t val)
{
	tb_byte_t b[4];
	tb_bits_set_s32_le(b, val);
	return tb_gstream_bwrit(gst, b, 4);
}

tb_bool_t tb_gstream_bwrit_u16_be(tb_gstream_t* gst, tb_uint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_u16_be(b, val);
	return tb_gstream_bwrit(gst, b, 2);
}
tb_bool_t tb_gstream_bwrit_s16_be(tb_gstream_t* gst, tb_sint16_t val)
{
	tb_byte_t b[2];
	tb_bits_set_s16_be(b, val);
	return tb_gstream_bwrit(gst, b, 2);
}

tb_bool_t tb_gstream_bwrit_u24_be(tb_gstream_t* gst, tb_uint32_t val)
{	
	tb_byte_t b[3];
	tb_bits_set_u24_be(b, val);
	return tb_gstream_bwrit(gst, b, 3);
}
tb_bool_t tb_gstream_bwrit_s24_be(tb_gstream_t* gst, tb_sint32_t val)
{
	tb_byte_t b[3];
	tb_bits_set_s24_be(b, val);
	return tb_gstream_bwrit(gst, b, 3);
}

tb_bool_t tb_gstream_bwrit_u32_be(tb_gstream_t* gst, tb_uint32_t val)
{	
	tb_byte_t b[4];
	tb_bits_set_u32_be(b, val);
	return tb_gstream_bwrit(gst, b, 4);
}
tb_bool_t tb_gstream_bwrit_s32_be(tb_gstream_t* gst, tb_sint32_t val)
{
	tb_byte_t b[4];
	tb_bits_set_s32_be(b, val);
	return tb_gstream_bwrit(gst, b, 4);
}
tb_uint64_t tb_gstream_load(tb_gstream_t* gst, tb_gstream_t* ist)
{
	tb_assert_and_check_return_val(gst && ist, 0);	

	// read data
	tb_byte_t 		data[TB_GSTREAM_BLOCK_MAXN];
	tb_uint64_t 	read = 0;
	tb_uint64_t 	left = tb_gstream_left(ist);
	do
	{
		// read data
		tb_long_t n = tb_gstream_aread(ist, data, TB_GSTREAM_BLOCK_MAXN);
		if (n > 0)
		{
			// writ data
			if (!tb_gstream_bwrit(gst, data, n)) break;

			// update read
			read += n;
		}
		else if (!n) 
		{
			// wait
			tb_long_t e = tb_gstream_wait(ist, TB_AIOO_ETYPE_READ, ist->timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOO_ETYPE_READ);
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

