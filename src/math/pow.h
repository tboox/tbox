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
 * \file		pow.h
 *
 */
#ifndef TB_MATH_POW_H
#define TB_MATH_POW_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "exp.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// fpow2i(x) == 2^x == x >= 0? (1 << x) : (1 / (1 << x))
#define TB_MATH_FPOW2I(x) 					(g_tb_math_fpow2i_table[((x) + 21) & 0x3f])

// fpow2f1(x) == exp(x * ln2), x = [-1, 1]
#define TB_MATH_FPOW2F1(x) 					TB_MATH_FEXPF1(((x) * TB_MATH_CONST_LN2))

// fpow2f(x) == 2^[x] * 2^(x - [x]) ~= 2^[x] * exp((x - [x]) * ln2)
//#define TB_MATH_FPOW2F(x) 				(TB_MATH_FPOW2I((tb_int_t)(x)) * TB_MATH_FEXPF1(((x) - (tb_int_t)(x)) * TB_MATH_CONST_LN2))
#define TB_MATH_FPOW2F(x) 					tb_math_fpow2f(x)

/* ////////////////////////////////////////////////////////////////////////
 * globals
 */
extern tb_float_t g_tb_math_fpow2i_table[53];

/* ////////////////////////////////////////////////////////////////////////
 * implements
 */
static __tplat_inline__ tb_float_t tb_math_fpow2f(tb_float_t x)
{
	tb_float_t a = (x - (tb_int_t)x) * TB_MATH_CONST_LN2;
	return (TB_MATH_FPOW2I((tb_int_t)x) * TB_MATH_FEXPF1(a));
}


#endif

