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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		trace.h
 *
 */
#ifndef TB_PREFIX_TRACE_H
#define TB_PREFIX_TRACE_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "type.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// tag
#if defined(TB_TRACE_ENABLE) && !defined(TB_PRINT_TAG)
# 	define TB_PRINT_TAG 									"tbox"
#endif

// print
#ifndef TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO
# 	if defined(TB_COMPILER_IS_MSVC) && (_MSC_VER >= 1300)
#		define tb_print_tag(tag, fmt, ...)					do { tb_printf("["tag"]: " fmt "\n" , __VA_ARGS__); } while (0)
# 		define tb_print(fmt, ...)							tb_print_tag(TB_PRINT_TAG, fmt, __VA_ARGS__)
# 	else
#		define tb_print_tag(tag, fmt, arg ...)				do { tb_printf("["tag"]: " fmt "\n" , ## arg); } while (0)
# 		define tb_print(fmt, arg ...)						tb_print_tag(TB_PRINT_TAG, fmt, ## arg)
# 	endif
#else
#	define tb_print_tag
#	define tb_print
#endif

// trace_tag, the private macro
#if defined(TB_TRACE_ENABLE) && !defined(TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO)
# 	if defined(TB_COMPILER_IS_MSVC) && (_MSC_VER >= 1300)
#		define tb_trace_tag(tag, fmt, ...)					tb_print_tag(tag, fmt, __VA_ARGS__)
# 		define tb_trace_line_tag(tag, fmt, ...) 			tb_print_tag(tag, "func: %s, line: %d, file: %s:\n" fmt, __tb_func__, __tb_line__, __tb_file__, __VA_ARGS__)
# 		define tb_trace_warning_tag(tag, fmt, ...) 			tb_print_tag(tag, "warning: func: %s, line: %d, file: %s:\n" fmt, __tb_func__, __tb_line__, __tb_file__, __VA_ARGS__)
# 	else
#		define tb_trace_tag(tag, fmt, arg ...)				tb_print_tag(tag, fmt, ## arg)
# 		define tb_trace_line_tag(tag, fmt, arg ...) 		tb_print_tag(tag, fmt " at func: %s, line: %d, file: %s", ##arg, __tb_func__, __tb_line__, __tb_file__)
# 		define tb_trace_warning_tag(tag, fmt, arg ...) 		tb_print_tag(tag, "warning: " fmt " at func: %s, line: %d, file: %s", ##arg, __tb_func__, __tb_line__, __tb_file__)
# 	endif
#elif !defined(TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO)
#	define tb_trace_tag(...)
# 	define tb_trace_line_tag(...)
# 	define tb_trace_warning_tag(...)
#else
#	define tb_trace_tag
# 	define tb_trace_line_tag
# 	define tb_trace_warning_tag
#endif

// trace, the debug print
#ifndef TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO
# 	if defined(TB_COMPILER_IS_MSVC) && (_MSC_VER >= 1300)
# 		define tb_trace(fmt, ...)							tb_trace_tag(TB_PRINT_TAG, fmt, __VA_ARGS__)
# 		define tb_trace_line(fmt, ...) 						tb_trace_line_tag(TB_PRINT_TAG, fmt, __VA_ARGS__)
# 		define tb_trace_warning(fmt, ...) 					tb_trace_warning_tag(TB_PRINT_TAG, fmt, __VA_ARGS__)
# 	else
# 		define tb_trace(fmt, arg ...)						tb_trace_tag(TB_PRINT_TAG, fmt, ## arg)
# 		define tb_trace_line(fmt, arg ...) 					tb_trace_line_tag(TB_PRINT_TAG, fmt, ## arg)
# 		define tb_trace_warning(fmt, arg ...) 				tb_trace_warning_tag(TB_PRINT_TAG, fmt, ## arg)
# 	endif
#else
# 	define tb_trace
# 	define tb_trace_line
# 	define tb_trace_warning
#endif

/* trace_impl
 *
 * only for the .c file, debug the module implementation
 * disable it if the module implementation is ok
 *
 * .e.g.1
 *
 * at file xxxx.c:
 *
 * // macros
 * #define TB_TRACE_IMPL_TAG "module"
 *
 * // includes
 * #include "tbox.h"
 *
 * // codes
 * tb_trace_impl("hello world");
 *
 * // output
 * "[tag]: [module]: hello world"
 *
 *
 * .e.g.2
 *
 * at file xxxx.c:
 *
 * // macros
 * // #define TB_TRACE_IMPL_TAG "module"
 *
 * // includes
 * #include "tbox.h"
 *
 * // codes
 * tb_trace_impl("hello world");
 *
 * // no output
 *
 */
#if defined(TB_TRACE_IMPL_TAG) && !defined(TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO)
# 	if defined(TB_COMPILER_IS_MSVC) && (_MSC_VER >= 1300)
# 		define tb_trace_impl(fmt, ...)						tb_trace("["TB_TRACE_IMPL_TAG"]: " fmt, __VA_ARGS__)
# 		define tb_trace_line_impl(fmt, ...) 				tb_trace_line("["TB_TRACE_IMPL_TAG"]: " fmt, __VA_ARGS__)
# 		define tb_trace_warning_impl(fmt, ...) 				tb_trace_warning("["TB_TRACE_IMPL_TAG"]: " fmt, __VA_ARGS__)
# 	else
# 		define tb_trace_impl(fmt, arg ...)					tb_trace("["TB_TRACE_IMPL_TAG"]: " fmt, ## arg)
# 		define tb_trace_line_impl(fmt, arg ...) 			tb_trace_line("["TB_TRACE_IMPL_TAG"]: " fmt, ## arg)
# 		define tb_trace_warning_impl(fmt, arg ...) 			tb_trace_warning("["TB_TRACE_IMPL_TAG"]: " fmt, ## arg)
# 	endif
#elif !defined(TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO)
#	define tb_trace_impl(...)
# 	define tb_trace_line_impl(...)
# 	define tb_trace_warning_impl(...)
#else
# 	define tb_trace_impl
# 	define tb_trace_line_impl
# 	define tb_trace_warning_impl
#endif

// print_impl
#if defined(TB_PRINT_IMPL_TAG) && !defined(TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO)
# 	if defined(TB_COMPILER_IS_MSVC) && (_MSC_VER >= 1300)
# 		define tb_print_impl(fmt, ...)						tb_print("["TB_PRINT_IMPL_TAG"]: " fmt, __VA_ARGS__)
# 	else
# 		define tb_print_impl(fmt, arg ...)					tb_print("["TB_PRINT_IMPL_TAG"]: " fmt, ## arg)
# 	endif
#elif !defined(TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO)
#	define tb_print_impl(...)
#else
# 	define tb_print_impl
#endif

// noimpl
#define tb_trace_noimpl() 									tb_trace_line("[no_impl]:")

// warning
#define tb_warning 											tb_trace_warning

// nosafe
#define tb_trace_nosafe() 									tb_trace_warning("no_safe")

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */

tb_void_t 		tb_printf(tb_char_t const* fmt, ...);

#endif


