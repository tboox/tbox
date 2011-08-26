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
 * \file		fixed30_arm.h
 *
 */
#ifndef TB_MATH_OPT_FIXED30_ARM_H
#define TB_MATH_OPT_FIXED30_ARM_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

#define tb_fixed30_mul(x) 			tb_fixed30_mul_asm(x)

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

static __tb_inline__ tb_fixed30_t tb_fixed30_mul_asm(tb_fixed30_t x, tb_fixed30_t y)
{
	__tb_register__ tb_fixed30_t t;
	__tb_asm__( "smull 	%0, %2, %1, %3 			\n"
				"mov 	%0, %0, lsr #30 		\n"
				"orr 	%0, %0, %2, lsl #2 		\n"
				: "=r"(x), "=&r"(y), "=r"(t)
				: "r"(x), "1"(y)
				:
				);
	return x;
}


#endif

