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
 * \file		prefix.h
 *
 */
#ifndef TB_EXTERNAL_PREFIX_H
#define TB_EXTERNAL_PREFIX_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// debug
#ifdef TB_DEBUG
#	define TB_DBG(format, arg...)					do { eplat_printf("[tb]:" format "\n" , ## arg); } while (0)
#	define TB_ASSERT(expr)							do { if (!(expr)) {eplat_printf("[tb]: assert failed at:%d: expr: %s file: %s\n", __LINE__, #expr, __FILE__); } } while(0)
#	define TB_MSG_ASSERT(expr, format, arg...)		do { if (!(expr)) {eplat_printf("[tb]: assert failed at:%d: expr: %s msg: " format " file: %s\n", __LINE__, #expr, ## arg, __FILE__); }} while(0)
#else
#	define TB_DBG(format, arg...)
#	define TB_ASSERT(expr)
#	define TB_MSG_ASSERT(expr, format, arg...)
#endif

// filename max size
#define TB_PATH_MAX_SIZE 							(1024)

// the size of the static array
#define TB_STATIC_ARRAY_SIZE(a) 					(sizeof((a)) / sizeof((a)[0]))

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
