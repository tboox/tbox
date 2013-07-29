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
 * @file		http.c
 * @ingroup 	network
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 			"http"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "http.h"
#include "../aio/aio.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../stream/stream.h"
#include "../platform/platform.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the default timeout
#define TB_HTTP_TIMEOUT_DEFAULT 				(10000)

// the default max redirect
#define TB_HTTP_MRDT_DEFAULT 					(10)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the http step type
typedef enum __tb_http_state_t
{
	TB_HTTP_STEP_NONE 	= 0
,	TB_HTTP_STEP_CONN 	= 1
,	TB_HTTP_STEP_REQT 	= 2
,	TB_HTTP_STEP_RESP 	= 4
,	TB_HTTP_STEP_SEEK 	= 8
,	TB_HTTP_STEP_NEVT 	= 16
,	TB_HTTP_STEP_POST 	= 32

}tb_http_state_t;

// the http type
typedef struct __tb_http_t
{
	// the option
	tb_http_option_t 	option;

	// the status 
	tb_http_status_t 	status;

	// the stream
	tb_gstream_t* 		stream;

	// the spool
	tb_handle_t			spool;

	// the step
	tb_size_t 			step;

	// the try count
	tb_size_t 			tryn;

	// the redirect count
	tb_size_t 			rdtn;

	// the data && size for request and response
	tb_pstring_t 		data;
	tb_size_t 			size;

	// the chunk size
	tb_size_t 			chunked_read;
	tb_size_t 			chunked_size;

}tb_http_t;

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_char_t const* tb_http_methods[] = 
{
	"GET"
, 	"POST"
, 	"HEAD"
, 	"PUT"
, 	"OPTIONS"
, 	"DELETE"
, 	"TRACE"
, 	"CONNECT"
};

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_http_option_init(tb_http_t* http)
{
	// init default
	http->option.method 	= TB_HTTP_METHOD_GET;
	http->option.rdtm 		= TB_HTTP_MRDT_DEFAULT;
	http->option.timeout 	= TB_HTTP_TIMEOUT_DEFAULT;
	http->option.version 	= TB_HTTP_VERSION_11;
	http->option.post 		= 0;

	// init url
	if (!tb_url_init(&http->option.url)) return tb_false;

	// init head
	http->option.head = tb_hash_init(8, tb_item_func_str(tb_false, http->spool), tb_item_func_str(tb_false, http->spool));
	tb_assert_and_check_return_val(http->option.head, tb_false);

	// ok
	return tb_true;
}
static tb_void_t tb_http_option_exit(tb_http_t* http)
{
	// exit head
	if (http->option.head) tb_hash_exit(http->option.head);
	http->option.head = tb_null;

	// exit url
	tb_url_exit(&http->option.url);
}
static tb_bool_t tb_http_status_init(tb_http_t* http)
{
	http->status.version = TB_HTTP_VERSION_11;
	if (!tb_pstring_init(&http->status.content_type)) return tb_false;
	if (!tb_pstring_init(&http->status.location)) return tb_false;
	return tb_true;
}
static tb_void_t tb_http_status_exit(tb_http_t* http)
{
	tb_pstring_exit(&http->status.content_type);
	tb_pstring_exit(&http->status.location);
}
static tb_void_t tb_http_status_clear(tb_http_t* http)
{
	http->status.code = 0;
	http->status.bchunked = 0;
	http->status.content_size = 0;
	http->status.post_size = 0;
	tb_pstring_clear(&http->status.content_type);
	tb_pstring_clear(&http->status.location);

	// persistent state
#if 0
	http->status.version = TB_HTTP_VERSION_11;
	http->status.balive = 0;
	http->status.bseeked = 0;
	http->status.document_size = 0;
#endif
}
/* chunked_data
 *
 *   head     data   tail
 * ea5\r\n ..........\r\n e65\r\n..............\r\n 0\r\n\r\n
 * ---------------------- ------------------------- ---------
 *        chunk0                  chunk1               end
 */
static tb_long_t tb_http_chunked_aread(tb_http_t* http, tb_byte_t* data, tb_size_t size)
{
	// parse chunked head and chunked tail
	if (!http->chunked_size || http->chunked_read >= http->chunked_size)
	{
		// read chunked line
		tb_char_t 			ch[1];
		tb_long_t 			cn = 0;
		while (1)
		{
			// read char
			cn = tb_gstream_aread(http->stream, ch, 1);
			tb_assert_and_check_return_val(cn >= 0, -1);

			// no data? 
			tb_check_return_val(cn, 0);

			// there should be no '\0'
			tb_assert_and_check_return_val(*ch, -1);

			// append char to line
			if (*ch != '\n') tb_pstring_chrcat(&http->data, *ch);
			// is line end?
			else
			{
				// strip '\r' if exists
				tb_char_t const* 	pb = tb_pstring_cstr(&http->data);
				tb_size_t 			pn = tb_pstring_size(&http->data);
				tb_assert_and_check_return_val(pb, -1);

				if (pb[pn - 1] == '\r')
					tb_pstring_strip(&http->data, pn - 1);

				// is chunked tail? only "\r\n"
				if (!tb_pstring_size(&http->data)) 
				{
					// reset chunked size
					http->chunked_read = 0;
					http->chunked_size = 0;

					// continue
					return 0;
				}
				// is chunked head? parse size
				else
				{
					// parse size
					http->chunked_size = tb_s16tou32(pb);

					// is file end? "0\r\n\r\n"
					tb_check_return_val(http->chunked_size, -1);

					// clear data
					tb_pstring_clear(&http->data);

					// ok
					break;
				}
			}
		}
	}

	// check
	tb_assert_and_check_return_val(http->chunked_read < http->chunked_size, -1);

	// read chunked data
	tb_long_t r = tb_gstream_aread(http->stream, data, tb_min(size, http->chunked_size - http->chunked_read));

	// update read
	if (r > 0) http->chunked_read += r;

	// ok
	return r;
}
static tb_long_t tb_http_chunked_awrit(tb_http_t* http, tb_byte_t* data, tb_size_t size)
{
	tb_trace_noimpl();
	return -1;
}
static tb_long_t tb_http_connect(tb_http_t* http)
{
	// have been connected?
	tb_check_return_val(!(http->step & TB_HTTP_STEP_CONN), 1);

	// ioctl
	if (!http->tryn)
	{
		// clear status
		tb_http_status_clear(http);

		// ctrl stream
		tb_gstream_ctrl(http->stream, TB_GSTREAM_CMD_SET_SSL, tb_url_ssl_get(&http->option.url));
		tb_gstream_ctrl(http->stream, TB_GSTREAM_CMD_SET_HOST, tb_url_host_get(&http->option.url));
		tb_gstream_ctrl(http->stream, TB_GSTREAM_CMD_SET_PORT, tb_url_port_get(&http->option.url));
		tb_gstream_ctrl(http->stream, TB_GSTREAM_CMD_SET_PATH, tb_url_path_get(&http->option.url));
		tb_gstream_ctrl(http->stream, TB_GSTREAM_CMD_SET_SFUNC, &http->option.sfunc);
	}

	// tryn++
	http->tryn++;

	// need wait connection
	http->step &= ~TB_HTTP_STEP_NEVT;

	// open stream
	tb_trace_impl("connect: try");
	tb_long_t r = tb_gstream_aopen(http->stream);
	tb_check_return_val(r > 0, r);

	// ok
	http->step |= TB_HTTP_STEP_CONN;
	http->tryn = 0;
	tb_trace_impl("connect: ok");
	return r;
}
static tb_long_t tb_http_request(tb_http_t* http)
{
	tb_check_return_val(!(http->step & TB_HTTP_STEP_REQT), 1);

	// tryn++
	http->tryn++;

	// format it first if the request is null
	if (!tb_pstring_size(&http->data))
	{
		// check size
		tb_assert_and_check_return_val(!http->size, -1);

		// init string
		tb_char_t  		b[64];
		tb_sstring_t 	s;
		if (!tb_sstring_init(&s, b, 64)) return -1;

		// init method
		tb_assert_and_check_return_val(http->option.method < tb_arrayn(tb_http_methods), -1);
		tb_char_t const* method = tb_http_methods[http->option.method];
		tb_assert_and_check_return_val(method, -1);

		// init path
		tb_char_t const* path = tb_url_path_get(&http->option.url);
		tb_assert_and_check_return_val(path, -1);

		// init args
		tb_char_t const* args = tb_url_args_get(&http->option.url);

		// init host
		tb_char_t const* host = tb_url_host_get(&http->option.url);
		tb_assert_and_check_return_val(host, -1);
		tb_hash_set(http->option.head, "Host", host);

		// init accept
		if (!tb_hash_get(http->option.head, "Accept")) 
			tb_hash_set(http->option.head, "Accept", "*/*");

		// init connection
		tb_hash_set(http->option.head, "Connection", http->option.balive? "keep-alive" : "close");

		// init range
		tb_sstring_clear(&s);
		if (http->option.range.bof && http->option.range.eof > http->option.range.bof)
			tb_sstring_cstrfcpy(&s, "bytes=%llu-%llu", http->option.range.bof, http->option.range.eof);
		else if (http->option.range.bof && !http->option.range.eof)
			tb_sstring_cstrfcpy(&s, "bytes=%llu-", http->option.range.bof);
		else if (!http->option.range.bof && http->option.range.eof)
			tb_sstring_cstrfcpy(&s, "bytes=0-%llu", http->option.range.eof);

		if (tb_sstring_size(&s)) 
			tb_hash_set(http->option.head, "Range", tb_sstring_cstr(&s));

		// init post
		if (http->option.method == TB_HTTP_METHOD_POST)
		{
			// append post size
			tb_sstring_cstrfcpy(&s, "%llu", http->option.post);
			tb_hash_set(http->option.head, "Content-Length", tb_sstring_cstr(&s));
		}

		// check head
		tb_assert_and_check_return_val(tb_hash_size(http->option.head), -1);

		// append method
		tb_pstring_cstrcat(&http->data, method);
	
		// append ' '
		tb_pstring_chrcat(&http->data, ' ');

		// append path
		tb_pstring_cstrcat(&http->data, path);

		// append args if exists
		if (args) 
		{
			tb_pstring_chrcat(&http->data, '?');
			tb_pstring_cstrcat(&http->data, args);
		}
	
		// append ' '
		tb_pstring_chrcat(&http->data, ' ');

		// append version, HTTP/1.1
		tb_pstring_cstrfcat(&http->data, "HTTP/1.%1u\r\n", http->option.version);

		// append key: value
		tb_size_t itor = tb_iterator_head(http->option.head);
		tb_size_t tail = tb_iterator_tail(http->option.head);
		for (; itor != tail; itor = tb_iterator_next(http->option.head, itor))
		{
			tb_hash_item_t const* item = tb_iterator_item(http->option.head, itor);
			if (item && item->name && item->data) 
				tb_pstring_cstrfcat(&http->data, "%s: %s\r\n", (tb_char_t const*)item->name, (tb_char_t const*)item->data);
		}
	
		// append end
		tb_pstring_cstrcat(&http->data, "\r\n");

		// exit string
		tb_sstring_exit(&s);

		// check request
		tb_assert_and_check_return_val(tb_pstring_size(&http->data) && tb_pstring_cstr(&http->data), -1);

		// trace
		tb_trace_impl("request:\n%s", tb_pstring_cstr(&http->data));
	}

	// the head data && size
	tb_char_t const* 	head_data = tb_pstring_cstr(&http->data);
	tb_size_t 			head_size = tb_pstring_size(&http->data);
	tb_assert_and_check_return_val(head_data && head_size, -1);
	
	// send head
	if (http->size < head_size)
	{
		// need wait if no data
		http->step &= ~TB_HTTP_STEP_NEVT;

		// send head
		tb_trace_impl("request: send: head: %lu <? %lu", http->size, head_size);
		while (http->size < head_size)
		{
			// writ data
			tb_long_t real = tb_gstream_awrit(http->stream, head_data + http->size, head_size - http->size);
			tb_assert_and_check_return_val(real >= 0, -1);

			// no data? 
			tb_check_return_val(real, 0);

			// update size
			http->size += real;
		}

		// check
		tb_assert_and_check_return_val(http->size == head_size, -1);
	}

	// need wait if no data
	http->step &= ~TB_HTTP_STEP_NEVT;

	// flush writed data
	tb_long_t r = tb_gstream_afwrit(http->stream, tb_null, 0);

	// continue it if has data
	tb_check_return_val(r < 0, 0);

	// finish it
	http->step |= TB_HTTP_STEP_REQT;
	http->tryn = 0;

	// reset data
	http->size = 0;
	tb_pstring_clear(&http->data);

	// send post
	if (http->option.method == TB_HTTP_METHOD_POST)
	{	
		http->step &= ~TB_HTTP_STEP_NEVT;
		http->step |= TB_HTTP_STEP_POST;
	}

	// ok
	tb_trace_impl("request: ok");
	return 1;
}
/*
 * HTTP/1.1 206 Partial Content
 * Date: Fri, 23 Apr 2010 05:25:45 GMT
 * Server: Apache/2.2.9 (Ubuntu) PHP/5.2.6-2ubuntu4.5 with Suhosin-Patch
 * Last-Modified: Mon, 08 Mar 2010 09:58:09 GMT
 * ETag: "6cc014-8f47f-481471a322e40"
 * Accept-Ranges: bytes
 * Content-Length: 586879
 * Content-Range: bytes 0-586878/586879
 * Connection: close
 * Content-Type: application/x-shockwave-flash
 */
static tb_bool_t tb_http_response_done(tb_http_t* http)
{
	// line && size
	tb_char_t* 	line = tb_pstring_cstr(&http->data);
	tb_size_t 	size = tb_pstring_size(&http->data);
	tb_assert_and_check_return_val(line && size, tb_false);

	// init 
	tb_char_t* 	p = line;

	// the first line? 
	if (!http->size)
	{
		// seek to the http version
		while (*p && *p != '.') p++; 
		tb_assert_and_check_return_val(*p, tb_false);
		p++;

		// parse version
		tb_assert_and_check_return_val((*p - '0') < 2, tb_false);
		http->status.version = *p - '0';
	
		// seek to the http code
		p++; while (tb_isspace(*p)) p++;

		// parse code
		tb_assert_and_check_return_val(*p && tb_isdigit(*p), tb_false);
		http->status.code = tb_stou32(p);

		// check error code: 4xx & 5xx
		if (http->status.code >= 400 && http->status.code < 600) return tb_false;
	}
	// key: value?
	else
	{
		// seek to value
		while (*p && *p != ':') p++;
		tb_assert_and_check_return_val(*p, tb_false);
		p++; while (tb_isspace(*p)) p++;
		tb_assert_and_check_return_val(*p, tb_false);

		// parse content size
		if (!tb_strnicmp(line, "Content-Length", 14))
		{
			http->status.content_size = tb_stou64(p);
			if (!http->status.document_size) 
				http->status.document_size = http->status.content_size;
		}
		// parse content range: "bytes $from-$to/$document_size"
		else if (!tb_strnicmp(line, "Content-Range", 13))
		{
			tb_hize_t from = 0;
			tb_hize_t to = 0;
			tb_hize_t document_size = 0;
			if (!tb_strncmp(p, "bytes ", 6)) 
			{
				p += 6;
				from = tb_stou64(p);
				while (*p && *p != '-') p++;
				if (*p && *p++ == '-') to = tb_stou64(p);
				while (*p && *p != '/') p++;
				if (*p && *p++ == '/') document_size = tb_stou64(p);
			}
			// no stream, be able to seek
			http->status.bseeked = 1;
			http->status.document_size = document_size;
			if (!http->status.content_size) 
			{
				if (from && to > from) http->status.content_size = to - from;
				else if (!from && to) http->status.content_size = to;
				else if (from && !to && document_size > from) http->status.content_size = document_size - from;
				else http->status.content_size = document_size;
			}
		}
		// parse accept-ranges: "bytes "
		else if (!tb_strnicmp(line, "Accept-Ranges", 13))
		{
			// no stream, be able to seek
			http->status.bseeked = 1;
		}
		// parse content type
		else if (!tb_strnicmp(line, "Content-Type", 12)) 
		{
			tb_pstring_cstrcpy(&http->status.content_type, p);
			tb_assert_and_check_return_val(tb_pstring_size(&http->status.content_type), tb_false);
		}
		// parse transfer encoding
		else if (!tb_strnicmp(line, "Transfer-Encoding", 17))
		{
			if (!tb_stricmp(p, "chunked")) http->status.bchunked = 1;
		}
		// parse location
		else if (!tb_strnicmp(line, "Location", 8)) 
		{
			// redirect? check code: 301 - 303
			tb_assert_and_check_return_val(http->status.code > 300 && http->status.code < 304, tb_false);

			// init redirect
			http->rdtn = 0;

			// save location
			tb_pstring_cstrcpy(&http->status.location, p);
		}
		// parse connection
		else if (!tb_strnicmp(line, "Connection", 10))
		{
			http->status.balive = !tb_stricmp(p, "close")? 0 : 1;
		}
	}

	// ok
	return tb_true;
}
static tb_long_t tb_http_response(tb_http_t* http)
{
	tb_check_return_val(!(http->step & TB_HTTP_STEP_RESP), 1);

	// init
	tb_char_t 			ch[1];
	tb_long_t 			cn = 0;

	// tryn++
	http->tryn++;

	// need wait if no data
	http->step &= ~TB_HTTP_STEP_NEVT;

	// read response
	while (1)
	{
		// read char
		cn = tb_gstream_aread(http->stream, ch, 1);

		// abort?
		tb_check_return_val(cn >= 0, -1);

		// no data? 
		tb_check_return_val(cn, 0);

		// there should be no '\0'
		tb_assert_and_check_return_val(*ch, -1);

		// append char to line
		if (*ch != '\n') tb_pstring_chrcat(&http->data, *ch);
		// is line end?
		else
		{
			// strip '\r' if exists
			tb_char_t const* 	pb = tb_pstring_cstr(&http->data);
			tb_size_t 			pn = tb_pstring_size(&http->data);
			tb_assert_and_check_return_val(pb, -1);

			if (pb[pn - 1] == '\r')
				tb_pstring_strip(&http->data, pn - 1);

			// trace
			tb_trace_impl("response: %s", pb);

			// do callback
			if (http->option.hfunc) if (!http->option.hfunc((tb_handle_t)http, pb)) return -1;
			
			// end?
			if (!tb_pstring_size(&http->data)) break;

			// done it
			if (!tb_http_response_done(http)) return -1;

			// clear data
			tb_pstring_clear(&http->data);

			// line++
			http->size++;
		}
	}

	// finish it
	http->step |= TB_HTTP_STEP_RESP;
	http->tryn = 0;

	// reset data
	http->size = 0;
	tb_pstring_clear(&http->data);

	// reset chunked size
	http->chunked_read = 0;
	http->chunked_size = 0;

	// ok
	tb_trace_impl("response: ok");
	return 1;
}

static tb_long_t tb_http_redirect(tb_http_t* http)
{
	tb_check_return_val(tb_pstring_size(&http->status.location), 1);

	// redirect
	if (http->rdtn < http->option.rdtm)
	{
		// not keep-alive?
		if (!http->status.balive) 
		{
			// close stream
			tb_long_t r = tb_gstream_aclose(http->stream);
			tb_assert_and_check_return_val(r >= 0, -1);

			// continue ?
			tb_check_return_val(r, 0);
		}
		else
		{
			// flush readed data
			tb_long_t r = tb_gstream_afread(http->stream, tb_null, 0);

			// continue it if has data
			tb_check_return_val(r < 0, 0);
		}

		// set url
		tb_trace_impl("redirect: %s", tb_pstring_cstr(&http->status.location));
		if (!tb_url_set(&http->option.url, tb_pstring_cstr(&http->status.location))) return -1;

		// set keep-alive
		http->option.balive = http->status.balive;

		// set version
		http->option.version = http->status.version;

		// redirect++
		http->rdtn++;
		
		// reset step, no event now, need not wait
		http->step = ((http->step & TB_HTTP_STEP_SEEK)? TB_HTTP_STEP_SEEK : TB_HTTP_STEP_NONE) | TB_HTTP_STEP_NEVT;
		http->tryn = 0;

		// reset chunk size
		http->chunked_read = 0;
		http->chunked_size = 0;

		// continue 
		return 0;
	}

	// ok
	return 1;
}
static tb_long_t tb_http_seek(tb_http_t* http, tb_hize_t offset)
{
	tb_check_return_val(!(http->step & TB_HTTP_STEP_SEEK), 1);
	
	// not keep-alive?
	if (!http->status.balive) 
	{
		// close stream
		tb_long_t r = tb_gstream_aclose(http->stream);
		tb_assert_and_check_return_val(r >= 0, -1);

		// continue ?
		tb_check_return_val(r, 0);
	}

	// set keep-alive
	http->option.balive = http->status.balive;

	// set version
	http->option.version = http->status.version;

	// reset redirect
	http->rdtn = 0;
	
	// reset step, no event now, need not wait
	http->step = TB_HTTP_STEP_SEEK | TB_HTTP_STEP_NEVT;
	http->tryn = 0;

	// reset chunk size
	http->chunked_read = 0;
	http->chunked_size = 0;

	// set range
	http->option.range.bof = offset;
	http->option.range.eof = http->status.document_size? http->status.document_size - 1 : 0;

	// ok
	tb_trace_impl("seek: %llu", offset);
	return 1;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_handle_t tb_http_init()
{
	// init http
	tb_http_t* http = tb_malloc0(sizeof(tb_http_t));
	tb_assert_and_check_return_val(http, tb_null);

	// init stream
	http->stream = tb_gstream_init_sock();
	tb_assert_and_check_goto(http->stream, fail);

	// init spool
	http->spool = tb_spool_init(TB_SPOOL_GROW_MICRO, 0);
	tb_assert_and_check_goto(http->spool, fail);

	// init data
	if (!tb_pstring_init(&http->data)) goto fail;

	// init option
	if (!tb_http_option_init(http)) goto fail;

	// init status
	if (!tb_http_status_init(http)) goto fail;

	// ok
	return (tb_handle_t)http;

fail:
	if (http) tb_http_exit((tb_handle_t)http);
	return tb_null;
}
tb_void_t tb_http_exit(tb_handle_t handle)
{
	if (handle)
	{
		tb_http_t* http = (tb_http_t*)handle;

		// close it
		tb_http_bclose(handle);
		
		// exit status
		tb_http_status_exit(http);

		// exit option
		tb_http_option_exit(http);

		// exit data
		tb_pstring_exit(&http->data);

		// exit spool
		if (http->spool) tb_spool_exit(http->spool);
		http->spool = tb_null;

		// exit stream
		if (http->stream) tb_gstream_exit(http->stream);
		http->stream = tb_null;

		// free it
		tb_free(http);
	}
}
tb_long_t tb_http_wait(tb_handle_t handle, tb_size_t etype, tb_long_t timeout)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, -1);
	
	// wait event
	tb_size_t e = TB_AIOO_ETYPE_NULL;
	if (!(http->step & TB_HTTP_STEP_NEVT))
	{
		if (!(http->step & TB_HTTP_STEP_CONN)) e = TB_AIOO_ETYPE_CONN;
		else if (!(http->step & TB_HTTP_STEP_REQT)) e = TB_AIOO_ETYPE_WRIT;
		else if (!(http->step & TB_HTTP_STEP_RESP)) e = TB_AIOO_ETYPE_READ;
		else e = etype;
	}
		
	// wait
	return e? tb_gstream_wait(http->stream, e, timeout) : etype;
}
tb_long_t tb_http_aopen(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, -1);

	// check stream
	tb_long_t r = -1;

	// connect
	r = tb_http_connect(http);
	tb_check_return_val(r > 0, r);
	
	// request
	r = tb_http_request(http);
	tb_check_return_val(r > 0, r);
		
	// no post? wait response
	if (!(http->step & TB_HTTP_STEP_POST))
	{
		// response
		r = tb_http_response(http);
		tb_check_return_val(r > 0, r);

		// redirect
		r = tb_http_redirect(http);
		tb_check_return_val(r > 0, r);
	}

	// ok
	return r;
}
tb_bool_t tb_http_bopen(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(handle, tb_false);

	// dump option
#if defined(__tb_debug__) && defined(TB_TRACE_IMPL_TAG)
	tb_http_option_dump(handle);
#endif

	// try opening it
	tb_long_t r = 0;
	while (!(r = tb_http_aopen(handle)))
	{
		// wait
		r = tb_http_wait(handle, TB_AIOO_ETYPE_EALL, http->option.timeout);

		// fail or timeout?
		tb_check_break(r > 0);
	}

	// dump status
#if defined(__tb_debug__) && defined(TB_TRACE_IMPL_TAG)
	tb_http_status_dump(handle);
#endif

	// close it if fail
	if (r <= 0) tb_http_bclose(handle);

	// ok?
	return r > 0? tb_true : tb_false;
}
tb_long_t tb_http_aclose(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, -1);

	// have step? clear it
	if (http->step)
	{
		// connected?
		if (http->step & TB_HTTP_STEP_CONN) 
		{
			// not keep-alive?
			if (!http->status.balive)
			{
				// close stream
				tb_long_t r = tb_gstream_aclose(http->stream);
				tb_assert_and_check_return_val(r >= 0, -1);

				// continue ?
				tb_check_return_val(r, 0);
			}
			// clear stream
			else tb_gstream_clear(http->stream);
		}

		// clear status
		tb_http_status_clear(http);

		// reset data
		http->size = 0;
		tb_pstring_clear(&http->data);

		// clear step
		http->step = TB_HTTP_STEP_NONE;
		http->tryn = 0;

		// clear redirect
		http->rdtn = 0;

		// reset chunk size
		http->chunked_read = 0;
		http->chunked_size = 0;
	}

	// ok
	return 1;
}
tb_bool_t tb_http_bclose(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(handle, tb_false);

	// try opening it
	tb_long_t 	r = 0;
	tb_hong_t 	t = tb_mclock();
	while (!(r = tb_http_aclose(handle)))
	{
		// timeout?
		if (tb_mclock() - t > http->option.timeout) break;

		// sleep some time
		tb_usleep(100);
	}

	// ok?
	return r > 0? tb_true : tb_false;
}
tb_long_t tb_http_aseek(tb_handle_t handle, tb_hize_t offset)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, -1);
	
	// check stream
	tb_long_t r = -1;

	// seek
	r = tb_http_seek(http, offset);
	tb_check_return_val(r > 0, r);

	// open
	r = tb_http_aopen(http);
	tb_check_return_val(r > 0, r);

	// ok
	http->step &= ~TB_HTTP_STEP_SEEK;
	return r;
}
tb_bool_t tb_http_bseek(tb_handle_t handle, tb_hize_t offset)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(handle, tb_false);

	// init step
	http->step &= ~TB_HTTP_STEP_SEEK;

	// try seeking it
	tb_long_t r = 0;
	while (!(r = tb_http_aseek(handle, offset)))
	{
		// wait
		r = tb_http_wait(handle, TB_AIOO_ETYPE_EALL, http->option.timeout);

		// fail or timeout?
		tb_check_break(r > 0);
	}

	// clear step
	http->step &= ~TB_HTTP_STEP_SEEK;

	// ok?
	return r > 0? tb_true : tb_false;
}
tb_long_t tb_http_awrit(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream && (http->step & TB_HTTP_STEP_POST), -1);

	// writ the post data
	tb_hong_t ok = http->option.bchunked? tb_http_chunked_awrit(http, data, size) : tb_gstream_awrit(http->stream, data, size);
	tb_check_return_val(ok >= 0, -1);

	// save the post size
	if (ok > 0) http->status.post_size += ok;

	// finish post? flush it
	if (http->status.post_size >= http->option.post)
	{
		// flush writed data
		ok = tb_gstream_afwrit(http->stream, tb_null, 0);

		// continue it if has data
		tb_check_return_val(ok < 0, 0);
	}

	// ok?
	return ok;
}
tb_long_t tb_http_aread(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, -1);

	// has post? 
	if (http->step & TB_HTTP_STEP_POST)
	{
		// finish post? read response
		tb_assert_and_check_return_val(http->status.post_size >= http->option.post, -1);

		// response
		tb_long_t ok = tb_http_response(http);
		tb_check_return_val(ok > 0, ok);

		// redirect
		ok = tb_http_redirect(http);
		tb_assert_and_check_return_val(ok != 0, -1); // FIXME: post redirect is not supported
		tb_check_return_val(ok > 0, ok);

		// reset status
		http->status.post_size = 0;
		http->step &= ~TB_HTTP_STEP_POST;
	}

	// read
	return http->status.bchunked? tb_http_chunked_aread(http, data, size) : tb_gstream_aread(http->stream, data, size);
}
tb_bool_t tb_http_bwrit(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, tb_false);

	// writ
	tb_long_t writ = 0;
	while (writ < size)
	{
		// writ data
		tb_long_t n = tb_http_awrit(handle, data + writ, size - writ);

		// update size
		if (n > 0) writ += n;
		// no data?
		else if (!n)
		{
			// wait
			tb_long_t e = tb_http_wait(handle, TB_AIOO_ETYPE_WRIT, http->option.timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOO_ETYPE_WRIT);
		}
		else break;
	}

	// ok?
	return writ == size? tb_true : tb_false;
}
tb_bool_t tb_http_bread(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{	
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, tb_false);

	// read
	tb_long_t read = 0;
	while (read < size)
	{
		// read data
		tb_long_t n = tb_http_aread(handle, data + read, size - read);

		// update size
		if (n > 0) read += n;
		// no data?
		else if (!n)
		{
			// wait
			tb_long_t e = tb_http_wait(handle, TB_AIOO_ETYPE_READ, http->option.timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOO_ETYPE_READ);
		}
		else break;
	}

	// ok?
	return read == size? tb_true : tb_false;
}
tb_http_option_t* tb_http_option(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, tb_null);
	return &http->option;
}
tb_http_status_t const* tb_http_status(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, tb_null);
	return &http->status;
}

#ifdef __tb_debug__
tb_void_t tb_http_option_dump(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return(http);

	tb_trace("[http]: ======================================================================");
	tb_trace("[http]: option: ");
	tb_trace("[http]: option: url: %s", tb_url_get(&http->option.url));
	tb_trace("[http]: option: version: HTTP/1.%1u", http->option.version);
	tb_trace("[http]: option: method: %s", http->option.method < tb_arrayn(tb_http_methods)? tb_http_methods[http->option.method] : "none");
	tb_trace("[http]: option: rdtm: %d", http->option.rdtm);
	tb_trace("[http]: option: range: %llu-%llu", http->option.range.bof, http->option.range.eof);
	tb_trace("[http]: option: balive: %s", http->option.balive? "true" : "false");
	tb_trace("[http]: option: bchunked: %s", http->option.bchunked? "true" : "false");
}
tb_void_t tb_http_status_dump(tb_handle_t handle)
{
	tb_assert_and_check_return(handle);
	tb_http_t* http = (tb_http_t*)handle;

	tb_trace("[http]: ======================================================================");
	tb_trace("[http]: status: ");
	tb_trace("[http]: status: code: %d", http->status.code);
	tb_trace("[http]: status: version: HTTP/1.%1u", http->status.version);
	tb_trace("[http]: status: content:type: %s", tb_pstring_cstr(&http->status.content_type));
	tb_trace("[http]: status: content:size: %llu", http->status.content_size);
	tb_trace("[http]: status: document:size: %llu", http->status.document_size);
	tb_trace("[http]: status: location: %s", tb_pstring_cstr(&http->status.location));
	tb_trace("[http]: status: balive: %s", http->status.balive? "true" : "false");
	tb_trace("[http]: status: bseeked: %s", http->status.bseeked? "true" : "false");
	tb_trace("[http]: status: bchunked: %s", http->status.bchunked? "true" : "false");
}
#endif

