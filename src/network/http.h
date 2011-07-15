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

// the http option type
typedef struct __tb_http_option_t
{
	// the method
	tb_uint8_t 			method;

	// the max redirect
	tb_uint8_t 			redirect;

	// the port
	tb_uint16_t 		port;

	// is block?
	tb_uint16_t 		bblock : 1;

	// is https?
	tb_uint16_t 		bhttps : 1;

	// is keep alive?
	tb_uint16_t 		bkalive : 1;

	// timeout, ms
	tb_uint16_t 		timeout;

	// the head funcs
	tb_bool_t 			(*head_func)(tb_char_t const* line, void* priv);
	void* 				head_priv;

	// the ssl funcs
	tb_handle_t 		(*sopen_func)(tb_char_t const* host, tb_size_t port);
	void 				(*sclose_func)(tb_handle_t handle);
	tb_int_t 			(*sread_func)(tb_handle_t handle, tb_byte_t* data, tb_size_t size);
	tb_int_t 			(*swrite_func)(tb_handle_t handle, tb_byte_t const* data, tb_size_t size);

	// the post data
	tb_byte_t const* 	post_data;
	tb_size_t 			post_size;

	// the reference to cookies
	tb_cookies_t* 		cookies;

	// the request head
	tb_char_t 			head[TB_HTTP_HEAD_MAX];

	// the url
	tb_char_t 			url[TB_HTTP_URL_MAX];
	tb_char_t 			host[TB_HTTP_HOST_MAX];
	tb_char_t 			path[TB_HTTP_PATH_MAX];

}tb_http_option_t;

// the http status type
typedef struct __tb_http_status_t
{
	// the http code
	tb_uint16_t 		code;

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
	tb_uint8_t 			bhttps 		: 1;

	// is keep alive?
	tb_uint8_t 			bkalive		: 1;

	// the redirect count
	tb_uint8_t 			redirect;

	// the content size
	tb_size_t 			content_size;

	// the chunked size
	tb_size_t 			chunked_read;
	tb_size_t 			chunked_size;

	// the process line
	tb_char_t 			line[TB_HTTP_LINE_MAX];

	// the content type
	tb_char_t 			content_type[TB_HTTP_CONTENT_TYPE_MAX];

}tb_http_status_t;

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// create & destroy
tb_handle_t 			tb_http_create(tb_http_option_t const* option);
void 					tb_http_destroy(tb_handle_t handle);

// open & close
tb_bool_t 				tb_http_open(tb_handle_t handle);
void 					tb_http_close(tb_handle_t handle);

// options
void 					tb_http_option_dump(tb_handle_t handle);
tb_size_t 				tb_http_option_get_port(tb_handle_t handle);
tb_char_t const* 		tb_http_option_get_url(tb_handle_t handle);
tb_char_t const* 		tb_http_option_get_host(tb_handle_t handle);
tb_char_t const* 		tb_http_option_get_path(tb_handle_t handle);
tb_cookies_t* 			tb_http_option_get_cookies(tb_handle_t handle);

tb_bool_t 				tb_http_option_set_default(tb_handle_t handle);
tb_bool_t 				tb_http_option_set_method(tb_handle_t handle, tb_http_method_t request);
tb_bool_t 				tb_http_option_set_port(tb_handle_t handle, tb_uint16_t port);
tb_bool_t 				tb_http_option_set_url(tb_handle_t handle, tb_char_t const* url);
tb_bool_t 				tb_http_option_set_host(tb_handle_t handle, tb_char_t const* host);
tb_bool_t 				tb_http_option_set_path(tb_handle_t handle, tb_char_t const* path);
tb_bool_t 				tb_http_option_set_block(tb_handle_t handle, tb_bool_t bblock);
tb_bool_t 				tb_http_option_set_kalive(tb_handle_t handle, tb_bool_t bkalive);
tb_bool_t 				tb_http_option_set_timeout(tb_handle_t handle, tb_uint16_t timeout);
tb_bool_t 				tb_http_option_set_redirect(tb_handle_t handle, tb_uint8_t redirect);
tb_bool_t 				tb_http_option_set_head(tb_handle_t handle, tb_char_t const* head);
tb_bool_t 				tb_http_option_set_cookies(tb_handle_t handle, tb_cookies_t* cookies);
tb_bool_t 				tb_http_option_set_post(tb_handle_t handle, tb_byte_t const* data, tb_size_t size);
tb_bool_t 				tb_http_option_set_head_func(tb_handle_t handle, tb_bool_t (*head_func)(tb_char_t const* , void* ), void* head_priv);
tb_bool_t 				tb_http_option_set_sopen_func(tb_handle_t handle, tb_handle_t (*sopen_func)(tb_char_t const*, tb_size_t ));
tb_bool_t 				tb_http_option_set_sclose_func(tb_handle_t handle, void (*sclose_func)(tb_handle_t));
tb_bool_t 				tb_http_option_set_sread_func(tb_handle_t handle, tb_int_t (*sread_func)(tb_handle_t, tb_byte_t* , tb_size_t));
tb_bool_t 				tb_http_option_set_swrite_func(tb_handle_t handle, tb_int_t (*swrite_func)(tb_handle_t, tb_byte_t const* , tb_size_t));

// status
tb_http_status_t const*	tb_http_status(tb_handle_t handle);
tb_size_t				tb_http_status_content_size(tb_handle_t handle);
tb_char_t const*		tb_http_status_content_type(tb_handle_t handle);
tb_size_t				tb_http_status_code(tb_handle_t handle);
tb_bool_t				tb_http_status_ischunked(tb_handle_t handle);
tb_bool_t				tb_http_status_isredirect(tb_handle_t handle);
tb_bool_t				tb_http_status_iskalive(tb_handle_t handle);
tb_size_t				tb_http_status_redirect(tb_handle_t handle);
void 					tb_http_status_dump(tb_handle_t handle);

// write & read
tb_int_t 				tb_http_write(tb_handle_t handle, tb_byte_t* data, tb_size_t size);
tb_int_t 				tb_http_read(tb_handle_t handle, tb_byte_t* data, tb_size_t size);

tb_int_t 				tb_http_bwrite(tb_handle_t handle, tb_byte_t* data, tb_size_t size);
tb_int_t 				tb_http_bread(tb_handle_t handle, tb_byte_t* data, tb_size_t size);


#endif

