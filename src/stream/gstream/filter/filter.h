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
 * @file		filter.h
 *
 */
#ifndef TB_STREAM_GSTREAM_FLTR_H
#define TB_STREAM_GSTREAM_FLTR_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../gstream.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the gstream filter cache maxn
#define TB_GSTREAM_FLTR_CACHE_MAXN 					(TB_GSTREAM_BLOCK_MAXN)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the transform stream type
typedef struct __tb_gstream_filter_t
{
	// the stream base
	tb_gstream_t 		base;

	// the type
	tb_size_t 			type;

	// the last read & writ
	tb_long_t 			read;
	tb_long_t 			writ;

	// the reference to gstream
	tb_gstream_t* 		gstream;

	// the input data
	tb_byte_t 			ib[TB_GSTREAM_FLTR_CACHE_MAXN];
	tb_byte_t const* 	ip;
	tb_size_t 			in;

	// the output data
	tb_byte_t 			ob[TB_GSTREAM_FLTR_CACHE_MAXN];
	tb_byte_t* 			op;
	tb_size_t 			on;

	// spak
	tb_long_t 			(*spak)(tb_gstream_t* gstream, tb_long_t sync);

}tb_gstream_filter_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// cast
tb_gstream_filter_t* 	tb_gstream_filter_cast(tb_gstream_t* gstream);

// open 
tb_long_t 				tb_gstream_filter_open(tb_gstream_t* gstream);

// close
tb_long_t 				tb_gstream_filter_clos(tb_gstream_t* gstream);

// read
tb_long_t 				tb_gstream_filter_read(tb_gstream_t* gstream, tb_byte_t* data, tb_size_t size, tb_bool_t sync);

// wait
tb_long_t 				tb_gstream_filter_wait(tb_gstream_t* gstream, tb_size_t wait, tb_long_t timeout);

// ctrl
tb_bool_t 				tb_gstream_filter_ctrl(tb_gstream_t* gstream, tb_size_t cmd, tb_va_list_t args);

#endif
