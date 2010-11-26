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
 * \file		type.h
 *
 */
#ifndef TB_STREAM_ZSTREAM_VLC_TYPE_H
#define TB_STREAM_ZSTREAM_VLC_TYPE_H

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

// the callback type
struct __tb_zstream_vlc_t;
typedef void 		(*tb_zstream_vlc_set_t)(struct __tb_zstream_vlc_t* vlc, tb_uint32_t val, tb_bstream_t* bst);
typedef tb_uint32_t (*tb_zstream_vlc_get_t)(struct __tb_zstream_vlc_t* vlc, tb_bstream_t const* bst);
typedef void 		(*tb_zstream_vlc_close_t)(struct __tb_zstream_vlc_t* vlc);

// the vlc type
typedef enum __tb_zstream_vlc_type_t
{
	TB_ZSTREAM_VLC_TYPE_FIXED 	= 0
,	TB_ZSTREAM_VLC_TYPE_GOLOMB 	= 1
,	TB_ZSTREAM_VLC_TYPE_GAMMA 	= 2

}tb_zstream_vlc_type_t;

// the variable length coding type
typedef struct __tb_zstream_vlc_t
{
	// the vlc type
	tb_size_t 				type;

	// set value to the bits stream
	tb_zstream_vlc_set_t 	set;

	// get value from the bits stream
	tb_zstream_vlc_get_t 	get;

	// close it
	tb_zstream_vlc_close_t 	close;

}tb_zstream_vlc_t;


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

