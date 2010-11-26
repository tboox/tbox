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
 * \file		gamma.h
 *
 */
#ifndef TB_STREAM_ZSTREAM_VLC_GAMMA_H
#define TB_STREAM_ZSTREAM_VLC_GAMMA_H

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

// the gamma length coding type
typedef struct __tb_zstream_vlc_gamma_t
{
	// the base
	tb_zstream_vlc_t 		base;

}tb_zstream_vlc_gamma_t;


/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_zstream_vlc_t* tb_zstream_vlc_gamma_open(tb_zstream_vlc_gamma_t* gamma);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

