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
 * \file		memops.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "memops.h"

/* /////////////////////////////////////////////////////////
 * includes
 */

#if defined(TB_CONFIG_ARCH_x86)
# 	include "opt/memops_x86.h"
#elif defined(TB_CONFIG_ARCH_ARM)
# 	include "opt/memops_arm.h"
#elif defined(TB_CONFIG_ARCH_SH4)
# 	include "opt/memops_sh4.h"
#endif

/* /////////////////////////////////////////////////////////
 * interfaces 
 */

void tb_memset_u8(tb_byte_t* dst, tb_uint8_t src, tb_size_t size)
{
	tb_memset(dst, src, size);
}

#if 0
void tb_memset_u16(tb_byte_t* dst, tb_uint16_t src, tb_size_t size)
{
	if (!dst || !size) return ;
#ifdef TB_MEMOPS_OPT_MEMSET_U16
 	TB_MEMOPS_OPT_MEMSET_U16(dst, src, size);
#else
# 	if 0
	tb_uint16_t* p = (tb_uint16_t*)dst;
	tb_uint16_t* e = p + size;
	while (p < e) *p++ = src;
# 	else
	tb_byte_t b1 = (src) & 0xff;
	tb_byte_t b2 = (src >> 8) & 0xff;
	if (b1 == b2) tb_memset(dst, b1, size << 1);
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

#else
#include <emmintrin.h>
void tb_memset_u16(tb_byte_t* dst, tb_uint16_t src, tb_size_t size)
{
	tb_uint16_t* p = (tb_uint16_t*)dst;
    if (size >= 32) 
	{
		// aligned by 16-bytes
        for (; ((tb_size_t)p) & 0x0f; --size) *p++ = src;

		// left = size % 32
		tb_size_t left = size & 0x1f;
		size = (size - left) >> 5;

		// fill 4 x 8 bytes
        __m128i* 	d = (__m128i*)(p);
        __m128i 	v = _mm_set1_epi16(src);
        while (size) 
		{
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            --size;
        }
        p = (tb_uint16_t*)(d);
		size = left;
    }
	while (size--) *p++ = src;
}
 
#endif

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
	if ((b1 == b2) && (b1 == b3)) tb_memset(dst, b1, size * 3);
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

#if 0
void tb_memset_u32(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
	if (!dst || !size) return ;
#ifdef TB_MEMOPS_OPT_MEMSET_U32
	TB_MEMOPS_OPT_MEMSET_U32(dst, src, size);
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
	if ((b1 == b2) && (b1 == b3) && (b1 == b4)) tb_memset(dst, b1, size << 2);
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
#else
 
#include <emmintrin.h>
void tb_memset_u32(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
 	tb_uint32_t* p = (tb_uint32_t*)dst;
    if (size >= 16) 
	{
		// aligned by 16-bytes
        for (; ((tb_size_t)p) & 0x0f; --size) *p++ = src;

		// left = size % 16
		tb_size_t left = size & 0x0f;
		size = (size - left) >> 4;

		// fill 4 x 16 bytes
        __m128i* 	d = (__m128i*)(p);
        __m128i 	v = _mm_set1_epi32(src);
        while (size) 
		{
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            _mm_store_si128(d++, v);
            --size;
        }
        p = (tb_uint32_t*)(d);
		size = left;
    }
    while (size--) *dst++ = src;
}
#endif

void tb_memcpy(void* dst, void const* src, tb_size_t size)
{
	if (dst != src && size) memcpy(dst, src, size);
}
void tb_memmov(void* dst, void const* src, tb_size_t size)
{
	if (dst != src && size) memmove(dst, src, size);
	
#if 0
	tb_byte_t const* p = src;
	tb_byte_t const* e = p + size;
	while (p < e) *dst++ = *p++;
#endif
}

void tb_memset(void* dst, tb_size_t src, tb_size_t size)
{
	if (size) memset(dst, src, size);
}

