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
 * @file		tstream.h
 *
 */
#ifndef TB_STREAM_TSTREAM_H
#define TB_STREAM_TSTREAM_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../gstream.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the tstream cache maxn
#define TB_TSTREAM_CACHE_MAXN 					(TB_GSTREAM_BLOCK_MAXN)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the transform stream type
typedef struct __tb_tstream_t
{
	// the stream base
	tb_gstream_t 		base;

	// the type
	tb_size_t 			type;

	// the last read & writ
	tb_long_t 			read;
	tb_long_t 			writ;

	// the reference to gstream
	tb_gstream_t* 		gst;

	// the input data
	tb_byte_t 			ib[TB_TSTREAM_CACHE_MAXN];
	tb_byte_t const* 	ip;
	tb_size_t 			in;

	// the output data
	tb_byte_t 			ob[TB_TSTREAM_CACHE_MAXN];
	tb_byte_t* 			op;
	tb_size_t 			on;

	// spak
	tb_long_t 			(*spak)(tb_gstream_t* gst, tb_long_t sync);

}tb_tstream_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// cast
tb_tstream_t* 	tb_tstream_cast(tb_gstream_t* gst);

// open & close 
tb_long_t 		tb_tstream_aopen(tb_gstream_t* gst);
tb_long_t 		tb_tstream_aclose(tb_gstream_t* gst);

// read
tb_long_t 		tb_tstream_aread(tb_gstream_t* gst, tb_byte_t* data, tb_size_t size, tb_bool_t sync);

// wait
tb_long_t 		tb_tstream_wait(tb_gstream_t* gst, tb_size_t wait, tb_long_t timeout);

// ctrl
tb_bool_t 		tb_tstream_ctrl(tb_gstream_t* gst, tb_size_t cmd, tb_va_list_t args);

#endif
