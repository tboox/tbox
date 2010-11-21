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
#ifndef TB_STREAM_ESTREAM_H
#define TB_STREAM_ESTREAM_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "tstream.h"
#include "../encoding.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the encoding stream type
typedef struct __tb_estream_t
{
	// the stream base
	tb_tstream_t 					base;

	// the converter
	tb_encoding_converter_t const* 	src_c;
	tb_encoding_converter_t const* 	dst_c;

}tb_estream_t;

/* /////////////////////////////////////////////////////////
 * macros
 */


/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_tstream_t* 	tb_estream_open(tb_estream_t* est, tb_encoding_t src_e, tb_encoding_t dst_e);


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

