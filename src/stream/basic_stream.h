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
 * @file		basic_stream.h
 * @ingroup 	stream
 *
 */
#ifndef TB_STREAM_BASIC_STREAM_H
#define TB_STREAM_BASIC_STREAM_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../asio/asio.h"
#include "../libc/libc.h"
#include "../network/url.h"
#include "../memory/memory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the stream block maxn
#define TB_BASIC_STREAM_BLOCK_MAXN 					(8192)

// the stream bitops
#ifdef TB_WORDS_BIGENDIAN
# 	define tb_basic_stream_bread_u16_ne(stream) 		tb_basic_stream_bread_u16_be(stream)
# 	define tb_basic_stream_bread_s16_ne(stream) 		tb_basic_stream_bread_s16_be(stream)
# 	define tb_basic_stream_bread_u24_ne(stream) 		tb_basic_stream_bread_u24_be(stream)
# 	define tb_basic_stream_bread_s24_ne(stream) 		tb_basic_stream_bread_s24_be(stream)
# 	define tb_basic_stream_bread_u32_ne(stream) 		tb_basic_stream_bread_u32_be(stream)
# 	define tb_basic_stream_bread_s32_ne(stream) 		tb_basic_stream_bread_s32_be(stream)
# 	define tb_basic_stream_bread_u64_ne(stream) 		tb_basic_stream_bread_u64_be(stream)
# 	define tb_basic_stream_bread_s64_ne(stream) 		tb_basic_stream_bread_s64_be(stream)

# 	define tb_basic_stream_bwrit_u16_ne(stream, val) 	tb_basic_stream_bwrit_u16_be(stream, val)
# 	define tb_basic_stream_bwrit_s16_ne(stream, val)	tb_basic_stream_bwrit_s16_be(stream, val)
# 	define tb_basic_stream_bwrit_u24_ne(stream, val) 	tb_basic_stream_bwrit_u24_be(stream, val)
# 	define tb_basic_stream_bwrit_s24_ne(stream, val)	tb_basic_stream_bwrit_s24_be(stream, val)
# 	define tb_basic_stream_bwrit_u32_ne(stream, val)	tb_basic_stream_bwrit_u32_be(stream, val)
# 	define tb_basic_stream_bwrit_s32_ne(stream, val) 	tb_basic_stream_bwrit_s32_be(stream, val)
# 	define tb_basic_stream_bwrit_u64_ne(stream, val)	tb_basic_stream_bwrit_u64_be(stream, val)
# 	define tb_basic_stream_bwrit_s64_ne(stream, val) 	tb_basic_stream_bwrit_s64_be(stream, val)

#else
# 	define tb_basic_stream_bread_u16_ne(stream) 		tb_basic_stream_bread_u16_le(stream)
# 	define tb_basic_stream_bread_s16_ne(stream) 		tb_basic_stream_bread_s16_le(stream)
# 	define tb_basic_stream_bread_u24_ne(stream) 		tb_basic_stream_bread_u24_le(stream)
# 	define tb_basic_stream_bread_s24_ne(stream) 		tb_basic_stream_bread_s24_le(stream)
# 	define tb_basic_stream_bread_u32_ne(stream) 		tb_basic_stream_bread_u32_le(stream)
# 	define tb_basic_stream_bread_s32_ne(stream) 		tb_basic_stream_bread_s32_le(stream)
# 	define tb_basic_stream_bread_u64_ne(stream) 		tb_basic_stream_bread_u64_le(stream)
# 	define tb_basic_stream_bread_s64_ne(stream) 		tb_basic_stream_bread_s64_le(stream)

# 	define tb_basic_stream_bwrit_u16_ne(stream, val) 	tb_basic_stream_bwrit_u16_le(stream, val)
# 	define tb_basic_stream_bwrit_s16_ne(stream, val)	tb_basic_stream_bwrit_s16_le(stream, val)
# 	define tb_basic_stream_bwrit_u24_ne(stream, val) 	tb_basic_stream_bwrit_u24_le(stream, val)
# 	define tb_basic_stream_bwrit_s24_ne(stream, val)	tb_basic_stream_bwrit_s24_le(stream, val)
# 	define tb_basic_stream_bwrit_u32_ne(stream, val)	tb_basic_stream_bwrit_u32_le(stream, val)
# 	define tb_basic_stream_bwrit_s32_ne(stream, val) 	tb_basic_stream_bwrit_s32_le(stream, val)
# 	define tb_basic_stream_bwrit_u64_ne(stream, val)	tb_basic_stream_bwrit_u64_le(stream, val)
# 	define tb_basic_stream_bwrit_s64_ne(stream, val) 	tb_basic_stream_bwrit_s64_le(stream, val)

#endif

#ifdef TB_CONFIG_TYPE_FLOAT
# 	ifdef TB_FLOAT_BIGENDIAN
# 		define tb_basic_stream_bread_double_nbe(stream) 		tb_basic_stream_bread_double_bbe(stream)
# 		define tb_basic_stream_bread_double_nle(stream) 		tb_basic_stream_bread_double_ble(stream)

# 		define tb_basic_stream_bwrit_double_nbe(stream, val) 	tb_basic_stream_bwrit_double_bbe(stream, val)
# 		define tb_basic_stream_bwrit_double_nle(stream, val) 	tb_basic_stream_bwrit_double_ble(stream, val)
# 	else
# 		define tb_basic_stream_bread_double_nbe(stream) 		tb_basic_stream_bread_double_lbe(stream)
# 		define tb_basic_stream_bread_double_nle(stream) 		tb_basic_stream_bread_double_lle(stream)

# 		define tb_basic_stream_bwrit_double_nbe(stream, val) 	tb_basic_stream_bwrit_double_lbe(stream, val)
# 		define tb_basic_stream_bwrit_double_nle(stream, val) 	tb_basic_stream_bwrit_double_lle(stream, val)
# 	endif
# 	ifdef TB_WORDS_BIGENDIAN
# 		define tb_basic_stream_bread_float_ne(stream) 			tb_basic_stream_bread_float_be(stream)
# 		define tb_basic_stream_bwrit_float_ne(stream, val) 		tb_basic_stream_bwrit_float_be(stream, val)

# 		define tb_basic_stream_bread_double_nne(stream) 		tb_basic_stream_bread_double_nbe(stream)
# 		define tb_basic_stream_bread_double_bne(stream) 		tb_basic_stream_bread_double_bbe(stream)
# 		define tb_basic_stream_bread_double_lne(stream) 		tb_basic_stream_bread_double_lbe(stream)

# 		define tb_basic_stream_bwrit_double_nne(stream, val) 	tb_basic_stream_bwrit_double_nbe(stream, val)
# 		define tb_basic_stream_bwrit_double_bne(stream, val) 	tb_basic_stream_bwrit_double_bbe(stream, val)
# 		define tb_basic_stream_bwrit_double_lne(stream, val) 	tb_basic_stream_bwrit_double_lbe(stream, val)
# 	else
# 		define tb_basic_stream_bread_float_ne(stream) 			tb_basic_stream_bread_float_le(stream)
# 		define tb_basic_stream_bwrit_float_ne(stream, val) 		tb_basic_stream_bwrit_float_le(stream, val)

# 		define tb_basic_stream_bread_double_nne(stream) 		tb_basic_stream_bread_double_nle(stream)
# 		define tb_basic_stream_bread_double_bne(stream) 		tb_basic_stream_bread_double_ble(stream)
# 		define tb_basic_stream_bread_double_lne(stream) 		tb_basic_stream_bread_double_lle(stream)

# 		define tb_basic_stream_bwrit_double_nne(stream, val) 	tb_basic_stream_bwrit_double_nle(stream, val)
# 		define tb_basic_stream_bwrit_double_bne(stream, val) 	tb_basic_stream_bwrit_double_ble(stream, val)
# 		define tb_basic_stream_bwrit_double_lne(stream, val) 	tb_basic_stream_bwrit_double_lle(stream, val)
# 	endif
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the stream wait enum
typedef enum __tb_basic_stream_wait_e
{
 	TB_BASIC_STREAM_WAIT_NONE 					= TB_AIOE_CODE_NONE
, 	TB_BASIC_STREAM_WAIT_READ 					= TB_AIOE_CODE_RECV
, 	TB_BASIC_STREAM_WAIT_WRIT 					= TB_AIOE_CODE_SEND
, 	TB_BASIC_STREAM_WAIT_EALL 					= TB_AIOE_CODE_EALL

}tb_basic_stream_wait_e;

/// the generic stream type
typedef struct __tb_basic_stream_t
{	
	/// the base
	tb_stream_t 		base;

	/// the state
	tb_size_t 			state;

	/// the offset
	tb_hize_t 			offset;

	/// is writed?
	tb_uint8_t 			bwrited;

	/// the cache
	tb_queue_buffer_t 	cache;

	/// wait 
	tb_long_t 			(*wait)(tb_handle_t stream, tb_size_t wait, tb_long_t timeout);

	/// open
	tb_bool_t 			(*open)(tb_handle_t stream);

	/// clos
	tb_bool_t 			(*clos)(tb_handle_t stream);

	/// read
	tb_long_t 			(*read)(tb_handle_t stream, tb_byte_t* data, tb_size_t size);

	/// writ
	tb_long_t 			(*writ)(tb_handle_t stream, tb_byte_t const* data, tb_size_t size);

	/// seek
	tb_bool_t 			(*seek)(tb_handle_t stream, tb_hize_t offset);

	/// sync
	tb_bool_t 			(*sync)(tb_handle_t stream, tb_bool_t bclosing);

	/// exit
	tb_void_t 			(*exit)(tb_handle_t stream);

}tb_basic_stream_t;


/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init data stream 
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_data(tb_noarg_t);

/*! init file stream 
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_file(tb_noarg_t);

/*! init sock stream 
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_sock(tb_noarg_t);

/*! init http stream 
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_http(tb_noarg_t);

/*! init filter stream 
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_filter(tb_noarg_t);

/*! exit stream
 *
 * @param stream 		the stream
 */
tb_void_t 				tb_basic_stream_exit(tb_basic_stream_t* stream);

/*! init stream from url
 *
 * @param url 			the url
 * <pre>
 * data://base64
 * file://path or unix path: e.g. /root/xxxx/file
 * sock://host:port?tcp=
 * sock://host:port?udp=
 * socks://host:port
 * http://host:port/path?arg0=&arg1=...
 * https://host:port/path?arg0=&arg1=...
 * </pre>
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_from_url(tb_char_t const* url);

/*! init stream from data
 *
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_from_data(tb_byte_t const* data, tb_size_t size);

/*! init stream from file
 *
 * @param path 			the file path
 * @param mode 			the file mode, using the default ro mode if zero
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_from_file(tb_char_t const* path, tb_size_t mode);

/*! init stream from sock
 *
 * @param host 			the host
 * @param port 			the port
 * @param type 			the socket type, tcp or udp
 * @param bssl 			enable ssl?
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_from_sock(tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl);

/*! init stream from http or https
 *
 * @param host 			the host
 * @param port 			the port
 * @param path 			the path
 * @param bssl 			enable ssl?
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_from_http(tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl);

/*! init filter stream from null
 *
 * @param stream 	the stream
 *
 * @return 			the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_filter_from_null(tb_basic_stream_t* stream);

/*! init filter stream from zip
 *
 * @param stream 		the stream
 * @param algo 			the zip algorithm
 * @param action 		the zip action
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_filter_from_zip(tb_basic_stream_t* stream, tb_size_t algo, tb_size_t action);

/*! init filter stream from cache
 *
 * @param stream 		the stream
 * @param size 			the initial cache size, using the default size if be zero
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_filter_from_cache(tb_basic_stream_t* stream, tb_size_t size);

/*! init filter stream from charset
 *
 * @param stream 		the stream
 * @param fr 			the from charset
 * @param to 			the to charset
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_filter_from_charset(tb_basic_stream_t* stream, tb_size_t fr, tb_size_t to);

/*! init filter stream from chunked
 *
 * @param stream 		the stream
 * @param dechunked 	decode the chunked data?
 *
 * @return 				the stream
 */
tb_basic_stream_t* 		tb_basic_stream_init_filter_from_chunked(tb_basic_stream_t* stream, tb_bool_t dechunked);

/*! wait stream 
 *
 * blocking wait the single event object, so need not aiop 
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param stream 		the stream 
 * @param wait 			the wait type
 * @param timeout 		the timeout value, return immediately if 0, infinity if -1
 *
 * @return 				the event type, return 0 if timeout, return -1 if error
 */
tb_long_t 				tb_basic_stream_wait(tb_basic_stream_t* stream, tb_size_t wait, tb_long_t timeout);

/*! the state
 *
 * @param stream 		the stream
 *
 * @return 				the state
 */
tb_size_t 				tb_basic_stream_state(tb_basic_stream_t* stream);

/*! open stream
 *
 * @param stream 		the stream
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_open(tb_basic_stream_t* stream);

/*! close stream
 *
 * @param stream 		the stream
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_clos(tb_basic_stream_t* stream);

/*! read data, non-blocking
 *
 * @param stream 		the stream
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				the real size or -1
 */
tb_long_t 				tb_basic_stream_read(tb_basic_stream_t* stream, tb_byte_t* data, tb_size_t size);

/*! writ data, non-blocking
 *
 * @param stream 		the stream
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				the real size or -1
 */
tb_long_t 				tb_basic_stream_writ(tb_basic_stream_t* stream, tb_byte_t const* data, tb_size_t size);

/*! block read
 *
 * @param stream 		the stream
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bread(tb_basic_stream_t* stream, tb_byte_t* data, tb_size_t size);

/*! block writ
 *
 * @param stream 		the stream
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit(tb_basic_stream_t* stream, tb_byte_t const* data, tb_size_t size);

/*! sync stream
 *
 * @param stream 		the stream
 * @param bclosing 		is closing?
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_sync(tb_basic_stream_t* stream, tb_bool_t bclosing);

/*! need stream
 *
 * @param stream 		the stream
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_need(tb_basic_stream_t* stream, tb_byte_t** data, tb_size_t size);

/*! seek stream
 *
 * @param stream 		the stream
 * @param offset 		the offset
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_seek(tb_basic_stream_t* stream, tb_hize_t offset);

/*! skip stream
 *
 * @param stream 		the stream
 * @param size 			the size
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_skip(tb_basic_stream_t* stream, tb_hize_t size);

/*! block writ format data
 *
 * @param stream 		the stream
 * @param fmt 			the format
 *
 * @return 				the real size, failed: -1
 */
tb_long_t 				tb_basic_stream_printf(tb_basic_stream_t* stream, tb_char_t const* fmt, ...);

/*! block read line 
 *
 * @param stream 		the stream
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				the real size
 */
tb_long_t 				tb_basic_stream_bread_line(tb_basic_stream_t* stream, tb_char_t* data, tb_size_t size);

/*! block writ line 
 *
 * @param stream 		the stream
 * @param data 			the data
 * @param size 			the size
 *
 * @return 				the real size
 */
tb_long_t 				tb_basic_stream_bwrit_line(tb_basic_stream_t* stream, tb_char_t* data, tb_size_t size);

/*! block read uint8 integer
 *
 * @param stream 		the stream
 *
 * @return 				the uint8 integer
 */
tb_uint8_t 				tb_basic_stream_bread_u8(tb_basic_stream_t* stream);

/*! block read sint8 integer
 *
 * @param stream 		the stream
 *
 * @return 				the sint8 integer
 */
tb_sint8_t 				tb_basic_stream_bread_s8(tb_basic_stream_t* stream);

/*! block read uint16-le integer
 *
 * @param stream 		the stream
 *
 * @return 				the uint16-le integer
 */
tb_uint16_t 			tb_basic_stream_bread_u16_le(tb_basic_stream_t* stream);

/*! block read sint16-le integer
 *
 * @param stream 		the stream
 *
 * @return 				the sint16-le integer
 */
tb_sint16_t 			tb_basic_stream_bread_s16_le(tb_basic_stream_t* stream);

/*! block read uint24-le integer
 *
 * @param stream 		the stream
 *
 * @return 				the uint24-le integer
 */
tb_uint32_t 			tb_basic_stream_bread_u24_le(tb_basic_stream_t* stream);

/*! block read sint24-le integer
 *
 * @param stream 		the stream
 *
 * @return 				the sint24-le integer
 */
tb_sint32_t 			tb_basic_stream_bread_s24_le(tb_basic_stream_t* stream);

/*! block read uint32-le integer
 *
 * @param stream 		the stream
 *
 * @return 				the uint32-le integer
 */
tb_uint32_t 			tb_basic_stream_bread_u32_le(tb_basic_stream_t* stream);

/*! block read sint32-le integer
 *
 * @param stream 		the stream
 *
 * @return 				the sint32-le integer
 */
tb_sint32_t 			tb_basic_stream_bread_s32_le(tb_basic_stream_t* stream);

/*! block read uint64-le integer
 *
 * @param stream 		the stream
 *
 * @return 				the uint64-le integer
 */
tb_uint64_t 			tb_basic_stream_bread_u64_le(tb_basic_stream_t* stream);

/*! block read sint64-le integer
 *
 * @param stream 		the stream
 *
 * @return 				the sint64-le integer
 */
tb_sint64_t 			tb_basic_stream_bread_s64_le(tb_basic_stream_t* stream);

/*! block read uint16-be integer
 *
 * @param stream 		the stream
 *
 * @return 				the uint16-be integer
 */
tb_uint16_t 			tb_basic_stream_bread_u16_be(tb_basic_stream_t* stream);

/*! block read sint16-be integer
 *
 * @param stream 		the stream
 *
 * @return 				the sint16-be integer
 */
tb_sint16_t 			tb_basic_stream_bread_s16_be(tb_basic_stream_t* stream);

/*! block read uint24-be integer
 *
 * @param stream 		the stream
 *
 * @return 				the uint24-be integer
 */
tb_uint32_t 			tb_basic_stream_bread_u24_be(tb_basic_stream_t* stream);

/*! block read sint24-be integer
 *
 * @param stream 		the stream
 *
 * @return 				the sint24-be integer
 */
tb_sint32_t 			tb_basic_stream_bread_s24_be(tb_basic_stream_t* stream);

/*! block read uint32-be integer
 *
 * @param stream 		the stream
 *
 * @return 				the uint32-be integer
 */
tb_uint32_t 			tb_basic_stream_bread_u32_be(tb_basic_stream_t* stream);

/*! block read sint32-be integer
 *
 * @param stream 		the stream
 *
 * @return 				the sint32-be integer
 */
tb_sint32_t 			tb_basic_stream_bread_s32_be(tb_basic_stream_t* stream);

/*! block read uint64-be integer
 *
 * @param stream 		the stream
 *
 * @return 				the uint64-be integer
 */
tb_uint64_t 			tb_basic_stream_bread_u64_be(tb_basic_stream_t* stream);

/*! block read sint64-be integer
 *
 * @param stream 		the stream
 *
 * @return 				the sint64-be integer
 */
tb_sint64_t 			tb_basic_stream_bread_s64_be(tb_basic_stream_t* stream);

/*! block writ uint8 integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t				tb_basic_stream_bwrit_u8(tb_basic_stream_t* stream, tb_uint8_t val);

/*! block writ sint8 integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_s8(tb_basic_stream_t* stream, tb_sint8_t val);

/*! block writ uint16-le integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_u16_le(tb_basic_stream_t* stream, tb_uint16_t val);

/*! block writ sint16-le integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_s16_le(tb_basic_stream_t* stream, tb_sint16_t val);

/*! block writ uint24-le integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_u24_le(tb_basic_stream_t* stream, tb_uint32_t val);

/*! block writ sint24-le integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_s24_le(tb_basic_stream_t* stream, tb_sint32_t val);

/*! block writ uint32-le integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_u32_le(tb_basic_stream_t* stream, tb_uint32_t val);

/*! block writ sint32-le integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_s32_le(tb_basic_stream_t* stream, tb_sint32_t val);

/*! block writ uint64-le integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_u64_le(tb_basic_stream_t* stream, tb_uint64_t val);

/*! block writ sint64-le integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_s64_le(tb_basic_stream_t* stream, tb_sint64_t val);

/*! block writ uint16-be integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_u16_be(tb_basic_stream_t* stream, tb_uint16_t val);

/*! block writ sint16-be integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_s16_be(tb_basic_stream_t* stream, tb_sint16_t val);

/*! block writ uint24-be integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_u24_be(tb_basic_stream_t* stream, tb_uint32_t val);

/*! block writ sint24-be integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_s24_be(tb_basic_stream_t* stream, tb_sint32_t val);

/*! block writ uint32-be integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_u32_be(tb_basic_stream_t* stream, tb_uint32_t val);

/*! block writ sint32-be integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_s32_be(tb_basic_stream_t* stream, tb_sint32_t val);

/*! block writ uint64-be integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_u64_be(tb_basic_stream_t* stream, tb_uint64_t val);

/*! block writ sint64-be integer
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_s64_be(tb_basic_stream_t* stream, tb_sint64_t val);

#ifdef TB_CONFIG_TYPE_FLOAT

/*! read float-le number
 *
 * @param stream 		the stream
 *
 * @return 				the float-le number
 */
tb_float_t 				tb_basic_stream_bread_float_le(tb_basic_stream_t* stream);

/*! read float-be number
 *
 * @param stream 		the stream
 *
 * @return 				the float-be number
 */
tb_float_t 				tb_basic_stream_bread_float_be(tb_basic_stream_t* stream);

/*! read double-ble number
 *
 * @param stream 		the stream
 *
 * @return 				the double-ble number
 */
tb_double_t 			tb_basic_stream_bread_double_ble(tb_basic_stream_t* stream);

/*! read double-bbe number
 *
 * @param stream 		the stream
 *
 * @return 				the double-bbe number
 */
tb_double_t 			tb_basic_stream_bread_double_bbe(tb_basic_stream_t* stream);

/*! read double-lle number
 *
 * @param stream 		the stream
 *
 * @return 				the double-lle number
 */
tb_double_t 			tb_basic_stream_bread_double_lle(tb_basic_stream_t* stream);

/*! read double-lbe number
 *
 * @param stream 		the stream
 *
 * @return 				the double-lbe number
 */
tb_double_t 			tb_basic_stream_bread_double_lbe(tb_basic_stream_t* stream);

/*! writ float-le number
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_float_le(tb_basic_stream_t* stream, tb_float_t val);

/*! writ float-be number
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_float_be(tb_basic_stream_t* stream, tb_float_t val);

/*! writ double-ble number
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_double_ble(tb_basic_stream_t* stream, tb_double_t val);

/*! writ double-bbe number
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_double_bbe(tb_basic_stream_t* stream, tb_double_t val);

/*! writ double-lle number
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_double_lle(tb_basic_stream_t* stream, tb_double_t val);

/*! writ double-lbe number
 *
 * @param stream 		the stream
 * @param val 			the value
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_basic_stream_bwrit_double_lbe(tb_basic_stream_t* stream, tb_double_t val);

#endif

#endif
