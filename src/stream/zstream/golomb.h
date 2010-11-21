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
 * \file		tstream.h
 *
 */
#ifndef TB_STREAM_ZSTREAM_LZ77_H
#define TB_STREAM_ZSTREAM_LZ77_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the lz77 inflate zstream type
typedef struct __tb_lz77_inflate_zstream_t
{
	// the stream base
	tb_inflate_zstream_t 		base;

}tb_lz77_inflate_zstream_t;

// the lz77 deflate zstream type
typedef struct __tb_lz77_deflate_zstream_t
{
	// the stream base
	tb_deflate_zstream_t 		base;

}tb_lz77_deflate_zstream_t;


// the lz77 zstream type
typedef union __tb_lz77_zstream_t
{
	tb_lz77_inflate_zstream_t 	infst;
	tb_lz77_deflate_zstream_t 	defst;

}tb_lz77_zstream_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_tstream_t* 	tb_zstream_open_lz77_inflate(tb_lz77_inflate_zstream_t* zst);
tb_tstream_t* 	tb_zstream_open_lz77_deflate(tb_lz77_deflate_zstream_t* zst);
tb_tstream_t* 	tb_zstream_open_lz77(tb_lz77_zstream_t* zst, tb_size_t action);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

