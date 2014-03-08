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

/// the generic stream type
typedef struct __tb_gstream_t
{	
	/// the base
	tb_stream_t 		base;

	/// is writed?
	tb_uint8_t 			bwrited 	: 1;

	/// is cached?
	tb_uint8_t 			bcached 	: 1;

	/// the state
	tb_size_t 			state;

	/// the cache
	tb_qbuffer_t 		cache;

	/// the offset
	tb_hize_t 			offset;

	/// wait 
	tb_long_t 			(*wait)(tb_handle_t gstream, tb_size_t wait, tb_long_t timeout);

	/// open
	tb_bool_t 			(*open)(tb_handle_t gstream);

	/// clos
	tb_bool_t 			(*clos)(tb_handle_t gstream);

	/// read
	tb_long_t 			(*read)(tb_handle_t gstream, tb_byte_t* data, tb_size_t size);

	/// writ
	tb_long_t 			(*writ)(tb_handle_t gstream, tb_byte_t const* data, tb_size_t size);

	/// seek
	tb_bool_t 			(*seek)(tb_handle_t gstream, tb_hize_t offset);

	/// sync
	tb_bool_t 			(*sync)(tb_handle_t gstream, tb_bool_t bclosing);

	/// exit
	tb_void_t 			(*exit)(tb_handle_t gstream);

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
 * data://base64
 * file://path or unix path: e.g. /root/xxxx/file
 * sock://host:port?tcp=
 * sock://host:port?udp=
 * socks://host:port
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

/*! init filter stream from cache
 *
 * @param gstream 	the stream
 * @param size 		the initial cache size, using the default size if be zero
 *
 * @return 			the stream
 */
tb_gstream_t* 		tb_gstream_init_filter_from_cache(tb_gstream_t* gstream, tb_size_t size);

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

/*! open stream
 *
 * @param gstream 	the stream
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_open(tb_gstream_t* gstream);

/*! close stream
 *
 * @param gstream 	the stream
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_clos(tb_gstream_t* gstream);

/*! async read
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size or -1
 */
tb_long_t 			tb_gstream_aread(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size);

/*! async writ
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size or -1
 */
tb_long_t 			tb_gstream_awrit(tb_gstream_t* gstream, tb_byte_t const* data, tb_size_t size);

/*! async writf
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			the real size or -1
 */
tb_hong_t 			tb_gstream_awritf(tb_gstream_t* gstream, tb_handle_t file, tb_hize_t offset, tb_hize_t size);

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

/*! sync stream
 *
 * @param gstream 	the stream
 * @param bclosing 	is closing?
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_sync(tb_gstream_t* gstream, tb_bool_t bclosing);

/*! need stream
 *
 * @param gstream 	the stream
 * @param data 		the data
 * @param size 		the size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_need(tb_gstream_t* gstream, tb_byte_t** data, tb_size_t size);

/*! seek stream
 *
 * @param gstream 	the stream
 * @param offset 	the offset
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_seek(tb_gstream_t* gstream, tb_hize_t offset);

/*! skip stream
 *
 * @param gstream 	the stream
 * @param size 		the size
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_gstream_skip(tb_gstream_t* gstream, tb_hize_t size);

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

#endif

