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
 * \file		memops_arm.h
 *
 */
#ifndef TB_MEMORY_OPT_MEMOPS_ARM_H
#define TB_MEMORY_OPT_MEMOPS_ARM_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

#undef TB_MEMOPS_OPT_MEMSET_U8
#undef TB_MEMOPS_OPT_MEMSET_U16
#undef TB_MEMOPS_OPT_MEMSET_U32

#ifdef TB_CONFIG_ASSEMBLER_GAS
# 	define TB_MEMOPS_OPT_MEMSET_U16
# 	define TB_MEMOPS_OPT_MEMSET_U32
#endif

/* /////////////////////////////////////////////////////////
 * implemention
 */

#ifdef TB_CONFIG_ASSEMBLER_GAS
static __tb_inline__ void tb_memset_u16_opt_v1(tb_uint16_t* dst, tb_uint16_t src, tb_size_t size)
{
	// cache line: 16-bytes
	__tb_asm__ __tb_volatile__
	(
		"tst %2, #3\n\t" 						/* align by 4-bytes, if (dst & 0x3) *((tb_uint16_t*)dst) = src, dst += 2, size-- */
		"strneh %1, [%2], #2\n\t"
		"subnes %0, %0, #1\n\t"
		"orr %1, %1, %1, lsl #16\n\t" 			/* src |= src << 16, expand to 32-bits */
		"mov r3, %1\n\t" 						/* for storing data by 4x32bits */
		"mov r4, %1\n\t"
		"mov r5, %1\n\t"
		"1:\n\t" 								/* fill data by cache line size */
		"subs %0, %0, #8\n\t" 					/* size -= 4x2[x16bits] and update cpsr, assume 16-bytes cache lines */
		"stmhsia %2!, {%1, r3, r4, r5}\n\t" 	/* storing data by 4x32bits = 8[x16bits], cond: hs (if >= 0), ia: dst++ */
		"bhs 1b\n\t" 							/* goto 1b if hs (>= 0) */
		"add %0, %0, #8\n\t" 					/* fill the left data, size = left size (< 8[x16bits]) */
		"movs %0, %0, lsl #30\n\t" 				/* 1, 11000000000000000000000000000000 */
		"stmcsia %2!, {r4, r5}\n\t" 			/* store 2x32bits, cond: cs (if carry bit == 1, >= 4[x16bits]) */
		"strmi r3, [%2], #4\n\t" 				/* store 32bits, cond: mi (if negative bit == 1, >=2[x16bits]) */
		"movs %0, %0, lsl #2\n\t" 				/* 1, 00000000000000000000000000000000 */
		"strcsh %1, [%2], #2\n\t" 				/* store 16bits, cond: cs (if carry bit == 1, >= [x16bits]) */
		: : "r" (size), "r" (src), "r" (dst)
		: "r3", "r4", "r5"
	);
}
static __tb_inline__ void tb_memset_u16_opt_v2(tb_uint16_t* dst, tb_uint16_t src, tb_size_t size)
{
	// cache line: 32-bytes
	__tb_asm__ __tb_volatile__
	(
		"tst %2, #3\n\t" 					/* align by 4-bytes, if (dst & 0x3) *((tb_uint16_t*)dst) = src, dst += 2, size--*/
		"strneh %1, [%2], #2\n\t"
		"subnes %0, %0, #1\n\t"
		"orr %1, %1, %1, lsl #16\n\t" 			/* src |= src << 16, expand to 32-bits */
		"mov r3, %1\n\t" 						/* for storing data by 8x32bits */
		"mov r4, %1\n\t"
		"mov r5, %1\n\t"
		"1:\n\t" 								/* fill data by cache line size */
		"subs %0, %0, #16\n\t" 					/* size -= 8x2[x16bits] and update cpsr, assume 32-bytes cache lines */
		"stmhsia %2!, {%1, r3, r4, r5}\n\t" 	/* storing data by 8x32bits = 16[x16bits], cond: hs (if >= 0), ia: dst++ */
		"stmhsia %2!, {%1, r3, r4, r5}\n\t" 	
		"bhs 1b\n\t" 							/* goto 1b if hs (>= 0) */
		"add %0, %0, #16\n\t" 					/* fill the left data, size = left size (< 16[x16bits]) */
		"movs %0, %0, lsl #29\n\t" 				/* 1, 11100000000000000000000000000000 */
		"stmcsia %2!, {%1, r3, r4, r5}\n\t" 	/* store 4x32bits, cond: cs (if carry bit == 1, >= 8[x16bits]) */
		"stmmiia %2!, {r4, r5}\n\t" 			/* store 2x32bits, cond: mi (if negative bit == 1, >=4[x16bits]) */
		"movs %0, %0, lsl #2\n\t" 				/* 1, 10000000000000000000000000000000 */
		"strcs %1, [%2], #4\n\t" 				/* store 32bits, cond: cs (if carry bit == 1, >= 2[x16bits]) */
		"strmih r3, [%2], #2\n\t" 				/* store 16bits, cond: cs (if negative bit == 1, >= [x16bits]) */
		: : "r" (size), "r" (src), "r" (dst)
		: "r3", "r4", "r5"
	);
}
void tb_memset_u16(tb_byte_t* dst, tb_uint16_t src, tb_size_t size)
{
	if (!dst) return ;

	if (size > 1) tb_memset_u16_opt_v1((tb_uint16_t*)dst, src, size);
	else if (size == 1) *dst = src;
}
static __tb_inline__ void tb_memset_u32_opt_v1(tb_uint32_t* dst, tb_uint32_t src, tb_size_t size)
{
	// cache line: 16-bytes
	__tb_asm__ __tb_volatile__
	(
		"mov r3, %1\n\t" 						/* for storing data by 4x32bits */
		"mov r4, %1\n\t"
		"mov r5, %1\n\t"
		"1:\n\t" 								/* fill data by cache line size */
		"subs %0, %0, #4\n\t" 					/* size -= 4[x32bits] and update cpsr, assume 16-bytes cache lines */
		"stmhsia %2!, {%1, r3, r4, r5}\n\t" 	/* storing data by 4x32bits, cond: hs (if >= 0), ia: dst++ */
		"bhs 1b\n\t" 							/* goto 1b if hs (>= 0) */
		"add %0, %0, #4\n\t" 					/* fill the left data, size = left size (< 4[x32bits]) */
		"movs %0, %0, lsl #31\n\t" 				/* 1, 1000000000000000000000000000000 */
		"stmcsia %2!, {r4, r5}\n\t" 			/* store 2x32bits, cond: cs (if carry bit == 1, >= 2[x32bits]) */
		"strmi r3, [%2], #4\n\t" 				/* store 32bits, cond: mi (if negative bit == 1, >= [x32bits]) */
		: : "r" (size), "r" (src), "r" (dst)
		: "r3", "r4", "r5"
	);
}
static __tb_inline__ void tb_memset_u32_opt_v2(tb_uint32_t* dst, tb_uint32_t src, tb_size_t size)
{
	// cache line: 32-bytes
	__tb_asm__ __tb_volatile__
	(
		"mov r3, %1\n\t" 						/* for storing data by 4x32bits */
		"mov r4, %1\n\t"
		"mov r5, %1\n\t"
		"1:\n\t" 								/* fill data by cache line size */
		"subs %0, %0, #8\n\t" 					/* size -= 8[x16bits] and update cpsr, assume 32-bytes cache lines */
		"stmhsia %2!, {%1, r3, r4, r5}\n\t" 	/* storing data by 8x32bits, cond: hs (if >= 0), ia: dst++ */
		"stmhsia %2!, {%1, r3, r4, r5}\n\t" 	
		"bhs 1b\n\t" 							/* goto 1b if hs (>= 0) */
		"add %0, %0, #8\n\t" 					/* fill the left data, size = left size (< 8[x32bits]) */
		"movs %0, %0, lsl #30\n\t" 				/* 1, 1100000000000000000000000000000 */
		"stmcsia %2!, {%1, r3, r4, r5}\n\t" 	/* store 4x32bits, cond: cs (if carry bit == 1, >= 4[x32bits]) */
		"stmmiia %2!, {r4, r5}\n\t" 			/* store 2x32bits, cond: mi (if negative bit == 1, >=2[x32bits]) */
		"movs %0, %0, lsl #2\n\t" 				/* 1, 00000000000000000000000000000000 */
		"strcs %1, [%2], #4\n\t" 				/* store 32bits, cond: cs (if carry bit == 1, >= [x32bits]) */
		: : "r" (size), "r" (src), "r" (dst)
		: "r3", "r4", "r5"
	);
}
void tb_memset_u32(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
	if (!dst) return ;

	if (size > 1)  tb_memset_u32_opt_v1((tb_uint32_t*)dst, src, size);
	else if (size == 1) *dst = src;
}
#endif

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

