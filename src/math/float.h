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
 * \file		float.h
 *
 */
#ifndef TB_MATH_FLOAT_H
#define TB_MATH_FLOAT_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <math.h>

#if defined(TB_CONFIG_ARCH_x86)
# 	include "opt/float_x86.h"
#elif defined(TB_CONFIG_ARCH_ARM)
# 	include "opt/float_arm.h"
#elif defined(TB_CONFIG_ARCH_SH4)
# 	include "opt/float_sh4.h"
#endif

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// constant
#define TB_FLOAT_ONE 					(1.0f)
#define TB_FLOAT_HALF 					(0.5f)
#define TB_FLOAT_MAX 					(HUGE_VAL)
#define TB_FLOAT_MIN 					(-TB_FLOAT_MAX)
#define TB_FLOAT_NAN 					(NAN)
#define TB_FLOAT_INF					(INFINITY)
#define TB_FLOAT_PI 					(3.141592653589793)
#define TB_FLOAT_SQRT2 					(1.4142135623730951)

// cast
#define tb_float_to_float(x) 			((tb_float_t)(x))
#define tb_int_to_float(x) 				((tb_float_t)(x))
#define tb_float_to_int(x) 				((tb_int_t)(x))

// round
#define tb_float_round(x) 				((x) > 0? (tb_int32_t)((x) + 0.5) : (tb_int32_t)((x) - 0.5))
#define tb_float_ceil(x) 				((x) > 0? (tb_int32_t)((x) + 0.9999999999) : (tb_int32_t)(x))
#define tb_float_floor(x) 				((x) > 0? (tb_int32_t)(x) : (tb_int32_t)((x) - 0.9999999999))

// operations
#define tb_float_abs(x) 				tb_abs(x)
#define tb_float_avg(x, y) 				(((x) + (y)) * 0.5)

#ifndef tb_float_mul
    #define tb_float_mul(x, y) 			((x) * (y))
#endif

#ifndef tb_float_div
    #define tb_float_div(x, y) 			((x) / (y))
#endif

#ifndef tb_float_muladd
# 	define tb_float_muladd(x, y, a) 	((x) * (y) + (a))
#endif

#ifndef tb_float_square
    #define tb_float_square(x) 			((x) * (x))
#endif

#ifndef tb_float_sqrt
    #define tb_float_sqrt(x) 			tb_float_sqrt_generic(x)
#endif

#ifndef tb_float_sin
    #define tb_float_sin(x) 			tb_float_sin_generic(x)
#endif

#ifndef tb_float_cos
    #define tb_float_cos(x) 			tb_float_cos_generic(x)
#endif

#ifndef tb_float_tan
    #define tb_float_tan(x) 			tb_float_tan_generic(x)
#endif

#ifndef tb_float_asin
    #define tb_float_asin(x) 			tb_float_asin_generic(x)
#endif

#ifndef tb_float_acos
    #define tb_float_acos(x) 			tb_float_acos_generic(x)
#endif

#ifndef tb_float_atan
    #define tb_float_atan(x) 			tb_float_atan_generic(x)
#endif

#ifndef tb_float_exp
    #define tb_float_exp(x) 			tb_float_exp_generic(x)
#endif

#ifndef tb_float_ilog2
    #define tb_float_ilog2(x) 			tb_float_ilog2_generic(x)
#endif


/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_float_t 		tb_float_sqrt_generic(tb_float_t x);
tb_float_t 		tb_float_sin_generic(tb_float_t x);
tb_float_t 		tb_float_cos_generic(tb_float_t x);
tb_float_t 		tb_float_tan_generic(tb_float_t x);
tb_float_t 		tb_float_asin_generic(tb_float_t x);
tb_float_t 		tb_float_acos_generic(tb_float_t x);
tb_float_t 		tb_float_atan_generic(tb_float_t x);
tb_float_t 		tb_float_exp_generic(tb_float_t x);
tb_uint32_t 	tb_float_ilog2_generic(tb_float_t x);

#endif

