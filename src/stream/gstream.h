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
#include "../memory/memory.h"

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
# 	define tb_gstream_bread_u16_ne(gst) 		tb_gstream_bread_u16_be(gst)
# 	define tb_gstream_bread_s16_ne(gst) 		tb_gstream_bread_s16_be(gst)
# 	define tb_gstream_bread_u24_ne(gst) 		tb_gstream_bread_u24_be(gst)
# 	define tb_gstream_bread_s24_ne(gst) 		tb_gstream_bread_s24_be(gst)
# 	define tb_gstream_bread_u32_ne(gst) 		tb_gstream_bread_u32_be(gst)
# 	define tb_gstream_bread_s32_ne(gst) 		tb_gstream_bread_s32_be(gst)

# 	define tb_gstream_bwrit_u16_ne(gst, val) 	tb_gstream_bwrit_u16_be(gst, val)
# 	define tb_gstream_bwrit_s16_ne(gst, val)	tb_gstream_bwrit_s16_be(gst, val)
# 	define tb_gstream_bwrit_u24_ne(gst, val) 	tb_gstream_bwrit_u24_be(gst, val)
# 	define tb_gstream_bwrit_s24_ne(gst, val)	tb_gstream_bwrit_s24_be(gst, val)
# 	define tb_gstream_bwrit_u32_ne(gst, val)	tb_gstream_bwrit_u32_be(gst, val)
# 	define tb_gstream_bwrit_s32_ne(gst, val) 	tb_gstream_bwrit_s32_be(gst, val)

#else
# 	define tb_gstream_bread_u16_ne(gst) 		tb_gstream_bread_u16_le(gst)
# 	define tb_gstream_bread_s16_ne(gst) 		tb_gstream_bread_s16_le(gst)
# 	define tb_gstream_bread_u24_ne(gst) 		tb_gstream_bread_u24_le(gst)
# 	define tb_gstream_bread_s24_ne(gst) 		tb_gstream_bread_s24_le(gst)
# 	define tb_gstream_bread_u32_ne(gst) 		tb_gstream_bread_u32_le(gst)
# 	define tb_gstream_bread_s32_ne(gst) 		tb_gstream_bread_s32_le(gst)

# 	define tb_gstream_bwrit_u16_ne(gst, val) 	tb_gstream_bwrit_u16_le(gst, val)
# 	define tb_gstream_bwrit_s16_ne(gst, val)	tb_gstream_bwrit_s16_le(gst, val)
# 	define tb_gstream_bwrit_u24_ne(gst, val) 	tb_gstream_bwrit_u24_le(gst, val)
# 	define tb_gstream_bwrit_s24_ne(gst, val)	tb_gstream_bwrit_s24_le(gst, val)
# 	define tb_gstream_bwrit_u32_ne(gst, val)	tb_gstream_bwrit_u32_le(gst, val)
# 	define tb_gstream_bwrit_s32_ne(gst, val) 	tb_gstream_bwrit_s32_le(gst, val)

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
, 	TB_GSTREAM_TYPE_DATA 			= 1
, 	TB_GSTREAM_TYPE_FILE 			= 2
, 	TB_GSTREAM_TYPE_SOCK 			= 3
, 	TB_GSTREAM_TYPE_HTTP 			= 4
, 	TB_GSTREAM_TYPE_TRAN 			= 5

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

	// the sstream
,	TB_SSTREAM_CMD_SET_HOST 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_SOCK, 1)
,	TB_SSTREAM_CMD_SET_PORT 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_SOCK, 2)
,	TB_SSTREAM_CMD_SET_TYPE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_SOCK, 3)
,	TB_SSTREAM_CMD_SET_SSL 				= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_SOCK, 4)

	// the hstream
,	TB_HSTREAM_CMD_ISCHUNKED 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 1)
,	TB_HSTREAM_CMD_ISREDIRECT 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 2)

,	TB_HSTREAM_CMD_GET_CODE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 3)
,	TB_HSTREAM_CMD_GET_REDIRECT 		= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 4)
,	TB_HSTREAM_CMD_GET_COOKIES 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 5)

,	TB_HSTREAM_CMD_SET_HOST 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 6)
,	TB_HSTREAM_CMD_SET_PORT 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 7)
,	TB_HSTREAM_CMD_SET_PATH 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 8)
,	TB_HSTREAM_CMD_SET_HEAD 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 9)
,	TB_HSTREAM_CMD_SET_POST 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 10)
,	TB_HSTREAM_CMD_SET_SSL 				= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 11)
,	TB_HSTREAM_CMD_SET_RANGE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 12)
,	TB_HSTREAM_CMD_SET_METHOD 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 13)
,	TB_HSTREAM_CMD_SET_KALIVE 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 14)
,	TB_HSTREAM_CMD_SET_COOKIES 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 15)
,	TB_HSTREAM_CMD_SET_REDIRECT 		= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 16)
,	TB_HSTREAM_CMD_SET_HFUNC 			= TB_GSTREAM_CMD(TB_GSTREAM_TYPE_HTTP, 17)

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

	// is writed?
	tb_size_t 			bwrited 	: 1;

	// the timeout: ms
	tb_size_t 			timeout 	: 22;

	// the cache
	tb_qbuffer_t 		cache;

	// the offset
	tb_uint64_t 		offset;

	// the url
	tb_char_t* 			url;

	// async open
	tb_long_t 			(*aopen)(struct __tb_gstream_t* gst);

	// async close
	tb_long_t 			(*aclose)(struct __tb_gstream_t* gst);

	// async read
	tb_long_t 			(*aread)(struct __tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);

	// async writ
	tb_long_t 			(*awrit)(struct __tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);

	// async fread
	tb_long_t 			(*afread)(struct __tb_gstream_t* gst);

	// async fwrit
	tb_long_t 			(*afwrit)(struct __tb_gstream_t* gst);

	// seek
	tb_bool_t 			(*seek)(struct __tb_gstream_t* gst, tb_int64_t offset);

	// size
	tb_uint64_t 		(*size)(struct __tb_gstream_t* gst);

	// free
	tb_void_t 			(*free)(struct __tb_gstream_t* gst);

	// ioctl
	tb_bool_t 			(*ioctl0)(struct __tb_gstream_t* gst, tb_size_t cmd);
	tb_bool_t 			(*ioctl1)(struct __tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1);
	tb_bool_t 			(*ioctl2)(struct __tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1, tb_pointer_t arg2);

}tb_gstream_t;


/* /////////////////////////////////////////////////////////
 * interfaces
 */

// exit stream
tb_void_t 			tb_gstream_exit(tb_gstream_t* gst);

// init stream
tb_gstream_t* 		tb_gstream_init_data();
tb_gstream_t* 		tb_gstream_init_file();
tb_gstream_t* 		tb_gstream_init_sock();
tb_gstream_t* 		tb_gstream_init_http();
tb_gstream_t* 		tb_gstream_init_zip();
tb_gstream_t* 		tb_gstream_init_encoding();

/* init stream from url
 *
 * file://path or unix path: e.g. /root/xxxx/file
 * sock://host:port?tcp=&ssl=
 * http://host:port/path?arg0=&arg1=...
 * https://host:port/path?arg0=&arg1=...
 */
tb_gstream_t* 		tb_gstream_init_from_url(tb_char_t const* url);

// init stream from data
tb_gstream_t* 		tb_gstream_init_from_data(tb_byte_t const* data, tb_size_t size);

// init stream from file
tb_gstream_t* 		tb_gstream_init_from_file(tb_char_t const* path);

// init stream from sock
tb_gstream_t* 		tb_gstream_init_from_sock(tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl);

// init stream from http or https
tb_gstream_t* 		tb_gstream_init_from_http(tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl);

// init stream from zip
tb_gstream_t* 		tb_gstream_init_from_zip(tb_gstream_t* gst, tb_size_t algo, tb_size_t action);

// init stream from encoding
tb_gstream_t* 		tb_gstream_init_from_encoding(tb_gstream_t* gst, tb_size_t ie, tb_size_t oe);

// async open, allow multiple called before closing 
tb_long_t 			tb_gstream_aopen(tb_gstream_t* gst);

// block open, allow multiple called before closing 
tb_bool_t 			tb_gstream_bopen(tb_gstream_t* gst);

// async close, allow multiple called
tb_long_t 			tb_gstream_aclose(tb_gstream_t* gst);

// block close, allow multiple called
tb_bool_t 			tb_gstream_bclose(tb_gstream_t* gst);

// async flush read & writ data
tb_long_t 			tb_gstream_afread(tb_gstream_t* gst);
tb_long_t 			tb_gstream_afwrit(tb_gstream_t* gst);

// block flush read & writ data
tb_bool_t 			tb_gstream_bfread(tb_gstream_t* gst);
tb_bool_t 			tb_gstream_bfwrit(tb_gstream_t* gst);

// async read & writ data
tb_long_t 			tb_gstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);
tb_long_t 			tb_gstream_awrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);

// block read & writ data
tb_bool_t 			tb_gstream_bread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);
tb_bool_t 			tb_gstream_bwrit(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);

// block read & writ line
tb_long_t 			tb_gstream_bread_line(tb_gstream_t* gst, tb_char_t* data, tb_size_t size);
tb_long_t 			tb_gstream_bwrit_line(tb_gstream_t* gst, tb_char_t* data, tb_size_t size);

// async need data
tb_long_t 			tb_gstream_aneed(tb_gstream_t* gst, tb_byte_t** data, tb_size_t size);

// block need data
tb_bool_t 			tb_gstream_bneed(tb_gstream_t* gst, tb_byte_t** data, tb_size_t size);

// seek
tb_bool_t 			tb_gstream_seek(tb_gstream_t* gst, tb_int64_t offset, tb_size_t flag);

// skip
tb_bool_t 			tb_gstream_skip(tb_gstream_t* gst, tb_size_t size);

// format writ data - blocked
tb_long_t 			tb_gstream_printf(tb_gstream_t* gst, tb_char_t const* fmt, ...);

// load & save data
tb_uint64_t 		tb_gstream_load(tb_gstream_t* gst, tb_gstream_t* ist);
tb_uint64_t 		tb_gstream_save(tb_gstream_t* gst, tb_gstream_t* ost);

// block read integer
tb_uint8_t 			tb_gstream_bread_u8(tb_gstream_t* gst);
tb_sint8_t 			tb_gstream_bread_s8(tb_gstream_t* gst);

tb_uint16_t 		tb_gstream_bread_u16_le(tb_gstream_t* gst);
tb_sint16_t 		tb_gstream_bread_s16_le(tb_gstream_t* gst);

tb_uint32_t 		tb_gstream_bread_u24_le(tb_gstream_t* gst);
tb_sint32_t 		tb_gstream_bread_s24_le(tb_gstream_t* gst);

tb_uint32_t 		tb_gstream_bread_u32_le(tb_gstream_t* gst);
tb_sint32_t 		tb_gstream_bread_s32_le(tb_gstream_t* gst);

tb_uint16_t 		tb_gstream_bread_u16_be(tb_gstream_t* gst);
tb_sint16_t 		tb_gstream_bread_s16_be(tb_gstream_t* gst);

tb_uint32_t 		tb_gstream_bread_u24_be(tb_gstream_t* gst);
tb_sint32_t 		tb_gstream_bread_s24_be(tb_gstream_t* gst);

tb_uint32_t 		tb_gstream_bread_u32_be(tb_gstream_t* gst);
tb_sint32_t 		tb_gstream_bread_s32_be(tb_gstream_t* gst);

// block writ integer
tb_bool_t			tb_gstream_bwrit_u8(tb_gstream_t* gst, tb_uint8_t val);
tb_bool_t 			tb_gstream_bwrit_s8(tb_gstream_t* gst, tb_sint8_t val);

tb_bool_t 			tb_gstream_bwrit_u16_le(tb_gstream_t* gst, tb_uint16_t val);
tb_bool_t 			tb_gstream_bwrit_s16_le(tb_gstream_t* gst, tb_sint16_t val);

tb_bool_t 			tb_gstream_bwrit_u24_le(tb_gstream_t* gst, tb_uint32_t val);
tb_bool_t 			tb_gstream_bwrit_s24_le(tb_gstream_t* gst, tb_sint32_t val);

tb_bool_t 			tb_gstream_bwrit_u32_le(tb_gstream_t* gst, tb_uint32_t val);
tb_bool_t 			tb_gstream_bwrit_s32_le(tb_gstream_t* gst, tb_sint32_t val);

tb_bool_t 			tb_gstream_bwrit_u16_be(tb_gstream_t* gst, tb_uint16_t val);
tb_bool_t 			tb_gstream_bwrit_s16_be(tb_gstream_t* gst, tb_sint16_t val);

tb_bool_t 			tb_gstream_bwrit_u24_be(tb_gstream_t* gst, tb_uint32_t val);
tb_bool_t 			tb_gstream_bwrit_s24_be(tb_gstream_t* gst, tb_sint32_t val);

tb_bool_t 			tb_gstream_bwrit_u32_be(tb_gstream_t* gst, tb_uint32_t val);
tb_bool_t 			tb_gstream_bwrit_s32_be(tb_gstream_t* gst, tb_sint32_t val);

// status
tb_size_t 			tb_gstream_type(tb_gstream_t const* gst);
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

