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

/* /////////////////////////////////////////////////////////
 * types
 */

// the transform stream type: src => dst
typedef struct __tb_tstream_t
{
	tb_byte_t** 	src_next;
	tb_size_t 		src_avail;
	tb_size_t 		src_total;

	tb_byte_t** 	dst_next;
	tb_size_t 		dst_avail;
	tb_size_t 		dst_total;

}tb_tstream_t;

/* /////////////////////////////////////////////////////////
 * macros
 */


/* /////////////////////////////////////////////////////////
 * interfaces
 */


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

