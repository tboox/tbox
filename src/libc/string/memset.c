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
#include "../../utils/utils.h"

#ifndef TB_CONFIG_LIBC_HAVE_MEMSET
# 	if defined(TB_CONFIG_ARCH_x86)
# 		include "opt/x86/memset.c"
# 	elif defined(TB_CONFIG_ARCH_ARM)
# 		include "opt/arm/memset.c"
# 	elif defined(TB_CONFIG_ARCH_SH4)
# 		include "opt/sh4/memset.c"
# 	endif
#else
# 	include <string.h>
#endif

/* /////////////////////////////////////////////////////////
 * implemention 
 */
#if defined(TB_CONFIG_LIBC_HAVE_MEMSET)
tb_pointer_t tb_memset(tb_pointer_t s, tb_size_t c, tb_size_t n)
{
	tb_assert_and_check_return_val(s, TB_NULL);
	return memset(s, c, n);
}
#elif !defined(TB_LIBC_STRING_OPT_MEMSET_U8)
tb_pointer_t tb_memset(tb_pointer_t s, tb_size_t c, tb_size_t n)
{
	tb_assert_and_check_return_val(s, TB_NULL);

	__tb_register__ tb_byte_t* p = s;
	if (!n) return s;

	tb_byte_t b = (tb_byte_t)c;
#ifdef TB_CONFIG_BINARY_SMALL
	while (n--) *p++ = b;
#else
	tb_size_t l = n & 0x3; n = (n - l) >> 2;
	while (n--)
	{
		p[0] = b;
		p[1] = b;
		p[2] = b;
		p[3] = b;
		p += 4;
	}

	while (l--) *p++ = b;
#endif
	return s;
}
#endif

#ifndef TB_LIBC_STRING_OPT_MEMSET_U16
tb_pointer_t tb_memset_u16(tb_pointer_t s, tb_size_t c, tb_size_t n)
{
	tb_assert_and_check_return_val(s, TB_NULL);

	// align by 2-bytes 
	tb_assert(!(((tb_size_t)s) & 0x1));

	__tb_register__ tb_uint16_t* p = s;
	if (!n) return s;

	tb_uint16_t b = (tb_uint16_t)c;

#ifdef TB_CONFIG_BINARY_SMALL
	while (n--) *p++ = b;
#else
	tb_size_t l = n & 0x3; n = (n - l) >> 2;
	while (n--)
	{
		p[0] = b;
		p[1] = b;
		p[2] = b;
		p[3] = b;
		p += 4;
	}

	while (l--) *p++ = b;
#endif
	return s;
}
#endif

#ifndef TB_LIBC_STRING_OPT_MEMSET_U24
tb_pointer_t tb_memset_u24(tb_pointer_t s, tb_size_t c, tb_size_t n)
{
	tb_assert_and_check_return_val(s, TB_NULL);

	__tb_register__ tb_byte_t* p = s;
	if (!n) return s;

	tb_byte_t* e = p + (n * 3);
#ifdef TB_CONFIG_BINARY_SMALL
	for (; p < e; p += 3) tb_bits_set_u24_ne(p, c);
#else
	tb_size_t l = n & 0x3; n -= l;
	while (p < e)
	{
		tb_bits_set_u24_ne(p + 0, c);
		tb_bits_set_u24_ne(p + 3, c);
		tb_bits_set_u24_ne(p + 6, c);
		tb_bits_set_u24_ne(p + 9, c);
		p += 12;
	}

	while (l--)
	{
		tb_bits_set_u24_ne(p, c);
		p += 3;
	}
#endif
	return s;
}
#endif

#ifndef TB_LIBC_STRING_OPT_MEMSET_U32
tb_pointer_t tb_memset_u32(tb_pointer_t s, tb_size_t c, tb_size_t n)
{
	tb_assert_and_check_return_val(s, TB_NULL);

	// align by 4-bytes 
	tb_assert(!(((tb_size_t)s) & 0x3));

	__tb_register__ tb_uint32_t* p = s;
	if (!n) return s;

	tb_uint32_t b = (tb_uint32_t)c;
#ifdef TB_CONFIG_BINARY_SMALL
	while (n--) *p++ = b;
#else
	tb_size_t l = n & 0x3; n = (n - l) >> 2;
	while (n--)
	{
		p[0] = b;
		p[1] = b;
		p[2] = b;
		p[3] = b;
		p += 4;
	}

	while (l--) *p++ = b;
#endif
	return s;
}
#endif

