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
 * \file		tbox.h
 *
 */
#ifndef TB_TBOX_H
#define TB_TBOX_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "math.h"
#include "conv.h"
#include "bits.h"
#include "time.h"
#include "format.h"
#include "encoding.h"
#include "format.h"
#include "xml/xml.h"
#include "zip/zip.h"
#include "math/math.h"
#include "memory/memory.h"
#include "stream/stream.h"
#include "string/string.h"
#include "network/network.h"
#include "container/container.h"
#include "libs/libs.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t 	tb_init(tb_byte_t* data, tb_size_t size);
void 		tb_exit();


// c plus plus
#ifdef __cplusplus
}
#endif

#endif
