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
 * \file		memset.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#define TB_LIBC_STRING_OPT_MEMSET_U8

#if defined(TB_CONFIG_ASSEMBLER_GAS)
# 	define TB_LIBC_STRING_OPT_MEMSET_U16
# 	define TB_LIBC_STRING_OPT_MEMSET_U32
#endif
/* /////////////////////////////////////////////////////////
 * implemention
 */

#ifdef TB_CONFIG_ASSEMBLER_GAS
static __tb_inline__ tb_void_t tb_memset_u8_opt_v1(tb_byte_t* s, tb_byte_t c, tb_size_t n)
{

}
#endif

#ifdef TB_LIBC_STRING_OPT_MEMSET_U8
tb_void_t* tb_memset(tb_void_t* s, tb_size_t c, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(s, TB_NULL);
	if (!n) return s;

# 	if 1
	memset(s, c, n);
# 	elif defined(TB_CONFIG_ASSEMBLER_GAS)
	tb_memset_u8_opt_v1(s, (tb_byte_t)c, n);
# 	else
# 		error
# 	endif

	return s;
}
#endif


#ifdef TB_CONFIG_ASSEMBLER_GAS
static __tb_inline__ tb_void_t tb_memset_u16_opt_v1(tb_uint16_t* s, tb_uint16_t c, tb_size_t n)
{
	// cache line: 16-bytes
	__tb_asm__ __tb_volatile__
	(
		"tst %2, #3\n\t" 						/* align by 4-bytes, if (s & 0x3) *((tb_uint16_t*)s) = c, s += 2, n-- */
		"strneh %1, [%2], #2\n\t"
		"subnes %0, %0, #1\n\t"
		"orr %1, %1, %1, lsl #16\n\t" 			/* c |= c << 16, expand to 32-bits */
		"mov r3, %1\n\t" 						/* for storing data by 4x32bits */
		"mov r4, %1\n\t"
		"mov r5, %1\n\t"
		"1:\n\t" 								/* fill data by cache line n */
		"subs %0, %0, #8\n\t" 					/* n -= 4x2[x16bits] and update cpsr, assume 16-bytes cache lines */
		"stmhsia %2!, {%1, r3, r4, r5}\n\t" 	/* storing data by 4x32bits = 8[x16bits], cond: hs (if >= 0), ia: s++ */
		"bhs 1b\n\t" 							/* goto 1b if hs (>= 0) */
		"add %0, %0, #8\n\t" 					/* fill the left data, n = left n (< 8[x16bits]) */
		"movs %0, %0, lsl #30\n\t" 				/* 1, 11000000000000000000000000000000 */
		"stmcsia %2!, {r4, r5}\n\t" 			/* store 2x32bits, cond: cs (if carry bit == 1, >= 4[x16bits]) */
		"strmi r3, [%2], #4\n\t" 				/* store 32bits, cond: mi (if negative bit == 1, >=2[x16bits]) */
		"movs %0, %0, lsl #2\n\t" 				/* 1, 00000000000000000000000000000000 */
		"strcsh %1, [%2], #2\n\t" 				/* store 16bits, cond: cs (if carry bit == 1, >= [x16bits]) */
		: : "r" (n), "r" (c), "r" (s)
		: "r3", "r4", "r5"
	);
}
static __tb_inline__ tb_void_t tb_memset_u16_opt_v2(tb_uint16_t* s, tb_uint16_t c, tb_size_t n)
{
	// cache line: 32-bytes
	__tb_asm__ __tb_volatile__
	(
		"tst %2, #3\n\t" 					/* align by 4-bytes, if (s & 0x3) *((tb_uint16_t*)s) = c, s += 2, n--*/
		"strneh %1, [%2], #2\n\t"
		"subnes %0, %0, #1\n\t"
		"orr %1, %1, %1, lsl #16\n\t" 			/* c |= c << 16, expand to 32-bits */
		"mov r3, %1\n\t" 						/* for storing data by 8x32bits */
		"mov r4, %1\n\t"
		"mov r5, %1\n\t"
		"1:\n\t" 								/* fill data by cache line n */
		"subs %0, %0, #16\n\t" 					/* n -= 8x2[x16bits] and update cpsr, assume 32-bytes cache lines */
		"stmhsia %2!, {%1, r3, r4, r5}\n\t" 	/* storing data by 8x32bits = 16[x16bits], cond: hs (if >= 0), ia: s++ */
		"stmhsia %2!, {%1, r3, r4, r5}\n\t" 	
		"bhs 1b\n\t" 							/* goto 1b if hs (>= 0) */
		"add %0, %0, #16\n\t" 					/* fill the left data, n = left n (< 16[x16bits]) */
		"movs %0, %0, lsl #29\n\t" 				/* 1, 11100000000000000000000000000000 */
		"stmcsia %2!, {%1, r3, r4, r5}\n\t" 	/* store 4x32bits, cond: cs (if carry bit == 1, >= 8[x16bits]) */
		"stmmiia %2!, {r4, r5}\n\t" 			/* store 2x32bits, cond: mi (if negative bit == 1, >=4[x16bits]) */
		"movs %0, %0, lsl #2\n\t" 				/* 1, 10000000000000000000000000000000 */
		"strcs %1, [%2], #4\n\t" 				/* store 32bits, cond: cs (if carry bit == 1, >= 2[x16bits]) */
		"strmih r3, [%2], #2\n\t" 				/* store 16bits, cond: cs (if negative bit == 1, >= [x16bits]) */
		: : "r" (n), "r" (c), "r" (s)
		: "r3", "r4", "r5"
	);
}
#endif

#ifdef TB_LIBC_STRING_OPT_MEMSET_U16
tb_void_t* tb_memset_u16(tb_void_t* s, tb_size_t c, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(s, TB_NULL);

	// align by 2-bytes 
	TB_ASSERT(!(((tb_size_t)s) & 0x1));
	if (!n) return s;

# 	if defined(TB_CONFIG_ASSEMBLER_GAS)
	tb_memset_u16_opt_v1(s, (tb_uint16_t)c, n);
# 	else
# 		error
# 	endif

	return s;
}
#endif

#ifdef TB_CONFIG_ASSEMBLER_GAS
static __tb_inline__ tb_void_t tb_memset_u32_opt_v1(tb_uint32_t* s, tb_uint32_t c, tb_size_t n)
{
	// cache line: 16-bytes
	__tb_asm__ __tb_volatile__
	(
		"mov r3, %1\n\t" 						/* for storing data by 4x32bits */
		"mov r4, %1\n\t"
		"mov r5, %1\n\t"
		"1:\n\t" 								/* fill data by cache line n */
		"subs %0, %0, #4\n\t" 					/* n -= 4[x32bits] and update cpsr, assume 16-bytes cache lines */
		"stmhsia %2!, {%1, r3, r4, r5}\n\t" 	/* storing data by 4x32bits, cond: hs (if >= 0), ia: s++ */
		"bhs 1b\n\t" 							/* goto 1b if hs (>= 0) */
		"add %0, %0, #4\n\t" 					/* fill the left data, n = left n (< 4[x32bits]) */
		"movs %0, %0, lsl #31\n\t" 				/* 1, 1000000000000000000000000000000 */
		"stmcsia %2!, {r4, r5}\n\t" 			/* store 2x32bits, cond: cs (if carry bit == 1, >= 2[x32bits]) */
		"strmi r3, [%2], #4\n\t" 				/* store 32bits, cond: mi (if negative bit == 1, >= [x32bits]) */
		: : "r" (n), "r" (c), "r" (s)
		: "r3", "r4", "r5"
	);
}
static __tb_inline__ tb_void_t tb_memset_u32_opt_v2(tb_uint32_t* s, tb_uint32_t c, tb_size_t n)
{
	// cache line: 32-bytes
	__tb_asm__ __tb_volatile__
	(
		"mov r3, %1\n\t" 						/* for storing data by 4x32bits */
		"mov r4, %1\n\t"
		"mov r5, %1\n\t"
		"1:\n\t" 								/* fill data by cache line n */
		"subs %0, %0, #8\n\t" 					/* n -= 8[x16bits] and update cpsr, assume 32-bytes cache lines */
		"stmhsia %2!, {%1, r3, r4, r5}\n\t" 	/* storing data by 8x32bits, cond: hs (if >= 0), ia: s++ */
		"stmhsia %2!, {%1, r3, r4, r5}\n\t" 	
		"bhs 1b\n\t" 							/* goto 1b if hs (>= 0) */
		"add %0, %0, #8\n\t" 					/* fill the left data, n = left n (< 8[x32bits]) */
		"movs %0, %0, lsl #30\n\t" 				/* 1, 1100000000000000000000000000000 */
		"stmcsia %2!, {%1, r3, r4, r5}\n\t" 	/* store 4x32bits, cond: cs (if carry bit == 1, >= 4[x32bits]) */
		"stmmiia %2!, {r4, r5}\n\t" 			/* store 2x32bits, cond: mi (if negative bit == 1, >=2[x32bits]) */
		"movs %0, %0, lsl #2\n\t" 				/* 1, 00000000000000000000000000000000 */
		"strcs %1, [%2], #4\n\t" 				/* store 32bits, cond: cs (if carry bit == 1, >= [x32bits]) */
		: : "r" (n), "r" (c), "r" (s)
		: "r3", "r4", "r5"
	);
}
#endif

#ifdef TB_LIBC_STRING_OPT_MEMSET_U32
tb_void_t* tb_memset_u32(tb_void_t* s, tb_size_t c, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(s, TB_NULL);

	// align by 4-bytes 
	TB_ASSERT(!(((tb_size_t)s) & 0x3));
	if (!n) return s;

# 	if defined(TB_CONFIG_ASSEMBLER_GAS)
	tb_memset_u32_opt_v1(s, (tb_uint32_t)c, n);
# 	else
# 		error
# 	endif

	return s;
}
#endif


