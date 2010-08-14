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
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		zlib.h
 *
 */
#ifndef TB_ZLIB_H
#define TB_ZLIB_H

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

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_handle_t 		tb_zlib_create();
void 				tb_zlib_attach(tb_handle_t hz, tb_byte_t const* data, tb_size_t size);
tb_bool_t 			tb_zlib_inflate_partial(tb_handle_t hz, tb_byte_t* data, tb_size_t* size);
void 				tb_zlib_destroy(tb_handle_t hz);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

