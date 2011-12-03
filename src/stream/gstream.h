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
 * \file		gstream.h
 *
 */
#ifndef TB_STREAM_GSTREAM_H
#define TB_STREAM_GSTREAM_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// the stream command
#define TB_GSTREAM_CMD(type, cmd) 				(((type) << 16) | (cmd))
#define TB_TSTREAM_CMD(type, cmd) 				TB_GSTREAM_CMD(TB_GSTREAM_TYPE_TRAN, (((type) << 8) | (cmd)))

// the stream block maxn
#define TB_GSTREAM_BLOCK_MAXN 					(8192)

// the stream cache maxn
#define TB_GSTREAM_CACHE_MAXN 					(8192)

// the stream url maxn
#define TB_GSTREAM_URL_MAXN 					(8192)

// the stream timeout
#define TB_GSTREAM_TIMEOUT 						(10000)

// the stream bitops
#ifdef TB_WORDS_BIGENDIAN
# 	define tb_gstream_read_u16_ne(gst) 			tb_gstream_read_u16_be(gst)
# 	define tb_gstream_read_s16_ne(gst) 			tb_gstream_read_s16_be(gst)
# 	define tb_gstream_read_u24_ne(gst) 			tb_gstream_read_u24_be(gst)
# 	define tb_gstream_read_s24_ne(gst) 			tb_gstream_read_s24_be(gst)
# 	define tb_gstream_read_u32_ne(gst) 			tb_gstream_read_u32_be(gst)
# 	define tb_gstream_read_s32_ne(gst) 			tb_gstream_read_s32_be(gst)

# 	define tb_gstream_writ_u16_ne(gst, val) 	tb_gstream_writ_u16_be(gst, val)
# 	define tb_gstream_writ_s16_ne(gst, val)		tb_gstream_writ_s16_be(gst, val)
# 	define tb_gstream_writ_u24_ne(gst, val) 	tb_gstream_writ_u24_be(gst, val)
# 	define tb_gstream_writ_s24_ne(gst, val)		tb_gstream_writ_s24_be(gst, val)
# 	define tb_gstream_writ_u32_ne(gst, val)		tb_gstream_writ_u32_be(gst, val)
# 	define tb_gstream_writ_s32_ne(gst, val) 	tb_gstream_writ_s32_be(gst, val)

#else
# 	define tb_gstream_read_u16_ne(gst) 			tb_gstream_read_u16_le(gst)
# 	define tb_gstream_read_s16_ne(gst) 			tb_gstream_read_s16_le(gst)
# 	define tb_gstream_read_u24_ne(gst) 			tb_gstream_read_u24_le(gst)
# 	define tb_gstream_read_s24_ne(gst) 			tb_gstream_read_s24_le(gst)
# 	define tb_gstream_read_u32_ne(gst) 			tb_gstream_read_u32_le(gst)
# 	define tb_gstream_read_s32_ne(gst) 			tb_gstream_read_s32_le(gst)

# 	define tb_gstream_writ_u16_ne(gst, val) 	tb_gstream_writ_u16_le(gst, val)
# 	define tb_gstream_writ_s16_ne(gst, val)		tb_gstream_writ_s16_le(gst, val)
# 	define tb_gstream_writ_u24_ne(gst, val) 	tb_gstream_writ_u24_le(gst, val)
# 	define tb_gstream_writ_s24_ne(gst, val)		tb_gstream_writ_s24_le(gst, val)
# 	define tb_gstream_writ_u32_ne(gst, val)		tb_gstream_writ_u32_le(gst, val)
# 	define tb_gstream_writ_s32_ne(gst, val) 	tb_gstream_writ_s32_le(gst, val)

#endif

/* /////////////////////////////////////////////////////////
 * types
 */

// the stream seek type
typedef enum __tb_gstream_seek_t
{
 	TB_GSTREAM_SEEK_BEG 			= 0
, 	TB_GSTREAM_SEEK_CUR 			= 1
, 	TB_GSTREAM_SEEK_END 			= 2

}tb_gstream_seek_t;

// the gstream type
typedef enum __tb_gstream_type_t
{
 	TB_GSTREAM_TYPE_NULL 			= 0
, 	TB_GSTREAM_TYPE_HTTP 			= 1
, 	TB_GSTREAM_TYPE_FILE 			= 2
, 	TB_GSTREAM_TYPE_DATA 			= 3
, 	TB_GSTREAM_TYPE_TRAN 			= 4

}tb_gstream_type_t;

// the tstream type
typedef enum __tb_tstream_type_t
{
 	TB_TSTREAM_TYPE_NULL 			= 0
, 	TB_TSTREAM_TYPE_ENCODING 		= 1
, 	TB_TSTREAM_TYPE_ZIP 			= 2

}tb_tstream_type_t;

// the gstream command type
typedef enum __tb_gstream_cmd_t
{
	TB_GSTREAM_CMD_NULL 				= 0

	// the gstream
,	TB_GSTREAM_CMD_GET_URL 				= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 1)
,	TB_GSTREAM_CMD_GET_CACHE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 2)

,	TB_GSTREAM_CMD_SET_URL 				= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 3)
,	TB_GSTREAM_CMD_SET_TIMEOUT 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 4)
,	TB_GSTREAM_CMD_SET_CACHE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_NULL, 5)

	// the dstream
,	TB_DSTREAM_CMD_SET_DATA 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_DATA, 1)

	// the fstream
,	TB_FSTREAM_CMD_SET_FLAGS 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_FILE, 1)

	// the hstream
,	TB_HSTREAM_CMD_ISCHUNKED 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 1)
,	TB_HSTREAM_CMD_ISREDIRECT 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 2)

,	TB_HSTREAM_CMD_GET_CODE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 3)
,	TB_HSTREAM_CMD_GET_REDIRECT 		= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 4)
,	TB_HSTREAM_CMD_GET_COOKIES 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 5)

,	TB_HSTREAM_CMD_SET_METHOD 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 6)
,	TB_HSTREAM_CMD_SET_HEAD 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 7)
,	TB_HSTREAM_CMD_SET_POST 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 8)
,	TB_HSTREAM_CMD_SET_RANGE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 9)
,	TB_HSTREAM_CMD_SET_KALIVE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 10)
,	TB_HSTREAM_CMD_SET_COOKIES 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 11)
,	TB_HSTREAM_CMD_SET_REDIRECT 		= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 12)
,	TB_HSTREAM_CMD_SET_HEAD_FUNC 		= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 13)
,	TB_HSTREAM_CMD_SET_SOPEN_FUNC 		= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 14)
,	TB_HSTREAM_CMD_SET_SCLOSE_FUNC 		= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 15)
,	TB_HSTREAM_CMD_SET_SREAD_FUNC 		= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 16)
,	TB_HSTREAM_CMD_SET_SWRITE_FUNC 		= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 17)

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

// the generic stream type
typedef struct __tb_gstream_t
{	
	// the stream type
	tb_size_t 			type 		: 8;

	// is opened?
	tb_size_t 			bopened 	: 1;

	// the timeout: ms
	tb_size_t 			timeout 	: 23;

	// the url
	tb_char_t* 			url;

	// the cache data
	tb_byte_t* 			cache_data;
	tb_byte_t* 			cache_head;
	tb_size_t 			cache_size;
	tb_size_t 			cache_maxn;

	// open & close
	tb_bool_t 			(*open)(struct __tb_gstream_t* gst);
	tb_void_t 			(*close)(struct __tb_gstream_t* gst);
	tb_void_t 			(*free)(struct __tb_gstream_t* gst);

	// stream operations
	tb_long_t 			(*read)(struct __tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);
	tb_long_t 			(*writ)(struct __tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);
	tb_bool_t 			(*seek)(struct __tb_gstream_t* gst, tb_int64_t offset, tb_size_t flag);

	// stream size
	tb_uint64_t 		(*size)(struct __tb_gstream_t* gst);
	tb_uint64_t 		(*offset)(struct __tb_gstream_t* gst);

	// ioctl
	tb_bool_t 			(*ioctl0)(struct __tb_gstream_t* gst, tb_size_t cmd);
	tb_bool_t 			(*ioctl1)(struct __tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1);
	tb_bool_t 			(*ioctl2)(struct __tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1, tb_pointer_t arg2);

}tb_gstream_t;


/* /////////////////////////////////////////////////////////
 * interfaces
 */

// destroy stream
tb_void_t 			tb_gstream_destroy(tb_gstream_t* gst);

// the data stream
tb_gstream_t* 		tb_gstream_create_data();
tb_gstream_t* 		tb_gstream_create_from_data(tb_byte_t const* data, tb_size_t size);

// the protocol stream
tb_gstream_t* 		tb_gstream_create_file();
tb_gstream_t* 		tb_gstream_create_http();
tb_gstream_t* 		tb_gstream_create_from_url(tb_char_t const* url);

// the transform stream for encoding
tb_gstream_t* 		tb_gstream_create_encoding();
tb_gstream_t* 		tb_gstream_create_from_encoding(tb_gstream_t* gst, tb_size_t ie, tb_size_t oe);

// the transform stream for zip
tb_gstream_t* 		tb_gstream_create_zip();
tb_gstream_t* 		tb_gstream_create_from_zip(tb_gstream_t* gst, tb_size_t algo, tb_size_t action);

// open & close
tb_bool_t 			tb_gstream_open(tb_gstream_t* gst);
tb_void_t 			tb_gstream_close(tb_gstream_t* gst);

// read & writ data
tb_long_t 			tb_gstream_read(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);
tb_long_t 			tb_gstream_writ(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);

// read & writ data - blocked
tb_long_t 			tb_gstream_bread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);
tb_long_t 			tb_gstream_bwrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);

// read & writ line - blocked
tb_long_t 			tb_gstream_read_line(tb_gstream_t* gst, tb_char_t* data, tb_size_t size);
tb_long_t 			tb_gstream_writ_line(tb_gstream_t* gst, tb_char_t* data, tb_size_t size);

// need data - blocked
tb_byte_t* 			tb_gstream_need(tb_gstream_t* gst, tb_size_t size);

// seek
tb_bool_t 			tb_gstream_seek(tb_gstream_t* gst, tb_int64_t offset, tb_size_t flag);

// format writ data - blocked
tb_long_t 			tb_gstream_printf(tb_gstream_t* gst, tb_char_t const* fmt, ...);

// load & save data
tb_uint64_t 		tb_gstream_load(tb_gstream_t* gst, tb_gstream_t* ist);
tb_uint64_t 		tb_gstream_save(tb_gstream_t* gst, tb_gstream_t* ost);

// read integer - blocked
tb_uint8_t 			tb_gstream_read_u8(tb_gstream_t* gst);
tb_sint8_t 			tb_gstream_read_s8(tb_gstream_t* gst);

tb_uint16_t 		tb_gstream_read_u16_le(tb_gstream_t* gst);
tb_sint16_t 		tb_gstream_read_s16_le(tb_gstream_t* gst);

tb_uint32_t 		tb_gstream_read_u24_le(tb_gstream_t* gst);
tb_sint32_t 		tb_gstream_read_s24_le(tb_gstream_t* gst);

tb_uint32_t 		tb_gstream_read_u32_le(tb_gstream_t* gst);
tb_sint32_t 		tb_gstream_read_s32_le(tb_gstream_t* gst);

tb_uint16_t 		tb_gstream_read_u16_be(tb_gstream_t* gst);
tb_sint16_t 		tb_gstream_read_s16_be(tb_gstream_t* gst);

tb_uint32_t 		tb_gstream_read_u24_be(tb_gstream_t* gst);
tb_sint32_t 		tb_gstream_read_s24_be(tb_gstream_t* gst);

tb_uint32_t 		tb_gstream_read_u32_be(tb_gstream_t* gst);
tb_sint32_t 		tb_gstream_read_s32_be(tb_gstream_t* gst);

// writ integer - blocked
tb_bool_t			tb_gstream_writ_u8(tb_gstream_t* gst, tb_uint8_t val);
tb_bool_t 			tb_gstream_writ_s8(tb_gstream_t* gst, tb_sint8_t val);

tb_bool_t 			tb_gstream_writ_u16_le(tb_gstream_t* gst, tb_uint16_t val);
tb_bool_t 			tb_gstream_writ_s16_le(tb_gstream_t* gst, tb_sint16_t val);

tb_bool_t 			tb_gstream_writ_u24_le(tb_gstream_t* gst, tb_uint32_t val);
tb_bool_t 			tb_gstream_writ_s24_le(tb_gstream_t* gst, tb_sint32_t val);

tb_bool_t 			tb_gstream_writ_u32_le(tb_gstream_t* gst, tb_uint32_t val);
tb_bool_t 			tb_gstream_writ_s32_le(tb_gstream_t* gst, tb_sint32_t val);

tb_bool_t 			tb_gstream_writ_u16_be(tb_gstream_t* gst, tb_uint16_t val);
tb_bool_t 			tb_gstream_writ_s16_be(tb_gstream_t* gst, tb_sint16_t val);

tb_bool_t 			tb_gstream_writ_u24_be(tb_gstream_t* gst, tb_uint32_t val);
tb_bool_t 			tb_gstream_writ_s24_be(tb_gstream_t* gst, tb_sint32_t val);

tb_bool_t 			tb_gstream_writ_u32_be(tb_gstream_t* gst, tb_uint32_t val);
tb_bool_t 			tb_gstream_writ_s32_be(tb_gstream_t* gst, tb_sint32_t val);

// status
tb_uint64_t 		tb_gstream_size(tb_gstream_t const* gst);
tb_uint64_t 		tb_gstream_left(tb_gstream_t const* gst);
tb_uint64_t 		tb_gstream_offset(tb_gstream_t const* gst);
tb_size_t 			tb_gstream_timeout(tb_gstream_t const* gst);

// ioctl
tb_bool_t 			tb_gstream_ioctl0(tb_gstream_t* gst, tb_size_t cmd);
tb_bool_t 			tb_gstream_ioctl1(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1);
tb_bool_t 			tb_gstream_ioctl2(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1, tb_pointer_t arg2);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

