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
 * \file		trace.h
 *
 */
#ifndef TB_PREFIX_TRACE_H
#define TB_PREFIX_TRACE_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// tag
#define TB_TAG 											"tb"

// print
#ifndef TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO
#	define TB_PRINT_TAG(tag, fmt, arg...)				do { tb_printf("["tag"]: " fmt "\n" , ## arg); } while (0)
# 	define TB_PRINT(fmt, arg...)						TB_PRINT_TAG(TB_TAG, fmt, ## arg)
#else
#	define TB_PRINT_TAG(x)
#	define TB_PRINT(x)
#endif

// trace
#if defined(TB_DEBUG) && !defined(TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO)
#	define TB_TRACE_TAG(tag, fmt, arg...)				TB_PRINT_TAG(tag, fmt, ## arg)
# 	define TB_TRACE(fmt, arg...)						TB_PRINT(fmt, ## arg)

# 	define TB_TRACE_LINE_TAG(tag, fmt, arg...) 			TB_PRINT_TAG(tag, fmt "at func: %s, line: %d, file: %s", ##arg, __tb_func__, __tb_line__, __tb_file__)
# 	define TB_TRACE_LINE(fmt, arg...) 					TB_TRACE_LINE_TAG(TB_TAG, fmt, ## arg)

# 	define TB_DBG(fmt, arg...)							TB_TRACE(fmt, ## arg) 				//!< discarded
#else
#	define TB_TRACE_TAG(x)
#	define TB_TRACE(x)
#	define TB_DBG(x)
#endif

#define TB_TRACE_NOIMPL_TAG(tag) 						TB_TRACE_LINE_TAG(tag, "[no_impl]: ")
#define TB_TRACE_NOIMPL() 								TB_TRACE_NOIMPL_TAG(TB_TAG)
#define TB_NOT_IMPLEMENT() 								TB_TRACE_NOIMPL() 				//!< discarded


// c plus plus
#ifdef __cplusplus
}
#endif

#endif


