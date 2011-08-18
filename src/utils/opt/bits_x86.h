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
 * \file		bits_x86.h
 *
 */
#ifndef TB_UTILS_OPT_BITS_x86_H
#define TB_UTILS_OPT_BITS_x86_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
#define tb_bits_swap_u16(x) 	tb_bits_swap_u16_asm(x)
#define tb_bits_swap_u32(x) 	tb_bits_swap_u32_asm(x)

#ifdef TB_CONFIG_TYPE_INT64
//# 	define tb_bits_swap_u64(x) 	tb_bits_swap_u64_asm(x)
#endif
/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// swap
static __tb_inline__ tb_uint16_t const tb_bits_swap_u16_asm(tb_uint16_t x)
{
	__tb_asm__("rorw $8, %w0" : "+r"(x));
	return x;
}

static __tb_inline__ tb_uint32_t const tb_bits_swap_u32_asm(tb_uint32_t x)
{
#if 1
	__asm__("bswap   %0" : "+r" (x));
#else
	__tb_asm__( "rorw    $8,  %w0 \n\t"
				"rorl    $16, %0  \n\t"
				"rorw    $8,  %w0"
				: "+r"(x));
#endif
	return x;
}

#ifdef TB_CONFIG_TYPE_INT64
static __tb_inline__ tb_uint64_t const tb_bits_swap_u64_asm(tb_uint64_t x)
{
	// FIXME: swap u32?
	__tb_asm__("bswap  %0": "=r" (x) : "0" (x));
	return x;
}
#endif

#endif

