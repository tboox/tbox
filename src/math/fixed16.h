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
 * \file		fixed16.h
 *
 */
#ifndef TB_MATH_FIXED16_H
#define TB_MATH_FIXED16_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

#if defined(TB_CONFIG_ARCH_x86)
# 	include "opt/fixed16_x86.h"
#elif defined(TB_CONFIG_ARCH_ARM)
# 	include "opt/fixed16_arm.h"
#elif defined(TB_CONFIG_ARCH_SH4)
# 	include "opt/fixed16_sh4.h"
#endif

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// constant
#define TB_FIXED16_ONE 					(1 << 16)
#define TB_FIXED16_HALF 				(1 << 15)
#define TB_FIXED16_MAX 					(TB_MAXS32)
#define TB_FIXED16_MIN 					(-TB_FIXED16_MAX)
#define TB_FIXED16_NAN 					((tb_int_t)0x80000000)
#define TB_FIXED16_INF 					(TB_MAXS32)
#define TB_FIXED16_PI 					(0x3243f)
#define TB_FIXED16_SQRT2 				(92682)

// conversion
#ifdef TB_CONFIG_TYPE_FLOAT
# 	ifndef tb_fixed16_to_float
# 		define tb_fixed16_to_float(x) 	(((x) * 1.5258789e-5))
# 	endif
# 	ifndef tb_float_to_fixed16
# 		define tb_float_to_fixed16(x) 	((tb_fixed16_t)((x) * TB_FIXED16_ONE))
# 	endif
#endif

#ifdef TB_DEBUG
# 	define tb_int_to_fixed16(x) 		tb_int_to_fixed16_check(x)
# 	define tb_fixed16_to_int(x) 		tb_fixed16_to_int_check(x)
#else
# 	define tb_int_to_fixed16(x) 		(tb_fixed16_t)((x) << 16)
# 	define tb_fixed16_to_int(x) 		(tb_int_t)((x) >> 16)
#endif

// round
#define tb_fixed16_round(x) 			(((x) + TB_FIXED16_HALF) >> 16)
#define tb_fixed16_ceil(x) 				(((x) + TB_FIXED16_ONE - 1) >> 16)
#define tb_fixed16_floor(x) 			((x) >> 16)

// operations
#define tb_fixed16_abs(x) 				tb_abs(x)
#define tb_fixed16_avg(x, y) 			(((x) + (y)) >> 1)

#ifdef TB_CONFIG_TYPE_INT64
# 	ifndef tb_fixed16_mul
# 		define tb_fixed16_mul(x, y) 	tb_fixed16_mul_int64(x, y)
# 	endif
# 	ifndef tb_fixed16_square
# 	 	define tb_fixed16_square(x) 	tb_fixed16_square_int64(x)
# 	endif
#endif

#ifndef tb_fixed16_mul
    #define tb_fixed16_mul(x, y) 		tb_fixed16_mul_generic(x, y)
#endif

#ifndef tb_fixed16_div
    #define tb_fixed16_div(x, y) 		tb_fixed16_div_generic(x, y)
#endif

#ifndef tb_fixed16_muladd
# 	define tb_fixed16_muladd(x, y, a) 	(tb_fixed16_mul(x, y) + (a))
#endif

#ifndef tb_fixed16_square
    #define tb_fixed16_square(x) 		tb_fixed16_square_generic(x)
#endif

#ifndef tb_fixed16_sqrt
    #define tb_fixed16_sqrt(x) 			tb_fixed16_sqrt_generic(x)
#endif

#ifndef tb_fixed16_sin
    #define tb_fixed16_sin(x) 			tb_fixed16_sin_generic(x)
#endif

#ifndef tb_fixed16_cos
    #define tb_fixed16_cos(x) 			tb_fixed16_cos_generic(x)
#endif

#ifndef tb_fixed16_tan
    #define tb_fixed16_tan(x) 			tb_fixed16_tan_generic(x)
#endif

#ifndef tb_fixed16_asin
    #define tb_fixed16_asin(x) 			tb_fixed16_asin_generic(x)
#endif

#ifndef tb_fixed16_acos
    #define tb_fixed16_acos(x) 			tb_fixed16_acos_generic(x)
#endif

#ifndef tb_fixed16_atan
    #define tb_fixed16_atan(x) 			tb_fixed16_atan_generic(x)
#endif

#ifndef tb_fixed16_exp
    #define tb_fixed16_exp(x) 			tb_fixed16_exp_generic(x)
#endif

#ifndef tb_fixed16_ilog2
    #define tb_fixed16_ilog2(x) 		tb_fixed16_ilog2_generic(x)
#endif


/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_fixed16_t tb_fixed16_mul_generic(tb_fixed16_t x, tb_fixed16_t y);
tb_fixed16_t tb_fixed16_div_generic(tb_fixed16_t x, tb_fixed16_t y);
tb_fixed16_t tb_fixed16_square_generic(tb_fixed16_t x, tb_fixed16_t y);
tb_fixed16_t tb_fixed16_mod_generic(tb_fixed16_t x, tb_fixed16_t y);
tb_fixed16_t tb_fixed16_sqrt_generic(tb_fixed16_t x);
tb_fixed16_t tb_fixed16_sin_generic(tb_fixed16_t x);
tb_fixed16_t tb_fixed16_cos_generic(tb_fixed16_t x);
tb_fixed16_t tb_fixed16_tan_generic(tb_fixed16_t x);
tb_fixed16_t tb_fixed16_asin_generic(tb_fixed16_t x);
tb_fixed16_t tb_fixed16_acos_generic(tb_fixed16_t x);
tb_fixed16_t tb_fixed16_atan_generic(tb_fixed16_t x);
tb_fixed16_t tb_fixed16_exp_generic(tb_fixed16_t x);
tb_uint32_t tb_fixed16_ilog2_generic(tb_fixed16_t x);

/* ////////////////////////////////////////////////////////////////////////
 * inlines
 */

#ifdef TB_DEBUG
static __tb_inline__ tb_fixed16_t tb_int_to_fixed16_check(tb_int_t x)
{
	// check overflow
	TB_ASSERT(x >= TB_MINS16 && x <= TB_MAXS16);
	return (x << 16);
}
static __tb_inline__ tb_int_t tb_fixed16_to_int_check(tb_fixed16_t x)
{
	// check overflow
	TB_ASSERT(x != TB_FIXED16_NAN);
	return (x >> 16);
}
#endif

#ifdef TB_CONFIG_TYPE_INT64
static __tb_inline__ tb_fixed16_t tb_fixed16_mul_int64(tb_fixed16_t x, tb_fixed16_t y)
{
	return (tb_fixed16_t)((tb_int64_t)x * y >> 16);
}
static __tb_inline__ tb_fixed16_t tb_fixed16_square_int64(tb_fixed16_t x)
{
	return (tb_fixed16_t)((tb_int64_t)x * x >> 16);
}
#endif


#endif

