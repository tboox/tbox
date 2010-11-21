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
 * \file		zstream.h
 *
 */
#ifndef TB_STREAM_ZSTREAM_ZSTREAM_H
#define TB_STREAM_ZSTREAM_ZSTREAM_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../tstream.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the zstream action type
typedef enum __tb_zstream_action_t
{
	TB_ZSTREAM_ACTION_NULL 		= 1
,	TB_ZSTREAM_ACTION_INFLATE 	= 2
,	TB_ZSTREAM_ACTION_DEFLATE 	= 3

}tb_zstream_action_t;

// the zstream algorithm type
typedef enum __tb_zstream_algo_t
{
	TB_ZSTREAM_ALGO_NULL 		= 0
,	TB_ZSTREAM_ALGO_RLE 		= 1
,	TB_ZSTREAM_ALGO_HUFFMAN 	= 2
,	TB_ZSTREAM_ALGO_LZ77 		= 3
,	TB_ZSTREAM_ALGO_LZW 		= 4
,	TB_ZSTREAM_ALGO_ZLIB 		= 5
,	TB_ZSTREAM_ALGO_ALGORITHM 	= 6
,	TB_ZSTREAM_ALGO_ZIX 		= 7

}tb_zstream_algo_t;

// the zstream type
typedef struct __tb_zstream_t
{
	// the stream base
	tb_tstream_t 		base;

	// the algorithm
	tb_uint8_t 			algo;

	// the action
	tb_uint8_t 			action;

}tb_zstream_t;

// the inflate zstream type
typedef struct __tb_inflate_zstream_t
{
	// the stream base
	tb_zstream_t 		base;

}tb_inflate_zstream_t;


// the deflate zstream type
typedef struct __tb_deflate_zstream_t
{
	// the stream base
	tb_zstream_t 		base;

}tb_deflate_zstream_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_zstream_algo_t 	tb_zstream_algo(tb_zstream_t* zst);
tb_zstream_action_t tb_zstream_action(tb_zstream_t* zst);
tb_char_t const* 	tb_zstream_name(tb_zstream_t* zst);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
