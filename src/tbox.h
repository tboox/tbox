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
#include "malloc.h"
#include "string.h"
#include "math.h"
#include "pool.h"
#include "bits.h"
#include "bswap.h"
#include "array.h"
#include "memops.h"
#include "stream.h"
#include "external/external.h"

#ifdef TB_CONFIG_ENCODING
# 	include "encoding.h"
#endif

#ifdef TB_CONFIG_FORMAT
# 	include "format.h"
#endif

#ifdef TB_CONFIG_XML
# 	include "xml/xml.h"
#endif

#ifdef TB_CONFIG_REGEX
# 	include "regex.h"
#endif

#ifdef TB_CONFIG_ZLIB
# 	include "zlib.h"
#endif

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
