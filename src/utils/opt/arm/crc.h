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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		crc.h
 *
 */
#ifndef TB_UTILS_OPT_ARM_CRC_H
#define TB_UTILS_OPT_ARM_CRC_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_ASSEMBLER_GAS
# 	define tb_crc32_encode(crc, ib, in, table) 		tb_crc32_encode_asm(crc, ib, in, table)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_ASSEMBLER_GAS
static tb_uint32_t tb_crc32_encode_asm(tb_uint32_t crc, tb_byte_t const* ib, tb_size_t in, tb_uint32_t const* table)
{
	// check
	tb_assert_and_check_return_val(ib && in && table, 0);

	// done
	__tb_asm__ __tb_volatile__
	(
		"odd:\n" 								/* while (1) { */
		" 	ands r4, %1, #3\n" 					/* if (!(ib & 0x3)) goto even; */
		"	beq	even\n"
		"	subs %2, %2, #1\n" 					/* if (--in < 0) goto end; */
		"	blt	end\n"
												/* calc: crc = table[(*ib++ ^ crc) & 0xff] ^ (crc >> 8) */
		"	ldrb r4, [%1], #1\n" 				/* r4 = *ib++ */
		"	eor	r4, %0, r4\n" 					/* r4 ^= crc */
		"	and	r4, r4, #255\n"					/* r4 &= 0xff */
		"	ldr	r4, [%3, r4, lsl #2]\n" 		/* r4 = table[r4] */
		"	eor	%0, r4, %0, lsr #8\n" 			/* crc = r4 ^ (crc >> 8) */
		"	b odd\n" 							/* } */

		"evenloop:\n"
		" 	ldmia %1!, {r4 - r9, sl, ip}\n" 	/* ib[0-7] => r4-r9, sl, ip, ib += 8 * sizeof(uint32) */

		"	eor	lr, %0, r4\n"  					/* calc: crc = table[((r4 >> 0) ^ crc) & 0xff] ^ (crc >> 8) */
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n" 
		"	eor	%0, lr, %0, lsr #8\n" 	

		"	eor	lr, %0, r4, lsr #8\n" 			/* calc: crc = table[((r4 >> 8) ^ crc) & 0xff] ^ (crc >> 8) */
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r4, lsr #16\n" 			/* calc: crc = table[((r4 >> 16) ^ crc) & 0xff] ^ (crc >> 8) */
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r4, lsr #24\n" 			/* calc: crc = table[((r4 >> 24) ^ crc) & 0xff] ^ (crc >> 8) */
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r5\n" 					/* calc: for r5 */
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r5, lsr #8\n"
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r5, lsr #16\n"
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r5, lsr #24\n"
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r6\n" 					/* calc: for r6 */
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r6, lsr #8\n"
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r6, lsr #16\n"
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r6, lsr #24\n"
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r7\n" 					/* calc: for r7 */
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r7, lsr #8\n"
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r7, lsr #16\n"
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, r7, lsr #24\n"
		"	and	lr, lr, #255\n"
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0 ,r8\n" 					/* calc: for r8 */
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0 ,r8, lsr #8\n"	
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0 ,r8, lsr #16\n"	
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0 ,r8, lsr #24\n"	
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0 ,r9\n" 					/* calc: for r9 */
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0 ,r9, lsr #8\n"	
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0 ,r9, lsr #16\n"	
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0 ,r9, lsr #24\n"	
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, sl\n" 					/* calc: for sl */
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, sl, lsr #8\n"	
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, sl, lsr #16\n"	
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, sl, lsr #24\n"	
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, ip\n" 					/* calc: for ip */
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, ip, lsr #8\n"	
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, ip, lsr #16\n"	
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"	eor	lr, %0, ip, lsr #24\n"	
		"	and	lr, lr, #255\n"	
		"	ldr	lr, [%3, lr, lsl #2]\n"
		"	eor	%0, lr, %0, lsr #8\n"

		"even:\n"
		" 	subs %2, %2, #32\n"					/* in -= 32; if (in >= 0) goto evenloop; */
		"	bge	evenloop\n"

		"	adds %2, %2, #32\n"					/* left: in += 32; if (in < 0) goto end; */
		"	ble	end\n"

												/* calc: the left data crc */
		"left:\n" 								/* do { */
		" 	ldrb r4, [%1], #1\n" 				/* calc: crc = table[(*ib++ ^ crc) & 0xff] ^ (crc >> 8) */
		"	eor	r4, %0, r4\n" 					
		"	and	r4, r4, #255\n"	
		"	ldr	r4, [%3, r4, lsl #2]\n"
		"	eor	%0, r4, %0, lsr #8\n"

		"	subs %2, %2, #1\n" 					/* } while (--in) */
		"	bne	left\n"
		"end:\n"

		: "+r"(crc)
		: "r"(ib), "r"(in), "r"(table)
		: "r4", "r5", "r6", "r7", "r8", "r9", "sl", "ip"
	);

	// ok?
	return crc;
}
#endif


#endif

