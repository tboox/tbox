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
 * @file		gstream.h
 * @ingroup 	stream
 *
 */
#ifndef TB_STREAM_GSTREAM_H
#define TB_STREAM_GSTREAM_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../asio/asio.h"
#include "../libc/libc.h"
#include "../network/url.h"
#include "../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the stream command
#define TB_GSTREAM_CTRL(type, ctrl) 			(((type) << 16) | (ctrl))
#define TB_GSTREAM_CTRL_FLTR(type, ctrl) 		TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_FLTR, (((type) << 8) | (ctrl)))

// the stream state
#define TB_GSTREAM_STATE(type, state) 			(((type) << 16) | (state))

// the stream block maxn
#define TB_GSTREAM_BLOCK_MAXN 					(8192)

// the stream bitops
#ifdef TB_WORDS_BIGENDIAN
# 	define tb_gstream_bread_u16_ne(gstream) 		tb_gstream_bread_u16_be(gstream)
# 	define tb_gstream_bread_s16_ne(gstream) 		tb_gstream_bread_s16_be(gstream)
# 	define tb_gstream_bread_u24_ne(gstream) 		tb_gstream_bread_u24_be(gstream)
# 	define tb_gstream_bread_s24_ne(gstream) 		tb_gstream_bread_s24_be(gstream)
# 	define tb_gstream_bread_u32_ne(gstream) 		tb_gstream_bread_u32_be(gstream)
# 	define tb_gstream_bread_s32_ne(gstream) 		tb_gstream_bread_s32_be(gstream)
# 	define tb_gstream_bread_u64_ne(gstream) 		tb_gstream_bread_u64_be(gstream)
# 	define tb_gstream_bread_s64_ne(gstream) 		tb_gstream_bread_s64_be(gstream)

# 	define tb_gstream_bwrit_u16_ne(gstream, val) 	tb_gstream_bwrit_u16_be(gstream, val)
# 	define tb_gstream_bwrit_s16_ne(gstream, val)	tb_gstream_bwrit_s16_be(gstream, val)
# 	define tb_gstream_bwrit_u24_ne(gstream, val) 	tb_gstream_bwrit_u24_be(gstream, val)
# 	define tb_gstream_bwrit_s24_ne(gstream, val)	tb_gstream_bwrit_s24_be(gstream, val)
# 	define tb_gstream_bwrit_u32_ne(gstream, val)	tb_gstream_bwrit_u32_be(gstream, val)
# 	define tb_gstream_bwrit_s32_ne(gstream, val) 	tb_gstream_bwrit_s32_be(gstream, val)
# 	define tb_gstream_bwrit_u64_ne(gstream, val)	tb_gstream_bwrit_u64_be(gstream, val)
# 	define tb_gstream_bwrit_s64_ne(gstream, val) 	tb_gstream_bwrit_s64_be(gstream, val)

#else
# 	define tb_gstream_bread_u16_ne(gstream) 		tb_gstream_bread_u16_le(gstream)
# 	define tb_gstream_bread_s16_ne(gstream) 		tb_gstream_bread_s16_le(gstream)
# 	define tb_gstream_bread_u24_ne(gstream) 		tb_gstream_bread_u24_le(gstream)
# 	define tb_gstream_bread_s24_ne(gstream) 		tb_gstream_bread_s24_le(gstream)
# 	define tb_gstream_bread_u32_ne(gstream) 		tb_gstream_bread_u32_le(gstream)
# 	define tb_gstream_bread_s32_ne(gstream) 		tb_gstream_bread_s32_le(gstream)
# 	define tb_gstream_bread_u64_ne(gstream) 		tb_gstream_bread_u64_le(gstream)
# 	define tb_gstream_bread_s64_ne(gstream) 		tb_gstream_bread_s64_le(gstream)

# 	define tb_gstream_bwrit_u16_ne(gstream, val) 	tb_gstream_bwrit_u16_le(gstream, val)
# 	define tb_gstream_bwrit_s16_ne(gstream, val)	tb_gstream_bwrit_s16_le(gstream, val)
# 	define tb_gstream_bwrit_u24_ne(gstream, val) 	tb_gstream_bwrit_u24_le(gstream, val)
# 	define tb_gstream_bwrit_s24_ne(gstream, val)	tb_gstream_bwrit_s24_le(gstream, val)
# 	define tb_gstream_bwrit_u32_ne(gstream, val)	tb_gstream_bwrit_u32_le(gstream, val)
# 	define tb_gstream_bwrit_s32_ne(gstream, val) 	tb_gstream_bwrit_s32_le(gstream, val)
# 	define tb_gstream_bwrit_u64_ne(gstream, val)	tb_gstream_bwrit_u64_le(gstream, val)
# 	define tb_gstream_bwrit_s64_ne(gstream, val) 	tb_gstream_bwrit_s64_le(gstream, val)

#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the gstream wait enum
typedef enum __tb_gstream_wait_e
{
 	TB_GSTREAM_WAIT_NONE 					= TB_AIOE_CODE_NONE
, 	TB_GSTREAM_WAIT_READ 					= TB_AIOE_CODE_RECV
, 	TB_GSTREAM_WAIT_WRIT 					= TB_AIOE_CODE_SEND
, 	TB_GSTREAM_WAIT_EALL 					= TB_AIOE_CODE_EALL

}tb_gstream_wait_e;

/// the gstream enum
typedef enum __tb_gstream_type_e
{
 	TB_GSTREAM_TYPE_NONE 					= 0
, 	TB_GSTREAM_TYPE_FILE 					= 1
, 	TB_GSTREAM_TYPE_SOCK 					= 2
, 	TB_GSTREAM_TYPE_HTTP 					= 3
, 	TB_GSTREAM_TYPE_DATA 					= 4
, 	TB_GSTREAM_TYPE_FLTR 					= 5

}tb_gstream_type_e;

/// the gstream ctrl enum
typedef enum __tb_gstream_ctrl_e
{
	TB_GSTREAM_CTRL_NONE 					= 0

	// the gstream
,	TB_GSTREAM_CTRL_GET_URL 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 1)
,	TB_GSTREAM_CTRL_GET_HOST 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 2)
,	TB_GSTREAM_CTRL_GET_PORT 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 3)
,	TB_GSTREAM_CTRL_GET_PATH 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 4)
,	TB_GSTREAM_CTRL_GET_SSL 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 5)
,	TB_GSTREAM_CTRL_GET_SFUNC 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 6)
,	TB_GSTREAM_CTRL_GET_CACHE 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 7)
,	TB_GSTREAM_CTRL_GET_TIMEOUT 			= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 8)
,	TB_GSTREAM_CTRL_IS_OPENED 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 9)

,	TB_GSTREAM_CTRL_SET_URL 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 11)
,	TB_GSTREAM_CTRL_SET_HOST 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 12)
,	TB_GSTREAM_CTRL_SET_PORT 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 13)
,	TB_GSTREAM_CTRL_SET_PATH 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 14)
,	TB_GSTREAM_CTRL_SET_SSL 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 15)
,	TB_GSTREAM_CTRL_SET_SFUNC 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 16)
,	TB_GSTREAM_CTRL_SET_CACHE 				= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 17)
,	TB_GSTREAM_CTRL_SET_TIMEOUT 			= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_NONE, 18)

	// the gstream for data
,	TB_GSTREAM_CTRL_DATA_SET_DATA 			= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_DATA, 1)

	// the gstream for file
,	TB_GSTREAM_CTRL_FILE_SET_MODE 			= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_FILE, 1)
,	TB_GSTREAM_CTRL_FILE_SET_HANDLE 		= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_FILE, 2)
,	TB_GSTREAM_CTRL_FILE_GET_HANDLE 		= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_FILE, 3)

	// the gstream for sock
,	TB_GSTREAM_CTRL_SOCK_SET_TYPE 			= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_SOCK, 1)
,	TB_GSTREAM_CTRL_SOCK_SET_HANDLE 		= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_SOCK, 2)
,	TB_GSTREAM_CTRL_SOCK_GET_HANDLE 		= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_SOCK, 3)

	// the gstream for http
,	TB_GSTREAM_CTRL_HTTP_GET_OPTION 		= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_HTTP, 1)
,	TB_GSTREAM_CTRL_HTTP_GET_STATUS 		= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_HTTP, 2)

	// the gstream for filter
,	TB_GSTREAM_CTRL_FLTR_GET_GSTREAM 		= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_FLTR, 1)
,	TB_GSTREAM_CTRL_FLTR_GET_FILTER 		= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_FLTR, 2)
,	TB_GSTREAM_CTRL_FLTR_SET_GSTREAM 		= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_FLTR, 3)
,	TB_GSTREAM_CTRL_FLTR_SET_FILTER 		= TB_GSTREAM_CTRL(TB_GSTREAM_TYPE_FLTR, 4)

}tb_gstream_ctrl_e;

/// the gstream state enum
typedef enum __tb_gstream_state_e
{
	TB_GSTREAM_STATE_OK 					= 0
,	TB_GSTREAM_STATE_UNKNOWN_ERROR 			= 1
,	TB_GSTREAM_STATE_WAIT_FAILED 			= 2
,	TB_GSTREAM_STATE_CLOSED 				= 3

,	TB_GSTREAM_SOCK_STATE_DNS_FAILED 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_SOCK, 1)
,	TB_GSTREAM_SOCK_STATE_CONNECT_FAILED	= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_SOCK, 2)
,	TB_GSTREAM_SOCK_STATE_SSL_FAILED		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_SOCK, 3)

,	TB_GSTREAM_HTTP_STATE_RESPONSE_204 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 1)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_300 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 2)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_301 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 3)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_302 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 4)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_303 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 5)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_304 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 6)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_400 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 7)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_401 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 8)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_402 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 9)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_403 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 10)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_404 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 11)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_405 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 12)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_406 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 13)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_407 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 14)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_408 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 15)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_409 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 16)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_410 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 17)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_411 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 18)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_412 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 19)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_413 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 20)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_414 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 21)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_415 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 22)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_416 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 23)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_500 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 24)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_501 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 25)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_502 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 26)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_503 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 27)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_504 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 28)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_505 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 29)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_506 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 30)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_507 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 31)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_UNK 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 32)
,	TB_GSTREAM_HTTP_STATE_RESPONSE_NUL 		= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 33)
,	TB_GSTREAM_HTTP_STATE_REQUEST_FAILED 	= TB_GSTREAM_STATE(TB_GSTREAM_TYPE_HTTP, 34)

}tb_gstream_state_e;

/// the gstream ssl func type
typedef struct __tb_gstream_sfunc_t
{
	/// the init func
	tb_handle_t 		(*init)(tb_handle_t gstream);

	/// the exit func
	tb_void_t 			(*exit)(tb_handle_t ssl);

	/// the read func
	tb_long_t 			(*read)(tb_handle_t ssl, tb_byte_t* data, tb_size_t size);

	/// the writ func
	tb_long_t 			(*writ)(tb_handle_t ssl, tb_byte_t const* data, tb_size_t size);

}tb_gstream_sfunc_t;

/// the generic stream type
typedef struct __tb_gstream_t
{	
	/// the type
	tb_uint32_t 		type 		: 8;

	/// is opened?
	tb_uint32_t 		bopened 	: 1;

	/// is writed?
	tb_uint32_t 		bwrited 	: 1;

	/// is cached?
	tb_uint32_t 		bcached 	: 1;

	/// the timeout: ms
	tb_uint32_t 		timeout 	: 21;

	/// the state
	tb_size_t 			state;

	/// the url
	tb_url_t 			url;

	/// the cache
	tb_qbuffer_t 		cache;

	/// the offset
	tb_hize_t 			offset;

	/// the ssl func
	tb_gstream_sfunc_t 	sfunc;

	/// wait 
	tb_long_t 			(*wait)(struct __tb_gstream_t* gstream, tb_size_t wait, tb_long_t timeout);

	/// open
	tb_long_t 			(*open)(struct __tb_gstream_t* gstream);

	/// clos
	tb_long_t 			(*clos)(struct __tb_gstream_t* gstream);

	/// read
	tb_long_t 			(*read)(struct __tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size, tb_bool_t sync);

	/// writ
	tb_long_t 			(*writ)(struct __tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size, tb_bool_t sync);

	/// seek
	tb_long_t 			(*seek)(struct __tb_gstream_t* gstream, tb_hize_t offset);

	/// size
	tb_hize_t 			(*size)(struct __tb_gstream_t* gstream);

	/// exit
	tb_void_t 			(*exit)(struct __tb_gstream_t* gstream);

	/// ctrl
	tb_bool_t 			(*ctrl)(struct __tb_gstream_t* gstream, tb_size_t ctrl, tb_va_list_t args);

}tb_gstream_t;


/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init data stream 
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_data(tb_noarg_t);

/*! init file stream 
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_file(tb_noarg_t);

/*! init sock stream 
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_sock(tb_noarg_t);

/*! init http stream 
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_http(tb_noarg_t);

/*! init filter stream 
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_filter(tb_noarg_t);

/*! exit stream
 *
 * @param gstream 	the stream
 */
tb_void_t 			tb_gstream_exit(tb_gstream_t* gstream);

/*! init stream from url
 *
 * @param url 		the url
 * <pre>
 * file://path or unix path: e.g. /root/xxxx/file
 * sock://host:port?tcp=
 * socks://host:port?udp=
 * http://host:port/path?arg0=&arg1=...
 * https://host:port/path?arg0=&arg1=...
 * </pre>
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_from_url(tb_char_t const* url);

/*! init stream from data
 *
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_from_data(tb_byte_t const* data, tb_size_t size);

/*! init stream from file
 *
 * @param path 		the file path
 * @param mode 		the file mode, using the default ro mode if zero
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_from_file(tb_char_t const* path, tb_size_t mode);

/*! init stream from sock
 *
 * @param host 		the host
 * @param port 		the port
 * @param type 		the socket type, tcp or udp
 * @param bssl 		enable ssl?
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_from_sock(tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl);

/*! init stream from http or https
 *
 * @param host 		the host
 * @param port 		the port
 * @param path 		the path
 * @param bssl 		enable ssl?
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_from_http(tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl);

/*! init filter stream from null
 *
 * @param gstream 	the stream
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_filter_from_null(tb_gstream_t* gstream);

/*! init filter stream from zip
 *
 * @param gstream 	the stream
 * @param algo 		the zip algorithom
 * @param action 	the zip action
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_filter_from_zip(tb_gstream_t* gstream, tb_size_t algo, tb_size_t action);

/*! init filter stream from charset
 *
 * @param gstream 	the stream
 * @param fr 		the from charset
 * @param to 		the to charset
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_filter_from_charset(tb_gstream_t* gstream, tb_size_t fr, tb_size_t to);

/*! init filter stream from chunked
 *
 * @param gstream 	the stream
 * @param dechunked decode the chunked data?
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_filter_from_chunked(tb_gstream_t* gstream, tb_bool_t dechunked);

/*! wait stream 
 *
 * blocking wait the single event object, so need not aiop 
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param gstream 		the gstream 
 * @param wait 		the wait type
 * @param timeout 	the timeout value, return immediately if 0, infinity if -1
 *
 * @return 			the event type, return 0 if timeout, return -1 if error
 */
tb_long_t 			tb_gstream_wait(tb_gstream_t* gstream, tb_size_t wait, tb_long_t timeout);

/*! the stream state
 *
 * @param gstream 	the stream
 *
 * @return 			the stream state
 */
tb_size_t 			tb_gstream_state(tb_gstream_t* gstream);

/*! the stream state string
 *
 * @param state 	the stream state
 *
 * @return 			the stream state string
 */
tb_char_t const* 	tb_gstream_state_cstr(tb_size_t state);

/*! the stream is end?
 *
 * @param gstream 	the stream
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_beof(tb_gstream_t* gstream);

/*! clear stream cache and reset status, but not close it
 *
 * @param gstream 	the stream
 */
tb_void_t 			tb_gstream_clear(tb_gstream_t* gstream);

/*! async open, allow multiple called before closing 
 *
 * @param gstream 	the stream
 *
 * @return 			ok: 1, continue: 0, failed: -1
 */
tb_long_t 			tb_gstream_aopen(tb_gstream_t* gstream);

/*! block open, allow multiple called before closing 
 *
 * @param gstream 	the stream
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bopen(tb_gstream_t* gstream);

/*! async close, allow multiple called
 *
 * @param gstream 	the stream
 *
 * @return 			ok: 1, continue: 0, failed: -1
 */
tb_long_t 			tb_gstream_aclos(tb_gstream_t* gstream);

/*! block close, allow multiple called
 *
 * @param gstream 	the stream
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bclos(tb_gstream_t* gstream);

/*! async read
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			ok: 1, continue: 0, failed: -1
 */
tb_long_t 			tb_gstream_aread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size);

/*! async writ
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			ok: 1, continue: 0, failed: -1
 */
tb_long_t 			tb_gstream_awrit(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size);

/*! block read
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size);

/*! block writ
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size);

/*! async flush read
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			ok: 1, continue: 0, failed: -1
 */
tb_long_t 			tb_gstream_afread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size);

/*! async flush writ
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			ok: 1, continue: 0, failed: -1
 */
tb_long_t 			tb_gstream_afwrit(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size);

/*! block flush read
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bfread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size);

/*! block flush writ
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bfwrit(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size);

/*! async need
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			ok: 1, continue: 0, failed: -1
 */
tb_long_t 			tb_gstream_aneed(tb_gstream_t* gstream, tb_byte_t** data, tb_size_t size);

/*! block need
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bneed(tb_gstream_t* gstream, tb_byte_t** data, tb_size_t size);

/*! async seek
 *
 * @param gstream 	the stream
 * @param offset 	the offset
 *
 * @return 			ok: 1, continue: 0, failed: -1
 */
tb_long_t 			tb_gstream_aseek(tb_gstream_t* gstream, tb_hize_t offset);

/*! block seek
 *
 * @param gstream 	the stream
 * @param offset 	the offset
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bseek(tb_gstream_t* gstream, tb_hize_t offset);

/*! async skip
 *
 * @param gstream 	the stream
 * @param size 		the size
 *
 * @return 			ok: 1, continue: 0, failed: -1
 */
tb_long_t 			tb_gstream_askip(tb_gstream_t* gstream, tb_hize_t size);

/*! block skip
 *
 * @param gstream 	the stream
 * @param size 		the size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bskip(tb_gstream_t* gstream, tb_hize_t size);

/*! block writ format data
 *
 * @param gstream 	the stream
 * @param fmt 		the format
 *
 * @return 			the real size
 */
tb_long_t 			tb_gstream_printf(tb_gstream_t* gstream, tb_char_t const* fmt, ...);

/*! block read line 
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size
 */
tb_long_t 			tb_gstream_bread_line(tb_gstream_t* gstream, tb_char_t* data, tb_size_t size);

/*! block writ line 
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size
 */
tb_long_t 			tb_gstream_bwrit_line(tb_gstream_t* gstream, tb_char_t* data, tb_size_t size);

/*! block read uint8 integer
 *
 * @param gstream 	the stream
 *
 * @return 			the uint8 integer
 */
tb_uint8_t 			tb_gstream_bread_u8(tb_gstream_t* gstream);

/*! block read sint8 integer
 *
 * @param gstream 	the stream
 *
 * @return 			the sint8 integer
 */
tb_sint8_t 			tb_gstream_bread_s8(tb_gstream_t* gstream);

/*! block read uint16-le integer
 *
 * @param gstream 	the stream
 *
 * @return 			the uint16-le integer
 */
tb_uint16_t 		tb_gstream_bread_u16_le(tb_gstream_t* gstream);

/*! block read sint16-le integer
 *
 * @param gstream 	the stream
 *
 * @return 			the sint16-le integer
 */
tb_sint16_t 		tb_gstream_bread_s16_le(tb_gstream_t* gstream);

/*! block read uint24-le integer
 *
 * @param gstream 	the stream
 *
 * @return 			the uint24-le integer
 */
tb_uint32_t 		tb_gstream_bread_u24_le(tb_gstream_t* gstream);

/*! block read sint24-le integer
 *
 * @param gstream 	the stream
 *
 * @return 			the sint24-le integer
 */
tb_sint32_t 		tb_gstream_bread_s24_le(tb_gstream_t* gstream);

/*! block read uint32-le integer
 *
 * @param gstream 	the stream
 *
 * @return 			the uint32-le integer
 */
tb_uint32_t 		tb_gstream_bread_u32_le(tb_gstream_t* gstream);

/*! block read sint32-le integer
 *
 * @param gstream 	the stream
 *
 * @return 			the sint32-le integer
 */
tb_sint32_t 		tb_gstream_bread_s32_le(tb_gstream_t* gstream);

/*! block read uint64-le integer
 *
 * @param gstream 	the stream
 *
 * @return 			the uint64-le integer
 */
tb_uint64_t 		tb_gstream_bread_u64_le(tb_gstream_t* gstream);

/*! block read sint64-le integer
 *
 * @param gstream 	the stream
 *
 * @return 			the sint64-le integer
 */
tb_sint64_t 		tb_gstream_bread_s64_le(tb_gstream_t* gstream);

/*! block read uint16-be integer
 *
 * @param gstream 	the stream
 *
 * @return 			the uint16-be integer
 */
tb_uint16_t 		tb_gstream_bread_u16_be(tb_gstream_t* gstream);

/*! block read sint16-be integer
 *
 * @param gstream 	the stream
 *
 * @return 			the sint16-be integer
 */
tb_sint16_t 		tb_gstream_bread_s16_be(tb_gstream_t* gstream);

/*! block read uint24-be integer
 *
 * @param gstream 	the stream
 *
 * @return 			the uint24-be integer
 */
tb_uint32_t 		tb_gstream_bread_u24_be(tb_gstream_t* gstream);

/*! block read sint24-be integer
 *
 * @param gstream 	the stream
 *
 * @return 			the sint24-be integer
 */
tb_sint32_t 		tb_gstream_bread_s24_be(tb_gstream_t* gstream);

/*! block read uint32-be integer
 *
 * @param gstream 	the stream
 *
 * @return 			the uint32-be integer
 */
tb_uint32_t 		tb_gstream_bread_u32_be(tb_gstream_t* gstream);

/*! block read sint32-be integer
 *
 * @param gstream 	the stream
 *
 * @return 			the sint32-be integer
 */
tb_sint32_t 		tb_gstream_bread_s32_be(tb_gstream_t* gstream);

/*! block read uint64-be integer
 *
 * @param gstream 	the stream
 *
 * @return 			the uint64-be integer
 */
tb_uint64_t 		tb_gstream_bread_u64_be(tb_gstream_t* gstream);

/*! block read sint64-be integer
 *
 * @param gstream 	the stream
 *
 * @return 			the sint64-be integer
 */
tb_sint64_t 		tb_gstream_bread_s64_be(tb_gstream_t* gstream);

/*! block writ uint8 integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t			tb_gstream_bwrit_u8(tb_gstream_t* gstream, tb_uint8_t val);

/*! block writ sint8 integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_s8(tb_gstream_t* gstream, tb_sint8_t val);

/*! block writ uint16-le integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_u16_le(tb_gstream_t* gstream, tb_uint16_t val);

/*! block writ sint16-le integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_s16_le(tb_gstream_t* gstream, tb_sint16_t val);

/*! block writ uint24-le integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_u24_le(tb_gstream_t* gstream, tb_uint32_t val);

/*! block writ sint24-le integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_s24_le(tb_gstream_t* gstream, tb_sint32_t val);

/*! block writ uint32-le integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_u32_le(tb_gstream_t* gstream, tb_uint32_t val);

/*! block writ sint32-le integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_s32_le(tb_gstream_t* gstream, tb_sint32_t val);

/*! block writ uint64-le integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_u64_le(tb_gstream_t* gstream, tb_uint64_t val);

/*! block writ sint64-le integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_s64_le(tb_gstream_t* gstream, tb_sint64_t val);

/*! block writ uint16-be integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_u16_be(tb_gstream_t* gstream, tb_uint16_t val);

/*! block writ sint16-be integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_s16_be(tb_gstream_t* gstream, tb_sint16_t val);

/*! block writ uint24-be integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_u24_be(tb_gstream_t* gstream, tb_uint32_t val);

/*! block writ sint24-be integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_s24_be(tb_gstream_t* gstream, tb_sint32_t val);

/*! block writ uint32-be integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_u32_be(tb_gstream_t* gstream, tb_uint32_t val);

/*! block writ sint32-be integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_s32_be(tb_gstream_t* gstream, tb_sint32_t val);

/*! block writ uint64-be integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_u64_be(tb_gstream_t* gstream, tb_uint64_t val);

/*! block writ sint64-be integer
 *
 * @param gstream 	the stream
 * @param val 		the value
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_bwrit_s64_be(tb_gstream_t* gstream, tb_sint64_t val);

/*! the stream type
 *
 * @param gstream 	the stream
 *
 * @return 			the stream type
 */
tb_size_t 			tb_gstream_type(tb_gstream_t const* gstream);

/*! the stream size
 *
 * @param gstream 	the stream
 *
 * @return 			the stream size
 */
tb_hize_t 			tb_gstream_size(tb_gstream_t const* gstream);

/*! the stream left size
 *
 * @param gstream 	the stream
 *
 * @return 			the stream left size
 */
tb_hize_t 			tb_gstream_left(tb_gstream_t const* gstream);

/*! the stream offset
 *
 * @param gstream 	the stream
 *
 * @return 			the stream offset
 */
tb_hize_t 			tb_gstream_offset(tb_gstream_t const* gstream);

/*! the stream timeout
 *
 * @param gstream 	the stream
 *
 * @return 			the stream timeout
 */
tb_size_t 			tb_gstream_timeout(tb_gstream_t const* gstream);

/*! ctrl stream
 *
 * @param gstream 	the stream
 * @param ctrl 		the ctrl command
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_ctrl(tb_gstream_t* gstream, tb_size_t ctrl, ...);

#endif

