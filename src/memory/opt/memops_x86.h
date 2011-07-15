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
 * \file		memops_x86.h
 *
 */
#ifndef TB_MEMORY_OPT_MEMOPS_x86_H
#define TB_MEMORY_OPT_MEMOPS_x86_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#ifdef TB_CONFIG_OPTI_SSE2_ENABLE
# 	include <emmintrin.h>
#endif

/* /////////////////////////////////////////////////////////
 * macros
 */

#undef TB_MEMOPS_OPT_MEMSET_U8
#undef TB_MEMOPS_OPT_MEMSET_U16
#undef TB_MEMOPS_OPT_MEMSET_U32

#if defined(TB_CONFIG_ASSEMBLER_GAS) || \
		defined(TB_CONFIG_OPTI_SSE2_ENABLE)
# 	define TB_MEMOPS_OPT_MEMSET_U16
# 	define TB_MEMOPS_OPT_MEMSET_U32
#endif

/* /////////////////////////////////////////////////////////
 * implemention
 */

#ifdef TB_CONFIG_ASSEMBLER_GAS
static __tb_inline__ void tb_memset_u16_opt_v1(tb_uint16_t* dst, tb_uint16_t src, tb_size_t size)
{
	// align by 4-bytes 
	if (((tb_size_t)dst) & 0x3)
	{
		*dst++ = src;
		--size;
	}

	__tb_asm__ __tb_volatile__
	(
		"cld\n\t" 							// clear the direction bit, dst++, not dst--
		"rep stosw" 						// *dst++ = ax
		: 									// no output registers
		: "c" (size), "a" (src), "D" (dst) 	// ecx = size, eax = src, edi = dst
	);
}
#endif

#ifdef TB_CONFIG_OPTI_SSE2_ENABLE
static __tb_inline__ void tb_memset_u16_opt_v2(tb_uint16_t* dst, tb_uint16_t src, tb_size_t size)
{
    if (size >= 32) 
	{
		// aligned by 16-bytes
        for (; ((tb_size_t)dst) & 0x0f; --size) *dst++ = src;

		// left = size % 32
		tb_size_t left = size & 0x1f;
		size = (size - left) >> 5;

		// fill 4 x 8 bytes
        __m128i* 	d = (__m128i*)(dst);
        __m128i 	v = _mm_set1_epi16(src);
        while (size) 
		{
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            --size;
        }
        dst = (tb_uint16_t*)(d);
		size = left;
    }
	while (size--) *dst++ = src;
}
#endif

#ifdef TB_MEMOPS_OPT_MEMSET_U16
void tb_memset_u16(tb_byte_t* dst, tb_uint16_t src, tb_size_t size)
{
	if (!dst) return ;

	if (size > 1)
	{
# 	if defined(TB_CONFIG_ASSEMBLER_GAS) && \
		defined(TB_CONFIG_OPTI_SSE2_ENABLE)
		if (size < 2049) tb_memset_u16_opt_v2((tb_uint16_t*)dst, src, size);
		else tb_memset_u16_opt_v1((tb_uint16_t*)dst, src, size);
# 	elif defined(TB_CONFIG_ASSEMBLER_GAS)
		tb_memset_u16_opt_v1((tb_uint16_t*)dst, src, size);
# 	elif defined(TB_CONFIG_OPTI_SSE2_ENABLE)
		tb_memset_u16_opt_v2((tb_uint16_t*)dst, src, size);
# 	else
# 		error
# 	endif
	}
	else if (size == 1) *dst = src;
}
#endif

#ifdef TB_CONFIG_ASSEMBLER_GAS
static __tb_inline__ void tb_memset_u32_opt_v1(tb_uint32_t* dst, tb_uint32_t src, tb_size_t size)
{
	__tb_asm__ __tb_volatile__
	(
		"cld\n\t" 							// clear the direction bit, dst++, not dst--
		"rep stosl" 						// *dst++ = eax
		: 									// no output registers
		: "c" (size), "a" (src), "D" (dst) 	// ecx = size, eax = src, edi = dst
	);
}
#endif

#ifdef TB_CONFIG_OPTI_SSE2_ENABLE
static __tb_inline__ void tb_memset_u32_opt_v2(tb_uint32_t* dst, tb_uint32_t src, tb_size_t size)
{
    if (size >= 16) 
	{
		// aligned by 16-bytes
        for (; ((tb_size_t)dst) & 0x0f; --size) *dst++ = src;

		// left = size % 16
		tb_size_t left = size & 0x0f;
		size = (size - left) >> 4;

		// fill 4 x 16 bytes
        __m128i* 	d = (__m128i*)(dst);
        __m128i 	v = _mm_set1_epi32(src);
        while (size) 
		{
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            --size;
        }
        dst = (tb_uint32_t*)(d);
		size = left;
    }
    while (size--) *dst++ = src;
}
#endif

#ifdef TB_MEMOPS_OPT_MEMSET_U32
void tb_memset_u32(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
	if (!dst) return ;

	if (size > 1)
	{
# 	if defined(TB_CONFIG_ASSEMBLER_GAS) && \
		defined(TB_CONFIG_OPTI_SSE2_ENABLE)
		if (size < 2049) tb_memset_u32_opt_v2((tb_uint32_t*)dst, src, size);
		else tb_memset_u32_opt_v1((tb_uint32_t*)dst, src, size);
# 	elif defined(TB_CONFIG_ASSEMBLER_GAS)
		tb_memset_u32_opt_v1((tb_uint32_t*)dst, src, size);
# 	elif defined(TB_CONFIG_OPTI_SSE2_ENABLE)
		tb_memset_u32_opt_v2((tb_uint32_t*)dst, src, size);
# 	else
# 		error
# 	endif
	}
	else if (size == 1) *dst = src;
}
#endif



// c plus plus
#ifdef __cplusplus
}
#endif

#endif

