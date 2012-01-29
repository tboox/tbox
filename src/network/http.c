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
 * \file		http.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "http.h"
#include "../aio/aio.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../stream/stream.h"
#include "../platform/platform.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// the default timeout
#define TB_HTTP_TIMEOUT_DEFAULT 				(10000)

// the default max redirect
#define TB_HTTP_MRDT_DEFAULT 					(10)

/* ////////////////////////////////////////////////////////////////////////
 * types
 */

// the http state type
typedef enum __tb_http_state_t
{
	TB_HTTP_STATE_NULL 			= 0
,	TB_HTTP_STATE_CONNECTED 	= 1
,	TB_HTTP_STATE_REQUESTED 	= 2
,	TB_HTTP_STATE_RESPONSED 	= 4
,	TB_HTTP_STATE_REDIRECTED 	= 8

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
	tb_spool_t* 		spool;

	// the state
	tb_size_t 			state;
	
	// the data && size for request and response
	tb_pstring_t 		data;
	tb_size_t 			size;

}tb_http_t;

/* ////////////////////////////////////////////////////////////////////////
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

/* ////////////////////////////////////////////////////////////////////////
 * implemention
 */
static tb_bool_t tb_http_option_init(tb_http_t* http)
{
	// init default
	http->option.method 	= TB_HTTP_METHOD_GET;
	http->option.mrdt 		= TB_HTTP_MRDT_DEFAULT;
	http->option.timeout 	= TB_HTTP_TIMEOUT_DEFAULT;
	http->option.version 	= TB_HTTP_VERSION_11;

	// init url
	if (!tb_url_init(&http->option.url)) return TB_FALSE;

	// init post
	if (!tb_pstring_init(&http->option.post)) return TB_FALSE;

	// init head
	http->option.head = tb_hash_init(8, tb_item_func_str(TB_FALSE, http->spool), tb_item_func_str(TB_FALSE, http->spool));
	tb_assert_and_check_return_val(http->option.head, TB_FALSE);

	// ok
	return TB_TRUE;
}
static tb_void_t tb_http_option_exit(tb_http_t* http)
{
	// exit head
	if (http->option.head) tb_hash_exit(http->option.head);
	http->option.head = TB_NULL;

	// exit post
	tb_pstring_exit(&http->option.post);

	// exit url
	tb_url_exit(&http->option.url);
}
static tb_bool_t tb_http_status_init(tb_http_t* http)
{
	return tb_pstring_init(&http->status.content_type);
}
static tb_void_t tb_http_status_exit(tb_http_t* http)
{
	tb_pstring_exit(&http->status.content_type);
}
static tb_long_t tb_http_connect(tb_http_t* http)
{
	tb_check_return_val(!(http->state & TB_HTTP_STATE_CONNECTED), 1);

	// ioctl
	tb_gstream_ctrl1(http->stream, TB_GSTREAM_CMD_SET_SSL, tb_url_ssl_get(&http->option.url));
	tb_gstream_ctrl1(http->stream, TB_GSTREAM_CMD_SET_HOST, tb_url_host_get(&http->option.url));
	tb_gstream_ctrl1(http->stream, TB_GSTREAM_CMD_SET_PORT, tb_url_port_get(&http->option.url));
	tb_gstream_ctrl1(http->stream, TB_GSTREAM_CMD_SET_PATH, tb_url_path_get(&http->option.url));

	// open stream
	tb_long_t r = tb_gstream_aopen(http->stream);
	tb_trace_tag("http", "connect: %d", r);
	tb_check_return_val(r > 0, r);

	// ok
	http->state |= TB_HTTP_STATE_CONNECTED;
	return r;
}
static tb_long_t tb_http_request(tb_http_t* http)
{
	tb_check_return_val(!(http->state & TB_HTTP_STATE_REQUESTED), 1);

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
			tb_size_t size = tb_pstring_size(&http->option.post);
			tb_assert_and_check_return_val(size, -1);
			
			tb_sstring_cstrfcpy(&s, "%u", size);
			tb_hash_set(http->option.head, "Content-Length", tb_sstring_cstr(&s));
		}

		// init cookie
		if (http->option.cookies)
		{
			tb_char_t const* cookie = tb_cookies_get(http->option.cookies, host, path, tb_url_ssl_get(&http->option.url));
			if (cookie) tb_hash_set(http->option.head, "Cookie", cookie);
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
		tb_size_t itor = tb_hash_itor_head(http->option.head);
		tb_size_t tail = tb_hash_itor_tail(http->option.head);
		for (; itor != tail; itor = tb_hash_itor_next(http->option.head, itor))
		{
			tb_hash_item_t const* item = tb_hash_itor_const_at(http->option.head, itor);
			if (item && item->name && item->data) 
				tb_pstring_cstrfcat(&http->data, "%s: %s\r\n", (tb_char_t const*)item->name, (tb_char_t const*)item->data);
		}
	
		// append end
		tb_pstring_cstrcat(&http->data, "\r\n");

		// exit string
		tb_sstring_exit(&s);

		// check request
		tb_assert_and_check_return_val(tb_pstring_size(&http->data) && tb_pstring_cstr(&http->data), -1);

		// dump
		tb_trace_tag("http", "request:\n%s", tb_pstring_cstr(&http->data));
	}

	// data && size
	tb_char_t const* 	data = tb_pstring_cstr(&http->data);
	tb_size_t 			size = tb_pstring_size(&http->data);

	// check
	tb_assert_and_check_return_val(data && size && http->size < size, -1);

	// send request
	tb_long_t r = tb_gstream_awrit(http->stream, data + http->size, size - http->size);
	tb_trace_tag("http", "send request: %d", r);
	tb_check_return_val(r > 0, r);

	// update size
	http->size += r;

	// ok?
	if (http->size == size) 
	{
		// finish it
		http->state |= TB_HTTP_STATE_REQUESTED;

		// reset data
		http->size = 0;
		tb_pstring_clear(&http->data);
	}

	return r;
}
static tb_long_t tb_http_response(tb_http_t* http)
{
	tb_check_return_val(!(http->state & TB_HTTP_STATE_RESPONSED), 1);

	// init
	tb_long_t 			r = -1;
	tb_char_t 			ch[1];
	tb_long_t 			cn = 0;
	tb_char_t const* 	pb = tb_pstring_cstr(&http->data);

	// read response
	while (1)
	{
		// read char
		cn = tb_gstream_aread(http->stream, ch, 1);

		// no data? continue
		tb_check_return_val(cn, 0);
		tb_assert_and_check_return_val(cn == 1, -1);

		// append char to line
		if (ch != '\n')  
		{
			// append it
			tb_pstring_chrcat(&http->data, ch);

			// no data? continue?
			if (!ch) r = 0;
		}
		// is line end?
		else
		{
			// finish line
			if (data && size && data[size - 1] == '\r')
				tb_pstring_strip(&http->data, size - 1);

			// process line
			tb_trace_tag("http", "line: %s", data);

			// continue
			r = 0;
		}
	}

	return r;
}
/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_handle_t tb_http_init(tb_http_option_t const* option)
{
	// init http
	tb_http_t* http = tb_calloc(1, sizeof(tb_http_t));
	tb_assert_and_check_return_val(http, TB_NULL);

	// init stream
	http->stream = tb_gstream_init_sock();
	tb_assert_and_check_goto(http->stream, fail);

	// init spool
	http->spool = tb_spool_init(TB_SPOOL_SIZE_MICRO);
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
	return TB_NULL;
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
		http->spool = TB_NULL;

		// exit stream
		if (http->stream) tb_gstream_exit(http->stream);
		http->stream = TB_NULL;

		// free it
		tb_free(http);
	}
}
tb_handle_t tb_http_bare(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, TB_NULL);

	return tb_gstream_bare(http->stream);
}

tb_long_t tb_http_wait(tb_handle_t handle, tb_size_t etype, tb_long_t timeout)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, -1);

	return tb_gstream_wait(http->stream, etype, timeout);
}

#if 0
tb_long_t tb_http_aopen(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(handle, -1);

	// check stream
	tb_assert_and_check_return_val(http->stream, -1);
	
	// dump option
#ifdef TB_DEBUG
	tb_http_option_dump(http);
#endif

	// ioctl
	tb_gstream_ctrl1(http->stream, TB_SSTREAM_CMD_SET_SSL, http->option.bssl? TB_TRUE : TB_FALSE);
	tb_gstream_ctrl1(http->stream, TB_SSTREAM_CMD_SET_HOST, http->option.host);
	tb_gstream_ctrl1(http->stream, TB_SSTREAM_CMD_SET_PORT, http->option.port);

	// open stream
	if (!tb_gstream_aopen(http->stream)) return 0;

	// format http header
	tb_stack_string_t s;
	tb_pstring_init_stack_string(&s);
	tb_char_t const* 	head = tb_http_head_format(http, (tb_pstring_t*)&s);
	tb_size_t 			size = tb_pstring_size((tb_pstring_t*)&s);
	tb_assert_and_check_goto(head, fail);

	//tb_printf(head);
	
	// writ http request
	if (!tb_gstream_bwrit(http->stream, (tb_byte_t*)head, size)) goto fail;

	// writ post data
	if (http->option.method == TB_HTTP_METHOD_POST 
		&& http->option.post_data && http->option.post_size)
	{
		if (!tb_gstream_bwrit(http->stream, http->option.post_data, http->option.post_size))
			goto fail;
	}

	// reset some status
	http->status.bredirect = 0;
	http->status.balive = 0;
	http->status.content_size = 0;
	http->status.document_size = 0;
	http->status.chunked_read = 0;
	http->status.chunked_size = 0;
	http->status.code = 0;
	http->status.bseeked = 0;
	http->status.bchunked = 0;
	http->status.version = TB_HTTP_VERSION_10;
//	http->status.content_type[0] = '\0';
	http->status.line[0] = '\0';

	// handle response
	if (!tb_http_handle_response(http)) goto fail;

	// free it
	tb_pstring_exit((tb_pstring_t*)&s);

#ifdef TB_DEBUG
	tb_http_status_dump(http);
#endif

	// is redirect?
	if (http->status.bredirect)
	{
		// be able to redirect?
		if (http->status.redirect < http->option.redirect)
		{
			http->status.redirect++;

			// continue to open next
			return 0;
		}
	}

	// ok
	return 1;

fail:
	tb_pstring_exit((tb_pstring_t*)&s);
	if (http) tb_http_bclose((tb_handle_t)http);
	return -1;
}
#else
tb_long_t tb_http_aopen(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(handle, -1);

	// check stream
	tb_long_t r = -1;
	tb_assert_and_check_return_val(http->stream, r);

	// connect
	r = tb_http_connect(http);
	tb_check_return_val(r > 0, r);
	
	// request
	r = tb_http_request(http);
	tb_check_return_val(r > 0, r);
		
	// response
	r = tb_http_response(http);
	tb_check_return_val(r > 0, r);
	
	// ok
	return r;
}
#endif
tb_bool_t tb_http_bopen(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(handle, TB_FALSE);

	// dump option
#ifdef TB_DEBUG
	tb_http_option_dump(handle);
#endif

	// try opening it
	tb_long_t r = 0;
	while (!(r = tb_http_aopen(handle)))
	{
		// has aio event?
		tb_size_t e = TB_AIOO_ETYPE_NULL;
		if (!(http->state & TB_HTTP_STATE_CONNECTED)) e |= TB_AIOO_ETYPE_CONN;
		else if (!(http->state & TB_HTTP_STATE_REQUESTED)) e |= TB_AIOO_ETYPE_WRIT;
		else if (!(http->state & TB_HTTP_STATE_RESPONSED)) e |= TB_AIOO_ETYPE_READ;

		// need wait?
		if (e)
		{
			// timeout?
			r = tb_http_wait(handle, e, http->option.timeout);
			tb_assert_and_check_return_val(r >= 0, TB_FALSE);

			// timeout?
			tb_check_break(r);
		}
	}

	// dump status
#ifdef TB_DEBUG
	tb_http_status_dump(handle);
#endif

	// ok?
	return r > 0? TB_TRUE : TB_FALSE;
}
tb_long_t tb_http_aclose(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(handle, -1);

	// FIXME
	// close stream
	if (http->stream && !http->status.balive) tb_gstream_aclose(http->stream);

	// clear status
	tb_memset(&http->status, 0, sizeof(tb_http_status_t));

	// ok
	return 1;
}
tb_bool_t tb_http_bclose(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(handle, TB_FALSE);

	// try opening it
	tb_long_t 	r = 0;
	tb_int64_t 	t = tb_mclock();
	while (!(r = tb_http_aclose(handle)))
	{
		// timeout?
		if (tb_mclock() - t > http->option.timeout) break;

		// sleep some time
		tb_usleep(100);
	}

	// ok?
	return r > 0? TB_TRUE : TB_FALSE;
}
tb_long_t tb_http_awrit(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, -1);

	return tb_gstream_awrit(http->stream, data, size);
}
tb_long_t tb_http_aread(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, -1);

	if (http->status.bchunked)
	{
		tb_trace_noimpl();
		return -1;
	}
	else return tb_gstream_aread(http->stream, data, size);
}
tb_bool_t tb_http_bwrit(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, TB_FALSE);

	tb_long_t 	writ = 0;
	tb_bool_t 	wait = TB_FALSE;
	while (writ < size)
	{
		// writ data
		tb_long_t n = tb_http_awrit(handle, data + writ, size - writ);	
		if (n > 0)
		{
			// update writ
			writ += n;

			// no waiting
			wait = TB_FALSE;
		}
		else if (!n)
		{
			// no end?
			tb_check_break(!wait);

			// wait
			tb_long_t e = tb_http_wait(handle, TB_AIOO_ETYPE_WRIT, http->option.timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOO_ETYPE_WRIT);

			// be waiting
			wait = TB_TRUE;
		}
		else break;
	}

	// ok?
	return writ == size? TB_TRUE : TB_FALSE;
}
tb_bool_t tb_http_bread(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{	
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, TB_FALSE);

	tb_long_t 	read = 0;
	tb_bool_t 	wait = TB_FALSE;
	while (read < size)
	{
		// read data
		tb_long_t n = tb_http_aread(handle, data + read, size - read);	
		if (n > 0)
		{
			// update read
			read += n;

			// no waiting
			wait = TB_FALSE;
		}
		else if (!n)
		{
			// no end?
			tb_check_break(!wait);

			// wait
			tb_long_t e = tb_http_wait(handle, TB_AIOO_ETYPE_READ, http->option.timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOO_ETYPE_READ);

			// be waiting
			wait = TB_TRUE;
		}
		else break;
	}

	// ok?
	return read == size? TB_TRUE : TB_FALSE;
}
tb_http_option_t* tb_http_option(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_NULL);
	return &http->option;
}
tb_http_status_t const* tb_http_status(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_NULL);
	return &http->status;
}

#ifdef TB_DEBUG
tb_void_t tb_http_option_dump(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return(http);

	tb_trace_tag("http", "=============================================");
	tb_trace_tag("http", "option: ");
	tb_trace_tag("http", "option: url: %s", tb_url_get(&http->option.url));
	tb_trace_tag("http", "option: method: %s", http->option.method < tb_arrayn(tb_http_methods)? tb_http_methods[http->option.method] : "none");
	tb_trace_tag("http", "option: mrdt: %d", http->option.mrdt);
	tb_trace_tag("http", "option: range: %llu-%llu", http->option.range.bof, http->option.range.eof);
	tb_trace_tag("http", "option: balive: %s", http->option.balive? "true" : "false");

	if (http->option.cookies)
	{
		tb_cookies_dump(http->option.cookies);

		// get cookie
		tb_char_t const* value = tb_cookies_get_from_url(http->option.cookies, tb_url_get(&http->option.url));

		// format it
		if (value) 
		{
			tb_trace_tag("http", "option: cookie: %s", value);
		}
	}
}
tb_void_t tb_http_status_dump(tb_handle_t handle)
{
	tb_assert_and_check_return(handle);
	tb_http_t* http = (tb_http_t*)handle;

	tb_trace_tag("http", "=============================================");
	tb_trace_tag("http", "status: ");
	tb_trace_tag("http", "status: code: %d", http->status.code);
	tb_trace_tag("http", "status: version: %s", http->status.version == TB_HTTP_VERSION_11? "HTTP/1.1" : "HTTP/1.0");
	tb_trace_tag("http", "status: content:type: %s", tb_pstring_cstr(&http->status.content_type));
	tb_trace_tag("http", "status: content:size: %llu", http->status.content_size);
	tb_trace_tag("http", "status: document:size: %llu", http->status.document_size);
	tb_trace_tag("http", "status: bchunked: %s", http->status.bchunked? "true" : "false");
	tb_trace_tag("http", "status: bseeked: %s", http->status.bseeked? "true" : "false");
	tb_trace_tag("http", "status: balive: %s", http->status.balive? "true" : "false");
}
#endif

