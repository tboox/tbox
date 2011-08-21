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
 * \file		bits_arm.h
 *
 */
#ifndef TB_UTILS_OPT_BITS_ARM_H
#define TB_UTILS_OPT_BITS_ARM_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
//#define tb_bits_swap_u16(x) 	tb_bits_swap_u16_asm(x)
#define tb_bits_swap_u32(x) 	tb_bits_swap_u32_asm(x)

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// swap
static __tb_inline__ tb_uint16_t const tb_bits_swap_u16_asm(tb_uint16_t x)
{
	__tb_asm__("rev16 %0, %0" : "+r"(x));
	return x;
}

static __tb_inline__ tb_uint32_t const tb_bits_swap_u32_asm(tb_uint32_t x)
{
#if 0 // arm v6
	__tb_asm__("rev %0, %0" : "+r"(x));
#else
	tb_uint32_t t;
	__tb_asm__( "eor %1, %0, %0, ror #16 \n\t"
				"bic %1, %1, #0xff0000   \n\t"
				"mov %0, %0, ror #8      \n\t"
				"eor %0, %0, %1, lsr #8  \n\t"
				: "+r"(x), "=&r"(t));
#endif
	return x;
}


#endif

