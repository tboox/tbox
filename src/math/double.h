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
 * @file		double.h
 * @ingroup 	math
 *
 */
#ifndef TB_MATH_DOUBLE_H
#define TB_MATH_DOUBLE_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <math.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// constant
#define TB_DOUBLE_ONE 					(1.0f)
#define TB_DOUBLE_HALF 					(0.5f)
#define TB_DOUBLE_MAX 					(HUGE_VAL)
#define TB_DOUBLE_MIN 					(-TB_DOUBLE_MAX)
#define TB_DOUBLE_NAN 					(NAN)
#define TB_DOUBLE_INF					(INFINITY)
#define TB_DOUBLE_PI 					(3.141592653589793)
#define TB_DOUBLE_SQRT2 				(1.4142135623730951)

// cast
#define tb_int_to_double(x) 			((tb_double_t)(x))
#define tb_double_to_int(x) 			((tb_int_t)(x))

#define tb_long_to_double(x) 			((tb_double_t)(x))
#define tb_double_to_long(x) 			((tb_long_t)(x))

// round
#define tb_double_round(x) 				((x) > 0? (tb_int32_t)((x) + 0.5) : (tb_int32_t)((x) - 0.5))
#define tb_double_ceil(x) 				((x) > 0? (tb_int32_t)((x) + 0.9999999999) : (tb_int32_t)(x))
#define tb_double_floor(x) 				((x) > 0? (tb_int32_t)(x) : (tb_int32_t)((x) - 0.9999999999))

// abs
#define tb_double_abs(x) 				tb_abs(x)

// avg
#define tb_double_avg(x, y) 			(((x) + (y)) * 0.5)

// mul
#ifndef tb_double_mul
# 	define tb_double_mul(x, y) 			((x) * (y))
#endif

// div
#ifndef tb_double_div
# 	define tb_double_div(x, y) 			((x) / (y))
#endif

// imul
#ifndef tb_double_imul
# 	define tb_double_imul(x, y) 		((x) * (tb_double_t)(y))
#endif

// idiv
#ifndef tb_double_idiv
# 	define tb_double_idiv(x, y) 		((x) / (tb_double_t)(y))
#endif

// lsh
#ifndef tb_double_lsh
# 	define tb_double_lsh(x, y) 			((x) * (tb_double_t)(1 << (y)))
#endif

// rsh
#ifndef tb_double_rsh
# 	define tb_double_rsh(x, y) 			((x) / (tb_double_t)(1 << (y)))
#endif
	
// invert: 1 / x
#ifndef tb_double_invert
# 	define tb_double_invert(x) 			tb_double_div(TB_DOUBLE_ONE, x)
#endif

// sqre: x * x
#ifndef tb_double_sqre
# 	define tb_double_sqre(x) 			((x) * (x))
#endif

// sqrt
#ifndef tb_double_sqrt
# 	define tb_double_sqrt(x) 			tb_double_sqrt_generic(x)
#endif

// sin
#ifndef tb_double_sin
# 	define tb_double_sin(x) 			tb_double_sin_generic(x)
#endif

// cos
#ifndef tb_double_cos
# 	define tb_double_cos(x) 			tb_double_cos_generic(x)
#endif

// sincos
#ifndef tb_double_sincos
# 	define tb_double_sincos(x, s, c) 	tb_double_sincos_generic(x, s, c)
#endif

// tan
#ifndef tb_double_tan
# 	define tb_double_tan(x) 			tb_double_tan_generic(x)
#endif

// asin
#ifndef tb_double_asin
# 	define tb_double_asin(x) 			tb_double_asin_generic(x)
#endif

// acos
#ifndef tb_double_acos
# 	define tb_double_acos(x) 			tb_double_acos_generic(x)
#endif

// atan
#ifndef tb_double_atan
# 	define tb_double_atan(x) 			tb_double_atan_generic(x)
#endif

// atan2
#ifndef tb_double_atan2
# 	define tb_double_atan2(y, x) 		tb_double_atan2_generic(y, x)
#endif

// exp
#ifndef tb_double_exp
# 	define tb_double_exp(x) 			tb_double_exp_generic(x)
#endif

// expi
#ifndef tb_double_expi
# 	define tb_double_expi(x) 			tb_double_expi_generic(x)
#endif

// exp1
#ifndef tb_double_exp1
# 	define tb_double_exp1(x) 			tb_double_exp1_inline(x)
#endif

// ilog2 = floor(log2)
#ifndef tb_double_ilog2
# 	define tb_double_ilog2(x) 			tb_double_ilog2_generic(x)
#endif

// iclog2 = ceil(log2)
#ifndef tb_double_iclog2
# 	define tb_double_iclog2(x) 			tb_double_iclog2_generic(x)
#endif

// irlog2 = round(log2)
#ifndef tb_double_irlog2
# 	define tb_double_irlog2(x) 			tb_double_irlog2_generic(x)
#endif


/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_double_t 	tb_double_sqrt_generic(tb_double_t x);
tb_double_t 	tb_double_sin_generic(tb_double_t x);
tb_double_t 	tb_double_cos_generic(tb_double_t x);
tb_void_t 		tb_double_sincos_generic(tb_double_t x, tb_double_t* s, tb_double_t* c);
tb_double_t 	tb_double_tan_generic(tb_double_t x);
tb_double_t 	tb_double_asin_generic(tb_double_t x);
tb_double_t 	tb_double_acos_generic(tb_double_t x);
tb_double_t 	tb_double_atan_generic(tb_double_t x);
tb_double_t 	tb_double_atan2_generic(tb_double_t y, tb_double_t x);
tb_double_t 	tb_double_exp_generic(tb_double_t x);
tb_double_t 	tb_double_expi_generic(tb_uint16_t x);
tb_uint32_t 	tb_double_ilog2_generic(tb_double_t x);
tb_uint32_t 	tb_double_iclog2_generic(tb_double_t x);
tb_uint32_t 	tb_double_irlog2_generic(tb_double_t x);

/* ///////////////////////////////////////////////////////////////////////
 * inlines
 */
// 1 + x + (x^2) / 2 + (x^3) / 6, for x = [-1, 1]
static __tb_inline__ tb_double_t tb_double_exp1_inline(tb_double_t x)
{
	tb_assert(x >= -1 && x <= 1);
	return (1 + (x) + ((x) * (x)) / 2 + ((x) * (x) * (x)) / 6);
}


#endif

