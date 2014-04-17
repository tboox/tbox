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
 * @file		fixed6.h
 * @ingroup 	math
 *
 */
#ifndef TB_MATH_FIXED6_H
#define TB_MATH_FIXED6_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "fixed16.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// constant
#define TB_FIXED6_ONE 					(64)
#define TB_FIXED6_HALF 					(32)
#define TB_FIXED6_MAX 					(TB_MAXS16)
#define TB_FIXED6_MIN 					(-TB_FIXED6_MAX)
#define TB_FIXED6_NAN 					((tb_int_t)0x8000)
#define TB_FIXED6_INF 					(TB_MAXS16)
#define TB_FIXED6_PI 					(0xc9)
#define TB_FIXED6_SQRT2 				(0x5a)

// conversion
#ifdef TB_CONFIG_TYPE_FLOAT
# 	ifndef tb_fixed6_to_float
# 		define tb_fixed6_to_float(x) 	(((x) * 0.015625))
# 	endif
# 	ifndef tb_float_to_fixed6
# 		define tb_float_to_fixed6(x) 	((tb_fixed6_t)((x) * TB_FIXED6_ONE))
# 	endif
#endif

#ifdef __tb_debug__
# 	define tb_int_to_fixed6(x) 		tb_long_to_fixed6_check(x)
# 	define tb_fixed6_to_int(x) 		tb_fixed6_to_long_check(x)

# 	define tb_long_to_fixed6(x) 	tb_long_to_fixed6_check(x)
# 	define tb_fixed6_to_long(x) 	tb_fixed6_to_long_check(x)
#else
# 	define tb_int_to_fixed6(x) 		(tb_fixed6_t)((x) << 6)
# 	define tb_fixed6_to_int(x) 		(tb_int_t)((x) >> 6)

# 	define tb_long_to_fixed6(x) 	(tb_fixed6_t)((x) << 6)
# 	define tb_fixed6_to_long(x) 	(tb_long_t)((x) >> 6)
#endif

#define tb_fixed6_to_fixed16(x) 	((x) << 10)
#define tb_fixed16_to_fixed6(x) 	((x) >> 10)

// round
#define tb_fixed6_round(x) 			(((x) + TB_FIXED6_HALF) >> 6)
#define tb_fixed6_ceil(x) 			(((x) + TB_FIXED6_ONE - 1) >> 6)
#define tb_fixed6_floor(x) 			((x) >> 6)

// operations
#define tb_fixed6_abs(x) 			tb_abs(x)
#define tb_fixed6_avg(x, y) 		(((x) + (y)) >> 1)

#ifndef tb_fixed6_div
#	define tb_fixed6_div(x, y) 		tb_fixed6_div_inline(x, y)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

#ifdef __tb_debug__
static __tb_inline__ tb_fixed6_t tb_long_to_fixed6_check(tb_long_t x)
{
	// check overflow
	tb_assert(x >= -1024 && x <= 1024);
	return (x << 6);
}
static __tb_inline__ tb_long_t tb_fixed6_to_int_check(tb_fixed6_t x)
{
	// no overflow, < int16 ?
	tb_assert(x == (tb_int16_t)x);
	return (x >> 6);
}
#endif

// @note the return value is the fixed16 type
static __tb_inline__ tb_fixed16_t tb_fixed6_div_inline(tb_fixed6_t x, tb_fixed6_t y)
{
	tb_assert(y != 0);
	// no overflow, < int16 ?
	if (x == (tb_int16_t)x) return (x << 16) / y;
	else return tb_fixed16_div(x, y);
}

#endif

