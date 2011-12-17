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

// print
#ifndef TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO
#	define tb_print_tag(tag, fmt, arg...)				do { tb_printf("["tag"]: " fmt "\n" , ## arg); } while (0)
# 	define tb_print(fmt, arg...)						tb_print_tag(TB_PRINT_TAG, fmt, ## arg)
#else
#	define tb_print_tag
#	define tb_print
#endif

// trace
#if defined(TB_TRACE_ENABLE) && !defined(TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO)
#	define tb_trace_tag(tag, fmt, arg...)				tb_print_tag(tag, fmt, ## arg)
# 	define tb_trace_line_tag(tag, fmt, arg...) 			tb_print_tag(tag, fmt " at func: %s, line: %d, file: %s", ##arg, __tb_func__, __tb_line__, __tb_file__)
#elif !defined(TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO)
#	define tb_trace_tag(...)
# 	define tb_trace_line_tag(...)
#else
#	define tb_trace_tag
# 	define tb_trace_line_tag
#endif

#ifndef TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO
# 	define tb_trace(fmt, arg...)						tb_trace_tag(TB_PRINT_TAG, fmt, ## arg)
# 	define tb_trace_line(fmt, arg...) 					tb_trace_line_tag(TB_PRINT_TAG, fmt, ## arg)
#else
# 	define tb_trace
# 	define tb_trace_line
#endif

#define tb_trace_noimpl_tag(tag) 						tb_trace_line_tag(tag, "[no_impl]:")
#define tb_trace_noimpl() 								tb_trace_line("[no_impl]:")

// c plus plus
#ifdef __cplusplus
}
#endif

#endif


