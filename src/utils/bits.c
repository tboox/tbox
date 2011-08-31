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
 * \file		bits.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bits.h"

/* ////////////////////////////////////////////////////////////////////////
 * implemention
 */
tb_uint32_t tb_bits_get_ubits32(tb_byte_t const* p, tb_size_t b, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(p && b < 8 && n <= 32, 0);
	if (!n) return 0;

	if (!b && n == 1) return tb_bits_get_u1(p);
	else if (!b && n == 8) return tb_bits_get_u8(p);
	else if (!b && n == 16) return tb_bits_get_u16_be(p);
	else if (!b && n == 24) return tb_bits_get_u24_be(p);
	else if (!b && n == 32) return tb_bits_get_u32_be(p);
	else
	{
		tb_uint32_t x = 0;
		tb_size_t 	i = b; 
		tb_int_t 	j = 24;

		b += n;
		while (b > 7) 
		{
			x |= *p++ << (i + j);
			j -= 8;
			b -= 8;
		}
		if (b > 0) x |= j < 0? (*p >> (8 - i)) : *p << (i + j);

		return (n < 32)? (x >> (32 - n)) : x;
	}
}
tb_sint32_t tb_bits_get_sbits32(tb_byte_t const* p, tb_size_t b, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(p && b < 8 && n <= 32, 0);
	if (!n) return 0;

#if 0
	if (n > 1 && n < 32)
	{
		tb_sint32_t x = 0;
		tb_size_t 	i = b; 
		tb_int_t 	j = 24;

		b += n;
		while (b > 7) 
		{
			x |= *p++ << (i + j);
			j -= 8;
			b -= 8;
		}
		if (b > 0) x |= j < 0? (*p >> (8 - i)) : *p << (i + j);

		return (x >> (32 - n));
	}
	else return (tb_sint32_t)tb_bits_get_ubits32(p, b, n);
#else
	if (n > 1 && n < 32)
	{
		if (!tb_bits_get_ubits32(p, b, 1)) 
			return (tb_sint32_t)tb_bits_get_ubits32(p, b, n);
		else
		{
#if 0
			tb_uint32_t x = tb_bits_get_ubits32(p, b + 1, n - 1);
			return (tb_sint32_t)(x | 0x80000000);
#else
			tb_sint32_t x = -1;
			return ((x << (n - 1)) | tb_bits_get_ubits32(p, b + 1, n - 1));
#endif
		}
	}
	else return (tb_sint32_t)tb_bits_get_ubits32(p, b, n);
#endif
}
tb_void_t tb_bits_set_ubits32(tb_byte_t* p, tb_size_t b, tb_uint32_t x, tb_size_t n)
{
	TB_ASSERT_RETURN(p && b < 8 && n <= 32);
	if (!n) return ;

	if (!b && n == 1) tb_bits_set_u1(p, x);
	else if (!b && n == 8) tb_bits_set_u8(p, x);
	else if (!b && n == 16) tb_bits_set_u16_be(p, x);
	else if (!b && n == 24) tb_bits_set_u24_be(p, x);
	else if (!b && n == 32) tb_bits_set_u32_be(p, x);
	else
	{
		if (n < 32) x <<= (32 - n);
		while (n--) 
		{
			*p &= ~(0x1 << (7 - b));
			*p |= ((x >> 31) << (7 - b));

			x <<= 1;
			if (++b > 7) 
			{
				b = 0;
				p++;
			}
		}
	}
}
tb_void_t tb_bits_set_sbits32(tb_byte_t* p, tb_size_t b, tb_sint32_t x, tb_size_t n)
{
	TB_ASSERT_RETURN(p && b < 8 && n <= 32);
	if (!n) return ;

	if (x >= 0) tb_bits_set_ubits32(p, b, (tb_uint32_t)x, n);
	else if (n > 1)
	{
		tb_bits_set_ubits32(p, b, (((tb_uint32_t)x) >> 31) & 0x01, 1);
		tb_bits_set_ubits32(p, b + 1, (((tb_uint32_t)x) & 0x7fffffff), n - 1);
	}
	else 
	{
		// need 2-bits at least
		TB_ASSERT(0);
	}
}
