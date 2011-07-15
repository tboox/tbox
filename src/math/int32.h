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
 * \file		int32.h
 *
 */
#ifndef TB_MATH_INT32_H
#define TB_MATH_INT32_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// constant
#define TB_INT32_MAX 					(TB_MAXS32)
#define TB_INT32_MIN 					(TB_MINS32)

// clz
#ifdef TB_COMPILER_IS_GCC
# 	define tb_int32_clz(x) 				__builtin_clz(x)
#else 
# 	define tb_int32_clz(x) 				tb_int32_clz_generic(x)
#endif

// sign
#ifdef TB_DEBUG
# 	define tb_int32_get_sign(x) 		tb_int32_get_sign_check(x)
# 	define tb_int32_set_sign(x, s) 		tb_int32_set_sign_check(x, s)
#else
# 	define tb_int32_get_sign(x) 		((tb_int32_t)(x) >> 31)
# 	define tb_int32_set_sign(x, s) 		(((x) ^ (s)) - (s))
#endif


/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// sqrt
tb_uint32_t tb_int32_sqrt(tb_uint32_t x);

// log2
tb_uint32_t tb_int32_log2(tb_uint32_t x);
tb_uint32_t tb_int32_clog2(tb_uint32_t x);
tb_uint32_t tb_int32_rlog2(tb_uint32_t x);

// clz, count leading zeros
tb_size_t 	tb_int32_clz_generic(tb_uint32_t x);

// div
tb_int32_t 	tb_int32_div(tb_int32_t x, tb_int32_t y, tb_int_t nbits);

/* /////////////////////////////////////////////////////////
 * inline
 */

// returns -1 if x < 0, else returns 0
static __tb_inline__ tb_int32_t tb_int32_get_sign_check(tb_int32_t x)
{
	tb_int32_t s = ((tb_int32_t)(x) >> 31);
    TB_ASSERT((x < 0 && s == -1) || (x >= 0 && !s));
    return s;
}
// if s == -1, returns -x, else s must be 0, and returns x.
static __tb_inline__ tb_int32_t tb_int32_set_sign_check(tb_int32_t x, tb_int32_t s)
{
    TB_ASSERT(s == 0 || s == -1);
    return (x ^ s) - s;
}

#endif

