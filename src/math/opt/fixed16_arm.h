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
 * \file		fixed16_arm.h
 *
 */
#ifndef TB_MATH_OPT_FIXED16_ARM_H
#define TB_MATH_OPT_FIXED16_ARM_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"


/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef TB_CONFIG_TYPE_FLOAT
# 	define tb_float_to_fixed16(x) 	tb_float_to_fixed16_asm(x)
#endif

//#define tb_fixed16_mul(x) 			tb_fixed16_mul_asm(x)

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

#ifdef TB_CONFIG_TYPE_FLOAT
static __tb_inline__ tb_fixed16_t const tb_float_to_fixed16_asm(tb_float_t x)
{
	__tb_register__ tb_fixed16_t y, z;
	__tb_asm__( "movs 	%1, %3, lsl #1 			\n"
				"mov 	%2, #0x8E 				\n"
				"sub 	%1, %2, %1, lsr #24 	\n"
				"mov 	%2, %3, lsl #8 			\n"
				"orr 	%2, %2, #0x80000000 	\n"
				"mov 	%1, %2, lsr %1 			\n"
				"rsbcs 	%1, %1, #0 				\n"
				: "=r"(x), "=&r"(y), "=&r"(z)
				: "r"(x)
				: "cc");
	return y;
}
#endif

#if 0
static __tb_inline__ tb_fixed16_t tb_fixed16_mul_asm(tb_fixed16_t x, tb_fixed16_t y)
{
	__tb_register__ tb_fixed16_t t;
	__tb_asm__( "smull 	%0, %2, %1, %3 			\n"
				"mov 	%0, %0, lsr #16 		\n"
				"orr 	%0, %0, %2, lsl #2 		\n"
				: "=r"(x), "=&r"(y), "=r"(t)
				: "r"(x), "1"(y)
				:
				);
	return x;
}
#endif

#endif

