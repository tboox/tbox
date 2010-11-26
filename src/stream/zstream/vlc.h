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
 * \file		vlc.h
 *
 */
#ifndef TB_STREAM_ZSTREAM_VLC_H
#define TB_STREAM_ZSTREAM_VLC_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "vlc/fixed.h"
#include "vlc/golomb.h"
#include "vlc/gamma.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the vlc union type
typedef union __tb_zstream_vlc_union_t
{
	tb_zstream_vlc_fixed_t 		fixed;
	tb_zstream_vlc_golomb_t 	golomb;
	tb_zstream_vlc_gamma_t 		gamma;

}tb_zstream_vlc_union_t;


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

