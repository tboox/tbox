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
 * \file		assert.h
 *
 */
#ifndef TB_PREFIX_ASSERT_H
#define TB_PREFIX_ASSERT_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "trace.h"
#include "abort.h"
#include "check.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// assert
#if defined(TB_DEBUG) && !defined(TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO)
#	define TB_ASSERT_MESSAGE_TAG(tag, x, fmt, arg...)	do { if (!(x)) {TB_TRACE_LINE_TAG(tag, "[assert]: expr: %s, msg: " fmt, #x, ##arg); } } while(0)
# 	define TB_ASSERT_MESSAGE(x, fmt, arg...)			TB_ASSERT_MESSAGE_TAG(TB_TAG, x, fmt, ## arg)
# 	define TB_ASSERTM(x, fmt, arg...)					TB_ASSERT_MESSAGE(x, fmt, ## arg) 		//!< discarded
#else
#	define TB_ASSERT_MESSAGE_TAG(x)
# 	define TB_ASSERT_MESSAGE(x)
# 	define TB_ASSERTM(x)
#endif

#if defined(TB_DEBUG)
#	define TB_ASSERT_TAG(tag, x)						do { if (!(x)) {TB_TRACE_LINE_TAG(tag, "[assert]: expr: %s", #x); } } while(0)
#	define TB_ASSERT_ABORT_TAG(tag, x)					do { if (!(x)) {TB_TRACE_LINE_TAG(tag, "[assert]: expr: %s", #x); TB_ABORT_TAG(tag); } } while(0)
#	define TB_ASSERT_RETURN_TAG(tag, x)					do { if (!(x)) {TB_TRACE_LINE_TAG(tag, "[assert]: expr: %s", #x); return ; } } while(0)
#	define TB_ASSERT_RETURN_VAL_TAG(tag, x, v)			do { if (!(x)) {TB_TRACE_LINE_TAG(tag, "[assert]: expr: %s", #x); return (v); } } while(0)
#	define TB_ASSERT_GOTO_TAG(tag, x, b)				do { if (!(x)) {TB_TRACE_LINE_TAG(tag, "[assert]: expr: %s", #x); goto b; } } while(0)
#	define TB_ASSERT_BREAK_TAG(tag, x)					{ if (!(x)) {TB_TRACE_LINE_TAG(tag, "[assert]: expr: %s", #x); break ; } }
#	define TB_ASSERT_CONTINUE_TAG(tag, x, b)			{ if (!(x)) {TB_TRACE_LINE_TAG(tag, "[assert]: expr: %s", #x); continue ; } }

# 	define TB_ASSERT(x)									TB_ASSERT_TAG(TB_TAG, x)
# 	define TB_ASSERT_ABORT(x)							TB_ASSERT_ABORT_TAG(TB_TAG, x)
# 	define TB_ASSERT_RETURN(x)							TB_ASSERT_RETURN_TAG(TB_TAG, x)
# 	define TB_ASSERT_RETURN_VAL(x, v)					TB_ASSERT_RETURN_VAL_TAG(TB_TAG, x, v)
# 	define TB_ASSERT_GOTO(x, b)							TB_ASSERT_GOTO_TAG(TB_TAG, x, b)
# 	define TB_ASSERT_BREAK(x)							TB_ASSERT_BREAK_TAG(TB_TAG, x)
# 	define TB_ASSERT_CONTINUE(x)						TB_ASSERT_CONTINUE_TAG(TB_TAG, x)

# 	define TB_ASSERTA(x)								TB_ASSERT_ABORT(x) 						//!< discarded
#else
#	define TB_ASSERT_TAG(x)
#	define TB_ASSERT_ABORT_TAG(x)
#	define TB_ASSERT_RETURN_TAG(x)
#	define TB_ASSERT_RETURN_VAL_TAG(x)
#	define TB_ASSERT_GOTO_TAG(x)
#	define TB_ASSERT_BREAK_TAG(x)
#	define TB_ASSERT_CONTINUE_TAG(x)

# 	define TB_ASSERT(x)
# 	define TB_ASSERT_ABORT(x)
# 	define TB_ASSERT_RETURN(x)
# 	define TB_ASSERT_RETURN_VAL(x)
# 	define TB_ASSERT_GOTO(x)
# 	define TB_ASSERT_BREAK(x)
# 	define TB_ASSERT_CONTINUE(x)

# 	define TB_ASSERTA(x)
#endif

#define TB_ASSERT_STATIC(x) 							do { typedef int __tb_static_assert__[(x)? 1 : -1]; } while(0)
#define TB_STATIC_ASSERT(x) 							TB_ASSERT_STATIC(x)		//!< discarded


// c plus plus
#ifdef __cplusplus
}
#endif

#endif


