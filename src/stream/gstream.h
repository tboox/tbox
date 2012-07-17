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
#include "../libc/libc.h"
#include "../network/url.h"
#include "../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the stream command
#define TB_GSTREAM_CMD(type, cmd) 				(((type) << 16) | (cmd))
#define TB_TSTREAM_CMD(type, cmd) 				TB_GSTREAM_CMD(TB_GSTREAM_TYPE_TRAN, (((type) << 8) | (cmd)))

// the stream block maxn
#define TB_GSTREAM_BLOCK_MAXN 					(8192)

// the stream bitops
#ifdef TB_WORDS_BIGENDIAN
# 	define tb_gstream_bread_u16_ne(gst) 		tb_gstream_bread_u16_be(gst)
# 	define tb_gstream_bread_s16_ne(gst) 		tb_gstream_bread_s16_be(gst)
# 	define tb_gstream_bread_u24_ne(gst) 		tb_gstream_bread_u24_be(gst)
# 	define tb_gstream_bread_s24_ne(gst) 		tb_gstream_bread_s24_be(gst)
# 	define tb_gstream_bread_u32_ne(gst) 		tb_gstream_bread_u32_be(gst)
# 	define tb_gstream_bread_s32_ne(gst) 		tb_gstream_bread_s32_be(gst)
# 	define tb_gstream_bread_u64_ne(gst) 		tb_gstream_bread_u64_be(gst)
# 	define tb_gstream_bread_s64_ne(gst) 		tb_gstream_bread_s64_be(gst)

# 	define tb_gstream_bwrit_u16_ne(gst, val) 	tb_gstream_bwrit_u16_be(gst, val)
# 	define tb_gstream_bwrit_s16_ne(gst, val)	tb_gstream_bwrit_s16_be(gst, val)
# 	define tb_gstream_bwrit_u24_ne(gst, val) 	tb_gstream_bwrit_u24_be(gst, val)
# 	define tb_gstream_bwrit_s24_ne(gst, val)	tb_gstream_bwrit_s24_be(gst, val)
# 	define tb_gstream_bwrit_u32_ne(gst, val)	tb_gstream_bwrit_u32_be(gst, val)
# 	define tb_gstream_bwrit_s32_ne(gst, val) 	tb_gstream_bwrit_s32_be(gst, val)
# 	define tb_gstream_bwrit_u64_ne(gst, val)	tb_gstream_bwrit_u64_be(gst, val)
# 	define tb_gstream_bwrit_s64_ne(gst, val) 	tb_gstream_bwrit_s64_be(gst, val)

#else
# 	define tb_gstream_bread_u16_ne(gst) 		tb_gstream_bread_u16_le(gst)
# 	define tb_gstream_bread_s16_ne(gst) 		tb_gstream_bread_s16_le(gst)
# 	define tb_gstream_bread_u24_ne(gst) 		tb_gstream_bread_u24_le(gst)
# 	define tb_gstream_bread_s24_ne(gst) 		tb_gstream_bread_s24_le(gst)
# 	define tb_gstream_bread_u32_ne(gst) 		tb_gstream_bread_u32_le(gst)
# 	define tb_gstream_bread_s32_ne(gst) 		tb_gstream_bread_s32_le(gst)
# 	define tb_gstream_bread_u64_ne(gst) 		tb_gstream_bread_u64_le(gst)
# 	define tb_gstream_bread_s64_ne(gst) 		tb_gstream_bread_s64_le(gst)

# 	define tb_gstream_bwrit_u16_ne(gst, val) 	tb_gstream_bwrit_u16_le(gst, val)
# 	define tb_gstream_bwrit_s16_ne(gst, val)	tb_gstream_bwrit_s16_le(gst, val)
# 	define tb_gstream_bwrit_u24_ne(gst, val) 	tb_gstream_bwrit_u24_le(gst, val)
# 	define tb_gstream_bwrit_s24_ne(gst, val)	tb_gstream_bwrit_s24_le(gst, val)
# 	define tb_gstream_bwrit_u32_ne(gst, val)	tb_gstream_bwrit_u32_le(gst, val)
# 	define tb_gstream_bwrit_s32_ne(gst, val) 	tb_gstream_bwrit_s32_le(gst, val)
# 	define tb_gstream_bwrit_u64_ne(gst, val)	tb_gstream_bwrit_u64_le(gst, val)
# 	define tb_gstream_bwrit_s64_ne(gst, val) 	tb_gstream_bwrit_s64_le(gst, val)

#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the gstream type
typedef enum __tb_gstream_type_t
{
 	TB_GSTREAM_TYPE_NULL 			= 0
, 	TB_GSTREAM_TYPE_FILE 			= 1
, 	TB_GSTREAM_TYPE_SOCK 			= 2
, 	TB_GSTREAM_TYPE_HTTP 			= 3
, 	TB_GSTREAM_TYPE_DATA 			= 4
, 	TB_GSTREAM_TYPE_TRAN 			= 5

}tb_gstream_type_t;

/// the tstream type
typedef enum __tb_tstream_type_t
{
 	TB_TSTREAM_TYPE_NULL 			= 0
, 	TB_TSTREAM_TYPE_ENCODING 		= 1
, 	TB_TSTREAM_TYPE_ZIP 			= 2

}tb_tstream_type_t;

/// the gstream command type
typedef enum __tb_gstream_cmd_t
{
	TB_GSTREAM_CMD_NULL 				= 0

	// the gstream
,	TB_GSTREAM_CMD_GET_URL 				= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 1)
,	TB_GSTREAM_CMD_GET_HOST 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 2)
,	TB_GSTREAM_CMD_GET_PORT 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 3)
,	TB_GSTREAM_CMD_GET_PATH 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 4)
,	TB_GSTREAM_CMD_GET_SSL 				= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 5)
,	TB_GSTREAM_CMD_GET_CACHE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 6)
,	TB_GSTREAM_CMD_GET_TIMEOUT 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 7)

,	TB_GSTREAM_CMD_SET_URL 				= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 11)
,	TB_GSTREAM_CMD_SET_HOST 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 12)
,	TB_GSTREAM_CMD_SET_PORT 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 13)
,	TB_GSTREAM_CMD_SET_PATH 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 14)
,	TB_GSTREAM_CMD_SET_SSL 				= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 15)
,	TB_GSTREAM_CMD_SET_CACHE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 16)
,	TB_GSTREAM_CMD_SET_TIMEOUT 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 17)

	// the dstream
,	TB_DSTREAM_CMD_SET_DATA 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_DATA, 1)

	// the fstream
,	TB_FSTREAM_CMD_SET_FLAGS 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_FILE, 1)
,	TB_FSTREAM_CMD_SET_HANDLE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_FILE, 2)
,	TB_FSTREAM_CMD_GET_HANDLE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_FILE, 3)

	// the sstream
,	TB_SSTREAM_CMD_SET_TYPE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_SOCK, 1)
,	TB_SSTREAM_CMD_SET_HANDLE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_SOCK, 2)
,	TB_SSTREAM_CMD_GET_HANDLE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_SOCK, 3)

	// the hstream
,	TB_HSTREAM_CMD_GET_OPTION 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 1)
,	TB_HSTREAM_CMD_GET_STATUS 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 2)

	// the tstream
,	TB_TSTREAM_CMD_GET_GSTREAM 			= TB_TSTREAM_CMD(TB_TSTREAM_TYPE_NULL, 1)
,	TB_TSTREAM_CMD_SET_GSTREAM 			= TB_TSTREAM_CMD(TB_TSTREAM_TYPE_NULL, 2)

	// the estream
,	TB_ESTREAM_CMD_GET_IE 				= TB_TSTREAM_CMD(TB_TSTREAM_TYPE_ENCODING, 1)
,	TB_ESTREAM_CMD_GET_OE 				= TB_TSTREAM_CMD(TB_TSTREAM_TYPE_ENCODING, 2)

,	TB_ESTREAM_CMD_SET_IE 				= TB_TSTREAM_CMD(TB_TSTREAM_TYPE_ENCODING, 3)
,	TB_ESTREAM_CMD_SET_OE 				= TB_TSTREAM_CMD(TB_TSTREAM_TYPE_ENCODING, 4)

	// the zstream
,	TB_ZSTREAM_CMD_GET_ALGO				= TB_TSTREAM_CMD(TB_TSTREAM_TYPE_ZIP, 1)
,	TB_ZSTREAM_CMD_GET_ACTION			= TB_TSTREAM_CMD(TB_TSTREAM_TYPE_ZIP, 2)
,	TB_ZSTREAM_CMD_SET_ALGO				= TB_TSTREAM_CMD(TB_TSTREAM_TYPE_ZIP, 3)
,	TB_ZSTREAM_CMD_SET_ACTION			= TB_TSTREAM_CMD(TB_TSTREAM_TYPE_ZIP, 4)

}tb_gstream_cmd_t;

/// the generic stream type
typedef struct __tb_gstream_t
{	
	/// the stream type
	tb_size_t 			type 		: 8;

	/// is opened?
	tb_size_t 			bopened 	: 1;

	/// is writed?
	tb_size_t 			bwrited 	: 1;

	/// is cached?
	tb_size_t 			bcached 	: 1;

	/// the timeout: ms
	tb_size_t 			timeout 	: 21;

	/// the url
	tb_url_t 			url;

	/// the cache
	tb_qbuffer_t 		cache;

	/// the offset
	tb_hize_t 			offset;

	/// wait the aio event
	tb_long_t 			(*wait)(struct __tb_gstream_t* gst, tb_size_t etype, tb_long_t timeout);

	/// async open
	tb_long_t 			(*aopen)(struct __tb_gstream_t* gst);

	/// async close
	tb_long_t 			(*aclose)(struct __tb_gstream_t* gst);

	/// async read
	tb_long_t 			(*aread)(struct __tb_gstream_t* gst, tb_byte_t* data, tb_size_t size, tb_bool_t sync);

	/// async writ
	tb_long_t 			(*awrit)(struct __tb_gstream_t* gst, tb_byte_t* data, tb_size_t size, tb_bool_t sync);

	/// seek
	tb_long_t 			(*aseek)(struct __tb_gstream_t* gst, tb_hize_t offset);

	/// size
	tb_hize_t 			(*size)(struct __tb_gstream_t* gst);

	/// free
	tb_void_t 			(*free)(struct __tb_gstream_t* gst);

	/// ctrl
	tb_bool_t 			(*ctrl)(struct __tb_gstream_t* gst, tb_size_t cmd, tb_va_list_t args);

}tb_gstream_t;


/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/// exit stream
tb_void_t 			tb_gstream_exit(tb_gstream_t* gst);

// init stream
tb_gstream_t* 		tb_gstream_init_data();
tb_gstream_t* 		tb_gstream_init_file();
tb_gstream_t* 		tb_gstream_init_sock();
tb_gstream_t* 		tb_gstream_init_http();
tb_gstream_t* 		tb_gstream_init_null();
tb_gstream_t* 		tb_gstream_init_zip();
tb_gstream_t* 		tb_gstream_init_encoding();
tb_bool_t 			tb_gstream_init(tb_gstream_t* gst);

/*!init stream from url
 *
 * <pre>
 * file://path or unix path: e.g. /root/xxxx/file
 * files://home/file
 * sock://host:port?tcp=
 * socks://host:port?udp=
 * http://host:port/path?arg0=&arg1=...
 * https://host:port/path?arg0=&arg1=...
 * </pre>
 */
tb_gstream_t* 		tb_gstream_init_from_url(tb_char_t const* url);

/// init stream from data
tb_gstream_t* 		tb_gstream_init_from_data(tb_byte_t const* data, tb_size_t size);

/// init stream from file
tb_gstream_t* 		tb_gstream_init_from_file(tb_char_t const* path);

/// init stream from sock
tb_gstream_t* 		tb_gstream_init_from_sock(tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl);

/// init stream from http or https
tb_gstream_t* 		tb_gstream_init_from_http(tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl);

/// init stream from null
tb_gstream_t* 		tb_gstream_init_from_null(tb_gstream_t* gst);

/// init stream from zip
tb_gstream_t* 		tb_gstream_init_from_zip(tb_gstream_t* gst, tb_size_t algo, tb_size_t action);

/// init stream from encoding
tb_gstream_t* 		tb_gstream_init_from_encoding(tb_gstream_t* gst, tb_size_t ie, tb_size_t oe);

/*!wait the gstream 
 *
 * blocking wait the single event object, so need not aipp 
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param 	gst 	the gstream 
 * @param 	etype 	the waited event type, return the needed event type if TB_AIOO_ETYPE_NULL
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event type, return 0 if timeout, return -1 if error
 */
tb_long_t 			tb_gstream_wait(tb_gstream_t* gst, tb_size_t etype, tb_long_t timeout);

/// is end?
tb_bool_t 			tb_gstream_beof(tb_gstream_t* gst);

/// clear stream cache and reset status, but not close it
tb_void_t 			tb_gstream_clear(tb_gstream_t* gst);

/// async open, allow multiple called before closing 
tb_long_t 			tb_gstream_aopen(tb_gstream_t* gst);

/// block open, allow multiple called before closing 
tb_bool_t 			tb_gstream_bopen(tb_gstream_t* gst);

/// async close, allow multiple called
tb_long_t 			tb_gstream_aclose(tb_gstream_t* gst);

/// block close, allow multiple called
tb_bool_t 			tb_gstream_bclose(tb_gstream_t* gst);

// async read & writ data
tb_long_t 			tb_gstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);
tb_long_t 			tb_gstream_awrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);

// block read & writ data
tb_bool_t 			tb_gstream_bread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);
tb_bool_t 			tb_gstream_bwrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);

// async fread & fwrit data
tb_long_t 			tb_gstream_afread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);
tb_long_t 			tb_gstream_afwrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);

// block fread & fwrit data
tb_bool_t 			tb_gstream_bfread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);
tb_bool_t 			tb_gstream_bfwrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);

// need
tb_long_t 			tb_gstream_aneed(tb_gstream_t* gst, tb_byte_t** data, tb_size_t size);
tb_bool_t 			tb_gstream_bneed(tb_gstream_t* gst, tb_byte_t** data, tb_size_t size);

// seek
tb_long_t 			tb_gstream_aseek(tb_gstream_t* gst, tb_hize_t offset);
tb_bool_t 			tb_gstream_bseek(tb_gstream_t* gst, tb_hize_t offset);

// skip
tb_long_t 			tb_gstream_askip(tb_gstream_t* gst, tb_hize_t size);
tb_bool_t 			tb_gstream_bskip(tb_gstream_t* gst, tb_hize_t size);

/// block writ format data
tb_long_t 			tb_gstream_printf(tb_gstream_t* gst, tb_char_t const* fmt, ...);

// block load & save data
tb_hize_t 			tb_gstream_load(tb_gstream_t* gst, tb_gstream_t* ist);
tb_hize_t 			tb_gstream_save(tb_gstream_t* gst, tb_gstream_t* ost);

// block read & writ line
tb_long_t 			tb_gstream_bread_line(tb_gstream_t* gst, tb_char_t* data, tb_size_t size);
tb_long_t 			tb_gstream_bwrit_line(tb_gstream_t* gst, tb_char_t* data, tb_size_t size);

// block read integer
tb_uint8_t 			tb_gstream_bread_u8(tb_gstream_t* gst);
tb_sint8_t 			tb_gstream_bread_s8(tb_gstream_t* gst);

tb_uint16_t 		tb_gstream_bread_u16_le(tb_gstream_t* gst);
tb_sint16_t 		tb_gstream_bread_s16_le(tb_gstream_t* gst);

tb_uint32_t 		tb_gstream_bread_u24_le(tb_gstream_t* gst);
tb_sint32_t 		tb_gstream_bread_s24_le(tb_gstream_t* gst);

tb_uint32_t 		tb_gstream_bread_u32_le(tb_gstream_t* gst);
tb_sint32_t 		tb_gstream_bread_s32_le(tb_gstream_t* gst);

tb_uint64_t 		tb_gstream_bread_u64_le(tb_gstream_t* gst);
tb_sint64_t 		tb_gstream_bread_s64_le(tb_gstream_t* gst);

tb_uint16_t 		tb_gstream_bread_u16_be(tb_gstream_t* gst);
tb_sint16_t 		tb_gstream_bread_s16_be(tb_gstream_t* gst);

tb_uint32_t 		tb_gstream_bread_u24_be(tb_gstream_t* gst);
tb_sint32_t 		tb_gstream_bread_s24_be(tb_gstream_t* gst);

tb_uint32_t 		tb_gstream_bread_u32_be(tb_gstream_t* gst);
tb_sint32_t 		tb_gstream_bread_s32_be(tb_gstream_t* gst);

tb_uint64_t 		tb_gstream_bread_u64_be(tb_gstream_t* gst);
tb_sint64_t 		tb_gstream_bread_s64_be(tb_gstream_t* gst);

// block writ integer
tb_bool_t			tb_gstream_bwrit_u8(tb_gstream_t* gst, tb_uint8_t val);
tb_bool_t 			tb_gstream_bwrit_s8(tb_gstream_t* gst, tb_sint8_t val);

tb_bool_t 			tb_gstream_bwrit_u16_le(tb_gstream_t* gst, tb_uint16_t val);
tb_bool_t 			tb_gstream_bwrit_s16_le(tb_gstream_t* gst, tb_sint16_t val);

tb_bool_t 			tb_gstream_bwrit_u24_le(tb_gstream_t* gst, tb_uint32_t val);
tb_bool_t 			tb_gstream_bwrit_s24_le(tb_gstream_t* gst, tb_sint32_t val);

tb_bool_t 			tb_gstream_bwrit_u32_le(tb_gstream_t* gst, tb_uint32_t val);
tb_bool_t 			tb_gstream_bwrit_s32_le(tb_gstream_t* gst, tb_sint32_t val);

tb_bool_t 			tb_gstream_bwrit_u64_le(tb_gstream_t* gst, tb_uint64_t val);
tb_bool_t 			tb_gstream_bwrit_s64_le(tb_gstream_t* gst, tb_sint64_t val);

tb_bool_t 			tb_gstream_bwrit_u16_be(tb_gstream_t* gst, tb_uint16_t val);
tb_bool_t 			tb_gstream_bwrit_s16_be(tb_gstream_t* gst, tb_sint16_t val);

tb_bool_t 			tb_gstream_bwrit_u24_be(tb_gstream_t* gst, tb_uint32_t val);
tb_bool_t 			tb_gstream_bwrit_s24_be(tb_gstream_t* gst, tb_sint32_t val);

tb_bool_t 			tb_gstream_bwrit_u32_be(tb_gstream_t* gst, tb_uint32_t val);
tb_bool_t 			tb_gstream_bwrit_s32_be(tb_gstream_t* gst, tb_sint32_t val);

tb_bool_t 			tb_gstream_bwrit_u64_be(tb_gstream_t* gst, tb_uint64_t val);
tb_bool_t 			tb_gstream_bwrit_s64_be(tb_gstream_t* gst, tb_sint64_t val);

// status
tb_size_t 			tb_gstream_type(tb_gstream_t const* gst);
tb_hize_t 			tb_gstream_size(tb_gstream_t const* gst);
tb_hize_t 			tb_gstream_left(tb_gstream_t const* gst);
tb_hize_t 			tb_gstream_offset(tb_gstream_t const* gst);
tb_size_t 			tb_gstream_timeout(tb_gstream_t const* gst);

/// ctrl
tb_bool_t 			tb_gstream_ctrl(tb_gstream_t* gst, tb_size_t cmd, ...);

#endif

