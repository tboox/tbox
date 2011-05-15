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
 * \file		prefix.h
 *
 */
#ifndef TB_PREFIX_H
#define TB_PREFIX_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "tplat/tplat.h"
#include "option.h"
#include "type.h"
#include "fixed.h"
#include "malloc.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#ifndef TPLAT_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO

// debug
#ifdef TB_DEBUG
#	define TB_DBG(format, arg...)					do { tplat_printf("[tb]:" format "\n" , ## arg); } while (0)
#	define TB_ASSERT(expr)							do { if (!(expr)) {tplat_printf("[tb]: assert failed at:%d: expr: %s file: %s\n", __tplat_line__, #expr, __tplat_file__); } } while(0)
#	define TB_MSG_ASSERT(expr, format, arg...)		do { if (!(expr)) {tplat_printf("[tb]: assert failed at:%d: expr: %s msg: " format " file: %s\n", __tplat_line__, #expr, ## arg, __tplat_file__); }} while(0)
#else
#	define TB_DBG(format, arg...)
#	define TB_ASSERT(expr)
#	define TB_MSG_ASSERT(expr, format, arg...)
#endif

#else
		
// debug
#ifdef TB_DEBUG
#	define TB_DBG
#	define TB_ASSERT(expr)							do { if (!(expr)) {tplat_printf("[tb]: assert failed at:%d: expr: %s file: %s\n", __tplat_line__, #expr, __tplat_file__); } } while(0)
#	define TB_MSG_ASSERT
#else
#	define TB_DBG
#	define TB_ASSERT(expr)
#	define TB_MSG_ASSERT
#endif
#endif

// the size of the static array
#define TB_STATIC_ARRAY_SIZE(a) 					(sizeof((a)) / sizeof((a)[0]))

// check
#define TB_IF_FAIL_RETURN(x) 						do { if (!(x)) return ; } while (0)
#define TB_IF_FAIL_RETURN_VAL(x, v) 				do { if (!(x)) return (v); } while (0)
#define TB_IF_FAIL_GOTO(x, b) 						do { if (!(x)) goto b; } while (0)

#define TB_IF_REACHED_RETURN(x) 					do { return ; } while (0)
#define TB_IF_REACHED_RETURN_VAL(x, v) 				do { return (v); } while (0)
#define TB_IF_REACHED_GOTO(x, b) 					do { goto b; } while (0)

#define TB_ASSERT_RETURN(x) 						do { TB_ASSERT(x); if (!(x)) return ; } while (0)
#define TB_ASSERT_RETURN_VAL(x, v) 					do { TB_ASSERT(x); if (!(x)) return (v); } while (0)
#define TB_ASSERT_GOTO(x, b) 						do { TB_ASSERT(x); if (!(x)) goto b; } while (0)

// not implement
#define TB_NOT_IMPLEMENT() 							do { TB_DBG("[not_impl]: %s at %d file: %s", __tplat_func__, __tplat_line__, __tplat_file__); } while (0)


// c plus plus
#ifdef __cplusplus
}
#endif

#endif
