/*!The Tiny Box Library
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
 * \file		http.h
 *
 */
#ifndef TB_NETWORK_HTTP_H
#define TB_NETWORK_HTTP_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "cookies.h"
#include "../string/string.h"
#include "../stream/stream.h"
#include "../container/container.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_HTTP_LINE_MAX 						(8192)
#define TB_HTTP_HOST_MAX 						(1024)
#define TB_HTTP_PATH_MAX 						(4096)
#define TB_HTTP_URL_MAX 						((TB_HTTP_HOST_MAX) + (TB_HTTP_PATH_MAX) + 12)
#define TB_HTTP_CONTENT_TYPE_MAX 				(64)
#define TB_HTTP_HEAD_MAX 						(8192)
#define TB_HTTP_RESPONSE_MAX 					(4096)
/* ////////////////////////////////////////////////////////////////////////
 * types
 */

// the http method constant
typedef enum __tb_http_method_t
{
 	TB_HTTP_METHOD_GET 		= 0
, 	TB_HTTP_METHOD_POST 	= 1
, 	TB_HTTP_METHOD_HEAD 	= 2
, 	TB_HTTP_METHOD_PUT 		= 3
,	TB_HTTP_METHOD_OPTIONS 	= 4
, 	TB_HTTP_METHOD_DELETE 	= 5
, 	TB_HTTP_METHOD_TRACE 	= 6
, 	TB_HTTP_METHOD_CONNECT 	= 7

}tb_http_method_t;

// the http version constant
typedef enum __tb_http_version_t
{
 	TB_HTTP_VERSION_10
, 	TB_HTTP_VERSION_11

}tb_http_version_t;

// the http range type
typedef struct __tb_http_range_t
{
	// range
	tb_uint64_t 		bof;
	tb_uint64_t 		eof;

}tb_http_range_t;

// the http option type
typedef struct __tb_http_option_t
{
	// the method
	tb_uint8_t 			method;

	// the max redirect
	tb_uint8_t 			redirect;

	// the port
	tb_uint16_t 		port;

	// is ssl?
	tb_uint16_t 		bssl 	: 1;

	// is keep alive?
	tb_uint16_t 		bkalive : 1;

	// timeout, ms
	tb_size_t 			timeout;

	// range
	tb_http_range_t 	range;

	// the reference to cookies
	tb_cookies_t* 		cookies;

	// the head funcs
	tb_bool_t 			(*head_func)(tb_char_t const* line, tb_pointer_t priv);
	tb_pointer_t 		head_priv;

	// the head hash
	tb_hash_t* 			head_hash;

	// the head data
	tb_string_t 		head_data;

	// the post data
	tb_string_t 		post;

	// the response data
	tb_string_t 		resp;

	// the url data
	tb_string_t 		url;

	// the host data
	tb_string_t 		host;

	// the path data
	tb_string_t 		path;

}tb_http_option_t;

// the http state type
typedef enum __tb_http_state_t
{
	TB_HTTP_STATE_NULL 			= 0
,	TB_HTTP_STATE_OPEN 			= 1
,	TB_HTTP_STATE_REQUEST 		= 2
,	TB_HTTP_STATE_RESPONSE 		= 3
,	TB_HTTP_STATE_REDIRECT 		= 4
,	TB_HTTP_STATE_OK 			= 5

}tb_http_state_t;

// the http status type
typedef struct __tb_http_status_t
{
	// the http code
	tb_uint16_t 		code 		: 10;

	// the http state
	tb_uint16_t 		state 		: 6;

	// the http version
	tb_uint8_t 			version 	: 1;

	// the connection is closed?
	tb_uint8_t 			bclosed 	: 1;

	// be able to seek?
	tb_uint8_t 			bseeked		: 1;

	// is chunk data
	tb_uint8_t 			bchunked	: 1;

	// is redirect?
	tb_uint8_t 			bredirect 	: 1;

	// is https?
	tb_uint8_t 			bssl 		: 1;

	// is keep alive?
	tb_uint8_t 			bkalive		: 1;

	// the redirect count
	tb_uint8_t 			redirect;

	// the content size
	tb_uint64_t 		content_size;

	// the document size
	tb_uint64_t 		document_size;

	// the chunked size
	tb_size_t 			chunked_read;
	tb_size_t 			chunked_size;

	// the content type
	tb_char_t 			content_type[TB_HTTP_CONTENT_TYPE_MAX];

}tb_http_status_t;

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_handle_t 			tb_http_init(tb_http_option_t const* option);
tb_void_t 				tb_http_exit(tb_handle_t handle);

// async open, allow multiple called before closing 
tb_long_t 				tb_http_aopen(tb_handle_t handle);

// block open, allow multiple called before closing 
tb_bool_t 				tb_http_bopen(tb_handle_t handle);

// async close, allow multiple called
tb_long_t 				tb_http_aclose(tb_handle_t handle);

// block close, allow multiple called
tb_bool_t 				tb_http_bclose(tb_handle_t handle);

// async writ & read
tb_long_t 				tb_http_awrit(tb_handle_t handle, tb_byte_t* data, tb_size_t size);
tb_long_t 				tb_http_aread(tb_handle_t handle, tb_byte_t* data, tb_size_t size);

// block writ & read
tb_bool_t 				tb_http_bwrit(tb_handle_t handle, tb_byte_t* data, tb_size_t size);
tb_bool_t 				tb_http_bread(tb_handle_t handle, tb_byte_t* data, tb_size_t size);

// options
tb_void_t 				tb_http_option_dump(tb_handle_t handle);
tb_size_t 				tb_http_option_get_port(tb_handle_t handle);
tb_char_t const* 		tb_http_option_get_url(tb_handle_t handle);
tb_char_t const* 		tb_http_option_get_host(tb_handle_t handle);
tb_char_t const* 		tb_http_option_get_path(tb_handle_t handle);
tb_cookies_t* 			tb_http_option_get_cookies(tb_handle_t handle);

tb_bool_t 				tb_http_option_set_default(tb_handle_t handle);
tb_bool_t 				tb_http_option_set_method(tb_handle_t handle, tb_size_t method);
tb_bool_t 				tb_http_option_set_ssl(tb_handle_t handle, tb_bool_t bssl);
tb_bool_t 				tb_http_option_set_url(tb_handle_t handle, tb_char_t const* url);
tb_bool_t 				tb_http_option_set_port(tb_handle_t handle, tb_uint16_t port);
tb_bool_t 				tb_http_option_set_host(tb_handle_t handle, tb_char_t const* host);
tb_bool_t 				tb_http_option_set_path(tb_handle_t handle, tb_char_t const* path);
tb_bool_t 				tb_http_option_set_kalive(tb_handle_t handle, tb_bool_t bkalive);
tb_bool_t 				tb_http_option_set_timeout(tb_handle_t handle, tb_size_t timeout);
tb_bool_t 				tb_http_option_set_range(tb_handle_t handle, tb_http_range_t const* range);
tb_bool_t 				tb_http_option_set_redirect(tb_handle_t handle, tb_uint8_t redirect);
tb_bool_t 				tb_http_option_set_head(tb_handle_t handle, tb_char_t const* head);
tb_bool_t 				tb_http_option_set_cookies(tb_handle_t handle, tb_cookies_t* cookies);
tb_bool_t 				tb_http_option_set_post(tb_handle_t handle, tb_byte_t const* data, tb_size_t size);
tb_bool_t 				tb_http_option_set_hfunc(tb_handle_t handle, tb_bool_t (*head_func)(tb_char_t const* , tb_pointer_t ), tb_pointer_t head_priv);

// status
tb_http_status_t const*	tb_http_status(tb_handle_t handle);
tb_uint64_t				tb_http_status_content_size(tb_handle_t handle);
tb_char_t const*		tb_http_status_content_type(tb_handle_t handle);
tb_uint64_t				tb_http_status_document_size(tb_handle_t handle);
tb_size_t				tb_http_status_code(tb_handle_t handle);
tb_bool_t				tb_http_status_ischunked(tb_handle_t handle);
tb_bool_t				tb_http_status_isredirect(tb_handle_t handle);
tb_bool_t				tb_http_status_iskalive(tb_handle_t handle);
tb_bool_t				tb_http_status_isseeked(tb_handle_t handle);
tb_size_t				tb_http_status_redirect(tb_handle_t handle);
tb_void_t 				tb_http_status_dump(tb_handle_t handle);


#endif

