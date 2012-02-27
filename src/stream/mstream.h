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
 * \author		ruki
 * \file		mstream.h
 *
 */
#ifndef TB_STREAM_MSTREAM_H
#define TB_STREAM_MSTREAM_H

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
#define TB_MSTREAM_CMD(type, cmd) 				(((type) << 16) | (cmd))
#define TB_TSTREAM_CMD(type, cmd) 				TB_MSTREAM_CMD(TB_MSTREAM_TYPE_TRAN, (((type) << 8) | (cmd)))

// the stream block maxn
#define TB_MSTREAM_BLOCK_MAXN 					(8192)

// the stream bitops
#ifdef TB_WORDS_BIGENDIAN
# 	define tb_mstream_bread_u16_ne(mst) 		tb_mstream_bread_u16_be(mst)
# 	define tb_mstream_bread_s16_ne(mst) 		tb_mstream_bread_s16_be(mst)
# 	define tb_mstream_bread_u24_ne(mst) 		tb_mstream_bread_u24_be(mst)
# 	define tb_mstream_bread_s24_ne(mst) 		tb_mstream_bread_s24_be(mst)
# 	define tb_mstream_bread_u32_ne(mst) 		tb_mstream_bread_u32_be(mst)
# 	define tb_mstream_bread_s32_ne(mst) 		tb_mstream_bread_s32_be(mst)

# 	define tb_mstream_bwrit_u16_ne(mst, val) 	tb_mstream_bwrit_u16_be(mst, val)
# 	define tb_mstream_bwrit_s16_ne(mst, val)	tb_mstream_bwrit_s16_be(mst, val)
# 	define tb_mstream_bwrit_u24_ne(mst, val) 	tb_mstream_bwrit_u24_be(mst, val)
# 	define tb_mstream_bwrit_s24_ne(mst, val)	tb_mstream_bwrit_s24_be(mst, val)
# 	define tb_mstream_bwrit_u32_ne(mst, val)	tb_mstream_bwrit_u32_be(mst, val)
# 	define tb_mstream_bwrit_s32_ne(mst, val) 	tb_mstream_bwrit_s32_be(mst, val)

#else
# 	define tb_mstream_bread_u16_ne(mst) 		tb_mstream_bread_u16_le(mst)
# 	define tb_mstream_bread_s16_ne(mst) 		tb_mstream_bread_s16_le(mst)
# 	define tb_mstream_bread_u24_ne(mst) 		tb_mstream_bread_u24_le(mst)
# 	define tb_mstream_bread_s24_ne(mst) 		tb_mstream_bread_s24_le(mst)
# 	define tb_mstream_bread_u32_ne(mst) 		tb_mstream_bread_u32_le(mst)
# 	define tb_mstream_bread_s32_ne(mst) 		tb_mstream_bread_s32_le(mst)

# 	define tb_mstream_bwrit_u16_ne(mst, val) 	tb_mstream_bwrit_u16_le(mst, val)
# 	define tb_mstream_bwrit_s16_ne(mst, val)	tb_mstream_bwrit_s16_le(mst, val)
# 	define tb_mstream_bwrit_u24_ne(mst, val) 	tb_mstream_bwrit_u24_le(mst, val)
# 	define tb_mstream_bwrit_s24_ne(mst, val)	tb_mstream_bwrit_s24_le(mst, val)
# 	define tb_mstream_bwrit_u32_ne(mst, val)	tb_mstream_bwrit_u32_le(mst, val)
# 	define tb_mstream_bwrit_s32_ne(mst, val) 	tb_mstream_bwrit_s32_le(mst, val)

#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the stream seek type
typedef enum __tb_mstream_seek_t
{
 	TB_MSTREAM_SEEK_BEG 			= 0
, 	TB_MSTREAM_SEEK_CUR 			= 1
, 	TB_MSTREAM_SEEK_END 			= 2

}tb_mstream_seek_t;

// the mstream type
typedef enum __tb_mstream_type_t
{
 	TB_MSTREAM_TYPE_NULL 			= 0
, 	TB_MSTREAM_TYPE_FILE 			= 1
, 	TB_MSTREAM_TYPE_SOCK 			= 2
, 	TB_MSTREAM_TYPE_HTTP 			= 3
, 	TB_MSTREAM_TYPE_DATA 			= 4
, 	TB_MSTREAM_TYPE_TRAN 			= 5

}tb_mstream_type_t;

// the tstream type
typedef enum __tb_tstream_type_t
{
 	TB_TSTREAM_TYPE_NULL 			= 0
, 	TB_TSTREAM_TYPE_ENCODING 		= 1
, 	TB_TSTREAM_TYPE_ZIP 			= 2

}tb_tstream_type_t;

// the mstream command type
typedef enum __tb_mstream_cmd_t
{
	TB_MSTREAM_CMD_NULL 				= 0

	// the mstream
,	TB_MSTREAM_CMD_GET_URL 				= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 1)
,	TB_MSTREAM_CMD_GET_HOST 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 2)
,	TB_MSTREAM_CMD_GET_PORT 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 3)
,	TB_MSTREAM_CMD_GET_PATH 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 4)
,	TB_MSTREAM_CMD_GET_SSL 				= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 5)
,	TB_MSTREAM_CMD_GET_CACHE 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 6)
,	TB_MSTREAM_CMD_GET_TIMEOUT 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 7)

,	TB_MSTREAM_CMD_SET_URL 				= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 11)
,	TB_MSTREAM_CMD_SET_HOST 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 12)
,	TB_MSTREAM_CMD_SET_PORT 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 13)
,	TB_MSTREAM_CMD_SET_PATH 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 14)
,	TB_MSTREAM_CMD_SET_SSL 				= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 15)
,	TB_MSTREAM_CMD_SET_CACHE 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 16)
,	TB_MSTREAM_CMD_SET_TIMEOUT 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_NULL, 17)

	// the dstream
,	TB_DSTREAM_CMD_SET_DATA 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_DATA, 1)

	// the fstream
,	TB_FSTREAM_CMD_SET_FLAGS 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_FILE, 1)

	// the sstream
,	TB_SSTREAM_CMD_SET_TYPE 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_SOCK, 1)

	// the hstream
,	TB_HSTREAM_CMD_GET_OPTION 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_HTTP, 1)
,	TB_HSTREAM_CMD_GET_STATUS 			= TB_MSTREAM_CMD(TB_MSTREAM_TYPE_HTTP, 2)

	// the tstream
,	TB_TSTREAM_CMD_GET_MSTREAM 			= TB_TSTREAM_CMD(TB_TSTREAM_TYPE_NULL, 1)
,	TB_TSTREAM_CMD_SET_MSTREAM 			= TB_TSTREAM_CMD(TB_TSTREAM_TYPE_NULL, 2)

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

}tb_mstream_cmd_t;

// the generic stream type
typedef struct __tb_mstream_t
{	
	// the stream type
	tb_size_t 			type 		: 8;

	// is opened?
	tb_size_t 			bopened 	: 1;

	// is writed?
	tb_size_t 			bwrited 	: 1;

	// the timeout: ms
	tb_size_t 			timeout 	: 22;

	// the url
	tb_url_t 			url;

	// the cache
	tb_qbuffer_t 		cache;

	// the offset
	tb_hize_t 			offset;

	// the bare handle for aio
	tb_handle_t 		(*bare)(struct __tb_mstream_t* mst);

	// wait the aio event
	tb_long_t 			(*wait)(struct __tb_mstream_t* mst, tb_size_t etype, tb_long_t timeout);

	// async open
	tb_long_t 			(*aopen)(struct __tb_mstream_t* mst);

	// async close
	tb_long_t 			(*aclose)(struct __tb_mstream_t* mst);

	// async read
	tb_long_t 			(*aread)(struct __tb_mstream_t* mst, tb_byte_t* data, tb_size_t size, tb_bool_t sync);

	// async writ
	tb_long_t 			(*awrit)(struct __tb_mstream_t* mst, tb_byte_t* data, tb_size_t size, tb_bool_t sync);

	// seek
	tb_long_t 			(*aseek)(struct __tb_mstream_t* mst, tb_hize_t offset);

	// size
	tb_hize_t 			(*size)(struct __tb_mstream_t* mst);

	// free
	tb_void_t 			(*free)(struct __tb_mstream_t* mst);

	// ctrl
	tb_bool_t 			(*ctrl)(struct __tb_mstream_t* mst, tb_size_t cmd, tb_va_list_t args);

}tb_mstream_t;


/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// exit stream
tb_void_t 			tb_mstream_exit(tb_mstream_t* mst);

// init stream
tb_mstream_t* 		tb_mstream_init_data();
tb_mstream_t* 		tb_mstream_init_file();
tb_mstream_t* 		tb_mstream_init_sock();
tb_mstream_t* 		tb_mstream_init_http();
tb_mstream_t* 		tb_mstream_init_null();
tb_mstream_t* 		tb_mstream_init_zip();
tb_mstream_t* 		tb_mstream_init_encoding();
tb_bool_t 			tb_mstream_init(tb_mstream_t* mst);

/* init stream from url
 *
 * file://path or unix path: e.g. /root/xxxx/file
 * files://home/file
 * sock://host:port?tcp=
 * socks://host:port?udp=
 * http://host:port/path?arg0=&arg1=...
 * https://host:port/path?arg0=&arg1=...
 */
tb_mstream_t* 		tb_mstream_init_from_url(tb_char_t const* url);

// init stream from data
tb_mstream_t* 		tb_mstream_init_from_data(tb_byte_t const* data, tb_size_t size);

// init stream from file
tb_mstream_t* 		tb_mstream_init_from_file(tb_char_t const* path);

// init stream from sock
tb_mstream_t* 		tb_mstream_init_from_sock(tb_char_t const* host, tb_size_t port, tb_size_t type, tb_bool_t bssl);

// init stream from http or https
tb_mstream_t* 		tb_mstream_init_from_http(tb_char_t const* host, tb_size_t port, tb_char_t const* path, tb_bool_t bssl);

// init stream from null
tb_mstream_t* 		tb_mstream_init_from_null(tb_mstream_t* mst);

// init stream from zip
tb_mstream_t* 		tb_mstream_init_from_zip(tb_mstream_t* mst, tb_size_t algo, tb_size_t action);

// init stream from encoding
tb_mstream_t* 		tb_mstream_init_from_encoding(tb_mstream_t* mst, tb_size_t ie, tb_size_t oe);

// the bare handle for aio
tb_handle_t 		tb_mstream_bare(tb_mstream_t* mst);

/*!wait the mstream 
 *
 * blocking wait the single event object, so need not aiop 
 * return the event type if ok, otherwise return 0 for timeout
 *
 * @param 	mst 	the mstream 
 * @param 	etype 	the waited event type, return the needed event type if TB_AIOO_ETYPE_NULL
 * @param 	timeout the timeout value, return immediately if 0, infinity if -1
 *
 * @return 	the event type, return 0 if timeout, return -1 if error
 */
tb_long_t 			tb_mstream_wait(tb_mstream_t* mst, tb_size_t etype, tb_long_t timeout);

// clear stream
tb_void_t 			tb_mstream_clear(tb_mstream_t* mst);

// async open, allow multiple called before closing 
tb_long_t 			tb_mstream_aopen(tb_mstream_t* mst);

// block open, allow multiple called before closing 
tb_bool_t 			tb_mstream_bopen(tb_mstream_t* mst);

// async close, allow multiple called
tb_long_t 			tb_mstream_aclose(tb_mstream_t* mst);

// block close, allow multiple called
tb_bool_t 			tb_mstream_bclose(tb_mstream_t* mst);

// async read & writ data
tb_long_t 			tb_mstream_aread(tb_mstream_t* mst, tb_byte_t* data, tb_size_t size);
tb_long_t 			tb_mstream_awrit(tb_mstream_t* mst, tb_byte_t* data, tb_size_t size);

// block read & writ data
tb_bool_t 			tb_mstream_bread(tb_mstream_t* mst, tb_byte_t* data, tb_size_t size);
tb_bool_t 			tb_mstream_bwrit(tb_mstream_t* mst, tb_byte_t* data, tb_size_t size);

// async fread & fwrit data
tb_long_t 			tb_mstream_afread(tb_mstream_t* mst, tb_byte_t* data, tb_size_t size);
tb_long_t 			tb_mstream_afwrit(tb_mstream_t* mst, tb_byte_t* data, tb_size_t size);

// block fread & fwrit data
tb_bool_t 			tb_mstream_bfread(tb_mstream_t* mst, tb_byte_t* data, tb_size_t size);
tb_bool_t 			tb_mstream_bfwrit(tb_mstream_t* mst, tb_byte_t* data, tb_size_t size);

// need
tb_long_t 			tb_mstream_aneed(tb_mstream_t* mst, tb_byte_t** data, tb_size_t size);
tb_bool_t 			tb_mstream_bneed(tb_mstream_t* mst, tb_byte_t** data, tb_size_t size);

// seek
tb_long_t 			tb_mstream_aseek(tb_mstream_t* mst, tb_hize_t offset);
tb_bool_t 			tb_mstream_bseek(tb_mstream_t* mst, tb_hize_t offset);

// skip
tb_long_t 			tb_mstream_askip(tb_mstream_t* mst, tb_hize_t size);
tb_bool_t 			tb_mstream_bskip(tb_mstream_t* mst, tb_hize_t size);

// block writ format data
tb_long_t 			tb_mstream_printf(tb_mstream_t* mst, tb_char_t const* fmt, ...);

// block load & save data
tb_hize_t 			tb_mstream_load(tb_mstream_t* mst, tb_mstream_t* ist);
tb_hize_t 			tb_mstream_save(tb_mstream_t* mst, tb_mstream_t* ost);

// block read & writ line
tb_long_t 			tb_mstream_bread_line(tb_mstream_t* mst, tb_char_t* data, tb_size_t size);
tb_long_t 			tb_mstream_bwrit_line(tb_mstream_t* mst, tb_char_t* data, tb_size_t size);

// block read integer
tb_uint8_t 			tb_mstream_bread_u8(tb_mstream_t* mst);
tb_sint8_t 			tb_mstream_bread_s8(tb_mstream_t* mst);

tb_uint16_t 		tb_mstream_bread_u16_le(tb_mstream_t* mst);
tb_sint16_t 		tb_mstream_bread_s16_le(tb_mstream_t* mst);

tb_uint32_t 		tb_mstream_bread_u24_le(tb_mstream_t* mst);
tb_sint32_t 		tb_mstream_bread_s24_le(tb_mstream_t* mst);

tb_uint32_t 		tb_mstream_bread_u32_le(tb_mstream_t* mst);
tb_sint32_t 		tb_mstream_bread_s32_le(tb_mstream_t* mst);

tb_uint16_t 		tb_mstream_bread_u16_be(tb_mstream_t* mst);
tb_sint16_t 		tb_mstream_bread_s16_be(tb_mstream_t* mst);

tb_uint32_t 		tb_mstream_bread_u24_be(tb_mstream_t* mst);
tb_sint32_t 		tb_mstream_bread_s24_be(tb_mstream_t* mst);

tb_uint32_t 		tb_mstream_bread_u32_be(tb_mstream_t* mst);
tb_sint32_t 		tb_mstream_bread_s32_be(tb_mstream_t* mst);

// block writ integer
tb_bool_t			tb_mstream_bwrit_u8(tb_mstream_t* mst, tb_uint8_t val);
tb_bool_t 			tb_mstream_bwrit_s8(tb_mstream_t* mst, tb_sint8_t val);

tb_bool_t 			tb_mstream_bwrit_u16_le(tb_mstream_t* mst, tb_uint16_t val);
tb_bool_t 			tb_mstream_bwrit_s16_le(tb_mstream_t* mst, tb_sint16_t val);

tb_bool_t 			tb_mstream_bwrit_u24_le(tb_mstream_t* mst, tb_uint32_t val);
tb_bool_t 			tb_mstream_bwrit_s24_le(tb_mstream_t* mst, tb_sint32_t val);

tb_bool_t 			tb_mstream_bwrit_u32_le(tb_mstream_t* mst, tb_uint32_t val);
tb_bool_t 			tb_mstream_bwrit_s32_le(tb_mstream_t* mst, tb_sint32_t val);

tb_bool_t 			tb_mstream_bwrit_u16_be(tb_mstream_t* mst, tb_uint16_t val);
tb_bool_t 			tb_mstream_bwrit_s16_be(tb_mstream_t* mst, tb_sint16_t val);

tb_bool_t 			tb_mstream_bwrit_u24_be(tb_mstream_t* mst, tb_uint32_t val);
tb_bool_t 			tb_mstream_bwrit_s24_be(tb_mstream_t* mst, tb_sint32_t val);

tb_bool_t 			tb_mstream_bwrit_u32_be(tb_mstream_t* mst, tb_uint32_t val);
tb_bool_t 			tb_mstream_bwrit_s32_be(tb_mstream_t* mst, tb_sint32_t val);

// status
tb_size_t 			tb_mstream_type(tb_mstream_t const* mst);
tb_hize_t 			tb_mstream_size(tb_mstream_t const* mst);
tb_hize_t 			tb_mstream_left(tb_mstream_t const* mst);
tb_hize_t 			tb_mstream_offset(tb_mstream_t const* mst);
tb_size_t 			tb_mstream_timeout(tb_mstream_t const* mst);

// ctrl
tb_bool_t 			tb_mstream_ctrl(tb_mstream_t* mst, tb_size_t cmd, ...);

#endif

