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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		utils.h
 *
 */
#ifndef TB_PREFIX_UTILS_H
#define TB_PREFIX_UTILS_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// abs
#define tb_abs(x) 						((x) > 0? (x) : -(x))

// min & max
#define tb_max(x, y) 					(((x) > (y))? (x) : (y))
#define tb_min(x, y) 					(((x) < (y))? (x) : (y))

// min3 & max3
#define tb_max3(x, y, z) 				(((x) > (y))? (((x) > (z))? (x) : (z)) : (((y) > (z))? (y) : (z)))
#define tb_min3(x, y, z) 				(((x) < (y))? (((x) < (z))? (x) : (z)) : (((y) < (z))? (y) : (z)))

// the number of entries in the array
#define tb_arrayn(x) 					((sizeof((x)) / sizeof((x)[0])))

// ispow2: 1, 2, 4, 8, 16, 32, ...
#define tb_ispow2(x) 					(!((x) & ((x) - 1)) && (x))

// align
#define tb_align2(x) 					(((x) + 1) >> 1 << 1)
#define tb_align4(x) 					(((x) + 3) >> 2 << 2)
#define tb_align8(x) 					(((x) + 7) >> 3 << 3)
#define tb_align(x, b) 					(((x) + ((b) - 1)) & ~((b) - 1))
#define tb_align_pow2(x) 				(((x) > 1)? (tb_ispow2(x)? (x) : (1 << (32 - tb_bits_cl0_u32_be((tb_uint32_t)(x))))) : 2)

// offsetof
#if defined(TB_COMPILER_IS_GCC) \
	&&	TB_COMPILER_VERSION_BE(4, 1)
# 	define tb_offsetof(s, m) 			(tb_size_t)__builtin_offsetof(s, m)
#else
# 	define tb_offsetof(s, m) 			(tb_size_t)&(((s const*)0)->m)
#endif

// memsizeof
#define tb_memsizeof(s, m) 				sizeof(((s const*)0)->m)

// memtailof
#define tb_memtailof(s, m) 				(tb_offsetof(s, m) + tb_memsizeof(s, m))

// memdiffof: lm - rm
#define tb_memdiffof(s, lm, rm) 		(tb_memtailof(s, lm) - tb_memtailof(s, rm))

// check the offset and size of member for struct or union
#define tb_memberof_eq(ls, lm, rs, rm) 	((tb_offsetof(ls, lm) == tb_offsetof(rs, rm)) && (tb_memsizeof(ls, lm) == tb_memsizeof(rs, rm)))

// pointer to bool
#define tb_p2b(x) 						((tb_bool_t)(tb_size_t)(x))

// pointer to u8
#define tb_p2u8(x) 						((tb_uint8_t)(tb_size_t)(x))

// pointer to u16
#define tb_p2u16(x) 					((tb_uint16_t)(tb_size_t)(x))

// pointer to u32
#define tb_p2u32(x) 					((tb_uint32_t)(tb_size_t)(x))

// pointer to s8
#define tb_p2s8(x) 						((tb_sint8_t)(tb_long_t)(x))

// pointer to s16
#define tb_p2s16(x) 					((tb_sint16_t)(tb_long_t)(x))

// pointer to s32
#define tb_p2s32(x) 					((tb_sint32_t)(tb_long_t)(x))

// bool to pointer
#define tb_b2p(x) 						((tb_pointer_t)(tb_size_t)(x))

// unsigned integer to pointer
#define tb_u2p(x) 						((tb_pointer_t)(tb_size_t)(x))

// integer to pointer
#define tb_i2p(x) 						((tb_pointer_t)(tb_long_t)(x))

#endif


