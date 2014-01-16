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

// the default port
#define TB_HTTP_PORT_DEFAULT 					(80)
#define TB_HTTPS_PORT_DEFAULT 					(443)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the http method enum
typedef enum __tb_http_method_e
{
 	TB_HTTP_METHOD_GET 				= 0
, 	TB_HTTP_METHOD_POST 			= 1
, 	TB_HTTP_METHOD_HEAD 			= 2
, 	TB_HTTP_METHOD_PUT 				= 3
,	TB_HTTP_METHOD_OPTIONS 			= 4
, 	TB_HTTP_METHOD_DELETE 			= 5
, 	TB_HTTP_METHOD_TRACE 			= 6
, 	TB_HTTP_METHOD_CONNECT 			= 7

}tb_http_method_e;

/// the http version enum
typedef enum __tb_http_version_e
{
 	TB_HTTP_VERSION_10 				= 0
, 	TB_HTTP_VERSION_11 				= 1

}tb_http_version_e;

/// the http seek enum
typedef enum __tb_http_seek_e
{
 	TB_HTTP_SEEK_BEG 				= 0
, 	TB_HTTP_SEEK_CUR 				= 1
, 	TB_HTTP_SEEK_END 				= 2

}tb_http_seek_e;

/// the http error enum
typedef enum __tb_http_error_e
{
	TB_HTTP_ERROR_OK 				= 0
,	TB_HTTP_ERROR_RESPONSE_204 		= 1
,	TB_HTTP_ERROR_RESPONSE_300 		= 2
,	TB_HTTP_ERROR_RESPONSE_301 		= 3
,	TB_HTTP_ERROR_RESPONSE_302 		= 4
,	TB_HTTP_ERROR_RESPONSE_303 		= 5
,	TB_HTTP_ERROR_RESPONSE_304 		= 6
,	TB_HTTP_ERROR_RESPONSE_400 		= 7
,	TB_HTTP_ERROR_RESPONSE_401 		= 8
,	TB_HTTP_ERROR_RESPONSE_402 		= 9
,	TB_HTTP_ERROR_RESPONSE_403 		= 10
,	TB_HTTP_ERROR_RESPONSE_404 		= 11
,	TB_HTTP_ERROR_RESPONSE_405 		= 12
,	TB_HTTP_ERROR_RESPONSE_406 		= 13
,	TB_HTTP_ERROR_RESPONSE_407 		= 14
,	TB_HTTP_ERROR_RESPONSE_408 		= 15
,	TB_HTTP_ERROR_RESPONSE_409 		= 16
,	TB_HTTP_ERROR_RESPONSE_410 		= 17
,	TB_HTTP_ERROR_RESPONSE_411 		= 18
,	TB_HTTP_ERROR_RESPONSE_412 		= 19
,	TB_HTTP_ERROR_RESPONSE_413 		= 20
,	TB_HTTP_ERROR_RESPONSE_414 		= 21
,	TB_HTTP_ERROR_RESPONSE_415 		= 22
,	TB_HTTP_ERROR_RESPONSE_416 		= 23
,	TB_HTTP_ERROR_RESPONSE_500 		= 24
,	TB_HTTP_ERROR_RESPONSE_501 		= 25
,	TB_HTTP_ERROR_RESPONSE_502 		= 26
,	TB_HTTP_ERROR_RESPONSE_503 		= 27
,	TB_HTTP_ERROR_RESPONSE_504 		= 28
,	TB_HTTP_ERROR_RESPONSE_505 		= 29
,	TB_HTTP_ERROR_RESPONSE_506 		= 30
,	TB_HTTP_ERROR_RESPONSE_507 		= 31
,	TB_HTTP_ERROR_RESPONSE_UNK 		= 32
,	TB_HTTP_ERROR_RESPONSE_NUL 		= 33
, 	TB_HTTP_ERROR_UNKNOWN 			= 34
,	TB_HTTP_ERROR_DNS_FAILED 		= 35
,	TB_HTTP_ERROR_SSL_FAILED 		= 36
,	TB_HTTP_ERROR_CONNECT_FAILED	= 37
,	TB_HTTP_ERROR_REQUEST_FAILED	= 38
,	TB_HTTP_ERROR_WAIT_FAILED		= 39

}tb_http_error_e;

/// the http range type
typedef struct __tb_http_range_t
{
	/// the begin offset
	tb_hize_t 			bof;

	/// the end offset
	tb_hize_t 			eof;

}tb_http_range_t;

/// the http ssl func type
typedef struct __tb_http_sfunc_t
{
	/// the init func
	tb_handle_t 		(*init)(tb_handle_t gst);

	/// the exit func
	tb_void_t 			(*exit)(tb_handle_t ssl);

	/// the read func
	tb_long_t 			(*read)(tb_handle_t ssl, tb_byte_t* data, tb_size_t size);

	/// the writ func
	tb_long_t 			(*writ)(tb_handle_t ssl, tb_byte_t const* data, tb_size_t size);

}tb_http_sfunc_t;

/// the http option type
typedef struct __tb_http_option_t
{
	/// the method
	tb_uint16_t 		method 		: 4;

	/// the max redirect
	tb_uint16_t 		rdtm 		: 9;

	/// the http version
	tb_uint16_t 		version 	: 1;

	/// is alive?
	tb_uint16_t 		balive 		: 1;

	/// is unzip for gzip encoding?
	tb_uint16_t 		bunzip 		: 1;

	/// the url
	tb_url_t 			url;

	/// the post size
	tb_hize_t 			post;

	/// the head hash
	tb_handle_t 		head;

	/// timeout: ms
	tb_size_t 			timeout;

	/// range
	tb_http_range_t 	range;

	/// the user data
	tb_pointer_t 		udata;

	/// the ssl func
	tb_http_sfunc_t 	sfunc;

	/// the head func
	tb_bool_t 			(*hfunc)(tb_handle_t http, tb_char_t const* line);

}tb_http_option_t;

// the http status type
typedef struct __tb_http_status_t
{
	/// the http code
	tb_uint16_t 		code 		: 10;

	/// the http version
	tb_uint16_t 		version 	: 1;

	/// is keep alive?
	tb_uint16_t 		balive		: 1;

	/// be able to seek?
	tb_uint16_t 		bseeked		: 1;

	/// is chunked?
	tb_uint16_t 		bchunked	: 1;

	/// is gzip?
	tb_uint16_t 		bgzip		: 1;

	/// is deflate?
	tb_uint16_t 		bdeflate	: 1;

	/// the error
	tb_size_t 			error;

	/// the post size
	tb_hize_t 			post_size;

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
tb_long_t 				tb_http_aclose(tb_handle_t handle);

/*! block close, allow multiple called
 *
 * @param handle 		the http handle
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_http_bclose(tb_handle_t handle);

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
 *
 * @return 				the http option
 */
tb_http_option_t* 		tb_http_option(tb_handle_t handle);

/*! dump the http option
 *
 * @param handle 		the http handle
 */
tb_void_t 				tb_http_option_dump(tb_handle_t handle);

/*! the http status
 *
 * @param handle 		the http handle
 *
 * @return 				the http status
 */
tb_http_status_t const*	tb_http_status(tb_handle_t handle);

/*! dump the http status
 *
 * @param handle 		the http handle
 */
tb_void_t 				tb_http_status_dump(tb_handle_t handle);

#endif

