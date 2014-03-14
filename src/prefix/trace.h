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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
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

// the trace prefix
#ifndef TB_TRACE_PREFIX 
# 	define TB_TRACE_PREFIX									tb_null
#endif

// the trace module name
#ifndef TB_TRACE_MODULE_NAME
# 	define TB_TRACE_MODULE_NAME 							tb_null
#endif

// the trace module debug
#ifndef TB_TRACE_MODULE_DEBUG
# 	define TB_TRACE_MODULE_DEBUG 							(1)
#endif

// trace prefix
#if defined(TB_TRACE_ENABLE)
# 	if defined(TB_COMPILER_IS_GCC)
#		define tb_trace_p(prefix, fmt, arg ...)				tb_trace_done(prefix, TB_TRACE_MODULE_NAME, fmt, ## arg)
# 		define tb_trace_error_p(prefix, fmt, arg ...) 		tb_trace_done(prefix, TB_TRACE_MODULE_NAME, "[error]: " fmt " at func: %s, line: %d, file: %s", ##arg, __tb_func__, __tb_line__, __tb_file__)
# 		define tb_trace_assert_p(prefix, fmt, arg ...) 		tb_trace_done(prefix, TB_TRACE_MODULE_NAME, "[assert]: " fmt " at func: %s, line: %d, file: %s", ##arg, __tb_func__, __tb_line__, __tb_file__)
# 		define tb_trace_warning_p(prefix, fmt, arg ...) 	tb_trace_done(prefix, TB_TRACE_MODULE_NAME, "[warning]: " fmt " at func: %s, line: %d, file: %s", ##arg, __tb_func__, __tb_line__, __tb_file__)
# 	elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
#		define tb_trace_p(prefix, fmt, ...)					tb_trace_done(prefix, TB_TRACE_MODULE_NAME, fmt, __VA_ARGS__)
# 		define tb_trace_error_p(prefix, fmt, ...) 			tb_trace_done(prefix, TB_TRACE_MODULE_NAME, "[error]: func: %s, line: %d, file: %s:\n" fmt, __tb_func__, __tb_line__, __tb_file__, __VA_ARGS__)
# 		define tb_trace_assert_p(prefix, fmt, ...) 			tb_trace_done(prefix, TB_TRACE_MODULE_NAME, "[assert]: func: %s, line: %d, file: %s:\n" fmt, __tb_func__, __tb_line__, __tb_file__, __VA_ARGS__)
# 		define tb_trace_warning_p(prefix, fmt, ...) 		tb_trace_done(prefix, TB_TRACE_MODULE_NAME, "[warning]: func: %s, line: %d, file: %s:\n" fmt, __tb_func__, __tb_line__, __tb_file__, __VA_ARGS__)
# 	else
#		define tb_trace_p
# 		define tb_trace_error_p
#		define tb_trace_assert_p
#		define tb_trace_warning_p
# 	endif
#else
# 	if defined(TB_COMPILER_IS_GCC) || (defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0))
#		define tb_trace_p(prefix, fmt, ...)		
# 		define tb_trace_error_p(prefix, fmt, ...) 	
# 		define tb_trace_assert_p(prefix, fmt, ...) 	
# 		define tb_trace_warning_p(prefix, fmt, ...)
# 	else
#		define tb_trace_p
#		define tb_trace_error_p
#		define tb_trace_assert_p
#		define tb_trace_warning_p
# 	endif
#endif


/* trace
 *
 * at file xxxx.c:
 *
 * // macros
 * #define TB_TRACE_MODULE_NAME 	"module"
 * #define TB_TRACE_MODULE_DEBUG 	(1)
 *
 * // includes
 * #include "tbox.h"
 *
 * // trace
 * tb_trace_d("trace debug");
 * tb_trace_i("trace info");
 * tb_trace_e("trace error");
 * tb_trace_w("trace warning");
 *
 * // output for debug
 * "[prefix]: [module]: trace debug"
 * "[prefix]: [module]: trace info"
 * "[prefix]: [module]: [error]: trace error" at func: xxx, line: xxx, file: xxx
 * "[prefix]: [module]: [warning]: trace warning" at func: xxx, line: xxx, file: xxx
 *
 * // output for release or TB_TRACE_MODULE_DEBUG == 0
 * "[prefix]: [module]: trace info"
 * "[prefix]: [module]: [error]: trace error" at func: xxx, line: xxx, file: xxx
 * "[prefix]: [module]: [warning]: trace warning" at func: xxx, line: xxx, file: xxx
 *
 * note: [module]: will be not output if not define TB_TRACE_MODULE_NAME
 *
 */
#if defined(TB_TRACE_MODULE_DEBUG) && defined(__tb_debug__)
# 	if defined(TB_COMPILER_IS_GCC)
# 		define tb_trace_d(fmt, arg ...)					tb_trace_p(TB_TRACE_PREFIX, fmt, ## arg)
# 	elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
# 		define tb_trace_d(fmt, ...)						tb_trace_p(TB_TRACE_PREFIX, fmt, __VA_ARGS__)
# 	else
# 		define tb_trace_d
# 	endif
#else
# 	if defined(TB_COMPILER_IS_GCC) || (defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0))
# 		define tb_trace_d(fmt, ...)			
# 	else
# 		define tb_trace_d
# 	endif
#endif

#if defined(TB_COMPILER_IS_GCC)
#	define tb_trace_i(fmt, arg ...)					tb_trace_p(TB_TRACE_PREFIX, fmt, ## arg)
#	define tb_trace_e(fmt, arg ...)					tb_trace_error_p(TB_TRACE_PREFIX, fmt, ## arg)
#	define tb_trace_a(fmt, arg ...)					tb_trace_assert_p(TB_TRACE_PREFIX, fmt, ## arg)
#	define tb_trace_w(fmt, arg ...)					tb_trace_warning_p(TB_TRACE_PREFIX, fmt, ## arg)
#elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
# 	define tb_trace_i(fmt, ...)						tb_trace_p(TB_TRACE_PREFIX, fmt, __VA_ARGS__)
# 	define tb_trace_e(fmt, ...)						tb_trace_error_p(TB_TRACE_PREFIX, fmt, __VA_ARGS__)
# 	define tb_trace_a(fmt, ...)						tb_trace_assert_p(TB_TRACE_PREFIX, fmt, __VA_ARGS__)
# 	define tb_trace_w(fmt, ...)						tb_trace_warning_p(TB_TRACE_PREFIX, fmt, __VA_ARGS__)
#else
# 	define tb_trace_i
# 	define tb_trace_e
# 	define tb_trace_a
# 	define tb_trace_w
#endif


// tag
#ifndef TB_PRINT_TAG
# 	define TB_PRINT_TAG 									"tbox"
#endif

// print
#if defined(TB_COMPILER_IS_GCC)
#	define tb_print_tag(tag, fmt, arg ...)				do { tb_printf("["tag"]: " fmt "\n" , ## arg); } while (0)
# 	define tb_print(fmt, arg ...)						tb_print_tag(TB_PRINT_TAG, fmt, ## arg)
#elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
#	define tb_print_tag(tag, fmt, ...)					do { tb_printf("["tag"]: " fmt "\n" , __VA_ARGS__); } while (0)
# 	define tb_print(fmt, ...)							tb_print_tag(TB_PRINT_TAG, fmt, __VA_ARGS__)
#else
#	define tb_print_tag
#	define tb_print
#endif

// wprint
#if defined(TB_COMPILER_IS_GCC)
#	define tb_wprint_tag(tag, fmt, arg ...)				do { tb_wprintf(L"[" tag "]: " fmt "\n" , ## arg); } while (0)
# 	define tb_wprint(fmt, arg ...)						tb_wprint_tag(TB_PRINT_TAG, fmt, ## arg)
#elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
#	define tb_wprint_tag(tag, fmt, ...)					do { tb_wprintf(L"[" tag "]: " fmt "\n" , __VA_ARGS__); } while (0)
# 	define tb_wprint(fmt, ...)							tb_wprint_tag(TB_PRINT_TAG, fmt, __VA_ARGS__)
#else
#	define tb_wprint_tag
#	define tb_wprint
#endif

// trace_tag, the private macro
#if defined(TB_TRACE_ENABLE)
# 	if defined(TB_COMPILER_IS_GCC)
#		define tb_trace_tag(tag, fmt, arg ...)				tb_print_tag(tag, fmt, ## arg)
# 		define tb_trace_line_tag(tag, fmt, arg ...) 		tb_print_tag(tag, fmt " at func: %s, line: %d, file: %s", ##arg, __tb_func__, __tb_line__, __tb_file__)
# 		define tb_trace_warning_tag(tag, fmt, arg ...) 		tb_print_tag(tag, "warning: " fmt " at func: %s, line: %d, file: %s", ##arg, __tb_func__, __tb_line__, __tb_file__)
# 	elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
#		define tb_trace_tag(tag, fmt, ...)					tb_print_tag(tag, fmt, __VA_ARGS__)
# 		define tb_trace_line_tag(tag, fmt, ...) 			tb_print_tag(tag, "func: %s, line: %d, file: %s:\n" fmt, __tb_func__, __tb_line__, __tb_file__, __VA_ARGS__)
# 		define tb_trace_warning_tag(tag, fmt, ...) 			tb_print_tag(tag, "warning: func: %s, line: %d, file: %s:\n" fmt, __tb_func__, __tb_line__, __tb_file__, __VA_ARGS__)
# 	else
#		define tb_trace_tag
#		define tb_trace_line_tag
#		define tb_trace_warning_tag
# 	endif
#else
# 	if defined(TB_COMPILER_IS_GCC) || (defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0))
#		define tb_trace_tag(tag, fmt, ...)		
# 		define tb_trace_line_tag(tag, fmt, ...) 	
# 		define tb_trace_warning_tag(tag, fmt, ...)
# 	else
#		define tb_trace_tag
#		define tb_trace_line_tag
#		define tb_trace_warning_tag
# 	endif
#endif

// trace, the debug print
#if defined(TB_COMPILER_IS_GCC)
# 	define tb_trace(fmt, arg ...)						tb_trace_tag(TB_PRINT_TAG, fmt, ## arg)
# 	define tb_trace_line(fmt, arg ...) 					tb_trace_line_tag(TB_PRINT_TAG, fmt, ## arg)
# 	define tb_trace_warning(fmt, arg ...) 				tb_trace_warning_tag(TB_PRINT_TAG, fmt, ## arg)
#elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
# 	define tb_trace(fmt, ...)							tb_trace_tag(TB_PRINT_TAG, fmt, __VA_ARGS__)
# 	define tb_trace_line(fmt, ...) 						tb_trace_line_tag(TB_PRINT_TAG, fmt, __VA_ARGS__)
# 	define tb_trace_warning(fmt, ...) 					tb_trace_warning_tag(TB_PRINT_TAG, fmt, __VA_ARGS__)
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
#if defined(TB_TRACE_IMPL_TAG)
# 	if defined(TB_COMPILER_IS_GCC)
# 		define tb_trace_impl(fmt, arg ...)					tb_trace("["TB_TRACE_IMPL_TAG"]: " fmt, ## arg)
# 		define tb_trace_line_impl(fmt, arg ...) 			tb_trace_line("["TB_TRACE_IMPL_TAG"]: " fmt, ## arg)
# 		define tb_trace_warning_impl(fmt, arg ...) 			tb_trace_warning("["TB_TRACE_IMPL_TAG"]: " fmt, ## arg)
# 	elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
# 		define tb_trace_impl(fmt, ...)						tb_trace("["TB_TRACE_IMPL_TAG"]: " fmt, __VA_ARGS__)
# 		define tb_trace_line_impl(fmt, ...) 				tb_trace_line("["TB_TRACE_IMPL_TAG"]: " fmt, __VA_ARGS__)
# 		define tb_trace_warning_impl(fmt, ...) 				tb_trace_warning("["TB_TRACE_IMPL_TAG"]: " fmt, __VA_ARGS__)
# 	else
# 		define tb_trace_impl
# 		define tb_trace_line_impl
# 		define tb_trace_warning_impl
# 	endif
#else
# 	if defined(TB_COMPILER_IS_GCC) || (defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0))
# 		define tb_trace_impl(fmt, ...)			
# 		define tb_trace_line_impl(fmt, ...) 		
# 		define tb_trace_warning_impl(fmt, ...) 	
# 	else
# 		define tb_trace_impl
# 		define tb_trace_line_impl
# 		define tb_trace_warning_impl
# 	endif
#endif

// print_impl
#if defined(TB_PRINT_IMPL_TAG)
# 	if defined(TB_COMPILER_IS_GCC)
# 		define tb_print_impl(fmt, arg ...)					tb_print("["TB_PRINT_IMPL_TAG"]: " fmt, ## arg)
# 	elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
# 		define tb_print_impl(fmt, ...)						tb_print("["TB_PRINT_IMPL_TAG"]: " fmt, __VA_ARGS__)
# 	else
# 		define tb_print_impl
# 	endif
#else
# 	if defined(TB_COMPILER_IS_GCC) || (defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0))
# 		define tb_print_impl(fmt, ...)					
# 	else
# 		define tb_print_impl
# 	endif
#endif

// noimpl
#define tb_trace_noimpl() 									tb_trace_line("[noimpl]:")

// warning
#define tb_warning 											tb_trace_warning

// nosafe
#define tb_trace_nosafe() 									tb_trace_warning("nosafe")

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_void_t 		tb_printf(tb_char_t const* format, ...);
tb_void_t 		tb_wprintf(tb_wchar_t const* format, ...);
tb_void_t 		tb_trace_done(tb_char_t const* prefix, tb_char_t const* module, tb_char_t const* format, ...);

#endif


