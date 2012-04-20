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
 	TB_HTTP_VERSION_10 		= 0
, 	TB_HTTP_VERSION_11 		= 1

}tb_http_version_t;

// the http seek type
typedef enum __tb_http_seek_t
{
 	TB_HTTP_SEEK_BEG 		= 0
, 	TB_HTTP_SEEK_CUR 		= 1
, 	TB_HTTP_SEEK_END 		= 2

}tb_http_seek_t;

// the http range type
typedef struct __tb_http_range_t
{
	// range
	tb_hize_t 		bof;
	tb_hize_t 		eof;

}tb_http_range_t;

// the http option type
typedef struct __tb_http_option_t
{
	// the method
	tb_uint16_t 		method 		: 4;

	// the max redirect
	tb_uint16_t 		rdtm 		: 9;

	// the http version
	tb_uint16_t 		version 	: 1;

	// is alive?
	tb_uint16_t 		balive 		: 1;

	// is chunked?
	tb_uint16_t 		bchunked 	: 1;

	// the url
	tb_url_t 			url;

	// the post data
	tb_pstring_t 		post;

	// the head hash
	tb_hash_t* 			head;

	// timeout: ms
	tb_size_t 			timeout;

	// range
	tb_http_range_t 	range;

	// the user data
	tb_pointer_t 		udata;

	// the head func
	tb_bool_t 			(*hfunc)(tb_handle_t http, tb_char_t const* line);

}tb_http_option_t;

// the http status type
typedef struct __tb_http_status_t
{
	// the http code
	tb_uint16_t 		code 		: 10;

	// the http version
	tb_uint16_t 		version 	: 1;

	// is keep alive?
	tb_uint16_t 		balive		: 1;

	// be able to seek?
	tb_uint16_t 		bseeked		: 1;

	// is chunked?
	tb_uint16_t 		bchunked	: 1;

	// the document size
	tb_hize_t 			document_size;

	// the current content size, maybe in range
	tb_hize_t 			content_size;

	// the content type
	tb_pstring_t 		content_type;

	// the location
	tb_pstring_t 		location;

}tb_http_status_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_handle_t 			tb_http_init();
tb_void_t 				tb_http_exit(tb_handle_t handle);

/*!wait the http 
 *
 * blocking wait the single event object, so need not aipp 
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param 	handle 	the http handle 
 * @param 	etype 	the waited event type, return the needed event type if TB_AIOO_ETYPE_NULL
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event type, return 0 if timeout, return -1 if error
 */
tb_long_t 				tb_http_wait(tb_handle_t handle, tb_size_t etype, tb_long_t timeout);

// async open, allow multiple called before closing 
tb_long_t 				tb_http_aopen(tb_handle_t handle);

// block open, allow multiple called before closing 
tb_bool_t 				tb_http_bopen(tb_handle_t handle);

// async close, allow multiple called
tb_long_t 				tb_http_aclose(tb_handle_t handle);

// block close, allow multiple called
tb_bool_t 				tb_http_bclose(tb_handle_t handle);

// seek
tb_long_t 				tb_http_aseek(tb_handle_t handle, tb_hize_t offset);
tb_bool_t 				tb_http_bseek(tb_handle_t handle, tb_hize_t offset);

// async writ & read
tb_long_t 				tb_http_awrit(tb_handle_t handle, tb_byte_t* data, tb_size_t size);
tb_long_t 				tb_http_aread(tb_handle_t handle, tb_byte_t* data, tb_size_t size);

// block writ & read
tb_bool_t 				tb_http_bwrit(tb_handle_t handle, tb_byte_t* data, tb_size_t size);
tb_bool_t 				tb_http_bread(tb_handle_t handle, tb_byte_t* data, tb_size_t size);

// option
tb_http_option_t* 		tb_http_option(tb_handle_t handle);
tb_void_t 				tb_http_option_dump(tb_handle_t handle);

// status
tb_http_status_t const*	tb_http_status(tb_handle_t handle);
tb_void_t 				tb_http_status_dump(tb_handle_t handle);

#endif

