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
 * \file		golomb.h
 *
 */
#ifndef TB_STREAM_ZSTREAM_VLC_GOLOMB_H
#define TB_STREAM_ZSTREAM_VLC_GOLOMB_H

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

// adaptive golomb coding
#define TB_ZSTREAM_VLC_GOLOMB_ADAPTIVE

/* /////////////////////////////////////////////////////////
 * types
 */

// the golomb length coding type
typedef struct __tb_zstream_vlc_golomb_t
{
	// the base
	tb_zstream_vlc_t 		base;
	
	// the default value of m
	tb_size_t 				defm;

#ifdef TB_ZSTREAM_VLC_GOLOMB_ADAPTIVE
	// for computing the average value
	tb_size_t 				total;
	tb_size_t 				count;
#endif

}tb_zstream_vlc_golomb_t;


/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_zstream_vlc_t* tb_zstream_vlc_golomb_open(tb_zstream_vlc_golomb_t* golomb, tb_size_t defm);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

