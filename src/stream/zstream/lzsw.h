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
 * \file		lzsw.h
 *
 */
#ifndef TB_STREAM_ZSTREAM_LZSW_H
#define TB_STREAM_ZSTREAM_LZSW_H

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

// vlc 
#define TB_LZSW_VLC_TYPE_FIXED 			(0)
#define TB_LZSW_VLC_TYPE_GAMMA 			(1)
#define TB_LZSW_VLC_TYPE_GOLOMB 		(0)

// window
//#define TB_LZSW_WINDOW_SIZE_MAX 		(256)
#define TB_LZSW_WINDOW_SIZE_MAX 		(10)
//#define TB_LZSW_WINDOW_SIZE_MAX 		(65535)

#define TB_LZSW_WINDOW_FIND_HASH 		(1)
#define TB_LZSW_WINDOW_MAX				(768) 	// (256 + 256 + 256)

/* /////////////////////////////////////////////////////////
 * types
 */

#if TB_LZSW_WINDOW_FIND_HASH
// the node type
typedef struct __tb_lzsw_node_t
{
	// the signature
	tb_byte_t 					sign[3];

	// the circle offset
	tb_size_t 					coff;

	// the global offset
	//tb_size_t 					goff;

	// the next & prev
	tb_size_t 					prev;
	tb_size_t 					next;

}tb_lzsw_node_t;
#endif

// the inflate window type
typedef struct __tb_lzsw_inflate_window_t
{
	// the window rage
	tb_byte_t const* 			e;
	tb_size_t 					n;

	// the window bits
	tb_size_t 					b;

}tb_lzsw_inflate_window_t;


// the deflate window type
typedef struct __tb_lzsw_deflate_window_t
{
	// the window rage
	tb_byte_t const* 			e;
	tb_size_t 					n;

	// the window bits
	tb_size_t 					b;

#if TB_LZSW_WINDOW_FIND_HASH
	// the circle base
	tb_size_t 					base;

	// the window hash
	void* 						pool;
	tb_size_t 					hash[TB_LZSW_WINDOW_MAX];
#endif

}tb_lzsw_deflate_window_t;


// the lzsw inflate zstream type
typedef struct __tb_lzsw_inflate_zstream_t
{
	// the stream base
	tb_inflate_zstream_t 		base;

	// the reference to vlc
	tb_zstream_vlc_t* 			vlc;

	// the window 
	tb_lzsw_inflate_window_t  	window;

}tb_lzsw_inflate_zstream_t;

// the lzsw deflate zstream type
typedef struct __tb_lzsw_deflate_zstream_t
{
	// the stream base
	tb_deflate_zstream_t 		base;

	// the reference to vlc
	tb_zstream_vlc_t* 			vlc;

	// the window 
	tb_lzsw_deflate_window_t  	window;

}tb_lzsw_deflate_zstream_t;


// the lzsw zstream type
typedef union __tb_lzsw_zstream_t
{
	tb_lzsw_inflate_zstream_t 	infst;
	tb_lzsw_deflate_zstream_t 	defst;

}tb_lzsw_zstream_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_tstream_t* 	tb_zstream_open_lzsw_inflate(tb_lzsw_inflate_zstream_t* zst);
tb_tstream_t* 	tb_zstream_open_lzsw_deflate(tb_lzsw_deflate_zstream_t* zst);
tb_tstream_t* 	tb_zstream_open_lzsw(tb_lzsw_zstream_t* zst, tb_size_t action);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

