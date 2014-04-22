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
 * @file		http.h
 * @ingroup 	network
 *
 */
#ifndef TB_NETWORK_HTTP_H
#define TB_NETWORK_HTTP_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "url.h"
#include "cookies.h"
#include "../string/string.h"
#include "../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the http default timeout
#define TB_HTTP_DEFAULT_TIMEOUT 				(10000)

/// the http default redirect maxn
#define TB_HTTP_DEFAULT_REDIRECT 				(10)

/// the http default port
#define TB_HTTP_DEFAULT_PORT 					(80)

/// the http default port for ssl
#define TB_HTTP_DEFAULT_PORT_SSL 				(443)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the http method enum
typedef enum __tb_http_method_e
{
 	TB_HTTP_METHOD_GET 						= 0
, 	TB_HTTP_METHOD_POST 					= 1
, 	TB_HTTP_METHOD_HEAD 					= 2
, 	TB_HTTP_METHOD_PUT 						= 3
,	TB_HTTP_METHOD_OPTIONS 					= 4
, 	TB_HTTP_METHOD_DELETE 					= 5
, 	TB_HTTP_METHOD_TRACE 					= 6
, 	TB_HTTP_METHOD_CONNECT 					= 7

}tb_http_method_e;

/// the http option enum
typedef enum __tb_http_option_e
{
	TB_HTTP_OPTION_NONE 				= 0

,	TB_HTTP_OPTION_GET_SSL 				= 1
,	TB_HTTP_OPTION_GET_URL 				= 2
,	TB_HTTP_OPTION_GET_HOST 			= 3
,	TB_HTTP_OPTION_GET_PORT 			= 4
,	TB_HTTP_OPTION_GET_PATH 			= 5
,	TB_HTTP_OPTION_GET_HEAD 			= 6
,	TB_HTTP_OPTION_GET_RANGE 			= 7 
,	TB_HTTP_OPTION_GET_METHOD 			= 8
,	TB_HTTP_OPTION_GET_VERSION			= 9 
,	TB_HTTP_OPTION_GET_TIMEOUT			= 10
,	TB_HTTP_OPTION_GET_REDIRECT			= 11 
,	TB_HTTP_OPTION_GET_HEAD_FUNC		= 12
,	TB_HTTP_OPTION_GET_HEAD_PRIV		= 13
,	TB_HTTP_OPTION_GET_AUTO_UNZIP		= 14
,	TB_HTTP_OPTION_GET_POST_URL			= 15
,	TB_HTTP_OPTION_GET_POST_DATA		= 16
,	TB_HTTP_OPTION_GET_POST_FUNC		= 17
,	TB_HTTP_OPTION_GET_POST_PRIV		= 18
,	TB_HTTP_OPTION_GET_POST_LRATE		= 19

,	TB_HTTP_OPTION_SET_SSL 				= 51
,	TB_HTTP_OPTION_SET_URL 				= 52
,	TB_HTTP_OPTION_SET_HOST 			= 53
,	TB_HTTP_OPTION_SET_PORT 			= 54
,	TB_HTTP_OPTION_SET_PATH 			= 55
,	TB_HTTP_OPTION_SET_HEAD 			= 56
,	TB_HTTP_OPTION_SET_RANGE 			= 57
,	TB_HTTP_OPTION_SET_METHOD 			= 58
,	TB_HTTP_OPTION_SET_VERSION			= 59
,	TB_HTTP_OPTION_SET_TIMEOUT			= 60
,	TB_HTTP_OPTION_SET_REDIRECT			= 61
,	TB_HTTP_OPTION_SET_HEAD_FUNC		= 62
,	TB_HTTP_OPTION_SET_HEAD_PRIV		= 63
,	TB_HTTP_OPTION_SET_AUTO_UNZIP		= 64
,	TB_HTTP_OPTION_SET_POST_URL			= 65
,	TB_HTTP_OPTION_SET_POST_DATA		= 66
,	TB_HTTP_OPTION_SET_POST_FUNC		= 67
,	TB_HTTP_OPTION_SET_POST_PRIV		= 68
,	TB_HTTP_OPTION_SET_POST_LRATE		= 69

}tb_http_option_e;

/// the http range type
typedef struct __tb_http_range_t
{
	/// the begin offset
	tb_hize_t 			bof;

	/// the end offset
	tb_hize_t 			eof;

}tb_http_range_t;

/*! the http head func type
 *
 * @param http 			the http handle
 * @param line 			the http head line
 * @param priv 			the func private data
 *
 * @return 				tb_true: ok and continue it if need, tb_false: break it
 */
typedef tb_bool_t 		(*tb_http_head_func_t)(tb_handle_t http, tb_char_t const* line, tb_pointer_t priv);

/*! the http post func type
 *
 * @param http 			the http handle
 * @param offset 		the istream offset
 * @param size 			the istream size, no size: -1
 * @param save 			the saved size
 * @param rate 			the current rate, bytes/s
 * @param priv 			the func private data
 *
 * @return 				tb_true: ok and continue it if need, tb_false: break it
 */
typedef tb_bool_t 		(*tb_http_post_func_t)(tb_handle_t http, tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv);

/// the http option type
typedef struct __tb_http_option_t
{
	/// the method
	tb_uint16_t 		method 		: 4;

	/// auto unzip for gzip encoding?
	tb_uint16_t 		bunzip 		: 1;

	/// the http version, 0: HTTP/1.0, 1: HTTP/1.1
	tb_uint16_t 		version 	: 1;

	/// the redirect maxn
	tb_uint16_t 		redirect 	: 10;

	/// the url
	tb_url_t 			url;

	/// the request head 
	tb_hash_t* 			head;

	/// timeout: ms
	tb_size_t 			timeout;

	/// range
	tb_http_range_t 	range;

	/// the priv data
	tb_pointer_t 		head_priv;

	/// the head func
	tb_http_head_func_t head_func;

	/// the post url
	tb_url_t 			post_url;

	/// the post data
	tb_byte_t const* 	post_data;

	/// the post size
	tb_size_t 			post_size;

	/// the post func
	tb_http_post_func_t post_func;

	/// the post data
	tb_pointer_t 		post_priv;

	/// the post limit rate
	tb_size_t 			post_lrate;

}tb_http_option_t;

// the http status type
typedef struct __tb_http_status_t
{
	/// the http code
	tb_uint16_t 		code 		: 10;

	/// the http version
	tb_uint16_t 		version 	: 1;

	/// keep alive?
	tb_uint16_t 		balived		: 1;

	/// be able to seek?
	tb_uint16_t 		bseeked		: 1;

	/// is chunked?
	tb_uint16_t 		bchunked	: 1;

	/// is gzip?
	tb_uint16_t 		bgzip		: 1;

	/// is deflate?
	tb_uint16_t 		bdeflate	: 1;

	/// the state
	tb_size_t 			state;

	/// the document size
	tb_hize_t 			document_size;

	/// the current content size, maybe in range
	tb_hize_t 			content_size;

	/// the content type
	tb_scoped_string_t 	content_type;

	/// the location
	tb_scoped_string_t 	location;

}tb_http_status_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init http
 *
 * return 				the http handle
 */
tb_handle_t 			tb_http_init(tb_noarg_t);

/*! exit http
 *
 * @param handle 		the http handle
 */
tb_void_t 				tb_http_exit(tb_handle_t handle);

/*! kill http
 *
 * @param handle 		the http handle
 */
tb_void_t 				tb_http_kill(tb_handle_t handle);

/*! wait the http 
 *
 * blocking wait the single event object, so need not aiop 
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param handle 		the http handle 
 * @param aioe 			the aioe
 * @param timeout 		the timeout value, return immediately if 0, infinity if -1
 *
 * @return 				the event type, return 0 if timeout, return -1 if error
 */
tb_long_t 				tb_http_wait(tb_handle_t handle, tb_size_t aioe, tb_long_t timeout);

/*! open the http
 *
 * @param handle 		the http handle
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_http_open(tb_handle_t handle);

/*! close http
 *
 * @param handle 		the http handle
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_http_clos(tb_handle_t handle);

/*! seek http
 *
 * @param handle 		the http handle
 * @param offset 		the offset
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_http_seek(tb_handle_t handle, tb_hize_t offset);

/*! read data, non-blocking
 *
 * @param handle 		the http handle
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				ok: real size, continue: 0, fail: -1
 */
tb_long_t 				tb_http_read(tb_handle_t handle, tb_byte_t* data, tb_size_t size);

/*! read data, blocking
 *
 * @param handle 		the http handle
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_http_bread(tb_handle_t handle, tb_byte_t* data, tb_size_t size);

/*! the http option
 *
 * @param handle 		the http handle
 * @param option 		the option
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_http_option(tb_handle_t handle, tb_size_t option, ...);

/*! the http status
 *
 * @param handle 		the http handle
 *
 * @return 				the http status
 */
tb_http_status_t const*	tb_http_status(tb_handle_t handle);

#endif

