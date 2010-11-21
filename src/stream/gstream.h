/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
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
#include "../bits.h"
#include "../string/string.h"

#ifdef TB_CONFIG_ZLIB
# 	include "../zlib.h"
#endif
/* /////////////////////////////////////////////////////////
 * macros
 */

#ifdef TB_MEMORY_MODE_SMALL
# 	define TB_GSTREAM_DATA_MAX 		(4096)
# 	define TB_GSTREAM_ZDATA_MAX 	(TB_GSTREAM_DATA_MAX >> 1)
#else
# 	define TB_GSTREAM_DATA_MAX 		(8192)
# 	define TB_GSTREAM_ZDATA_MAX 	(TB_GSTREAM_DATA_MAX >> 1)
#endif

/* /////////////////////////////////////////////////////////
 * types
 */

// the stream flag type
typedef enum __tb_gstream_flag_t
{
	TB_GSTREAM_FLAG_NULL 			= 0
, 	TB_GSTREAM_FLAG_RO 				= 1
, 	TB_GSTREAM_FLAG_WO 				= 2
, 	TB_GSTREAM_FLAG_TRUNC 			= 4
, 	TB_GSTREAM_FLAG_ZLIB 			= 8
, 	TB_GSTREAM_FLAG_BLOCK 			= 16


}tb_gstream_flag_t;

// the stream seek type
typedef enum __tb_gstream_seek_t
{
 	TB_GSTREAM_SEEK_BEG 				= 0
, 	TB_GSTREAM_SEEK_CUR 				= 1
, 	TB_GSTREAM_SEEK_END 				= 2

}tb_gstream_seek_t;

// the generic stream type
typedef struct __tb_gstream_t
{
	// the stream flag
	tb_uint32_t 		flag;

	// the stream data
	tb_byte_t 			data[TB_GSTREAM_DATA_MAX];
	tb_byte_t* 			head;
	tb_size_t 			size;
	tb_size_t 			offset;

	// the private data
	tb_byte_t* 			pdata;

#ifdef TB_CONFIG_ZLIB

	// for zlib
	tb_handle_t 		hzlib;

	// the stream zdata, size must be TB_GSTREAM_DATA_MAX for tb_gstream_switch
	tb_byte_t 			zdata[TB_GSTREAM_DATA_MAX];

#endif

	// the url
	tb_string_t 		url;

	// the operations 
	tb_int_t 			(*read)(struct __tb_gstream_t* st, tb_byte_t* data, tb_size_t size);
	tb_int_t 			(*write)(struct __tb_gstream_t* st, tb_byte_t* data, tb_size_t size);
	tb_bool_t 			(*seek)(struct __tb_gstream_t* st, tb_int_t offset, tb_gstream_seek_t flag);
	tb_byte_t* 			(*need)(struct __tb_gstream_t* st, tb_size_t size);
	void 				(*close)(struct __tb_gstream_t* st);
	tb_size_t 			(*ssize)(struct __tb_gstream_t* st);

}tb_gstream_t;

// the http stream
typedef struct __tb_hstream_t
{
	tb_gstream_t 		base;

}tb_hstream_t;

// the file stream
typedef struct __tb_dstream_t
{
	tb_gstream_t 		base;
	tb_byte_t* 			data;
	tb_byte_t* 			head;
	tb_size_t 			size;

}tb_dstream_t;

// the file stream
typedef struct __tb_fstream_t
{
	tb_gstream_t 		base;
	tplat_handle_t 		hfile;

}tb_fstream_t;

// the union stream
typedef struct __tb_ustream_t
{
	// streams
	union
	{
		tb_dstream_t 	data;
		tb_hstream_t 	http;
		tb_fstream_t 	file;

	}u;

	// the reference to the specific stream
	tb_gstream_t* 			st;

}tb_ustream_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// the stream operation
tb_gstream_t* 		tb_gstream_open_from_http(tb_hstream_t* st, tb_char_t const* url, tb_gstream_flag_t flag);
tb_gstream_t* 		tb_gstream_open_from_file(tb_fstream_t* st, tb_char_t const* url, tb_gstream_flag_t flag);
tb_gstream_t* 		tb_gstream_open_from_data(tb_dstream_t* st, tb_byte_t const* data, tb_size_t size, tb_gstream_flag_t flag);
tb_gstream_t* 		tb_gstream_open(tb_ustream_t* st, tb_char_t const* url, tb_byte_t const* data, tb_size_t size, tb_gstream_flag_t flag);
void 				tb_gstream_close(tb_gstream_t* st);

tb_int_t 			tb_gstream_read(tb_gstream_t* st, tb_byte_t* data, tb_size_t size);
tb_int_t 			tb_gstream_write(tb_gstream_t* st, tb_byte_t* data, tb_size_t size);
tb_byte_t* 			tb_gstream_need(tb_gstream_t* st, tb_size_t size);
tb_bool_t 			tb_gstream_seek(tb_gstream_t* st, tb_int_t offset, tb_gstream_seek_t flag);
void 				tb_gstream_flush(tb_gstream_t* st);
tb_int_t 			tb_gstream_printf(tb_gstream_t* st, tb_char_t const* fmt, ...);
tb_size_t 			tb_gstream_size(tb_gstream_t const* st);
tb_size_t 			tb_gstream_offset(tb_gstream_t const* st);
tb_gstream_flag_t 	tb_gstream_flag(tb_gstream_t const* st);
tb_bool_t 			tb_gstream_switch(tb_gstream_t* st, tb_gstream_flag_t flag);
tb_char_t const* 	tb_gstream_url(tb_gstream_t const* st);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

