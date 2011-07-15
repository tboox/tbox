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
 * \file		scalar.h
 *
 */
#ifndef TB_MATH_SCALAR_H
#define TB_MATH_SCALAR_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "fixed.h"
#include "fixed6.h"
#include "fixed16.h"
#include "fixed30.h"

#ifdef TB_CONFIG_TYPE_FLOAT
# 	include "float.h"
#endif

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef TB_CONFIG_TYPE_SCALAR_IS_FIXED

#define TB_SCALAR_ONE 				TB_FIXED_ONE
#define TB_SCALAR_HALF 				TB_FIXED_HALF
#define TB_SCALAR_MAX 				TB_FIXED_MAX
#define TB_SCALAR_MIN 				TB_FIXED_MIN
#define TB_SCALAR_NAN 				TB_FIXED_NAN
#define TB_SCALAR_INF 				TB_FIXED_INF
#define TB_SCALAR_PI 				TB_FIXED_PI
#define TB_SCALAR_SQRT2 			TB_FIXED_SQRT2

// conversion
#ifdef TB_CONFIG_TYPE_FLOAT
# 	define tb_scalar_to_float(x) 	tb_fixed_to_float(x)
# 	define tb_float_to_scalar(x) 	tb_float_to_fixed(x)
#endif

#define tb_int_to_scalar(x) 		tb_int_to_fixed(x)
#define tb_scalar_to_int(x) 		tb_fixed_to_int(x)

#define tb_fixed_to_scalar(x) 		(x)
#define tb_scalar_to_fixed(x) 		(x)

#define tb_fixed6_to_scalar(x) 		tb_fixed6_to_fixed(x)
#define tb_scalar_to_fixed6(x) 		tb_fixed_to_fixed6(x)

#define tb_fixed30_to_scalar(x) 	tb_fixed30_to_fixed(x)
#define tb_scalar_to_fixed30(x) 	tb_fixed_to_fixed30(x)

// round
#define tb_scalar_round(x) 			tb_fixed_round(x)
#define tb_scalar_ceil(x) 			tb_fixed_ceil(x)
#define tb_scalar_floor(x) 			tb_fixed_floor(x)

// functions
#define tb_scalar_abs(x) 			tb_fixed_abs(x)
#define tb_scalar_avg(x, y) 		tb_fixed_avg(x, y)
#define tb_scalar_mul(x, y) 		tb_fixed_mul(x, y)
#define tb_scalar_div(x, y) 		tb_fixed_div(x, y)
#define tb_scalar_invert(x) 		tb_fixed_invert(x)
#define tb_scalar_sqre(x) 			tb_fixed_sqre(x)
#define tb_scalar_sqrt(x) 			tb_fixed_sqrt(x)
#define tb_scalar_sin(x) 			tb_fixed_sin(x)
#define tb_scalar_cos(x) 			tb_fixed_cos(x)
#define tb_scalar_sincos(x, s, c) 	tb_fixed_sincos(x, s, c)
#define tb_scalar_tan(x) 			tb_fixed_tan(x)
#define tb_scalar_asin(x) 			tb_fixed_asin(x)
#define tb_scalar_acos(x) 			tb_fixed_acos(x)
#define tb_scalar_atan(x) 			tb_fixed_atan(x)
#define tb_scalar_atan2(y, x) 		tb_fixed_atan2(y, x)
#define tb_scalar_exp(x) 			tb_fixed_exp(x)
#define tb_scalar_exp1(x) 			tb_fixed_exp1(x)
#define tb_scalar_expi(x) 			tb_fixed_expi(x)
#define tb_scalar_ilog2(x) 			tb_fixed_ilog2(x)
#define tb_scalar_iclog2(x) 		tb_fixed_iclog2(x)
#define tb_scalar_irlog2(x) 		tb_fixed_irlog2(x)

#elif defined(TB_CONFIG_TYPE_FLOAT)

#define TB_SCALAR_ONE 				TB_FLOAT_ONE
#define TB_SCALAR_HALF 				TB_FLOAT_HALF
#define TB_SCALAR_MAX 				TB_FLOAT_MAX
#define TB_SCALAR_MIN 				TB_FLOAT_MIN
#define TB_SCALAR_NAN 				TB_FLOAT_NAN
#define TB_SCALAR_INF 				TB_FLOAT_INF
#define TB_SCALAR_PI 				TB_FLOAT_PI
#define TB_SCALAR_SQRT2 			TB_FLOAT_SQRT2

// conversion
#ifdef TB_CONFIG_TYPE_FLOAT
# 	define tb_scalar_to_float(x) 	tb_float_to_float(x)
# 	define tb_float_to_scalar(x) 	tb_float_to_float(x)
#endif

#define tb_int_to_scalar(x) 		tb_int_to_float(x)
#define tb_scalar_to_int(x) 		tb_float_to_int(x)

#define tb_fixed_to_scalar(x) 		tb_fixed_to_float(x)
#define tb_scalar_to_fixed(x) 		tb_float_to_fixed(x)

#define tb_fixed6_to_scalar(x) 		tb_fixed6_to_float(x)
#define tb_scalar_to_fixed6(x) 		tb_float_to_fixed6(x)

#define tb_fixed30_to_scalar(x) 	tb_fixed30_to_float(x)
#define tb_scalar_to_fixed30(x) 	tb_float_to_fixed30(x)

// round
#define tb_scalar_round(x) 			tb_float_round(x)
#define tb_scalar_ceil(x) 			tb_float_ceil(x)
#define tb_scalar_floor(x) 			tb_float_floor(x)

// functions
#define tb_scalar_abs(x) 			tb_float_abs(x)
#define tb_scalar_avg(x, y) 		tb_float_avg(x, y)
#define tb_scalar_mul(x, y) 		tb_float_mul(x, y)
#define tb_scalar_div(x, y) 		tb_float_div(x, y)
#define tb_scalar_invert(x) 		tb_float_invert(x)
#define tb_scalar_sqre(x) 			tb_float_sqre(x)
#define tb_scalar_sqrt(x) 			tb_float_sqrt(x)
#define tb_scalar_sin(x) 			tb_float_sin(x)
#define tb_scalar_cos(x) 			tb_float_cos(x)
#define tb_scalar_sincos(x, s, c) 	tb_float_sincos(x, s, c)
#define tb_scalar_tan(x) 			tb_float_tan(x)
#define tb_scalar_asin(x) 			tb_float_asin(x)
#define tb_scalar_acos(x) 			tb_float_acos(x)
#define tb_scalar_atan(x) 			tb_float_atan(x)
#define tb_scalar_atan2(y, x) 		tb_float_atan2(y, x)
#define tb_scalar_exp(x) 			tb_float_exp(x)
#define tb_scalar_exp1(x) 			tb_float_exp1(x)
#define tb_scalar_expi(x) 			tb_float_expi(x)
#define tb_scalar_ilog2(x) 			tb_float_ilog2(x)
#define tb_scalar_iclog2(x) 		tb_float_iclog2(x)
#define tb_scalar_irlog2(x) 		tb_float_irlog2(x)

#else
# 	error float is not supported.
#endif

#endif

