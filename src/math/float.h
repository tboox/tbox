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
 * @file		float.h
 * @ingroup 	math
 *
 */
#ifndef TB_MATH_FLOAT_H
#define TB_MATH_FLOAT_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <math.h>

/* ///////////////////////////////////////////////////////////////////////
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
#define tb_int_to_float(x) 				((tb_float_t)(x))
#define tb_float_to_int(x) 				((tb_int_t)(x))

#define tb_long_to_float(x) 			((tb_float_t)(x))
#define tb_float_to_long(x) 			((tb_long_t)(x))

// round
#define tb_float_round(x) 				((x) > 0? (tb_int32_t)((x) + 0.5) : (tb_int32_t)((x) - 0.5))
#define tb_float_ceil(x) 				((x) > 0? (tb_int32_t)((x) + 0.9999999999) : (tb_int32_t)(x))
#define tb_float_floor(x) 				((x) > 0? (tb_int32_t)(x) : (tb_int32_t)((x) - 0.9999999999))

// abs
#define tb_float_abs(x) 				tb_abs(x)

// avg
#define tb_float_avg(x, y) 				(((x) + (y)) * 0.5)

// mul
#ifndef tb_float_mul
# 	define tb_float_mul(x, y) 			((x) * (y))
#endif

// div
#ifndef tb_float_div
# 	define tb_float_div(x, y) 			((x) / (y))
#endif

// imul
#ifndef tb_float_imul
# 	define tb_float_imul(x, y) 			((x) * (tb_float_t)(y))
#endif

// idiv
#ifndef tb_float_idiv
# 	define tb_float_idiv(x, y) 			((x) / (tb_float_t)(y))
#endif

// lsh
#ifndef tb_float_lsh
# 	define tb_float_lsh(x, y) 			((x) * (tb_float_t)(1 << (y)))
#endif

// rsh
#ifndef tb_float_rsh
# 	define tb_float_rsh(x, y) 			((x) / (tb_float_t)(1 << (y)))
#endif
	
// invert: 1 / x
#ifndef tb_float_invert
# 	define tb_float_invert(x) 			tb_float_div(TB_FLOAT_ONE, x)
#endif

// sqre: x * x
#ifndef tb_float_sqre
# 	define tb_float_sqre(x) 			((x) * (x))
#endif

// sqrt
#ifndef tb_float_sqrt
# 	define tb_float_sqrt(x) 			tb_float_sqrt_generic(x)
#endif

// sin
#ifndef tb_float_sin
# 	define tb_float_sin(x) 				tb_float_sin_generic(x)
#endif

// cos
#ifndef tb_float_cos
# 	define tb_float_cos(x) 				tb_float_cos_generic(x)
#endif

// sincos
#ifndef tb_float_sincos
# 	define tb_float_sincos(x, s, c) 	tb_float_sincos_generic(x, s, c)
#endif

// tan
#ifndef tb_float_tan
# 	define tb_float_tan(x) 				tb_float_tan_generic(x)
#endif

// asin
#ifndef tb_float_asin
# 	define tb_float_asin(x) 			tb_float_asin_generic(x)
#endif

// acos
#ifndef tb_float_acos
# 	define tb_float_acos(x) 			tb_float_acos_generic(x)
#endif

// atan
#ifndef tb_float_atan
# 	define tb_float_atan(x) 			tb_float_atan_generic(x)
#endif

// atan2
#ifndef tb_float_atan2
# 	define tb_float_atan2(y, x) 		tb_float_atan2_generic(y, x)
#endif

// exp
#ifndef tb_float_exp
# 	define tb_float_exp(x) 				tb_float_exp_generic(x)
#endif

// expi
#ifndef tb_float_expi
# 	define tb_float_expi(x) 			tb_float_expi_generic(x)
#endif

// exp1
#ifndef tb_float_exp1
# 	define tb_float_exp1(x) 			tb_float_exp1_inline(x)
#endif

// ilog2 = floor(log2)
#ifndef tb_float_ilog2
# 	define tb_float_ilog2(x) 			tb_float_ilog2_generic(x)
#endif

// iclog2 = ceil(log2)
#ifndef tb_float_iclog2
# 	define tb_float_iclog2(x) 			tb_float_iclog2_generic(x)
#endif

// irlog2 = round(log2)
#ifndef tb_float_irlog2
# 	define tb_float_irlog2(x) 			tb_float_irlog2_generic(x)
#endif


/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_float_t 	tb_float_sqrt_generic(tb_float_t x);
tb_float_t 	tb_float_sin_generic(tb_float_t x);
tb_float_t 	tb_float_cos_generic(tb_float_t x);
tb_void_t 	tb_float_sincos_generic(tb_float_t x, tb_float_t* s, tb_float_t* c);
tb_float_t 	tb_float_tan_generic(tb_float_t x);
tb_float_t 	tb_float_asin_generic(tb_float_t x);
tb_float_t 	tb_float_acos_generic(tb_float_t x);
tb_float_t 	tb_float_atan_generic(tb_float_t x);
tb_float_t 	tb_float_atan2_generic(tb_float_t y, tb_float_t x);
tb_float_t 	tb_float_exp_generic(tb_float_t x);
tb_float_t 	tb_float_expi_generic(tb_uint16_t x);
tb_uint32_t tb_float_ilog2_generic(tb_float_t x);
tb_uint32_t tb_float_iclog2_generic(tb_float_t x);
tb_uint32_t tb_float_irlog2_generic(tb_float_t x);

/* ///////////////////////////////////////////////////////////////////////
 * inlines
 */
// 1 + x + (x^2) / 2 + (x^3) / 6, for x = [-1, 1]
static __tb_inline__ tb_float_t tb_float_exp1_inline(tb_float_t x)
{
	tb_assert(x >= -1 && x <= 1);
	return (1 + (x) + ((x) * (x)) / 2 + ((x) * (x) * (x)) / 6);
}


#endif

