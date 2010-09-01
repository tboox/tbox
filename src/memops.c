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
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		memops.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "memops.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

/* /////////////////////////////////////////////////////////
 * interfaces 
 */

void tb_memset_u16(tb_byte_t* dst, tb_uint16_t src, tb_size_t size)
{
	if (!dst || !size) return ;
#if defined(TPLAT_ARCH_x86) && defined(TPLAT_ASSEMBLER_GAS)
# 	if 1
	__tplat_asm__
	(
		"cld\n\t" 		/* clear the direction bit, dst++, not dst-- */
		"rep stosw" 	/* *dst++ = ax */
		: 				/* no output registers */ 
		: "c" (size), "a" (src), "D" (dst) /* ecx = size, eax = src, edi = dst */
	); 
# 	else
	__tplat_asm__
	(
		"cld\n\t" 
		"mov %0, %%ecx\n\t"
		"mov %1, %%eax\n\t"
		"mov %2, %%edi\n\t"
		"rep stosw"
		: /* no output registers */ 
		: "m" (size), "m" (src), "m" (dst) 
		: "%ecx", "%eax", "%edi"
	); 
# 	endif
#elif defined(TPLAT_ARCH_SH4) && defined(TPLAT_ASSEMBLER_GAS)
# 	if 0
	dst += size << 1;
	__tplat_asm__
	(
		"1:\n\t" 
		"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
		"mov.w %1,@-%2\n\t" /* *--dst = src */
		"bf 1b\n\t"  		/* if T == 0 goto label 1: */
		: 					/* no output registers */ 
		: "r" (size), "r" (src), "r" (dst) /* constraint: register */
	); 
# 	else

	tb_size_t left = size & 0x3;
	dst += (size << 1);
	size >>= 2;
	if (!left)
	{
		__tplat_asm__ volatile
		(
			"1:\n\t" 
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (size), "r" (src), "r" (dst) /* constraint: register */
		); 
	}
	else
	{
		__tplat_asm__ volatile
		(
			"1:\n\t" 			/* fill left data */
			"dt %3\n\t"
			"mov.w %1,@-%2\n\t"
			"bf 1b\n\t"
			"2:\n\t"  			/* fill aligned data by 4 */
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"mov.w %1,@-%2\n\t" /* *--dst = src */
			"bf 2b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (size), "r" (src), "r" (dst), "r" (left) /* constraint: register */
		); 
	}

# 	endif
#else
# 	if 0
	tb_uint16_t* p = (tb_uint16_t*)dst;
	tb_uint16_t* e = p + size;
	while (p < e) *p++ = src;
# 	else
	tb_byte_t b1 = (src) & 0xff;
	tb_byte_t b2 = (src >> 8) & 0xff;
	if (b1 == b2) memset(dst, b1, size << 1);
	else 
	{
		// left = size % 4
		tb_size_t left = size & 0x3;
		size -= left;

		tb_uint16_t* p = (tb_uint16_t*)dst;
		tb_uint16_t* e = p + size;

		while (p < e)
		{
			p[0] = src;
			p[1] = src;
			p[2] = src;
			p[3] = src;
			p += 4;
		}

		while (left--) *p++ = src;
	}
# 	endif
#endif
}

void tb_memset_u24(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
	if (!dst || !size) return ;
#if 0
	tb_byte_t* p = dst;
	tb_byte_t* e = p + (size * 3);
	src &= 0xffffff;
	for (; p < e; p += 3) *((tb_uint32_t*)p) = src;
#else
	tb_byte_t b1 = (src) & 0xff;
	tb_byte_t b2 = (src >> 8) & 0xff;
	tb_byte_t b3 = (src >> 16) & 0xff;
	if ((b1 == b2) && (b1 == b3)) memset(dst, b1, size * 3);
	else 
	{
		// left = size % 4
		tb_size_t left = size & 0x3;
		size -= left;

		tb_byte_t* p = dst;
		tb_byte_t* e = p + (size * 3);
		src &= 0xffffff;
		while (p < e)
		{
			*((tb_uint32_t*)(p + 0)) = src;
			*((tb_uint32_t*)(p + 3)) = src;
			*((tb_uint32_t*)(p + 6)) = src;
			*((tb_uint32_t*)(p + 9)) = src;
			p += 12;
		}

		while (left--)
		{
			((tb_uint32_t*)p)[0] = src;
			p += 3;
		}
	}

#endif
}

void tb_memset_u32(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
	if (!dst || !size) return ;
#if defined(TPLAT_ARCH_x86) && defined(TPLAT_ASSEMBLER_GAS)
# 	if 1
	__tplat_asm__
	(
		"cld\n\t" 
		"rep stosl" 
		: /* no output registers */ 
		: "c" (size), "a" (src), "D" (dst) 
	); 
# 	else
	__tplat_asm__
	(
		"cld\n\t" 
		"mov %0, %%ecx\n\t"
		"mov %1, %%eax\n\t"
		"mov %2, %%edi\n\t"
		"rep stosl"
		: /* no output registers */ 
		: "m" (size), "m" (src), "m" (dst) 	/* constraint: memory */
		: "%ecx", "%eax", "%edi" 			/* these registers maybe modified */
	); 
# 	endif
#elif defined(TPLAT_ARCH_SH4) && defined(TPLAT_ASSEMBLER_GAS)
# 	if 0
	dst += size << 2;
	__tplat_asm__
	(
		"1:\n\t" 
		"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
		"mov.l %1,@-%2\n\t" /* *--dst = src */
		"bf 1b\n\t"  		/* if T == 0 goto label 1: */
		: 					/* no output registers */ 
		: "r" (size), "r" (src), "r" (dst) /* constraint: register */
	); 
# 	else
	tb_size_t left = size & 0x3;
	dst += (size << 2);
	if (!left)
	{
		size >>= 2;
		__tplat_asm__ volatile
		(
			"1:\n\t" 
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (size), "r" (src), "r" (dst) /* constraint: register */
		); 
	}
	else
	{
#if 0
		size >>= 2;
		__tplat_asm__ volatile
		(
			"1:\n\t" 			/* fill the left data */
			"dt %3\n\t"
			"mov.l %1,@-%2\n\t"
			"bf 1b\n\t"
			"2:\n\t" 			/* fill aligned data by 4 */
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"bf 2b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (size), "r" (src), "r" (dst), "r" (left) /* constraint: register */
		); 
#else
		__tplat_asm__
		(
			"1:\n\t" 
			"dt %0\n\t" 		/* i--, i > 0? T = 0 : 1 */
			"mov.l %1,@-%2\n\t" /* *--dst = src */
			"bf 1b\n\t"  		/* if T == 0 goto label 1: */
			: 					/* no output registers */ 
			: "r" (size), "r" (src), "r" (dst) /* constraint: register */
		); 
#endif
	}
# 	endif
#else
# 	if 0
	tb_uint32_t* p = (tb_uint32_t*)dst;
	tb_uint32_t* e = p + size;
	while (p < e) *p++ = src;
# 	else
	tb_byte_t b1 = (src) & 0xff;
	tb_byte_t b2 = (src >> 8) & 0xff;
	tb_byte_t b3 = (src >> 16) & 0xff;
	tb_byte_t b4 = (src >> 24) & 0xff;
	if ((b1 == b2) && (b1 == b3) && (b1 == b4)) memset(dst, b1, size << 2);
	else 
	{
		// left = size % 4
		tb_size_t left = size & 0x3;
		size -= left;

		tb_uint32_t* p = (tb_uint32_t*)dst;
		tb_uint32_t* e = p + size;
		while (p < e)
		{
			p[0] = src;
			p[1] = src;
			p[2] = src;
			p[3] = src;
			p += 4;
		}

		while (left--) *p++ = src;
	}
# 	endif
#endif
}

