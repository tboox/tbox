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
 * @file		http.c
 * @ingroup 	asio
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME 				"aicp_http"
#define TB_TRACE_MODULE_DEBUG 				(1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "http.h"
#include "aico.h"
#include "aicp.h"
#include "../zip/zip.h"
#include "../stream/stream.h"
#include "../network/network.h"
#include "../platform/platform.h"
#include "../algorithm/algorithm.h"
#include "../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the aicp http open and read type
typedef struct __tb_aicp_http_oread_t
{
	// the func
	tb_aicp_http_read_func_t 		func;

	// the priv
	tb_pointer_t 					priv;

	// the size
	tb_size_t 						size;

}tb_aicp_http_oread_t;

// the aicp http open and seek type
typedef struct __tb_aicp_http_oseek_t
{
	// the func
	tb_aicp_http_seek_func_t 		func;

	// the priv
	tb_pointer_t 					priv;

	// the offset
	tb_hize_t 						offset;

}tb_aicp_http_oseek_t;

// the aicp http type
typedef struct __tb_aicp_http_t
{
	// the option
	tb_http_option_t 				option;

	// the status 
	tb_http_status_t 				status;

	// the stream
	tb_async_stream_t* 				stream;

	// the sstream for sock
	tb_async_stream_t* 				sstream;

	// the cstream for chunked
	tb_async_stream_t* 				cstream;

	// the zstream for gzip/deflate
	tb_async_stream_t* 				zstream;

	// the transfer for post
	tb_handle_t 					transfer;

	// the pool for string
	tb_handle_t						pool;

	// the post file
	tb_handle_t 					post_file;

	// the line data
	tb_scoped_string_t 				line_data;

	// the line size
	tb_size_t 						line_size;

	// the cache data
	tb_scoped_buffer_t 				cache_data;

	// the cache read
	tb_size_t 						cache_read;

	// the redirect read
	tb_hize_t 						redirect_read;

	// the redirect tryn
	tb_size_t 						redirect_tryn;

	// the open and read, writ, seek, ...
	union
	{
		tb_aicp_http_oread_t 		read;
		tb_aicp_http_oseek_t 		seek;

	} 								open_and;

	// the func
	union
	{
		tb_aicp_http_open_func_t 	open;
		tb_aicp_http_read_func_t 	read;
		tb_aicp_http_seek_func_t 	seek;
		tb_aicp_http_task_func_t 	task;

	} 								func;

	// the priv
	tb_pointer_t 					priv;

}tb_aicp_http_t;

/* //////////////////////////////////////////////////////////////////////////////////////
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

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
static tb_bool_t tb_aicp_http_open_done(tb_aicp_http_t* http);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_http_option_init(tb_aicp_http_t* http)
{
	// init option using the default value
	http->option.method 	= TB_HTTP_METHOD_GET;
	http->option.redirect 	= TB_HTTP_DEFAULT_REDIRECT;
	http->option.timeout 	= TB_HTTP_DEFAULT_TIMEOUT;
	http->option.version 	= 1; // HTTP/1.1
	http->option.bunzip 	= 0;

	// init url
	if (!tb_url_init(&http->option.url)) return tb_false;

	// init post url
	if (!tb_url_init(&http->option.post_url)) return tb_false;

	// init head
	http->option.head = tb_hash_init(8, tb_item_func_str(tb_false, http->pool), tb_item_func_str(tb_false, http->pool));
	tb_assert_and_check_return_val(http->option.head, tb_false);

	// ok
	return tb_true;
}
static tb_void_t tb_aicp_http_option_exit(tb_aicp_http_t* http)
{
	// exit head
	if (http->option.head) tb_hash_exit(http->option.head);
	http->option.head = tb_null;

	// exit url
	tb_url_exit(&http->option.url);

	// exit post url
	tb_url_exit(&http->option.post_url);
}
#ifdef __tb_debug__
static tb_void_t tb_aicp_http_option_dump(tb_aicp_http_t* http)
{
	// check
	tb_assert_and_check_return(http);

	// dump option
	tb_trace_i("======================================================================");
	tb_trace_i("option: ");
	tb_trace_i("option: url: %s", tb_url_get(&http->option.url));
	tb_trace_i("option: version: HTTP/1.%1u", http->option.version);
	tb_trace_i("option: method: %s", http->option.method < tb_arrayn(g_http_methods)? g_http_methods[http->option.method] : "none");
	tb_trace_i("option: redirect: %d", http->option.redirect);
	tb_trace_i("option: range: %llu-%llu", http->option.range.bof, http->option.range.eof);
	tb_trace_i("option: bunzip: %s", http->option.bunzip? "true" : "false");

	// dump head 
	tb_for_all (tb_hash_item_t*, item, http->option.head)
	{
		if (item) tb_trace_i("option: head: %s: %s", item->name, item->data);
	}

	// dump end
	tb_trace_i("");
}
#endif
static tb_bool_t tb_aicp_http_status_init(tb_aicp_http_t* http)
{
	// init status using the default value
	http->status.version = 1;

	// init content type 
	if (!tb_scoped_string_init(&http->status.content_type)) return tb_false;

	// init location
	if (!tb_scoped_string_init(&http->status.location)) return tb_false;
	return tb_true;
}
static tb_void_t tb_aicp_http_status_exit(tb_aicp_http_t* http)
{
	// exit the content type
	tb_scoped_string_exit(&http->status.content_type);

	// exit location
	tb_scoped_string_exit(&http->status.location);
}
static tb_void_t tb_aicp_http_status_cler(tb_aicp_http_t* http, tb_bool_t host_changed)
{
	// clear status
	http->status.code = 0;
	http->status.bgzip = 0;
	http->status.bdeflate = 0;
	http->status.bchunked = 0;
	http->status.content_size = 0;
	http->status.document_size = 0;
	http->status.state = TB_STATE_OK;

	// clear content type
	tb_scoped_string_clear(&http->status.content_type);

	// clear location
	tb_scoped_string_clear(&http->status.location);

	// host is changed? clear the alived state
	if (host_changed)
	{
		http->status.version = 1;
		http->status.balived = 0;
		http->status.bseeked = 0;
	}
}
#ifdef __tb_debug__
static tb_void_t tb_aicp_http_status_dump(tb_aicp_http_t* http)
{
	// check
	tb_assert_and_check_return(http);

	// dump status
	tb_trace_i("======================================================================");
	tb_trace_i("status: ");
	tb_trace_i("status: code: %d", 				http->status.code);
	tb_trace_i("status: version: HTTP/1.%1u", 	http->status.version);
	tb_trace_i("status: content:type: %s", 		tb_scoped_string_cstr(&http->status.content_type));
	tb_trace_i("status: content:size: %llu", 	http->status.content_size);
	tb_trace_i("status: document:size: %llu", 	http->status.document_size);
	tb_trace_i("status: location: %s", 			tb_scoped_string_cstr(&http->status.location));
	tb_trace_i("status: bgzip: %s", 			http->status.bgzip? "true" : "false");
	tb_trace_i("status: bdeflate: %s", 			http->status.bdeflate? "true" : "false");
	tb_trace_i("status: balived: %s", 			http->status.balived? "true" : "false");
	tb_trace_i("status: bseeked: %s", 			http->status.bseeked? "true" : "false");
	tb_trace_i("status: bchunked: %s", 			http->status.bchunked? "true" : "false");

	// dump end
	tb_trace_i("");
}
#endif
static tb_char_t const* tb_aicp_http_head_format(tb_aicp_http_t* http, tb_hize_t post_size, tb_size_t* head_size, tb_size_t* state)
{
	// check
	tb_assert_and_check_return_val(http && head_size, tb_null);

	// clear line data
	tb_scoped_string_clear(&http->line_data);

	// init the head value
	tb_char_t  		data[64];
	tb_static_string_t 	value;
	if (!tb_static_string_init(&value, data, 64)) return tb_null;

	// init method
	tb_assert_and_check_return_val(http->option.method < tb_arrayn(g_http_methods), tb_null);
	tb_char_t const* method = g_http_methods[http->option.method];
	tb_assert_and_check_return_val(method, tb_null);

	// init path
	tb_char_t const* path = tb_url_path_get(&http->option.url);
	tb_assert_and_check_return_val(path, tb_null);

	// init args
	tb_char_t const* args = tb_url_args_get(&http->option.url);

	// init host
	tb_char_t const* host = tb_url_host_get(&http->option.url);
	tb_assert_and_check_return_val(host, tb_null);
	tb_hash_set(http->option.head, "Host", host);

	// init accept
	if (!tb_hash_get(http->option.head, "Accept")) 
		tb_hash_set(http->option.head, "Accept", "*/*");

	// init connection
	if (!tb_hash_get(http->option.head, "Connection")) 
		tb_hash_set(http->option.head, "Connection", "close");
	else if (http->status.balived) tb_hash_set(http->option.head, "Connection", "keep-alive");

	// init range
	if (http->option.range.bof && http->option.range.eof >= http->option.range.bof)
		tb_static_string_cstrfcpy(&value, "bytes=%llu-%llu", http->option.range.bof, http->option.range.eof);
	else if (http->option.range.bof && !http->option.range.eof)
		tb_static_string_cstrfcpy(&value, "bytes=%llu-", http->option.range.bof);
	else if (!http->option.range.bof && http->option.range.eof)
		tb_static_string_cstrfcpy(&value, "bytes=0-%llu", http->option.range.eof);
	else if (http->option.range.bof > http->option.range.eof)
	{
		// save state
		if (state) *state = TB_STATE_HTTP_RANGE_INVALID;
		return tb_null;
	}

	if (tb_static_string_size(&value)) 
		tb_hash_set(http->option.head, "Range", tb_static_string_cstr(&value));

	// init post
	if (http->option.method == TB_HTTP_METHOD_POST)
	{
		// append post size
		tb_static_string_cstrfcpy(&value, "%llu", post_size);
		tb_hash_set(http->option.head, "Content-Length", tb_static_string_cstr(&value));
	}

	// check head
	tb_assert_and_check_return_val(tb_hash_size(http->option.head), tb_null);

	// append method
	tb_scoped_string_cstrcat(&http->line_data, method);

	// append ' '
	tb_scoped_string_chrcat(&http->line_data, ' ');

	// append path
	tb_scoped_string_cstrcat(&http->line_data, path);

	// append args if exists
	if (args) 
	{
		tb_scoped_string_chrcat(&http->line_data, '?');
		tb_scoped_string_cstrcat(&http->line_data, args);
	}

	// append ' '
	tb_scoped_string_chrcat(&http->line_data, ' ');

	// append version, HTTP/1.1
	tb_scoped_string_cstrfcat(&http->line_data, "HTTP/1.%1u\r\n", http->option.version);

	// append key: value
	tb_for_all (tb_hash_item_t*, item, http->option.head)
	{
		if (item && item->name && item->data) 
			tb_scoped_string_cstrfcat(&http->line_data, "%s: %s\r\n", (tb_char_t const*)item->name, (tb_char_t const*)item->data);
	}

	// append end
	tb_scoped_string_cstrcat(&http->line_data, "\r\n");

	// exit the head value
	tb_static_string_exit(&value);

	// save the head size
	*head_size = tb_scoped_string_size(&http->line_data);
	
	// ok
	return tb_scoped_string_cstr(&http->line_data);
}
static tb_bool_t tb_aicp_http_open_read_func(tb_handle_t http, tb_size_t state, tb_http_status_t const* status, tb_pointer_t priv)
{
	// check
	tb_aicp_http_oread_t* oread = (tb_aicp_http_oread_t*)priv;
	tb_assert_and_check_return_val(http && status && oread && oread->func, tb_false);

	// done
	tb_bool_t ok = tb_true;
	do
	{
		// ok? 
		tb_check_break(state == TB_STATE_OK);

		// reset state
		state = TB_STATE_UNKNOWN_ERROR;
	
		// read it
		if (!tb_aicp_http_read(http, oread->size, oread->func, oread->priv)) break;

		// ok
		state = TB_STATE_OK;

	} while (0);
 
	// failed?
	if (state != TB_STATE_OK) 
	{
		// done func
		ok = oread->func(http, state, tb_null, 0, oread->size, oread->priv);
	}
 
	// ok?
	return ok;
}
static tb_bool_t tb_aicp_http_open_seek_func(tb_handle_t http, tb_size_t state, tb_http_status_t const* status, tb_pointer_t priv)
{
	// check
	tb_aicp_http_oseek_t* oseek = (tb_aicp_http_oseek_t*)priv;
	tb_assert_and_check_return_val(http && status && oseek && oseek->func, tb_false);

	// done
	tb_bool_t ok = tb_true;
	do
	{
		// ok? 
		tb_check_break(state == TB_STATE_OK);

		// reset state
		state = TB_STATE_UNKNOWN_ERROR;
	
		// seek it
		if (!tb_aicp_http_seek(http, oseek->offset, oseek->func, oseek->priv)) break;

		// ok
		state = TB_STATE_OK;

	} while (0);
 
	// failed?
	if (state != TB_STATE_OK) 
	{
		// done func
		ok = oseek->func(http, state, 0, oseek->priv);
	}
 
	// ok?
	return ok;
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
static tb_bool_t tb_aicp_http_head_resp_done(tb_aicp_http_t* http)
{
	// check
	tb_assert_and_check_return_val(http && http->sstream, tb_false);

	// line && size
	tb_char_t const* 	line = tb_scoped_string_cstr(&http->line_data);
	tb_size_t 			size = tb_scoped_string_size(&http->line_data);
	tb_assert_and_check_return_val(line && size, tb_false);

	// init 
	tb_char_t const* 	p = line;

	// the first line? 
	if (!http->line_size)
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
			http->status.state = TB_STATE_OK;
		else if (http->status.code == 204)
			http->status.state = TB_STATE_HTTP_RESPONSE_204;
		else if (http->status.code >= 300 && http->status.code <= 304)
			http->status.state = TB_STATE_HTTP_RESPONSE_300 + (http->status.code - 300);
		else if (http->status.code >= 400 && http->status.code <= 416)
			http->status.state = TB_STATE_HTTP_RESPONSE_400 + (http->status.code - 400);
		else if (http->status.code >= 500 && http->status.code <= 507)
			http->status.state = TB_STATE_HTTP_RESPONSE_500 + (http->status.code - 500);
		else http->status.state = TB_STATE_HTTP_RESPONSE_UNK;

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
			tb_scoped_string_cstrcpy(&http->status.content_type, p);
			tb_assert_and_check_return_val(tb_scoped_string_size(&http->status.content_type), tb_false);
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

			// save location
			tb_scoped_string_cstrcpy(&http->status.location, p);
		}
		// parse connection
		else if (!tb_strnicmp(line, "Connection", 10))
		{
			// keep alive?
			http->status.balived = !tb_stricmp(p, "close")? 0 : 1;

			// ctrl stream for sock
			if (!tb_stream_ctrl(http->sstream, TB_STREAM_CTRL_SOCK_KEEP_ALIVE, http->status.balived? tb_true : tb_false)) return tb_false;
		}
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_http_head_redt_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.open, tb_false);

	// trace
	tb_trace_d("head: redt: real: %lu, size: %lu, state: %s", real, size, tb_state_cstr(state));

	// done
	do
	{
		// ok? 
		if (state == TB_STATE_OK)
		{
			// save read
			http->redirect_read += real;

			// continue?
			if (http->redirect_read < http->status.content_size) return tb_true;
		}

		// ok? 
		tb_check_break(state == TB_STATE_OK || state == TB_STATE_CLOSED);

		// redirect failed
		state = TB_STATE_HTTP_REDIRECT_FAILED;

		// close stream
		if (http->stream) tb_async_stream_clos(http->stream, tb_true);
		http->stream = http->sstream;

		// exit transfer
		if (http->transfer) tb_transfer_exit(http->transfer, tb_true);
		http->transfer = tb_null;

		// done location url
		tb_char_t const* location = tb_scoped_string_cstr(&http->status.location);
		tb_assert_and_check_break(location);

		// trace
		tb_trace_d("redirect: %s", location);

		// only path?
		if (location[0] == '/') tb_url_path_set(&http->option.url, location);
		// full url?
		else
		{
			// set url
			if (!tb_url_set(&http->option.url, location)) break;
		}

		// done open
		if (!tb_aicp_http_open_done(http)) break;

		// ok
		return tb_false;

	} while (0);

	// sync state
	http->status.state = state;

	// done func
	http->func.open(http, state, &http->status, http->priv);

	// break
	return tb_false;
}
static tb_bool_t tb_aicp_http_head_read_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.open, tb_false);

	// trace
	tb_trace_d("head: read: real: %lu, size: %lu, state: %s", real, size, tb_state_cstr(state));

	// done
	do
	{
		// ok? 
		tb_check_break(state == TB_STATE_OK);

		// reset state
		state = TB_STATE_UNKNOWN_ERROR;

		// walk	
		tb_long_t 			ok = 0;
		tb_char_t 			ch = '\0';
		tb_char_t const* 	p = (tb_char_t const*)data;
		tb_char_t const* 	e = p + real;
		while (p < e)
		{
			// the char
			ch = *p++;

			// error end?
			if (!ch)
			{
				ok = -1;
				tb_assert(0);
				break;
			}

			// append char to line
			if (ch != '\n') tb_scoped_string_chrcat(&http->line_data, ch);
			// is line end?
			else
			{
				// strip '\r' if exists
				tb_char_t const* 	pb = tb_scoped_string_cstr(&http->line_data);
				tb_size_t 			pn = tb_scoped_string_size(&http->line_data);
				if (!pb || !pn)
				{
					ok = -1;
					tb_assert(0);
					break;
				}

				if (pb[pn - 1] == '\r')
					tb_scoped_string_strip(&http->line_data, pn - 1);

				// trace
				tb_trace_d("response: %s", pb);
	 
				// do callback
				if (http->option.head_func && !http->option.head_func((tb_handle_t)http, pb, http->option.head_priv)) 
				{
					ok = -1;
					tb_assert(0);
					break;
				}
				
				// end?
				if (!tb_scoped_string_size(&http->line_data)) 
				{
					// ok
					ok = 1;
					break;
				}

				// done the head response
				if (!tb_aicp_http_head_resp_done(http)) 
				{	
					// save the error state
					if (http->status.state != TB_STATE_OK) state = http->status.state;

					// error
					ok = -1;
					break;
				}

				// clear line data
				tb_scoped_string_clear(&http->line_data);

				// line++
				http->line_size++;
			}
		}

		// continue ?
		if (!ok) return tb_true;
		// end?
		else if (ok > 0) 
		{
			// trace
			tb_trace_d("head: read: end, left: %lu", e - p);
 
			// trace
			tb_trace_d("response: ok");

			// redirect?
			if (tb_scoped_string_size(&http->status.location) && http->redirect_tryn++ < http->option.redirect)
			{
				// save the redirect read
				http->redirect_read = e - p;

				// read the left data
				if (http->redirect_read < http->status.content_size)
				{
					if (!tb_async_stream_read(http->stream, 0, tb_aicp_http_head_redt_func, http)) break;
				}
				// no left data, redirect it directly
				else tb_aicp_http_head_redt_func(http->stream, TB_STATE_OK, tb_null, 0, 0, http);
				return tb_false;
			}

			// switch to cstream if chunked
			if (http->status.bchunked)
			{
				// init cstream
				if (http->cstream)
				{
					if (!tb_stream_ctrl(http->cstream, TB_STREAM_CTRL_FLTR_SET_STREAM, http->stream)) break;
				}
				else http->cstream = tb_async_stream_init_filter_from_chunked(http->stream, tb_true);
				tb_assert_and_check_break(http->cstream);

				// push the left data to filter
				if (p < e)
				{
					// the filter
					tb_stream_filter_t* filter = tb_null;
					if (!tb_stream_ctrl(http->cstream, TB_STREAM_CTRL_FLTR_GET_FILTER, &filter)) break;
					tb_assert_and_check_break(filter);

					// clear filter
					tb_stream_filter_cler(filter);

					// push data
					if (!tb_stream_filter_push(filter, (tb_byte_t const*)p, e - p)) break;
					p = e;
				}

				// try to open cstream directly, because the stream have been opened 
				if (!tb_async_stream_open_try(http->cstream)) break;

				// using cstream
				http->stream = http->cstream;

				// disable seek
				http->status.bseeked = 0;
			}

			// switch to zstream if gzip or deflate
			if (http->option.bunzip && (http->status.bgzip || http->status.bdeflate))
			{
#ifdef TB_CONFIG_THIRD_HAVE_ZLIB
				// init zstream
				if (http->zstream)
				{
					if (!tb_stream_ctrl(http->zstream, TB_STREAM_CTRL_FLTR_SET_STREAM, http->stream)) break;
				}
				else http->zstream = tb_async_stream_init_filter_from_zip(http->stream, http->status.bgzip? TB_ZIP_ALGO_GZIP : TB_ZIP_ALGO_ZLIB, TB_ZIP_ACTION_INFLATE);
				tb_assert_and_check_break(http->zstream);

				// the filter
				tb_stream_filter_t* filter = tb_null;
				if (!tb_stream_ctrl(http->zstream, TB_STREAM_CTRL_FLTR_GET_FILTER, &filter)) break;
				tb_assert_and_check_break(filter);

				// clear filter
				tb_stream_filter_cler(filter);

				// limit the filter input size
				if (http->status.content_size) tb_stream_filter_limit(filter, http->status.content_size);

				// push the left data to filter
				if (p < e)
				{
					// push data
					if (!tb_stream_filter_push(filter, (tb_byte_t const*)p, e - p)) break;
					p = e;
				}

				// try to open zstream directly, because the stream have been opened 
				if (!tb_async_stream_open_try(http->zstream)) break;

				// using zstream
				http->stream = http->zstream;

				// disable seek
				http->status.bseeked = 0;
#else
				// trace
				tb_trace_w("gzip is not supported now! please enable it from config if you need it.");

				// not supported
				state = TB_STATE_HTTP_GZIP_NOT_SUPPORTED;
				break;
#endif
			}

			// cache the left data
			if (p < e) tb_scoped_buffer_memncat(&http->cache_data, (tb_byte_t const*)p, e - p);
			p = e;

			// ok
			state = TB_STATE_OK;

			// dump status
#if defined(__tb_debug__) && TB_TRACE_MODULE_DEBUG
			tb_aicp_http_status_dump(http);
#endif
		}
		// error?
		else 
		{
			// trace
			tb_trace_d("head: read: error, state: %s", tb_state_cstr(state));
		}

	} while (0);

	// sync state
	http->status.state = state;

	// done func
	http->func.open(http, state, &http->status, http->priv);

	// break 
	return tb_false;
}
static tb_bool_t tb_aicp_http_head_post_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.open, tb_false);

	// trace
	tb_trace_d("head: post: percent: %llu%%, size: %lu, state: %s", size > 0? (offset * 100 / size) : 0, save, tb_state_cstr(state));

	// done
	tb_bool_t bpost = tb_false;
	do
	{
		// done func
		if (http->option.post_func && !http->option.post_func(http, state, offset, size, save, rate, http->option.post_priv)) 
		{
			state = TB_STATE_UNKNOWN_ERROR;
			break;
		}
			
		// ok? continue to post
		if (state == TB_STATE_OK) bpost = tb_true;
		// closed? read head
		else if (state == TB_STATE_CLOSED)
		{
			// reset state
			state = TB_STATE_UNKNOWN_ERROR;

			// clear line size
			http->line_size = 0;

			// clear line data
			tb_scoped_string_clear(&http->line_data);

			// clear cache data
			tb_scoped_buffer_clear(&http->cache_data);
			http->cache_read = 0;

			// post read 
			if (!tb_async_stream_read(http->stream, 0, tb_aicp_http_head_read_func, http)) break;
		}
		// failed?
		else break;

		// ok
		state = TB_STATE_OK;

	} while (0);

	// failed?
	if (state != TB_STATE_OK)
	{
		// done func
		http->status.state = state;
		http->func.open(http, state, &http->status, http->priv);
	}
 
	// ok?
	return bpost;
}
static tb_bool_t tb_aicp_http_head_writ_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.open, tb_false);

	// trace
	tb_trace_d("head: writ: real: %lu, size: %lu, state: %s", real, size, tb_state_cstr(state));

	// done
	tb_bool_t bwrit = tb_false;
	do
	{
		// ok? 
		tb_check_break(state == TB_STATE_OK);

		// reset state
		state = TB_STATE_UNKNOWN_ERROR;

		// not finished? continue it
		if (real < size)
		{
			// continue to writ
			bwrit = tb_true;
		}
		// finished? post data
		else if (http->option.method == TB_HTTP_METHOD_POST)
		{
			// check
			tb_assert_and_check_break(http->transfer);
 
			// post data
			if (!tb_transfer_save(http->transfer, tb_aicp_http_head_post_func, http)) break;
		}
		// finished? read data
		else
		{
			// clear line size
			http->line_size = 0;

			// clear line data
			tb_scoped_string_clear(&http->line_data);

			// clear cache data
			tb_scoped_buffer_clear(&http->cache_data);
			http->cache_read = 0;

			// post read 
			if (!tb_async_stream_read(http->stream, 0, tb_aicp_http_head_read_func, http)) break;
		}

		// ok
		state = TB_STATE_OK;

	} while (0);
 
	// failed?
	if (state != TB_STATE_OK) 
	{
		// done func
		http->status.state = state;
		http->func.open(http, state, &http->status, http->priv);
	}
 
	// ok?
	return bwrit;
}
static tb_bool_t tb_aicp_http_post_open_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.open, tb_false);

	// trace
	tb_trace_d("post: open: offset: %lu, size: %lu, state: %s", offset, size, tb_state_cstr(state));

	// done
	tb_bool_t ok = tb_true;
	do
	{
		// ok? 
		tb_check_break(state == TB_STATE_OK);

		// reset state
		state = TB_STATE_UNKNOWN_ERROR;

		// no post size?
		if (size < 0)
		{
			state = TB_STATE_HTTP_POST_FAILED;
			break;
		}

		// the head data and size
		tb_size_t 			head_size = 0;
		tb_char_t const* 	head_data = tb_aicp_http_head_format(http, size, &head_size, &state);
		tb_check_break(head_data && head_size);
		
		// trace
		tb_trace_d("request[%lu]:\n%s", head_size, head_data);

		// post writ head
		if (!tb_async_stream_writ(http->stream, (tb_byte_t const*)head_data, head_size, tb_aicp_http_head_writ_func, http)) break;

		// ok
		state = TB_STATE_OK;

	} while (0);
 
	// failed?
	if (state != TB_STATE_OK) 
	{
		// done func
		http->status.state = state;
		ok = http->func.open(http, state, &http->status, http->priv);
	}
 
	// ok?
	return ok;
}
static tb_bool_t tb_aicp_http_sock_open_func(tb_async_stream_t* astream, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.open, tb_false);

	// trace
	tb_trace_d("sock: open: state: %s", tb_state_cstr(state));

	// done
	tb_bool_t ok = tb_true;
	do
	{
		// ok? 
		tb_check_break(state == TB_STATE_OK);

		// reset state
		state = TB_STATE_UNKNOWN_ERROR;

		// get?
		if (http->option.method == TB_HTTP_METHOD_GET)
		{
			// the head data and size
			tb_size_t 			head_size = 0;
			tb_char_t const* 	head_data = tb_aicp_http_head_format(http, 0, &head_size, tb_null);
			tb_check_break(head_data && head_size);
			
			// trace
			tb_trace_d("request:\n%s", head_data);

			// post writ head
			ok = tb_async_stream_owrit(http->stream, (tb_byte_t const*)head_data, head_size, tb_aicp_http_head_writ_func, http);
		}
		// post?
		else if (http->option.method == TB_HTTP_METHOD_POST)
		{
			// check
			tb_assert_and_check_break(!http->transfer && !http->post_file);

			// init transfer
			tb_char_t const* 	url = tb_url_get(&http->option.post_url);
			if (http->option.post_data && http->option.post_size)
				http->transfer = tb_transfer_init_da(http->option.post_data, http->option.post_size, http->stream, 0);
			else if (url) http->transfer = tb_transfer_init_ua(url, http->stream, 0);
			tb_assert_and_check_break(http->transfer);

			// limit rate
			if (http->option.post_lrate) tb_transfer_limitrate(http->transfer, http->option.post_lrate);

			// open transfer
			ok = tb_transfer_open(http->transfer, tb_aicp_http_post_open_func, http);
		}
		else tb_assert_and_check_break(0);

		// ok
		state = TB_STATE_OK;

	} while (0);
 
	// failed?
	if (state != TB_STATE_OK) 
	{
		// done func
		http->status.state = state;
		ok = http->func.open(http, state, &http->status, http->priv);
	}
 
	// ok?
	return ok;
}
static tb_bool_t tb_aicp_http_read_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.read, tb_false);

	// trace
	tb_trace_d("read: real: %lu, state: %s", real, tb_state_cstr(state));

	// done func
	return http->func.read(http, state, data, real, size, http->priv);
}
static tb_bool_t tb_aicp_http_task_func(tb_async_stream_t* astream, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.task, tb_false);

	// trace
	tb_trace_d("task: state: %s", tb_state_cstr(state));

	// done func
	return http->func.task(http, state, http->priv);
}
static tb_bool_t tb_aicp_http_open_done(tb_aicp_http_t* http)
{
	// check
	tb_assert_and_check_return_val(http && http->func.open, tb_false);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// check stream
		tb_assert_and_check_break(http->stream && http->stream == http->sstream);

		// the host is changed?
		tb_bool_t 			host_changed = tb_true;
		tb_char_t const* 	host_old = tb_null;
		tb_char_t const* 	host_new = tb_url_host_get(&http->option.url);
		tb_stream_ctrl(http->stream, TB_STREAM_CTRL_GET_HOST, &host_old);
		if (host_old && host_new && !tb_stricmp(host_old, host_new)) host_changed = tb_false;

		// trace
		tb_trace_d("connect: host: %s", host_changed? "changed" : "keep");

		// ctrl stream
		if (!tb_stream_ctrl(http->stream, TB_STREAM_CTRL_SET_URL, tb_url_get(&http->option.url))) break;
		if (!tb_stream_ctrl(http->stream, TB_STREAM_CTRL_SET_TIMEOUT, http->option.timeout)) break;

		// dump option
#if defined(__tb_debug__) && TB_TRACE_MODULE_DEBUG
		tb_aicp_http_option_dump(http);
#endif

		// clear status
		tb_aicp_http_status_cler(http, host_changed);

		// open the stream
		ok = tb_async_stream_open(http->stream, tb_aicp_http_sock_open_func, http);

	} while (0);

	// ok?
	return ok;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_aicp_http_init(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp, tb_null);

	// done
	tb_bool_t 		ok = tb_false;
	tb_aicp_http_t* http = tb_null;
	do
	{
		// make http
		http = tb_malloc0(sizeof(tb_aicp_http_t));
		tb_assert_and_check_break(http);

		// init stream
		http->stream = http->sstream = tb_async_stream_init_sock(aicp);
		tb_assert_and_check_break(http->stream);

		// init pool
		http->pool = tb_block_pool_init(TB_BLOCK_POOL_GROW_MICRO, 0);
		tb_assert_and_check_break(http->pool);

		// init line data
		if (!tb_scoped_string_init(&http->line_data)) break;

		// init cache data
		if (!tb_scoped_buffer_init(&http->cache_data)) break;
		http->cache_read = 0;

		// init option
		if (!tb_aicp_http_option_init(http)) break;

		// init status
		if (!tb_aicp_http_status_init(http)) break;

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		if (http) tb_aicp_http_exit(http, tb_false);
		http = tb_null;
	}

	// ok?
	return http;
}
tb_void_t tb_aicp_http_kill(tb_handle_t handle)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return(http);

	// trace
	tb_trace_d("kill: ..");

	// kill stream
	if (http->stream) tb_stream_kill(http->stream);

	// kill transfer
	if (http->transfer) tb_transfer_kill(http->transfer);
}
tb_void_t tb_aicp_http_clos(tb_handle_t handle, tb_bool_t bcalling)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return(http);

	// trace
	tb_trace_d("clos: ..");

	// close stream
	if (http->stream) tb_async_stream_clos(http->stream, bcalling);
	http->stream = http->sstream;

	// exit transfer
	if (http->transfer) tb_transfer_exit(http->transfer, bcalling);
	http->transfer = tb_null;

	// exit the post file
	if (http->post_file) tb_file_exit(http->post_file);
	http->post_file = tb_null;

	// trace
	tb_trace_d("clos: ok");
}
tb_void_t tb_aicp_http_exit(tb_handle_t handle, tb_bool_t bcalling)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return(http);

	// trace
	tb_trace_d("exit: ..");

	// close it
	tb_aicp_http_clos(handle, bcalling);

	// exit zstream
	if (http->zstream) tb_async_stream_exit(http->zstream, bcalling);
	http->zstream = tb_null;

	// exit cstream
	if (http->cstream) tb_async_stream_exit(http->cstream, bcalling);
	http->cstream = tb_null;

	// exit sstream
	if (http->sstream) tb_async_stream_exit(http->sstream, bcalling);
	http->sstream = tb_null;

	// exit stream
	http->stream = tb_null;
	
	// exit transfer
	if (http->transfer) tb_transfer_exit(http->transfer, bcalling);
	http->transfer = tb_null;

	// exit the post file
	if (http->post_file) tb_file_exit(http->post_file);
	http->post_file = tb_null;

	// exit status
	tb_aicp_http_status_exit(http);

	// exit option
	tb_aicp_http_option_exit(http);

	// exit line data
	tb_scoped_string_exit(&http->line_data);

	// exit cache data
	tb_scoped_buffer_exit(&http->cache_data);

	// exit pool
	if (http->pool) tb_block_pool_exit(http->pool);
	http->pool = tb_null;

	// free it
	tb_free(http);

	// trace
	tb_trace_d("exit: ok");
}
tb_bool_t tb_aicp_http_open(tb_handle_t handle, tb_aicp_http_open_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && func, tb_false);

	// init open
	http->func.open = func;
	http->priv 		= priv;

	// clear redirect
	http->redirect_tryn = 0;

	// done open
	return tb_aicp_http_open_done(http);
}
tb_bool_t tb_aicp_http_read(tb_handle_t handle, tb_size_t size, tb_aicp_http_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream && func, tb_false);

	// post read
	return tb_aicp_http_read_after(handle, 0, size, func, priv);
}
tb_bool_t tb_aicp_http_read_after(tb_handle_t handle, tb_size_t delay, tb_size_t size, tb_aicp_http_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream && func, tb_false);

	// read the cache data first, note: must be reentrant
	tb_byte_t const* 	cache_data = tb_scoped_buffer_data(&http->cache_data);
	tb_size_t 			cache_size = tb_scoped_buffer_size(&http->cache_data);
	if (cache_data && cache_size && http->cache_read < cache_size)
	{
		// read cache
		http->cache_read = cache_size;

		// done func
		tb_bool_t ok = func(http, TB_STATE_OK, cache_data, cache_size, cache_size, priv);

		// clear cache data
		tb_scoped_buffer_clear(&http->cache_data);
		http->cache_read = 0;

		// break?
		tb_check_return_val(ok, tb_true);
	}

	// init read
	http->func.read = func;
	http->priv 		= priv;

	// post read
	return tb_async_stream_read_after(http->stream, delay, size, tb_aicp_http_read_func, http);
}
tb_bool_t tb_aicp_http_seek(tb_handle_t handle, tb_hize_t offset, tb_aicp_http_seek_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream && func, tb_false);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// close stream
		if (http->stream) tb_async_stream_clos(http->stream, tb_true);
		http->stream = http->sstream;

		// exit transfer
		if (http->transfer) tb_transfer_exit(http->transfer, tb_true);
		http->transfer = tb_null;

		// trace
		tb_trace_d("seek: %llu", offset);

		// init open
		http->func.open = tb_aicp_http_open_seek_func;
		http->priv 		= &http->open_and.seek;

		// init open and seek
		http->open_and.seek.func = func;
		http->open_and.seek.priv = priv;
		http->open_and.seek.offset = offset;

		// clear redirect
		http->redirect_tryn = 0;

		// set range
		http->option.range.bof = offset;
		http->option.range.eof = http->status.document_size? http->status.document_size - 1 : 0;

		// done open
		if (!tb_aicp_http_open_done(http)) break;

		// ok
		ok = tb_true;

	} while (0);

	// ok?
	return ok;
}
tb_bool_t tb_aicp_http_task(tb_handle_t handle, tb_size_t delay, tb_aicp_http_task_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream && func, tb_false);

	// init task
	http->func.task = func;
	http->priv 		= priv;

	// post task
	return tb_async_stream_task(http->stream, delay, tb_aicp_http_task_func, http);
}
tb_bool_t tb_aicp_http_oread(tb_handle_t handle, tb_size_t size, tb_aicp_http_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && func, tb_false);

	// init open and read
	http->open_and.read.func = func;
	http->open_and.read.priv = priv;
	http->open_and.read.size = size;
	return tb_aicp_http_open(http, tb_aicp_http_open_read_func, &http->open_and.read);
}
tb_bool_t tb_aicp_http_oseek(tb_handle_t handle, tb_hize_t offset, tb_aicp_http_seek_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && func, tb_false);

	// open and seek
	return tb_aicp_http_seek(http, offset, func, priv);
}
tb_aicp_t* tb_aicp_http_aicp(tb_handle_t handle)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, tb_null);

	// the aicp 
	return tb_async_stream_aicp(http->stream);
}
tb_bool_t tb_aicp_http_option(tb_handle_t handle, tb_size_t option, ...)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && http->sstream && option, tb_false);

	// init args
	tb_va_list_t args;
    tb_va_start(args, option);

	// done
	switch (option)
	{
	case TB_HTTP_OPTION_SET_URL:
		{
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

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
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

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
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

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
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

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
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

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
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

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
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

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
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

			// head priv
			tb_pointer_t head_priv = (tb_pointer_t)tb_va_arg(args, tb_pointer_t);

			// set head priv
			http->option.head_priv = head_priv;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_HEAD_PRIV:
		{
			// phead priv
			tb_pointer_t* phead_priv = (tb_pointer_t*)tb_va_arg(args, tb_pointer_t*);
			tb_assert_and_check_return_val(phead_priv, tb_false);

			// get head priv
			*phead_priv = http->option.head_priv;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_RANGE:
		{
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

			// set range
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
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

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
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

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
	case TB_HTTP_OPTION_SET_POST_URL:
		{
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

			// url
			tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
			tb_assert_and_check_return_val(url, tb_false);

			// clear post data and size
			http->option.post_data = tb_null;
			http->option.post_size = 0;
			
			// set url
			if (tb_url_set(&http->option.post_url, url)) return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_POST_URL:
		{
			// purl
			tb_char_t const** purl = (tb_char_t const**)tb_va_arg(args, tb_char_t const**);
			tb_assert_and_check_return_val(purl, tb_false);

			// get url
			tb_char_t const* url = tb_url_get(&http->option.post_url);
			tb_assert_and_check_return_val(url, tb_false);

			// ok
			*purl = url;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_POST_DATA:
		{
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

			// post data
			tb_byte_t const* 	data = (tb_byte_t const*)tb_va_arg(args, tb_byte_t const*);

			// post size
			tb_size_t 			size = (tb_size_t)tb_va_arg(args, tb_size_t);

			// clear post url
			tb_url_clear(&http->option.post_url);
			
			// set post data
			http->option.post_data = data;
			http->option.post_size = size;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_POST_DATA:
		{
			// pdata and psize
			tb_byte_t const** 	pdata = (tb_byte_t const**)tb_va_arg(args, tb_byte_t const**);
			tb_size_t* 			psize = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(pdata && psize, tb_false);

			// get post data and size
			*pdata = http->option.post_data;
			*psize = http->option.post_size;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_POST_FUNC:
		{
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

			// func
			tb_http_post_func_t func = (tb_http_post_func_t)tb_va_arg(args, tb_http_post_func_t);

			// set post func
			http->option.post_func = func;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_POST_FUNC:
		{
			// pfunc
			tb_http_post_func_t* pfunc = (tb_http_post_func_t*)tb_va_arg(args, tb_http_post_func_t*);
			tb_assert_and_check_return_val(pfunc, tb_false);

			// get post func
			*pfunc = http->option.post_func;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_POST_PRIV:
		{
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

			// post priv
			tb_pointer_t priv = (tb_pointer_t)tb_va_arg(args, tb_pointer_t);

			// set post priv
			http->option.post_priv = priv;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_POST_PRIV:
		{
			// ppost priv
			tb_pointer_t* ppriv = (tb_pointer_t*)tb_va_arg(args, tb_pointer_t*);
			tb_assert_and_check_return_val(ppriv, tb_false);

			// get post priv
			*ppriv = http->option.post_priv;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_POST_LRATE:
		{
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

			// post lrate
			tb_size_t lrate = (tb_size_t)tb_va_arg(args, tb_size_t);

			// set post lrate
			http->option.post_lrate = lrate;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_GET_POST_LRATE:
		{
			// ppost lrate
			tb_size_t* plrate = (tb_size_t*)tb_va_arg(args, tb_size_t*);
			tb_assert_and_check_return_val(plrate, tb_false);
 
			// get post lrate
			*plrate = http->option.post_lrate;
			return tb_true;
		}
		break;
	case TB_HTTP_OPTION_SET_AUTO_UNZIP:
		{
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

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
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

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
			// check opened?
			tb_assert_and_check_return_val(!tb_stream_is_opened(http->sstream), tb_false);

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
