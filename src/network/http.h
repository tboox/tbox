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
 * @file		http.h
 * @ingroup 	network
 *
 */
#ifndef TB_NETWORK_HTTP_H
#define TB_NETWORK_HTTP_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "url.h"
#include "cookies.h"
#include "../string/string.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
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

/* ///////////////////////////////////////////////////////////////////////
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
,	TB_HTTP_OPTION_GET_POST_SIZE 		= 14
,	TB_HTTP_OPTION_GET_AUTO_UNZIP		= 15

,	TB_HTTP_OPTION_SET_SSL 				= 16
,	TB_HTTP_OPTION_SET_URL 				= 17
,	TB_HTTP_OPTION_SET_HOST 			= 18
,	TB_HTTP_OPTION_SET_PORT 			= 19
,	TB_HTTP_OPTION_SET_PATH 			= 20
,	TB_HTTP_OPTION_SET_HEAD 			= 21
,	TB_HTTP_OPTION_SET_RANGE 			= 22
,	TB_HTTP_OPTION_SET_METHOD 			= 23
,	TB_HTTP_OPTION_SET_VERSION			= 24
,	TB_HTTP_OPTION_SET_TIMEOUT			= 25
,	TB_HTTP_OPTION_SET_REDIRECT			= 26
,	TB_HTTP_OPTION_SET_HEAD_FUNC		= 27
,	TB_HTTP_OPTION_SET_HEAD_PRIV		= 28
,	TB_HTTP_OPTION_SET_POST_SIZE 		= 29
,	TB_HTTP_OPTION_SET_AUTO_UNZIP		= 30

}tb_http_option_e;

/// the http range type
typedef struct __tb_http_range_t
{
	/// the begin offset
	tb_hize_t 			bof;

	/// the end offset
	tb_hize_t 			eof;

}tb_http_range_t;

/// the http head func type
typedef tb_bool_t (*tb_http_head_func_t)(tb_handle_t http, tb_char_t const* line, tb_pointer_t priv);

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

	/// the post size
	tb_hize_t 			post;

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

	/// the astream or gstream state
	tb_size_t 			state;

	/// the document size
	tb_hize_t 			document_size;

	/// the current content size, maybe in range
	tb_hize_t 			content_size;

	/// the content type
	tb_pstring_t 		content_type;

	/// the location
	tb_pstring_t 		location;

}tb_http_status_t;

/* ///////////////////////////////////////////////////////////////////////
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

/*! async open, allow multiple called before closing 
 *
 * @param handle 		the http handle
 *
 * @return 				ok: 1, continue: 0, fail: -1
 */
tb_long_t 				tb_http_aopen(tb_handle_t handle);

/*! block open, allow multiple called before closing 
 *
 * @param handle 		the http handle
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_http_bopen(tb_handle_t handle);

/*! async close, allow multiple called
 *
 * @param handle 		the http handle
 *
 * @return 				ok: 1, continue: 0, fail: -1
 */
tb_long_t 				tb_http_aclos(tb_handle_t handle);

/*! block close, allow multiple called
 *
 * @param handle 		the http handle
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_http_bclos(tb_handle_t handle);

/*! async seek
 *
 * @param handle 		the http handle
 * @param offset 		the offset
 *
 * @return 				ok: 1, continue: 0, fail: -1
 */
tb_long_t 				tb_http_aseek(tb_handle_t handle, tb_hize_t offset);

/*! block seek
 *
 * @param handle 		the http handle
 * @param offset 		the offset
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_http_bseek(tb_handle_t handle, tb_hize_t offset);

/*! async writ
 *
 * @param handle 		the http handle
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				ok: real size, continue: 0, fail: -1
 */
tb_long_t 				tb_http_awrit(tb_handle_t handle, tb_byte_t const* data, tb_size_t size);

/*! block writ
 *
 * @param handle 		the http handle
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_http_bwrit(tb_handle_t handle, tb_byte_t const* data, tb_size_t size);

/*! async read
 *
 * @param handle 		the http handle
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				ok: real size, continue: 0, fail: -1
 */
tb_long_t 				tb_http_aread(tb_handle_t handle, tb_byte_t* data, tb_size_t size);

/*! block read
 *
 * @param handle 		the http handle
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_http_bread(tb_handle_t handle, tb_byte_t* data, tb_size_t size);

/*! async flush writ
 *
 * @param handle 		the http handle
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				ok: 1, continue: 0, failed: -1
 */
tb_long_t 				tb_http_afwrit(tb_handle_t handle, tb_byte_t const* data, tb_size_t size);

/*! block flush writ
 *
 * @param handle 		the http handle
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_http_bfwrit(tb_handle_t handle, tb_byte_t const* data, tb_size_t size);

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

