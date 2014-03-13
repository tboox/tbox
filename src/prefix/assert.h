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
 * @file		assert.h
 *
 */
#ifndef TB_PREFIX_ASSERT_H
#define TB_PREFIX_ASSERT_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "trace.h"
#include "abort.h"
#include "check.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the assert backtrace prefix
#define TB_ASSERT_BACKTRACE_PREFIX 							"    "

// the assert backtrace nframe
#define TB_ASSERT_BACKTRACE_NFRAME 							(16)

// the assert backtrace dump
#define tb_assert_backtrace_dump() 							tb_backtrace_dump(TB_ASSERT_BACKTRACE_PREFIX, tb_null, TB_ASSERT_BACKTRACE_NFRAME)

// assert
#if defined(TB_ASSERT_ENABLE) 
# 	if defined(TB_COMPILER_IS_GCC)
#		define tb_assert_message_tag(tag, x, fmt, arg...)	do { if (!(x)) {tb_trace_line_tag(tag, "[assert]: expr: %s, msg: " fmt, #x, ##arg); tb_assert_backtrace_dump(); } } while(0)
# 	elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
#		define tb_assert_message_tag(tag, x, fmt, ...)		do { if (!(x)) {tb_trace_line_tag(tag, "[assert]: expr: %s, msg: " fmt, #x, __VA_ARGS__); tb_assert_backtrace_dump(); } } while(0)
# 	else
#		define tb_assert_message_tag 
# 	endif
#else
# 	if defined(TB_COMPILER_IS_GCC) || (defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0))
#		define tb_assert_message_tag(tag, x, fmt, ...)
# 	else
#		define tb_assert_message_tag 
# 	endif
#endif

#ifdef TB_ASSERT_ENABLE
# 	define tb_assert_tag(tag, x)							do { if (!(x)) {tb_trace_line_tag(tag, "[assert]: expr: %s", #x); tb_assert_backtrace_dump(); } } while(0)
# 	define tb_assert_abort_tag(tag, x)						do { if (!(x)) {tb_trace_line_tag(tag, "[assert]: expr: %s", #x); tb_assert_backtrace_dump(); tb_abort(); } } while(0)
# 	define tb_assert_return_tag(tag, x)						do { if (!(x)) {tb_trace_line_tag(tag, "[assert]: expr: %s", #x); tb_assert_backtrace_dump(); return ; } } while(0)
# 	define tb_assert_return_val_tag(tag, x, v)				do { if (!(x)) {tb_trace_line_tag(tag, "[assert]: expr: %s", #x); tb_assert_backtrace_dump(); return (v); } } while(0)
# 	define tb_assert_goto_tag(tag, x, b)					do { if (!(x)) {tb_trace_line_tag(tag, "[assert]: expr: %s", #x); tb_assert_backtrace_dump(); goto b; } } while(0)
# 	define tb_assert_break_tag(tag, x)						{ if (!(x)) {tb_trace_line_tag(tag, "[assert]: expr: %s", #x); tb_assert_backtrace_dump(); break ; } }
# 	define tb_assert_continue_tag(tag, x)					{ if (!(x)) {tb_trace_line_tag(tag, "[assert]: expr: %s", #x); tb_assert_backtrace_dump(); continue ; } }
# 	define tb_assert_and_check_abort_tag(tag, x)			tb_assert_abort_tag(tag, x)
# 	define tb_assert_and_check_return_tag(tag, x)			tb_assert_return_tag(tag, x)
# 	define tb_assert_and_check_return_val_tag(tag, x, v)	tb_assert_return_val_tag(tag, x, v)
# 	define tb_assert_and_check_goto_tag(tag, x, b)			tb_assert_goto_tag(tag, x, b)
# 	define tb_assert_and_check_break_tag(tag, x)			tb_assert_break_tag(tag, x)
# 	define tb_assert_and_check_continue_tag(tag, x)			tb_assert_continue_tag(tag, x)
#else
# 	define tb_assert_tag(tag, x)
# 	define tb_assert_abort_tag(tag, x)
# 	define tb_assert_return_tag(tag, x)
# 	define tb_assert_return_val_tag(tag, x, v)
# 	define tb_assert_goto_tag(tag, x, b)
# 	define tb_assert_break_tag(tag, x)
# 	define tb_assert_continue_tag(tag, x)
# 	define tb_assert_and_check_abort_tag(tag, x)			tb_check_abort(x)
# 	define tb_assert_and_check_return_tag(tag, x)			tb_check_return(x)
# 	define tb_assert_and_check_return_val_tag(tag, x, v)	tb_check_return_val(x, v)
# 	define tb_assert_and_check_goto_tag(tag, x, b)			tb_check_goto(x, b)
# 	define tb_assert_and_check_break_tag(tag, x)			tb_check_break(x)
# 	define tb_assert_and_check_continue_tag(tag, x)			tb_check_continue(x)
#endif

#if defined(TB_COMPILER_IS_GCC)
# 	define tb_assert_message(x, fmt, arg...)			tb_assert_message_tag(TB_PRINT_TAG, x, fmt, ## arg)
#elif defined(TB_COMPILER_IS_MSVC) && TB_COMPILER_VERSION_BE(13, 0)
# 	define tb_assert_message(x, fmt, ...)				tb_assert_message_tag(TB_PRINT_TAG, x, fmt, __VA_ARGS__)
#else
# 	define tb_assert_message
#endif

#define tb_assert(x)										tb_assert_tag(TB_PRINT_TAG, x)
#define tb_assert_abort(x)									tb_assert_abort_tag(TB_PRINT_TAG, x)
#define tb_assert_return(x)									tb_assert_return_tag(TB_PRINT_TAG, x)
#define tb_assert_return_val(x, v)							tb_assert_return_val_tag(TB_PRINT_TAG, x, v)
#define tb_assert_goto(x, b)								tb_assert_goto_tag(TB_PRINT_TAG, x, b)
#define tb_assert_break(x)									tb_assert_break_tag(TB_PRINT_TAG, x)
#define tb_assert_continue(x)								tb_assert_continue_tag(TB_PRINT_TAG, x)
#define tb_assert_and_check_abort(x)						tb_assert_and_check_abort_tag(TB_PRINT_TAG, x)
#define tb_assert_and_check_return(x)						tb_assert_and_check_return_tag(TB_PRINT_TAG, x)
#define tb_assert_and_check_return_val(x, v)				tb_assert_and_check_return_val_tag(TB_PRINT_TAG, x, v)
#define tb_assert_and_check_goto(x, b)						tb_assert_and_check_goto_tag(TB_PRINT_TAG, x, b)
#define tb_assert_and_check_break(x)						tb_assert_and_check_break_tag(TB_PRINT_TAG, x)
#define tb_assert_and_check_continue(x)						tb_assert_and_check_continue_tag(TB_PRINT_TAG, x)

#define tb_assert_static(x) 								do { typedef int __tb_static_assert__[(x)? 1 : -1]; } while(0)

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_void_t tb_backtrace_dump(tb_char_t const* prefix, tb_pointer_t* frames, tb_size_t nframe);

#endif


