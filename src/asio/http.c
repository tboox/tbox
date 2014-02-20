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
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 				"aicp_http"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "http.h"
#include "aico.h"
#include "aicp.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the aicp http open and read type
typedef struct __tb_aicp_http_oread_t
{
	// the func
	tb_aicp_http_read_func_t 		func;

	// the priv
	tb_pointer_t 					priv;

	// the maxn
	tb_size_t 						maxn;

}tb_aicp_http_oread_t;

// the aicp http open and writ type
typedef struct __tb_aicp_http_owrit_t
{
	// the func
	tb_aicp_http_writ_func_t 		func;

	// the priv
	tb_pointer_t 					priv;

	// the data
	tb_byte_t const* 				data;

	// the size
	tb_size_t 						size;

}tb_aicp_http_owrit_t;

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
	tb_astream_t* 					stream;

	// the sstream for sock
	tb_astream_t* 					sstream;

	// the cstream for chunked
	tb_astream_t* 					cstream;

	// the zstream for gzip/deflate
	tb_astream_t* 					zstream;

	// the pool for string
	tb_handle_t						pool;

	// the data for request and response
	tb_pstring_t 					data;

	// the size for request and response
	tb_size_t 						size;

	// the open and read, writ, seek, ...
	union
	{
		tb_aicp_http_oread_t 		read;
		tb_aicp_http_owrit_t 		writ;
		tb_aicp_http_oseek_t 		seek;

	} 								open_and;

	// the func
	union
	{
		tb_aicp_http_open_func_t 	open;
		tb_aicp_http_read_func_t 	read;
		tb_aicp_http_writ_func_t 	writ;
		tb_aicp_http_seek_func_t 	seek;
		tb_aicp_http_sync_func_t 	sync;
		tb_aicp_http_task_func_t 	task;

	} 								func;

	// the priv
	tb_pointer_t 					priv;

}tb_aicp_http_t;

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
static tb_bool_t tb_aicp_http_option_init(tb_aicp_http_t* http)
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
static tb_void_t tb_aicp_http_option_exit(tb_aicp_http_t* http)
{
	// exit head
	if (http->option.head) tb_hash_exit(http->option.head);
	http->option.head = tb_null;

	// exit url
	tb_url_exit(&http->option.url);
}
#ifdef __tb_debug__
static tb_void_t tb_aicp_http_option_dump(tb_aicp_http_t* http)
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
static tb_bool_t tb_aicp_http_status_init(tb_aicp_http_t* http)
{
	// init status using the default value
	http->status.version = 1;

	// init content type 
	if (!tb_pstring_init(&http->status.content_type)) return tb_false;

	// init location
	if (!tb_pstring_init(&http->status.location)) return tb_false;
	return tb_true;
}
static tb_void_t tb_aicp_http_status_exit(tb_aicp_http_t* http)
{
	// exit the content type
	tb_pstring_exit(&http->status.content_type);

	// exit location
	tb_pstring_exit(&http->status.location);
}
static tb_void_t tb_aicp_http_status_cler(tb_aicp_http_t* http)
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
static tb_void_t tb_aicp_http_status_dump(tb_aicp_http_t* http)
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
static tb_bool_t tb_aicp_http_oread_func(tb_handle_t http, tb_size_t state, tb_http_status_t const* status, tb_pointer_t priv)
{
	// check
	tb_aicp_http_oread_t* oread = (tb_aicp_http_oread_t*)priv;
	tb_assert_and_check_return_val(http && status && oread && oread->func, tb_false);

	// done
	tb_bool_t ok = tb_true;
	do
	{
		// ok? 
		tb_check_break(state == TB_ASTREAM_STATE_OK);

		// reset state
		state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	
		// read it
		if (!tb_aicp_http_read(http, oread->maxn, oread->func, oread->priv)) break;

		// ok
		state = TB_ASTREAM_STATE_OK;

	} while (0);
 
	// failed?
	if (state != TB_ASTREAM_STATE_OK) 
	{
		// done func
		ok = oread->func(http, state, tb_null, 0, oread->maxn, oread->priv);
	}
 
	// ok?
	return ok;
}
static tb_bool_t tb_aicp_http_owrit_func(tb_handle_t http, tb_size_t state, tb_http_status_t const* status, tb_pointer_t priv)
{
	// check
	tb_aicp_http_owrit_t* owrit = (tb_aicp_http_owrit_t*)priv;
	tb_assert_and_check_return_val(http && status && owrit && owrit->func, tb_false);

	// done
	tb_bool_t ok = tb_true;
	do
	{
		// ok? 
		tb_check_break(state == TB_ASTREAM_STATE_OK);

		// reset state
		state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	
		// writ it
		if (!tb_aicp_http_writ(http, owrit->data, owrit->size, owrit->func, owrit->priv)) break;

		// ok
		state = TB_ASTREAM_STATE_OK;

	} while (0);
 
	// failed?
	if (state != TB_ASTREAM_STATE_OK) 
	{
		// done func
		ok = owrit->func(http, state, owrit->data, 0, owrit->size, owrit->priv);
	}
 
	// ok?
	return ok;
}
static tb_bool_t tb_aicp_http_oseek_func(tb_handle_t http, tb_size_t state, tb_http_status_t const* status, tb_pointer_t priv)
{
	// check
	tb_aicp_http_oseek_t* oseek = (tb_aicp_http_oseek_t*)priv;
	tb_assert_and_check_return_val(http && status && oseek && oseek->func, tb_false);

	// done
	tb_bool_t ok = tb_true;
	do
	{
		// ok? 
		tb_check_break(state == TB_ASTREAM_STATE_OK);

		// reset state
		state = TB_ASTREAM_STATE_UNKNOWN_ERROR;
	
		// seek it
		if (!tb_aicp_http_seek(http, oseek->offset, oseek->func, oseek->priv)) break;

		// ok
		state = TB_ASTREAM_STATE_OK;

	} while (0);
 
	// failed?
	if (state != TB_ASTREAM_STATE_OK) 
	{
		// done func
		ok = oseek->func(http, state, 0, oseek->priv);
	}
 
	// ok?
	return ok;
}
static tb_bool_t tb_aicp_http_hread_func(tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.open, tb_false);

	// trace
	tb_trace_impl("head: read: real: %lu, size: %lu, state: %s", real, size, tb_astream_state_cstr(state));

	// done
	tb_bool_t ok = tb_true;
	do
	{
		// ok? 
		tb_check_break(state == TB_ASTREAM_STATE_OK);

		// reset state
		state = TB_ASTREAM_STATE_UNKNOWN_ERROR;


		// ok
		state = TB_ASTREAM_STATE_OK;

	} while (0);
 
	// failed?
	if (state != TB_ASTREAM_STATE_OK) 
	{
		// done func
		ok = http->func.open(http, state, &http->status, http->priv);
	}
 
	// ok?
	return ok;
}
static tb_bool_t tb_aicp_http_hwrit_func(tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.open, tb_false);

	// trace
	tb_trace_impl("head: writ: real: %lu, size: %lu, state: %s", real, size, tb_astream_state_cstr(state));

	// done
	tb_bool_t ok = tb_true;
	do
	{
		// ok? 
		tb_check_break(state == TB_ASTREAM_STATE_OK);

		// reset state
		state = TB_ASTREAM_STATE_UNKNOWN_ERROR;

		// clear data
		tb_pstring_clear(&http->data);

		// TODO: done post
		// finished? read head
		if (real >= size)
		{
			// post read 
			if (!tb_astream_read(http->stream, 0, tb_aicp_http_hread_func, http)) break;
		}

		// ok
		state = TB_ASTREAM_STATE_OK;

	} while (0);
 
	// failed?
	if (state != TB_ASTREAM_STATE_OK) 
	{
		// done func
		ok = http->func.open(http, state, &http->status, http->priv);
	}
 
	// ok?
	return ok;
}
static tb_bool_t tb_aicp_http_read_func(tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.read, tb_false);

	// trace
	tb_trace_impl("read: real: %lu, state: %s", real, tb_astream_state_cstr(state));

	// done func
	return http->func.read(http, state, data, real, size, http->priv);
}
static tb_bool_t tb_aicp_http_writ_func(tb_astream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.writ, tb_false);

	// trace
	tb_trace_impl("writ: real: %lu, state: %s", real, tb_astream_state_cstr(state));

	// done func
	return http->func.writ(http, state, data, real, size, http->priv);
}
static tb_bool_t tb_aicp_http_seek_func(tb_astream_t* astream, tb_size_t state, tb_hize_t offset, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.seek, tb_false);

	// trace
	tb_trace_impl("seek: offset: %llu, state: %s", offset, tb_astream_state_cstr(state));

	// done func
	return http->func.seek(http, state, offset, http->priv);
}
static tb_bool_t tb_aicp_http_sync_func(tb_astream_t* astream, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.sync, tb_false);

	// trace
	tb_trace_impl("sync: state: %s", tb_astream_state_cstr(state));

	// done func
	return http->func.sync(http, state, http->priv);
}
static tb_bool_t tb_aicp_http_task_func(tb_astream_t* astream, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http && http->stream && http->func.task, tb_false);

	// trace
	tb_trace_impl("task: state: %s", tb_astream_state_cstr(state));

	// done func
	return http->func.task(http, state, http->priv);
}

/* ///////////////////////////////////////////////////////////////////////
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
		http->stream = http->sstream = tb_astream_init_sock(aicp);
		tb_assert_and_check_break(http->stream);

		// init pool
		http->pool = tb_spool_init(TB_SPOOL_GROW_MICRO, 0);
		tb_assert_and_check_break(http->pool);

		// init data
		if (!tb_pstring_init(&http->data)) break;

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

	// kill stream
	if (http->stream) tb_astream_kill(http->stream);
}
tb_void_t tb_aicp_http_clos(tb_handle_t handle, tb_bool_t bcalling)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return(http);

	// close stream
	if (http->stream) tb_astream_clos(http->stream, bcalling);
	http->stream = http->sstream;

	// clear status
	tb_aicp_http_status_cler(http);

	// clear data
	tb_pstring_clear(&http->data);
}
tb_void_t tb_aicp_http_exit(tb_handle_t handle, tb_bool_t bcalling)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return(http);

	// close it
	tb_aicp_http_clos(handle, bcalling);

	// exit zstream
	if (http->zstream) tb_astream_exit(http->zstream, bcalling);
	http->zstream = tb_null;

	// exit cstream
	if (http->cstream) tb_astream_exit(http->cstream, bcalling);
	http->cstream = tb_null;

	// exit sstream
	if (http->sstream) tb_astream_exit(http->sstream, bcalling);
	http->sstream = tb_null;

	// exit stream
	http->stream = tb_null;
	
	// exit status
	tb_aicp_http_status_exit(http);

	// exit option
	tb_aicp_http_option_exit(http);

	// exit data
	tb_pstring_exit(&http->data);

	// exit pool
	if (http->pool) tb_spool_exit(http->pool);
	http->pool = tb_null;

	// free it
	tb_free(http);
}
tb_bool_t tb_aicp_http_open(tb_handle_t handle, tb_aicp_http_open_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && func, tb_false);

	// done
	tb_bool_t ok = tb_false;
	do
	{
		// check stream
		tb_assert_and_check_break(http->stream && http->stream == http->sstream);

		// clear status
		tb_aicp_http_status_cler(http);

		// ctrl stream
		if (!tb_astream_ctrl(http->stream, TB_ASTREAM_CTRL_SET_SSL, tb_url_ssl_get(&http->option.url))) break;
		if (!tb_astream_ctrl(http->stream, TB_ASTREAM_CTRL_SET_HOST, tb_url_host_get(&http->option.url))) break;
		if (!tb_astream_ctrl(http->stream, TB_ASTREAM_CTRL_SET_PORT, tb_url_port_get(&http->option.url))) break;
		if (!tb_astream_ctrl(http->stream, TB_ASTREAM_CTRL_SET_PATH, tb_url_path_get(&http->option.url))) break;

		// dump option
#if defined(__tb_debug__) && defined(TB_TRACE_IMPL_TAG)
		tb_aicp_http_option_dump(http);
#endif

		// init the head data
		tb_pstring_clear(&http->data);

		// init the head value
		tb_char_t  		data[64];
		tb_sstring_t 	value;
		if (!tb_sstring_init(&value, data, 64)) break;

		// init method
		tb_assert_and_check_break(http->option.method < tb_arrayn(g_http_methods));
		tb_char_t const* method = g_http_methods[http->option.method];
		tb_assert_and_check_break(method);

		// init path
		tb_char_t const* path = tb_url_path_get(&http->option.url);
		tb_assert_and_check_break(path);

		// init args
		tb_char_t const* args = tb_url_args_get(&http->option.url);

		// init host
		tb_char_t const* host = tb_url_host_get(&http->option.url);
		tb_assert_and_check_break(host);
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
		tb_assert_and_check_break(tb_hash_size(http->option.head));

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

		// the head data and size
		tb_char_t const* 	head_data = tb_pstring_cstr(&http->data);
		tb_size_t 			head_size = tb_pstring_size(&http->data);
		tb_assert_and_check_break(head_data && head_size);
		
		// trace
		tb_trace_impl("request:\n%s", head_data);

		// init open
		http->func.open = func;
		http->priv 		= priv;

		// post writ
		ok = tb_astream_owrit(http->stream, head_data, head_size, tb_aicp_http_hwrit_func, http);

	} while (0);

	// ok?
	return ok;
}
tb_bool_t tb_aicp_http_read(tb_handle_t handle, tb_size_t maxn, tb_aicp_http_read_func_t func, tb_pointer_t priv)
{
	return tb_aicp_http_read_after(handle, 0, maxn, func, priv);
}
tb_bool_t tb_aicp_http_writ(tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicp_http_writ_func_t func, tb_pointer_t priv)
{
	return tb_aicp_http_writ_after(handle, 0, data, size, func, priv);
}
tb_bool_t tb_aicp_http_seek(tb_handle_t handle, tb_hize_t offset, tb_aicp_http_seek_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream && func, tb_false);

	// init seek
	http->func.seek = func;
	http->priv 		= priv;

	// post sync
	return tb_astream_seek(http->stream, offset, tb_aicp_http_seek_func, http);
}
tb_bool_t tb_aicp_http_sync(tb_handle_t handle, tb_bool_t bclosing, tb_aicp_http_sync_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream && func, tb_false);

	// init sync
	http->func.sync = func;
	http->priv 		= priv;

	// post sync
	return tb_astream_sync(http->stream, bclosing, tb_aicp_http_sync_func, http);
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
	return tb_astream_task(http->stream, delay, tb_aicp_http_task_func, http);
}
tb_bool_t tb_aicp_http_oread(tb_handle_t handle, tb_size_t maxn, tb_aicp_http_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && func, tb_false);

	// init open and read
	http->open_and.read.func = func;
	http->open_and.read.priv = priv;
	http->open_and.read.maxn = maxn;
	return tb_aicp_http_open(http, tb_aicp_http_oread_func, &http->open_and.read);
}
tb_bool_t tb_aicp_http_owrit(tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicp_http_writ_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && func, tb_false);

	// init open and writ
	http->open_and.writ.func = func;
	http->open_and.writ.priv = priv;
	http->open_and.writ.data = data;
	http->open_and.writ.size = size;
	return tb_aicp_http_open(http, tb_aicp_http_owrit_func, &http->open_and.writ);
}
tb_bool_t tb_aicp_http_oseek(tb_handle_t handle, tb_hize_t offset, tb_aicp_http_seek_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && func, tb_false);

	// init open and seek
	http->open_and.seek.func = func;
	http->open_and.seek.priv = priv;
	http->open_and.seek.offset = offset;
	return tb_aicp_http_open(http, tb_aicp_http_oseek_func, &http->open_and.seek);
}
tb_bool_t tb_aicp_http_read_after(tb_handle_t handle, tb_size_t delay, tb_size_t maxn, tb_aicp_http_read_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream && func, tb_false);

	// init read
	http->func.read = func;
	http->priv 		= priv;

	// post task
	return tb_astream_read_after(http->stream, delay, maxn, tb_aicp_http_read_func, http);
}
tb_bool_t tb_aicp_http_writ_after(tb_handle_t handle, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_aicp_http_writ_func_t func, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream && data && size && func, tb_false);

	// init writ
	http->func.writ = func;
	http->priv 		= priv;

	// post task
	return tb_astream_writ_after(http->stream, delay, data, size, tb_aicp_http_writ_func, http);
}
tb_aicp_t* tb_aicp_http_aicp(tb_handle_t handle)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && http->stream, tb_null);

	// the aicp 
	return tb_astream_aicp(http->stream);
}
tb_bool_t tb_aicp_http_option(tb_handle_t handle, tb_size_t option, ...)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && http->sstream && option, tb_false);

	// check opened?
	tb_bool_t bopened = tb_false;
	if (!tb_astream_ctrl(http->sstream, TB_ASTREAM_CTRL_IS_OPENED, &bopened)) return tb_false;
	tb_assert_and_check_return_val(!bopened, tb_false);

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
