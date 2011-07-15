/*!The Tiny Box Library
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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		zlibraw.h
 *
 */
#ifndef TB_ZIP_ZLIBRAW_H
#define TB_ZIP_ZLIBRAW_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#ifdef TB_CONFIG_LIBS_ZLIB
# 	include "../libs/zlib/zlib.h"
#endif

/* /////////////////////////////////////////////////////////
 * types
 */

// the zlibraw zip type
typedef struct __tb_zip_zlibraw_t
{
	// the zip base
	tb_zip_t 		base;

#ifdef TB_CONFIG_LIBS_ZLIB
	z_stream 		zst;
#endif

}tb_zip_zlibraw_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_zip_t* 	tb_zip_zlibraw_open(tb_zip_zlibraw_t* zlibraw, tb_zip_action_t action);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

