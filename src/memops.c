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
#if defined(TB_ASM_INLINE_ENABLE) && \
		!defined(TPLAT_CONFIG_COMPILER_NOT_SUPPORT_INLINE)
# 	if defined(TPLAT_ARCH_x86)
# 		define TB_ASM_ENABLE_memset_u16
//# 		define TB_ASM_ENABLE_memset_u24
# 		define TB_ASM_ENABLE_memset_u32
# 	elif defined(TPLAT_ARCH_ARM)
//# 		define TB_ASM_ENABLE_memset_u16
//# 		define TB_ASM_ENABLE_memset_u24
//# 		define TB_ASM_ENABLE_memset_u32
# 	elif defined(TPLAT_ARCH_SH4)
//# 		define TB_ASM_ENABLE_memset_u16
//# 		define TB_ASM_ENABLE_memset_u24
//# 		define TB_ASM_ENABLE_memset_u32
# 	elif defined(TPLAT_ARCH_MIPS)
//# 		define TB_ASM_ENABLE_memset_u16
//# 		define TB_ASM_ENABLE_memset_u24
//# 		define TB_ASM_ENABLE_memset_u32
# 	elif defined(TPLAT_ARCH_SPARC)
//# 		define TB_ASM_ENABLE_memset_u16
//# 		define TB_ASM_ENABLE_memset_u24
//# 		define TB_ASM_ENABLE_memset_u32
# 	elif defined(TPLAT_ARCH_PPC)
//# 		define TB_ASM_ENABLE_memset_u16
//# 		define TB_ASM_ENABLE_memset_u24
//# 		define TB_ASM_ENABLE_memset_u32
# 	endif
#endif

/* /////////////////////////////////////////////////////////
 * interfaces 
 */
#ifndef TB_ASM_ENABLE_memset_u16
#if 0
void tb_memset_u16(tb_byte_t* dst, tb_uint16_t src, tb_size_t size)
{
	tb_uint16_t* p = (tb_uint16_t*)dst;
	tb_uint16_t* e = p + size;
	while (p < e) *p++ = src;
}
#else
void tb_memset_u16(tb_byte_t* dst, tb_uint16_t src, tb_size_t size)
{
	wmemset(dst, src, size);
#if 0
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
#endif
}
#endif
#elif defined(TPLAT_ARCH_x86)
void tb_memset_u16(tb_byte_t* dst, tb_uint16_t src, tb_size_t size)
{
#ifdef TPLAT_COMPILER_IS_GCC
#if 1
	__tplat_asm__
	(
		"cld\n\t" 
		"rep stosw" 
		: /* no output registers */ 
		: "c" (size), "a" (src), "D" (dst) 
	); 
#else
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
#endif
#else
# 	error this compiler has not specific __tplat_asm__ code
#endif
}

#endif 

#ifndef TB_ASM_ENABLE_memset_u24
#if 0
void tb_memset_u24(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
	tb_byte_t* p = dst;
	tb_byte_t* e = p + (size * 3);
	src &= 0xffffff;
	for (; p < e; p += 3) *((tb_uint32_t*)p) = src;
}
#else
void tb_memset_u24(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
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
}
#endif
#endif

#ifndef TB_ASM_ENABLE_memset_u32
#if 0
void tb_memset_u32(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
	tb_uint32_t* p = (tb_uint32_t*)dst;
	tb_uint32_t* e = p + size;
	while (p < e) *p++ = src;
}
#else
void tb_memset_u32(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
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
}
#endif
#elif defined(TPLAT_ARCH_x86)
void tb_memset_u32(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
#ifdef TPLAT_COMPILER_IS_GCC
#if 1
	__tplat_asm__
	(
		"cld\n\t" 
		"rep stosl" 
		: /* no output registers */ 
		: "c" (size), "a" (src), "D" (dst) 
	); 
#else
	__tplat_asm__
	(
		"cld\n\t" 
		"mov %0, %%ecx\n\t"
		"mov %1, %%eax\n\t"
		"mov %2, %%edi\n\t"
		"rep stosl"
		: /* no output registers */ 
		: "m" (size), "m" (src), "m" (dst) 
		: "%ecx", "%eax", "%edi"
	); 
#endif
#else
# 	error this compiler has not specific __tplat_asm__ code
#endif
}
#endif

