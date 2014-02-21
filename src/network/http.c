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
#define TB_TRACE_IMPL_TAG 			"http"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "http.h"
#include "../asio/asio.h"
#include "../zip/zip.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../stream/stream.h"
#include "../platform/platform.h"
#include "../container/container.h"

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

	// the sstream for sock
	tb_gstream_t* 		sstream;

	// the cstream for chunked
	tb_gstream_t* 		cstream;

	// the zstream for gzip/deflate
	tb_gstream_t* 		zstream;

	// the pool for string
	tb_handle_t			pool;

	// the step
	tb_size_t 			step;

	// the try count
	tb_size_t 			tryn;

	// the redirect count
	tb_size_t 			rdtn;

	// the data for request and response
	tb_pstring_t 		data;

	// the size for request and response
	tb_size_t 			size;

	/// the real post size
	tb_hize_t 			post;

}tb_http_t;

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_char_t const* g_http_methods[] = 
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
	// init option using the default value
	http->option.method 	= TB_HTTP_METHOD_GET;
	http->option.redirect 	= TB_HTTP_DEFAULT_REDIRECT;
	http->option.timeout 	= TB_HTTP_DEFAULT_TIMEOUT;
	http->option.version 	= 1; // HTTP/1.1
	http->option.post 		= 0;
	http->option.bunzip 	= 0;

	// init url
	if (!tb_url_init(&http->option.url)) return tb_false;

	// init head
	http->option.head = tb_hash_init(8, tb_item_func_str(tb_false, http->pool), tb_item_func_str(tb_false, http->pool));
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
#ifdef __tb_debug__
static tb_void_t tb_http_option_dump(tb_http_t* http)
{
	// check
	tb_assert_and_check_return(http);

	// dump option
	tb_print("[http]: ======================================================================");
	tb_print("[http]: option: ");
	tb_print("[http]: option: url: %s", tb_url_get(&http->option.url));
	tb_print("[http]: option: version: HTTP/1.%1u", http->option.version);
	tb_print("[http]: option: method: %s", http->option.method < tb_arrayn(g_http_methods)? g_http_methods[http->option.method] : "none");
	tb_print("[http]: option: redirect: %d", http->option.redirect);
	tb_print("[http]: option: range: %llu-%llu", http->option.range.bof, http->option.range.eof);
	tb_print("[http]: option: bunzip: %s", http->option.bunzip? "true" : "false");

	// dump head 
	tb_size_t itor = tb_iterator_head(http->option.head);
	tb_size_t tail = tb_iterator_tail(http->option.head);
	for (; itor != tail; itor = tb_iterator_next(http->option.head, itor))
	{
		tb_hash_item_t const* item = tb_iterator_item(http->option.head, itor);
		if (item) tb_print("[http]: option: head: %s: %s", item->name, item->data);
	}

	// dump end
	tb_print("");
}
#endif
static tb_bool_t tb_http_status_init(tb_http_t* http)
{
	// init status using the default value
	http->status.version = 1;

	// init content type 
	if (!tb_pstring_init(&http->status.content_type)) return tb_false;

	// init location
	if (!tb_pstring_init(&http->status.location)) return tb_false;
	return tb_true;
}
static tb_void_t tb_http_status_exit(tb_http_t* http)
{
	// exit the content type
	tb_pstring_exit(&http->status.content_type);

	// exit location
	tb_pstring_exit(&http->status.location);
}
static tb_void_t tb_http_status_cler(tb_http_t* http)
{
	// clear status
	http->status.code = 0;
	http->status.bgzip = 0;
	http->status.bdeflate = 0;
	http->status.bchunked = 0;
	http->status.content_size = 0;
	http->status.state = TB_GSTREAM_STATE_OK;

	// clear content type
	tb_pstring_clear(&http->status.content_type);

	// clear location
	tb_pstring_clear(&http->status.location);

	// persistent state
#if 0
	http->status.version = 1;
	http->status.balived = 0;
	http->status.bseeked = 0;
	http->status.document_size = 0;
#endif
}
#ifdef __tb_debug__
static tb_void_t tb_http_status_dump(tb_http_t* http)
{
	// check
	tb_assert_and_check_return(http);

	// dump status
	tb_print("[http]: ======================================================================");
	tb_print("[http]: status: ");
	tb_print("[http]: status: code: %d", http->status.code);
	tb_print("[http]: status: version: HTTP/1.%1u", http->status.version);
	tb_print("[http]: status: content:type: %s", tb_pstring_cstr(&http->status.content_type));
	tb_print("[http]: status: content:size: %llu", http->status.content_size);
	tb_print("[http]: status: document:size: %llu", http->status.document_size);
	tb_print("[http]: status: location: %s", tb_pstring_cstr(&http->status.location));
	tb_print("[http]: status: bgzip: %s", http->status.bgzip? "true" : "false");
	tb_print("[http]: status: bdeflate: %s", http->status.bdeflate? "true" : "false");
	tb_print("[http]: status: balived: %s", http->status.balived? "true" : "false");
	tb_print("[http]: status: bseeked: %s", http->status.bseeked? "true" : "false");
	tb_print("[http]: status: bchunked: %s", http->status.bchunked? "true" : "false");

	// dump end
	tb_print("");
}
#endif
static tb_long_t tb_http_connect(tb_http_t* http)
{
	// have been connected?
	tb_check_return_val(!(http->step & TB_HTTP_STEP_CONN), 1);

	// ioctl
	if (!http->tryn)
	{
		// clear status
		tb_http_status_cler(http);

		// ctrl stream
		tb_gstream_ctrl(http->stream, TB_GSTREAM_CTRL_SET_SSL, tb_url_ssl_get(&http->option.url));
		tb_gstream_ctrl(http->stream, TB_GSTREAM_CTRL_SET_HOST, tb_url_host_get(&http->option.url));
		tb_gstream_ctrl(http->stream, TB_GSTREAM_CTRL_SET_PORT, tb_url_port_get(&http->option.url));
		tb_gstream_ctrl(http->stream, TB_GSTREAM_CTRL_SET_PATH, tb_url_path_get(&http->option.url));

		// dump option
#if defined(__tb_debug__) && defined(TB_TRACE_IMPL_TAG)
		tb_http_option_dump(http);
#endif
	}

	// tryn++
	http->tryn++;

	// need wait connection
	http->step &= ~TB_HTTP_STEP_NEVT;

	// open stream
	tb_trace_impl("connect: try");
	tb_long_t r = tb_gstream_aopen(http->stream);

	// save state if failed?
	if (r < 0) 
	{
		switch (tb_gstream_state(http->stream))
		{
		case TB_GSTREAM_SOCK_STATE_DNS_FAILED:
			http->status.state = TB_GSTREAM_SOCK_STATE_DNS_FAILED;
			break;
		case TB_GSTREAM_SOCK_STATE_SSL_FAILED:
			http->status.state = TB_GSTREAM_SOCK_STATE_SSL_FAILED;
			break;
		case TB_GSTREAM_SOCK_STATE_CONNECT_FAILED:
		default:
			http->status.state = TB_GSTREAM_SOCK_STATE_CONNECT_FAILED;
			break;
		}
	}

	// failed or continue?
	tb_check_return_val(r > 0, r);

	// ok
	http->step |= TB_HTTP_STEP_CONN;
	http->tryn = 0;
	http->status.state = TB_GSTREAM_STATE_OK;
	tb_trace_impl("connect: ok");
	return r;
}
static tb_long_t tb_http_request(tb_http_t* http)
{
	// check
	tb_check_return_val(!(http->step & TB_HTTP_STEP_REQT), 1);

	// tryn++
	http->tryn++;

	// format it first if the request is null
	if (!tb_pstring_size(&http->data))
	{
		// check size
		tb_assert_and_check_return_val(!http->size, -1);

		// init the head value
		tb_char_t  		data[64];
		tb_sstring_t 	value;
		if (!tb_sstring_init(&value, data, 64)) return -1;

		// init method
		tb_assert_and_check_return_val(http->option.method < tb_arrayn(g_http_methods), -1);
		tb_char_t const* method = g_http_methods[http->option.method];
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
		if (!tb_hash_get(http->option.head, "Connection")) 
			tb_hash_set(http->option.head, "Connection", "close");

		// init range
		if (http->option.range.bof && http->option.range.eof > http->option.range.bof)
			tb_sstring_cstrfcpy(&value, "bytes=%llu-%llu", http->option.range.bof, http->option.range.eof);
		else if (http->option.range.bof && !http->option.range.eof)
			tb_sstring_cstrfcpy(&value, "bytes=%llu-", http->option.range.bof);
		else if (!http->option.range.bof && http->option.range.eof)
			tb_sstring_cstrfcpy(&value, "bytes=0-%llu", http->option.range.eof);

		if (tb_sstring_size(&value)) 
			tb_hash_set(http->option.head, "Range", tb_sstring_cstr(&value));

		// init post
		if (http->option.method == TB_HTTP_METHOD_POST)
		{
			// append post size
			tb_sstring_cstrfcpy(&value, "%llu", http->option.post);
			tb_hash_set(http->option.head, "Content-Length", tb_sstring_cstr(&value));
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

		// exit the head value
		tb_sstring_exit(&value);

		// check request
		tb_assert_and_check_return_val(tb_pstring_size(&http->data) && tb_pstring_cstr(&http->data), -1);

		// trace
		tb_trace_impl("request:\n%s", tb_pstring_cstr(&http->data));
	}

	// the head data and size
	tb_char_t const* 	head_data = tb_pstring_cstr(&http->data);
	tb_size_t 			head_size = tb_pstring_size(&http->data);
	tb_assert_and_check_return_val(head_data && head_size, -1);
	
	// send head
	if (http->size < head_size)
	{
		// need wait if no data
		http->step &= ~TB_HTTP_STEP_NEVT;

		// send head
		while (http->size < head_size)
		{
			// writ data
			tb_long_t real = tb_gstream_awrit(http->stream, head_data + http->size, head_size - http->size);

			// save state if failed
			if (real < 0) http->status.state = TB_GSTREAM_HTTP_STATE_REQUEST_FAILED;

			// check failed
			tb_assert_and_check_return_val(real >= 0, -1);

			// no data? 
			tb_check_return_val(real, 0);

			// update size
			http->size += real;
		}

		// trace
		tb_trace_impl("request: send: head: %lu <? %lu", http->size, head_size);

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
	http->status.state = TB_GSTREAM_HTTP_STATE_RESPONSE_NUL;

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
	// check
	tb_assert_and_check_return_val(http && http->sstream, tb_false);

	// line && size
	tb_char_t const* 	line = tb_pstring_cstr(&http->data);
	tb_size_t 			size = tb_pstring_size(&http->data);
	tb_assert_and_check_return_val(line && size, tb_false);

	// init 
	tb_char_t const* 	p = line;

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

		// save state
		if (http->status.code == 200 || http->status.code == 206)
			http->status.state = TB_GSTREAM_STATE_OK;
		else if (http->status.code == 204)
			http->status.state = TB_GSTREAM_HTTP_STATE_RESPONSE_204;
		else if (http->status.code >= 300 && http->status.code <= 304)
			http->status.state = TB_GSTREAM_HTTP_STATE_RESPONSE_300 + (http->status.code - 300);
		else if (http->status.code >= 400 && http->status.code <= 416)
			http->status.state = TB_GSTREAM_HTTP_STATE_RESPONSE_400 + (http->status.code - 400);
		else if (http->status.code >= 500 && http->status.code <= 507)
			http->status.state = TB_GSTREAM_HTTP_STATE_RESPONSE_500 + (http->status.code - 500);
		else http->status.state = TB_GSTREAM_HTTP_STATE_RESPONSE_UNK;

		// check state code: 4xx & 5xx
		if (http->status.code >= 400 && http->status.code < 600) return tb_false;
	}
	// key: value?
	else
	{
		// seek to value
		while (*p && *p != ':') p++;
		tb_assert_and_check_return_val(*p, tb_false);
		p++; while (*p && tb_isspace(*p)) p++;

		// no value
		tb_check_return_val(*p, tb_true);

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
		// parse content encoding
		else if (!tb_strnicmp(line, "Content-Encoding", 16))
		{
			if (!tb_stricmp(p, "gzip")) http->status.bgzip = 1;
			else if (!tb_stricmp(p, "deflate")) http->status.bdeflate = 1;
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
			// keep alive?
			http->status.balived = !tb_stricmp(p, "close")? 0 : 1;

			// ctrl stream for sock
			if (!tb_gstream_ctrl(http->sstream, TB_GSTREAM_CTRL_SOCK_KEEP_ALIVE, http->status.balived? tb_true : tb_false)) return tb_false;
		}
	}

	// ok
	return tb_true;
}
static tb_long_t tb_http_response(tb_http_t* http)
{
	// check
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
			tb_assert_and_check_return_val(pb && pn, -1);

			if (pb[pn - 1] == '\r')
				tb_pstring_strip(&http->data, pn - 1);

			// trace
			tb_trace_impl("response: %s", pb);
 
			// do callback
			if (http->option.head_func && !http->option.head_func((tb_handle_t)http, pb, http->option.head_priv)) return -1;
			
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

	// clear line
	http->size = 0;

	// clear data
	tb_pstring_clear(&http->data);

	// switch to cstream if chunked
	if (http->status.bchunked)
	{
		// init cstream
		if (http->cstream)
		{
			if (!tb_gstream_ctrl(http->cstream, TB_GSTREAM_CTRL_FLTR_SET_GSTREAM, http->stream)) return -1;
		}
		else http->cstream = tb_gstream_init_filter_from_chunked(http->stream, tb_true);
		tb_assert_and_check_return_val(http->cstream, -1);

		// open cstream, need not async
		if (!tb_gstream_bopen(http->cstream)) return -1;

		// using cstream
		http->stream = http->cstream;

		// disable seek
		http->status.bseeked = 0;
	}

	// switch to zstream if gzip or deflate
	if (http->option.bunzip && (http->status.bgzip || http->status.bdeflate))
	{
		// init zstream
		if (http->zstream)
		{
			if (!tb_gstream_ctrl(http->zstream, TB_GSTREAM_CTRL_FLTR_SET_GSTREAM, http->stream)) return -1;
		}
		else http->zstream = tb_gstream_init_filter_from_zip(http->stream, http->status.bgzip? TB_ZIP_ALGO_GZIP : TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_INFLATE);
		tb_assert_and_check_return_val(http->zstream, -1);

		// open zstream, need not async
		if (!tb_gstream_bopen(http->zstream)) return -1;

		// using zstream
		http->stream = http->zstream;

		// disable seek
		http->status.bseeked = 0;
	}

	// trace
	tb_trace_impl("response: ok");

	// dump status
#if defined(__tb_debug__) && defined(TB_TRACE_IMPL_TAG)
	tb_http_status_dump(http);
#endif

	// ok
	return 1;
}

static tb_long_t tb_http_redirect(tb_http_t* http)
{
	// check
	tb_check_return_val(tb_pstring_size(&http->status.location), 1);

	// redirect
	if (http->rdtn < http->option.redirect)
	{
		// exit zstream
		if (http->zstream)
		{
			tb_gstream_exit(http->zstream);
			http->zstream = tb_null;
		}

		// exit cstream
		if (http->cstream)
		{
			tb_gstream_exit(http->cstream);
			http->cstream = tb_null;
		}

		// switch to sstream
		http->stream = http->sstream;

		// not keep-alive?
		if (!http->status.balived) 
		{
			// close stream
			tb_long_t r = tb_gstream_aclos(http->stream);
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

		// set version
		http->option.version = http->status.version;

		// redirect++
		http->rdtn++;
		
		// reset step, no event now, need not wait
		http->step = ((http->step & TB_HTTP_STEP_SEEK)? TB_HTTP_STEP_SEEK : TB_HTTP_STEP_NONE) | TB_HTTP_STEP_NEVT;
		http->tryn = 0;

		// clear some status
		http->status.code = 0;
		http->status.bgzip = 0;
		http->status.bdeflate = 0;
		http->status.bseeked = 0;
		http->status.bchunked = 0;
		http->status.content_size = 0;
		http->status.document_size = 0;
		http->status.state = TB_GSTREAM_STATE_OK;

		// continue 
		return 0;
	}

	// ok
	return 1;
}
static tb_long_t tb_http_seek(tb_http_t* http, tb_hize_t offset)
{
	// check
	tb_check_return_val(!(http->step & TB_HTTP_STEP_SEEK), 1);
	
	// exit zstream
	if (http->zstream)
	{
		tb_gstream_exit(http->zstream);
		http->zstream = tb_null;
	}

	// exit cstream
	if (http->cstream)
	{
		tb_gstream_exit(http->cstream);
		http->cstream = tb_null;
	}

	// switch to sstream
	http->stream = http->sstream;

	// not keep-alive?
	if (!http->status.balived) 
	{
		// close stream
		tb_long_t r = tb_gstream_aclos(http->stream);
		tb_assert_and_check_return_val(r >= 0, -1);

		// continue ?
		tb_check_return_val(r, 0);
	}

	// set version
	http->option.version = http->status.version;

	// reset redirect
	http->rdtn = 0;
	
	// reset step, no event now, need not wait
	http->step = TB_HTTP_STEP_SEEK | TB_HTTP_STEP_NEVT;
	http->tryn = 0;

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
	// done
	tb_bool_t 	ok = tb_false;
	tb_http_t* 	http = tb_null;
	do
	{
		// make http
		http = tb_malloc0(sizeof(tb_http_t));
		tb_assert_and_check_break(http);

		// init stream
		http->stream = http->sstream = tb_gstream_init_sock();
		tb_assert_and_check_break(http->stream);

		// init pool
		http->pool = tb_spool_init(TB_SPOOL_GROW_MICRO, 0);
		tb_assert_and_check_break(http->pool);

		// init data
		if (!tb_pstring_init(&http->data)) break;

		// init option
		if (!tb_http_option_init(http)) break;

		// init status
		if (!tb_http_status_init(http)) break;

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		if (http) tb_http_exit(http);
		http = tb_null;
	}

	// ok?
	return http;
}
tb_void_t tb_http_exit(tb_handle_t handle)
{
	// check
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return(http);

	// close it
	tb_http_bclos(handle);

	// exit zstream
	if (http->zstream) tb_gstream_exit(http->zstream);
	http->zstream = tb_null;

	// exit cstream
	if (http->cstream) tb_gstream_exit(http->cstream);
	http->cstream = tb_null;

	// exit sstream
	if (http->sstream) tb_gstream_exit(http->sstream);
	http->sstream = tb_null;

	// exit stream
	http->stream = tb_null;
	
	// exit status
	tb_http_status_exit(http);

	// exit option
	tb_http_option_exit(http);

	// exit data
	tb_pstring_exit(&http->data);

	// exit pool
	if (http->pool) tb_spool_exit(http->pool);
	http->pool = tb_null;

	// free it
	tb_free(http);
}
tb_long_t tb_http_wait(tb_handle_t handle, tb_size_t aioe, tb_long_t timeout)
{
	// check
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, -1);
	
	// wait event
	tb_size_t e = TB_AIOE_CODE_NONE;
	if (!(http->step & TB_HTTP_STEP_NEVT))
	{
		if (!(http->step & TB_HTTP_STEP_CONN)) e = TB_AIOE_CODE_CONN;
		else if (!(http->step & TB_HTTP_STEP_REQT)) e = TB_AIOE_CODE_SEND;
		else if (!(http->step & TB_HTTP_STEP_RESP)) e = TB_AIOE_CODE_RECV;
		else e = aioe;
	}
		
	// wait
	tb_long_t ok = e? tb_gstream_wait(http->stream, e, timeout) : aioe;

	// save state if failed?
	if (ok < 0) 
	{
		switch (tb_gstream_state(http->stream))
		{
		case TB_GSTREAM_SOCK_STATE_DNS_FAILED:
			http->status.state = TB_GSTREAM_SOCK_STATE_DNS_FAILED;
			break;
		case TB_GSTREAM_SOCK_STATE_CONNECT_FAILED:
			http->status.state = TB_GSTREAM_SOCK_STATE_CONNECT_FAILED;
			break;
		default:
			{
				if (!(http->step & TB_HTTP_STEP_NEVT))
				{
					if (!(http->step & TB_HTTP_STEP_CONN)) http->status.state = TB_GSTREAM_SOCK_STATE_CONNECT_FAILED;
					else if (!(http->step & TB_HTTP_STEP_REQT)) http->status.state = TB_GSTREAM_HTTP_STATE_REQUEST_FAILED;
					else if (!(http->step & TB_HTTP_STEP_RESP)) http->status.state = TB_GSTREAM_HTTP_STATE_RESPONSE_NUL;
				}
			}
			break;
		}

		// unknown wait failed
		if (!http->status.state) http->status.state = TB_GSTREAM_STATE_WAIT_FAILED;
	}

	// ok?
	return ok;
}
tb_long_t tb_http_aopen(tb_handle_t handle)
{
	// check
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

	// try opening it
	tb_long_t r = 0;
	while (!(r = tb_http_aopen(handle)))
	{
		// wait
		r = tb_http_wait(handle, TB_AIOE_CODE_EALL, http->option.timeout);

		// fail or timeout?
		tb_check_break(r > 0);
	}

	// close it if fail
	if (r <= 0) tb_http_bclos(handle);

	// ok?
	return r > 0? tb_true : tb_false;
}
tb_long_t tb_http_aclos(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, -1);

	// have step? clear it
	if (http->step)
	{	
		// close stream
		tb_long_t r = tb_gstream_aclos(http->stream);
		tb_assert_and_check_return_val(r >= 0, -1);

		// continue ?
		tb_check_return_val(r, 0);

		// switch to sstream
		http->stream = http->sstream;

		// clear status
		tb_http_status_cler(http);

		// clear data
		http->size = 0;
		tb_pstring_clear(&http->data);

		// clear step
		http->step = TB_HTTP_STEP_NONE;
		http->tryn = 0;

		// clear redirect
		http->rdtn = 0;

		// clear post
		http->post = 0;
	}

	// ok
	return 1;
}
tb_bool_t tb_http_bclos(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(handle, tb_false);

	// try opening it
	tb_long_t 	r = 0;
	tb_hong_t 	t = tb_mclock();
	while (!(r = tb_http_aclos(handle)))
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
	// check
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, -1);

	// be able to seek?
	tb_check_return_val(http->status.bseeked, -1);

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
	// check
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(handle, tb_false);

	// be able to seek?
	tb_check_return_val(http->status.bseeked, tb_false);

	// init step
	http->step &= ~TB_HTTP_STEP_SEEK;

	// try seeking it
	tb_long_t r = 0;
	while (!(r = tb_http_aseek(handle, offset)))
	{
		// wait
		r = tb_http_wait(handle, TB_AIOE_CODE_EALL, http->option.timeout);

		// fail or timeout?
		tb_check_break(r > 0);
	}

	// clear step
	http->step &= ~TB_HTTP_STEP_SEEK;

	// ok?
	return r > 0? tb_true : tb_false;
}
tb_long_t tb_http_awrit(tb_handle_t handle, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream && (http->step & TB_HTTP_STEP_POST), -1);
	tb_assert_and_check_return_val(http->post < http->option.post, -1);

	// writ the post data
	tb_long_t ok = tb_gstream_awrit(http->stream, data, size);
	tb_check_return_val(ok >= 0, -1);

	// save the post size
	if (ok > 0) http->post += ok;

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
		tb_assert_and_check_return_val(http->post >= http->option.post, -1);

		// response
		tb_long_t ok = tb_http_response(http);
		tb_check_return_val(ok > 0, ok);

		// redirect
		ok = tb_http_redirect(http);
		tb_assert_and_check_return_val(ok != 0, -1); // FIXME: post redirect is not supported
		tb_check_return_val(ok > 0, ok);

		// reset status
		http->post = 0;
		http->step &= ~TB_HTTP_STEP_POST;
	}

	// read
	return tb_gstream_aread(http->stream, data, size);
}
tb_bool_t tb_http_bwrit(tb_handle_t handle, tb_byte_t const* data, tb_size_t size)
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
			tb_long_t e = tb_http_wait(handle, TB_AIOE_CODE_SEND, http->option.timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOE_CODE_SEND);
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
		tb_long_t real = tb_http_aread(handle, data + read, size - read);

		// update size
		if (real > 0) read += real;
		// no data?
		else if (!real)
		{
			// wait
			tb_long_t e = tb_http_wait(handle, TB_AIOE_CODE_RECV, http->option.timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has read?
			tb_assert_and_check_break(e & TB_AIOE_CODE_RECV);
		}
		else break;
	}

	// ok?
	return read == size? tb_true : tb_false;
}
tb_long_t tb_http_afwrit(tb_handle_t handle, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream && (http->step & TB_HTTP_STEP_POST), -1);

	// has post data? flush it
	tb_long_t ok = 0;
	if (data && size)
	{
		// check
		tb_assert_and_check_return_val(http->post < http->option.post, -1);

		// flush writing the post data
		ok = tb_gstream_afwrit(http->stream, data, size);
		tb_check_return_val(ok >= 0, -1);

		// save the post size
		if (ok > 0) http->post += ok;
	}
	else 
	{
		// check
		tb_assert_and_check_return_val(http->post == http->option.post, -1);

		// flush stream
		ok = tb_gstream_afwrit(http->stream, tb_null, 0);
	}

	// ok?
	return ok;
}
tb_bool_t tb_http_bfwrit(tb_handle_t handle, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, tb_false);

	// has data
	if (data && size)
	{
		// writ data to cache
		tb_long_t writ = 0;
		while (writ < size)
		{
			// writ data
			tb_long_t real = tb_gstream_afwrit(http->stream, data + writ, size - writ);	

			// has data?
			if (real > 0) writ += real;
			// no data?
			else if (!real)
			{
				// wait
				tb_long_t e = tb_gstream_wait(http->stream, TB_AIOE_CODE_SEND, http->option.timeout);
				tb_assert_and_check_break(e >= 0);

				// timeout?
				tb_check_break(e);

				// has writ?
				tb_assert_and_check_break(e & TB_AIOE_CODE_SEND);
			}
			else break;
		}

		// ok?
		return (writ == size? tb_true : tb_false);
	}
	// only flush the cache data
	else
	{
		while (!tb_gstream_afwrit(http->stream, tb_null, 0))
		{
			// wait
			tb_long_t e = tb_gstream_wait(http->stream, TB_AIOE_CODE_SEND, http->option.timeout);
			tb_assert_and_check_break(e >= 0);

			// timeout?
			tb_check_break(e);

			// has writ?
			tb_assert_and_check_break(e & TB_AIOE_CODE_SEND);
		}
	}

	// ok
	return tb_true;
}
tb_bool_t tb_http_option(tb_handle_t handle, tb_size_t option, ...)
{
	// check
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http && option, tb_false);

	// check connected?
	tb_assert_and_check_return_val(!(http->step & TB_HTTP_STEP_CONN), tb_false);

	// init args
	tb_va_list_t args;
    tb_va_start(args, option);

	// done
	switch (option)
	{
	case TB_HTTP_OPTION_SET_URL:
		{
			// url
			tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(url, tb_false);
			
			// set url
			if (tb_url_set(&http->option.url, url)) return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_URL:
		{
			// purl
			tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(purl, tb_false);

			// get url
			tb_char_t const* url = tb_url_get(&http->option.url);
			tb_assert_and_check_return_val(url, tb_false);

			// ok
			*purl = url;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_HOST:
		{
			// host
			tb_char_t const* host = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(host, tb_false);

			// set host
			tb_url_host_set(&http->option.url, host);
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_HOST:
		{
			// phost
			tb_char_t const** phost = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(phost, tb_false); 

			// get host
			tb_char_t const* host = tb_url_host_get(&http->option.url);
			tb_assert_and_check_return_val(host, tb_false);

			// ok
			*phost = host;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_PORT:
		{
			// port
			tb_size_t port = (tb_size_t)tb_va_arg(args, tb_size_t);
			tb_assert_and_check_return_val(port, tb_false);

			// set port
			tb_url_port_set(&http->option.url, port);
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_PORT:
		{
			// pport
			tb_size_t* pport = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pport, tb_false);

			// get port
			*pport = tb_url_port_get(&http->option.url);
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_PATH:
		{
			// path
			tb_char_t const* path = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(path, tb_false);
 
			// set path
			tb_url_path_set(&http->option.url, path);
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_PATH:
		{
			// ppath
			tb_char_t const** ppath = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(ppath, tb_false);

			// get path
			tb_char_t const* path = tb_url_path_get(&http->option.url);
			tb_assert_and_check_return_val(path, tb_false);

			// ok
			*ppath = path;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_METHOD:
		{
			// method
			tb_size_t method = (tb_size_t)tb_va_arg(args, tb_size_t);

			// set method
			http->option.method = method;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_METHOD:
		{
			// pmethod
			tb_size_t* pmethod = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pmethod, tb_false);

			// get method
			*pmethod = http->option.method;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_HEAD:
		{
			// check
			tb_assert_and_check_return_val(http->option.head, tb_false);

			// key
			tb_char_t const* key = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(key, tb_false);

 			// val
			tb_char_t const* val = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(val, tb_false);
 
			// set head
			tb_hash_set(http->option.head, key, val);
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_HEAD:
		{
			// check
			tb_assert_and_check_return_val(http->option.head, tb_false);

			// key
			tb_char_t const* key = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(key, tb_false);

			// pval
			tb_char_t const** pval = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(pval, tb_false);

			// get val
			tb_char_t const* val = tb_hash_get(http->option.head, key);
			tb_assert_and_check_return_val(val, tb_false);

			// ok
			*pval = val;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_HEAD_FUNC:
		{
			// head_func
			tb_http_head_func_t head_func = (tb_http_head_func_t)tb_va_arg(args, tb_http_head_func_t);

			// set head_func
			http->option.head_func = head_func;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_HEAD_FUNC:
		{
			// phead_func
			tb_http_head_func_t* phead_func = (tb_http_head_func_t*)tb_va_arg(args, tb_http_head_func_t*);
			tb_assert_and_check_return_val(phead_func, tb_false);

			// get head_func
			*phead_func = http->option.head_func;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_HEAD_PRIV:
		{
			// head_priv
			tb_pointer_t head_priv = (tb_pointer_t)tb_va_arg(args, tb_pointer_t);

			// set head_priv
			http->option.head_priv = head_priv;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_HEAD_PRIV:
		{
			// phead_priv
			tb_pointer_t* phead_priv = (tb_pointer_t*)tb_va_arg(args, tb_pointer_t*);
			tb_assert_and_check_return_val(phead_priv, tb_false);

			// get head_priv
			*phead_priv = http->option.head_priv;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_RANGE:
		{
			http->option.range.bof = (tb_hize_t)tb_va_arg(args, tb_hize_t);
			http->option.range.eof = (tb_hize_t)tb_va_arg(args, tb_hize_t);
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_RANGE:
		{
			// pbof
			tb_hize_t* pbof = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
			tb_assert_and_check_return_val(pbof, tb_false);

			// peof
			tb_hize_t* peof = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
			tb_assert_and_check_return_val(peof, tb_false);

			// ok
			*pbof = http->option.range.bof;
			*peof = http->option.range.eof;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_SSL:
		{
			// bssl
			tb_bool_t bssl = (tb_bool_t)tb_va_arg(args, tb_bool_t);

			// set ssl
			tb_url_ssl_set(&http->option.url, bssl);
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_SSL:
		{
			// pssl
			tb_bool_t* pssl = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
			tb_assert_and_check_return_val(pssl, tb_false);

			// get ssl
			*pssl = tb_url_ssl_get(&http->option.url);
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_TIMEOUT:
		{
			// set timeout
			http->option.timeout = (tb_size_t)tb_va_arg(args, tb_size_t);
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_TIMEOUT:
		{
			// ptimeout
			tb_size_t* ptimeout = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(ptimeout, tb_false);

			// get timeout
			*ptimeout = http->option.timeout;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_POST_SIZE:
		{
			// post
			tb_hize_t post = (tb_hize_t)tb_va_arg(args, tb_hize_t);

			// set post
			http->option.post = post;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_POST_SIZE:
		{
			// ppost
			tb_hize_t* ppost = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
			tb_assert_and_check_return_val(ppost, tb_false);

			// get post
			*ppost = http->option.post;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_AUTO_UNZIP:
		{
			// bunzip
			tb_bool_t bunzip = (tb_bool_t)tb_va_arg(args, tb_bool_t);

			// set bunzip
			http->option.bunzip = bunzip? 1 : 0;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_AUTO_UNZIP:
		{
			// pbunzip
			tb_bool_t* pbunzip = (tb_bool_t*)tb_va_arg(args, tb_bool_t*);
			tb_assert_and_check_return_val(pbunzip, tb_false);

			// get bunzip
			*pbunzip = http->option.bunzip? tb_true : tb_false;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_REDIRECT:
		{
			// redirect
			tb_size_t redirect = (tb_size_t)tb_va_arg(args, tb_size_t);

			// set redirect
			http->option.redirect = redirect;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_REDIRECT:
		{
			// predirect
			tb_size_t* predirect = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(predirect, tb_false);

			// get redirect
			*predirect = http->option.redirect;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_VERSION:
		{
			// version
			tb_size_t version = (tb_size_t)tb_va_arg(args, tb_size_t);

			// set version
			http->option.version = version;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_VERSION:
		{
			// pversion
			tb_size_t* pversion = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pversion, tb_false);

			// get version
			*pversion = http->option.version;
			return tb_true;
		}
		break;
	default:
		break;
	}
	return tb_false;
}
tb_http_status_t const* tb_http_status(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, tb_null);
	return &http->status;
}

