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
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// the default port
#define TB_HTTP_PORT_DEFAULT 					(80)
#define TB_HTTPS_PORT_DEFAULT 					(443)

// the default timeout
#define TB_HTTP_TIMEOUT_DEFAULT 				(10000)

// the max redirect
#define TB_HTTP_REDIRECT_MAX 					(10)

/* ////////////////////////////////////////////////////////////////////////
 * types
 */

// the http type
typedef struct __tb_http_t
{
	// the socket stream
	tb_gstream_t* 		stream;

	// the option
	tb_http_option_t 	option;

	// the status 
	tb_http_status_t 	status;

}tb_http_t;

/* ////////////////////////////////////////////////////////////////////////
 * globals
 */

static tb_http_option_t g_http_option_default = 
{
	// method
	TB_HTTP_METHOD_GET

	// max redirect
, 	TB_HTTP_REDIRECT_MAX

	// port
, 	TB_HTTP_PORT_DEFAULT

	// is ssl?
, 	0

	// is keep alive?
, 	0

	// timeout
,	TB_HTTP_TIMEOUT_DEFAULT

	// range
, 	{0, 0}

	// head func
, 	TB_NULL
, 	TB_NULL

	// post data
,	TB_NULL
, 	0

	// cookies
, 	TB_NULL

	// head
, 	{0}

	// url
, 	{0}
, 	{0}
, 	{0}

};
 
/* ////////////////////////////////////////////////////////////////////////
 * details
 */


/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_handle_t tb_http_init(tb_http_option_t const* option)
{
	// alloc
	tb_http_t* http = tb_calloc(1, sizeof(tb_http_t));
	tb_assert_and_check_return_val(http, TB_NULL);

	// init
	http->option = option? *option : g_http_option_default;

	// init stream
	http->stream = tb_gstream_init_sock();
	tb_assert_and_check_goto(http->stream, fail);

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

		// exit stream
		if (http->stream) tb_gstream_exit(http->stream);

		// free it
		tb_free(http);
	}
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
	tb_gstream_ioctl1(http->stream, TB_SSTREAM_CMD_SET_SSL, http->option.bssl? TB_TRUE : TB_FALSE);
	tb_gstream_ioctl1(http->stream, TB_SSTREAM_CMD_SET_HOST, http->option.host);
	tb_gstream_ioctl1(http->stream, TB_SSTREAM_CMD_SET_PORT, http->option.port);

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
	http->status.bkalive = 0;
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
	tb_assert_and_check_return_val(http->stream, -1);
	
	// dump option
#ifdef TB_DEBUG
	tb_http_option_dump(http);
#endif

	// ok
	return 1;
}
#endif
tb_bool_t tb_http_bopen(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(handle, TB_FALSE);

	// try opening it
	tb_long_t 	r = 0;
	tb_int64_t 	t = tb_mclock();
	while (!(r = tb_http_aopen(handle)))
	{
		// timeout?
		if (tb_mclock() - t > http->option.timeout) break;

		// sleep some time
		tb_usleep(100);
	}

	// ok?
	return r > 0? TB_TRUE : TB_FALSE;
}
tb_long_t tb_http_aclose(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(handle, -1);

	// FIXME
	// close stream
	if (http->stream && !http->status.bkalive) tb_gstream_aclose(http->stream);

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
	tb_int64_t 	time = tb_mclock();
	while (writ < size)
	{
		// writ data
		tb_long_t n = tb_http_awrit(handle, data + writ, size - writ);	
		if (n > 0)
		{
			// update writ
			writ += n;

			// update clock
			time = tb_mclock();
		}
		else if (!n)
		{
			// timeout?
			if (tb_mclock() - time > http->option.timeout) break;

			// sleep some time
			tb_usleep(100);
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
	tb_int64_t 	time = tb_mclock();
	while (read < size)
	{
		// read data
		tb_long_t n = tb_http_aread(handle, data + read, size - read);	
		if (n > 0)
		{
			// update read
			read += n;

			// update clock
			time = tb_mclock();
		}
		else if (!n)
		{
			// timeout?
			if (tb_mclock() - time > http->option.timeout) break;

			// sleep some time
			tb_usleep(100);
		}
		else break;
	}

	// ok?
	return read == size? TB_TRUE : TB_FALSE;
}

tb_size_t tb_http_option_get_port(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, g_http_option_default.port);

	return http->option.port;
}
tb_char_t const* tb_http_option_get_url(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_NULL);

	tb_trace_noimpl();
	return TB_NULL;
	//return http->option.url;
}
tb_char_t const* tb_http_option_get_host(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_NULL);

	tb_trace_noimpl();
	return TB_NULL;
	//return http->option.host;
}
tb_char_t const* tb_http_option_get_path(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_NULL);

	tb_trace_noimpl();
	return TB_NULL;
	//return http->option.path;
}
tb_cookies_t* tb_http_option_get_cookies(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_NULL);

	return http->option.cookies;
}
tb_bool_t tb_http_option_set_default(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	http->option = g_http_option_default;
	return TB_TRUE;
}
tb_bool_t tb_http_option_set_method(tb_handle_t handle, tb_size_t method)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	http->option.method = method;
	return TB_TRUE;
}

tb_bool_t tb_http_option_set_ssl(tb_handle_t handle, tb_bool_t bssl)
{	
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	http->option.bssl = bssl? 1 : 0;
	return TB_TRUE;
}
tb_bool_t tb_http_option_set_port(tb_handle_t handle, tb_uint16_t port)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	http->option.port = port;
	return TB_TRUE;
}
tb_bool_t tb_http_option_set_url(tb_handle_t handle, tb_char_t const* url)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

#if 0
	return tb_http_split_url(http, url);
#else
	tb_trace_noimpl();
	return TB_FALSE;
#endif
}
tb_bool_t tb_http_option_set_host(tb_handle_t handle, tb_char_t const* host)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

#if 0
	tb_strncpy(http->option.host, host? host : "", TB_HTTP_HOST_MAX);
	http->option.host[TB_HTTP_HOST_MAX - 1] = '\0';
	return TB_TRUE;
#else
	tb_trace_noimpl();
	return TB_FALSE;
#endif
}
tb_bool_t tb_http_option_set_path(tb_handle_t handle, tb_char_t const* path)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

#if 0
	tb_strncpy(http->option.path, path? path : "", TB_HTTP_PATH_MAX);
	http->option.path[TB_HTTP_PATH_MAX - 1] = '\0';
	return TB_TRUE;
#else
	tb_trace_noimpl();
	return TB_FALSE;
#endif
}
tb_bool_t tb_http_option_set_kalive(tb_handle_t handle, tb_bool_t bkalive)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	http->option.bkalive = bkalive == TB_TRUE? 1 : 0;
	return TB_TRUE;
}
tb_bool_t tb_http_option_set_timeout(tb_handle_t handle, tb_size_t timeout)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	http->option.timeout = timeout;
	return TB_TRUE;
}
tb_bool_t tb_http_option_set_range(tb_handle_t handle, tb_http_range_t const* range)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	if (range) http->option.range = *range;
	else tb_memset(&http->option.range, 0, sizeof(tb_http_range_t));
	return TB_TRUE;
}
tb_bool_t tb_http_option_set_redirect(tb_handle_t handle, tb_uint8_t redirect)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	http->option.redirect = redirect;
	return TB_TRUE;
}

tb_bool_t tb_http_option_set_head(tb_handle_t handle, tb_char_t const* head)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

#if 0
	tb_strncpy(http->option.head, head? head : "", TB_HTTP_HEAD_MAX);
	http->option.head[TB_HTTP_HEAD_MAX - 1] = '\0';	
	return TB_TRUE;
#else
	tb_trace_noimpl();
	return TB_FALSE;
#endif
}

tb_bool_t tb_http_option_set_cookies(tb_handle_t handle, tb_cookies_t* cookies)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	http->option.cookies = cookies;
	return TB_TRUE;
}
tb_bool_t tb_http_option_set_post(tb_handle_t handle, tb_byte_t const* data, tb_size_t size)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);
	
#if 0
	tb_assert_and_check_return_val(data && size, TB_FALSE);
	http->option.post_data = data;
	http->option.post_size = size;
	return TB_TRUE;
#else
	tb_trace_noimpl();
	return TB_FALSE;
#endif
}
tb_bool_t tb_http_option_set_hfunc(tb_handle_t handle, tb_bool_t (*head_func)(tb_char_t const* , tb_pointer_t ), tb_pointer_t head_priv)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	http->option.head_func = head_func;
	http->option.head_priv = head_priv;
	return TB_TRUE;
}

tb_http_status_t const*	tb_http_status(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_NULL);

	return &http->status;
}

tb_uint64_t tb_http_status_content_size(tb_handle_t handle)
{	
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, 0);

	return http->status.content_size;
}

tb_uint64_t tb_http_status_document_size(tb_handle_t handle)
{	
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, 0);

	return http->status.document_size;
}
tb_char_t const* tb_http_status_content_type(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_NULL);

	tb_trace_noimpl();
	return TB_NULL;
//	return http->status.content_type;
}

tb_bool_t tb_http_status_ischunked(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	return http->status.bchunked? TB_TRUE : TB_FALSE;
}
tb_bool_t tb_http_status_isredirect(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	return http->status.bredirect? TB_TRUE : TB_FALSE;
}
tb_bool_t tb_http_status_iskalive(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	return http->status.bkalive? TB_TRUE : TB_FALSE;
}
tb_bool_t tb_http_status_isseeked(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, TB_FALSE);

	return http->status.bseeked? TB_TRUE : TB_FALSE;
}
tb_size_t tb_http_status_redirect(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, 0);

	return http->status.redirect;
}
tb_size_t tb_http_status_code(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return_val(http, 0);

	return http->status.code;
}
#ifdef TB_DEBUG
tb_void_t tb_http_option_dump(tb_handle_t handle)
{
	tb_http_t* http = (tb_http_t*)handle;
	tb_assert_and_check_return(http);

	tb_trace("[http]: =============================================");
	tb_trace("[http]: option: ");
//	tb_trace("[http]: option: url: %s", http->option.url);
//	tb_trace("[http]: option: host: %s", http->option.host);
//	tb_trace("[http]: option: path: %s", http->option.path);
	tb_trace("[http]: option: port: %d", http->option.port);
//	tb_trace("[http]: option: method: %s", tb_http_method_string(http->option.method));
	tb_trace("[http]: option: redirect: %d", http->option.redirect);
	tb_trace("[http]: option: https: %s", http->option.bssl? "true" : "false");
	tb_trace("[http]: option: range: %llu-%llu", http->option.range.bof, http->option.range.eof);
	tb_trace("[http]: option: keepalive: %s", http->option.bkalive? "true" : "false");

	if (http->option.cookies)
	{
		//tb_cookies_dump(http->option.cookies);

#if 0
		// get cookie
		tb_char_t const* value = tb_cookies_get_from_url(http->option.cookies, http->option.url);

		// format it
		if (value) 
		{
			tb_trace("[http]: option: cookie: %s", value);
		}
#else
		tb_trace_noimpl();
#endif
	}
}
tb_void_t tb_http_status_dump(tb_handle_t handle)
{
	tb_assert_and_check_return(handle);
	tb_http_t* http = (tb_http_t*)handle;

	tb_trace("[http]: =============================================");
	tb_trace("[http]: status: ");
	tb_trace("[http]: status: code: %d", http->status.code);
	tb_trace("[http]: status: version: %s", http->status.version == TB_HTTP_VERSION_11? "HTTP/1.1" : "HTTP/1.0");
//	tb_trace("[http]: status: content:type: %s", http->status.content_type);
	tb_trace("[http]: status: content:size: %llu", http->status.content_size);
	tb_trace("[http]: status: document:size: %llu", http->status.document_size);
	tb_trace("[http]: status: chunked:read: %d", http->status.chunked_read);
	tb_trace("[http]: status: chunked:size: %d", http->status.chunked_size);
	tb_trace("[http]: status: redirect: %d", http->status.redirect);
	tb_trace("[http]: status: bredirect: %s", http->status.bredirect? "true" : "false");
	tb_trace("[http]: status: bchunked: %s", http->status.bchunked? "true" : "false");
	tb_trace("[http]: status: bseeked: %s", http->status.bseeked? "true" : "false");
	tb_trace("[http]: status: bkalive: %s", http->status.bkalive? "true" : "false");
}
#endif

