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

// the aicp http type
typedef struct __tb_aicp_http_t
{
	// the option
	tb_http_option_t 	option;

	// the status 
	tb_http_status_t 	status;

	// the stream
	tb_astream_t* 		stream;

	// the sstream for sock
	tb_astream_t* 		sstream;

	// the cstream for chunked
	tb_astream_t* 		cstream;

	// the zstream for gzip/deflate
	tb_astream_t* 		zstream;

	// the pool for string
	tb_handle_t			pool;

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
static tb_bool_t tb_http_option_init(tb_aicp_http_t* http)
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
static tb_void_t tb_http_option_exit(tb_aicp_http_t* http)
{
	// exit head
	if (http->option.head) tb_hash_exit(http->option.head);
	http->option.head = tb_null;

	// exit url
	tb_url_exit(&http->option.url);
}
#ifdef __tb_debug__
static tb_void_t tb_http_option_dump(tb_aicp_http_t* http)
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
static tb_bool_t tb_http_status_init(tb_aicp_http_t* http)
{
	// init status using the default value
	http->status.version = 1;

	// init content type 
	if (!tb_pstring_init(&http->status.content_type)) return tb_false;

	// init location
	if (!tb_pstring_init(&http->status.location)) return tb_false;
	return tb_true;
}
static tb_void_t tb_http_status_exit(tb_aicp_http_t* http)
{
	// exit the content type
	tb_pstring_exit(&http->status.content_type);

	// exit location
	tb_pstring_exit(&http->status.location);
}
static tb_void_t tb_http_status_cler(tb_aicp_http_t* http)
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
static tb_void_t tb_http_status_dump(tb_aicp_http_t* http)
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

static tb_bool_t tb_aicp_http_open_func(tb_astream_t* astream, tb_size_t state, tb_pointer_t priv)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)priv;
	tb_assert_and_check_return_val(http, tb_false);


	// ok
	return tb_true;
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

	// exit zstream, FIXME: will close sock for keep-alive
	if (http->zstream) tb_astream_exit(http->zstream, bcalling);
	http->zstream = tb_null;

	// exit cstream, FIXME: will close sock for keep-alive
	if (http->cstream) tb_astream_exit(http->cstream, bcalling);
	http->cstream = tb_null;

	// switch to sstream
	http->stream = http->sstream;

	// close or clear sstream
	if (http->sstream)
	{
		// not keep-alive?
		if (!http->status.balived) tb_astream_clos(http->sstream, bcalling);
		// TODO, clear stream
//		else tb_astream_cler(http->sstream);
	}

	// clear status
	tb_http_status_cler(http);
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
	tb_http_status_exit(http);

	// exit option
	tb_http_option_exit(http);

	// exit pool
	if (http->pool) tb_spool_exit(http->pool);
	http->pool = tb_null;

	// free it
	tb_free(http);
}
tb_bool_t tb_aicp_http_open_impl(tb_handle_t handle, tb_aicp_http_open_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	// check
	tb_aicp_http_t* http = (tb_aicp_http_t*)handle;
	tb_assert_and_check_return_val(http && func, tb_false);

	return tb_false;
}
tb_bool_t tb_aicp_http_read_impl(tb_handle_t handle, tb_size_t maxn, tb_aicp_http_read_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	return tb_false;
}
tb_bool_t tb_aicp_http_writ_impl(tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicp_http_writ_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	return tb_false;
}
tb_bool_t tb_aicp_http_seek_impl(tb_handle_t handle, tb_hize_t offset, tb_aicp_http_seek_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	return tb_false;
}
tb_bool_t tb_aicp_http_sync_impl(tb_handle_t handle, tb_bool_t bclosing, tb_aicp_http_sync_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	return tb_false;
}
tb_bool_t tb_aicp_http_task_impl(tb_handle_t handle, tb_size_t delay, tb_aicp_http_task_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	return tb_false;
}
tb_bool_t tb_aicp_http_oread_impl(tb_handle_t handle, tb_size_t maxn, tb_aicp_http_read_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	return tb_false;
}
tb_bool_t tb_aicp_http_owrit_impl(tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicp_http_writ_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	return tb_false;
}
tb_bool_t tb_aicp_http_oseek_impl(tb_handle_t handle, tb_hize_t offset, tb_aicp_http_seek_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	return tb_false;
}
tb_bool_t tb_aicp_http_read_after_impl(tb_handle_t handle, tb_size_t delay, tb_size_t maxn, tb_aicp_http_read_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	return tb_false;
}
tb_bool_t tb_aicp_http_writ_after_impl(tb_handle_t handle, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_aicp_http_writ_func_t func, tb_pointer_t priv __tb_debug_decl__)
{
	return tb_false;
}
tb_aicp_t* tb_aicp_http_aicp(tb_handle_t handle)
{
	return tb_null;
}
tb_bool_t tb_aicp_http_option(tb_handle_t handle, tb_size_t option, ...)
{
	return tb_false;
}
