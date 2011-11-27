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
 * \file		tstream.h
 *
 */
#ifndef TB_STREAM_TSTREAM_H
#define TB_STREAM_TSTREAM_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../gstream.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the tstream status
typedef enum __tb_tstream_status_t
{
 	TB_TSTREAM_STATUS_OK 	= 0
, 	TB_TSTREAM_STATUS_END 	= 1
,	TB_TSTREAM_STATUS_FAIL 	= 2

}tb_tstream_status_t;

// the transform stream type
typedef struct __tb_tstream_t
{
	// the stream base
	tb_gstream_t 		base;

	// the type
	tb_size_t 			type;

	// the status
	tb_size_t 			status;

	// the reference to gstream
	tb_gstream_t* 		gst;
	
	// the input data
	tb_byte_t 			ib[TB_GSTREAM_BLOCK_SIZE];
	tb_byte_t const* 	ip;
	tb_size_t 			in;

	// the output data
	tb_byte_t 			ob[TB_GSTREAM_BLOCK_SIZE];
	tb_byte_t* 			op;
	tb_size_t 			on;

	// spank
	tb_bool_t 			(*spank)(tb_gstream_t* gst);

}tb_tstream_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_tstream_t* 	tb_tstream_cast(tb_gstream_t* gst);

tb_bool_t 		tb_tstream_open(tb_gstream_t* gst);
tb_void_t 		tb_tstream_close(tb_gstream_t* gst);

tb_int_t 		tb_tstream_read(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size);

tb_bool_t 		tb_tstream_ioctl0(tb_gstream_t* gst, tb_size_t cmd);
tb_bool_t 		tb_tstream_ioctl1(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1);
tb_bool_t 		tb_tstream_ioctl2(tb_gstream_t* gst, tb_size_t cmd, tb_pointer_t arg1, tb_pointer_t arg2);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
