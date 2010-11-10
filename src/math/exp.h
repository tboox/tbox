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
 * \file		exp.h
 *
 */
#ifndef TB_MATH_EXP_H
#define TB_MATH_EXP_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// for x is int, x = [-31, 31]
#define TB_MATH_FEXPI(x) 					(g_tb_math_fexpi_table[((x) + 15) & 0x3f])

// for x = [-1, 1]
#define TB_MATH_FEXPF1(x) 					(1 + (x) + ((x) * (x)) / 2 + ((x) * (x) * (x)) / 6)

// for x = [-inf, inf]
//#define TB_MATH_FEXPF(x) 					(TB_MATH_FEXPI(((tb_int_t)(x))) * TB_MATH_FEXPF1(((x) - (tb_int_t)(x))))
#define TB_MATH_FEXPF(x) 					tb_math_fexpf(x)

/* ////////////////////////////////////////////////////////////////////////
 * globals
 */
extern tb_float_t g_tb_math_fexpi_table[47];


/* ////////////////////////////////////////////////////////////////////////
 * implements
 */

static __tplat_inline__ tb_float_t tb_math_fexpf(tb_float_t x)
{
	tb_float_t a = x - (tb_int_t)x;
	return (TB_MATH_FEXPI(((tb_int_t)x)) * TB_MATH_FEXPF1(a));
}



#endif

