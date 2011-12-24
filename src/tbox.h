/*!The Treasure Box Library
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
#include "platform/platform.h"
#include "container/container.h"
#include "encoding/encoding.h"
#include "network/network.h"
#include "format/format.h"
#include "memory/memory.h"
#include "stream/stream.h"
#include "string/string.h"
#include "event/event.h"
#include "utils/utils.h"
#include "math/math.h"
#include "libc/libc.h"
#include "xml/xml.h"
#include "zip/zip.h"
#include "libs/libs.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_bool_t 			tb_init(tb_byte_t* data, tb_size_t size);
tb_void_t 			tb_exit();

// version
tb_char_t const* 	tb_version();


// c plus plus
#ifdef __cplusplus
}
#endif

#endif
