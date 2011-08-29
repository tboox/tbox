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
 * \file		memmov.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_void_t* tb_memset(tb_void_t* s, tb_size_t c, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(s, TB_NULL);

#if 0
	tb_int_t reg, edi;
	__tb_asm__ __tb_volatile__
	(
		/* Most of the time, count is divisible by 4 and nonzero */
		/* It's better to make this case faster */
		/*	"	jecxz	9f\n" - (optional) count == 0: goto ret */
		" 	mov	%%ecx, %1\n"
		" 	shr	$2, %%ecx\n"
		" 	jz	1f\n" /* zero words: goto fill_bytes */
		/* extend 8-bit fill to 32 bits */
		" 	movzx	%%al, %%eax\n" /* 3 bytes */
		/* or:	"	and	$0xff, %%eax\n" - 5 bytes */
		" 	imul	$0x01010101, %%eax\n" /* 6 bytes */
		/* fill full words */
		" 	rep; stosl\n"
		/* fill 0-3 bytes */
		"1:	and	$3, %1\n"
		"	jz	9f\n" /* (count & 3) == 0: goto end */
		"2:	stosb\n"
		"	dec	%1\n"
		"	jnz	2b\n"
		/* end */
		"9:\n"

		: "=&D" (edi), "=&r" (reg)
		: "0" (s), "a" (c), "c" (n)
 		: "memory"
	);
	return s;
#else
	return memset(s, c, n);
#endif
}
