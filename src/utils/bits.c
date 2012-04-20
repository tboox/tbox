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
 * @file		bits.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bits.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_uint32_t tb_bits_get_ubits32(tb_byte_t const* p, tb_size_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(p && n <= 32, 0);
	if (!n) return 0;

	p += b >> 3; b &= 0x07;
	if (!b && n == 1) return tb_bits_get_u1(p);
	else if (!b && n == 8) return tb_bits_get_u8(p);
	else if (!b && n == 16) return tb_bits_get_u16_be(p);
	else if (!b && n == 24) return tb_bits_get_u24_be(p);
	else if (!b && n == 32) return tb_bits_get_u32_be(p);
	else
	{
#if defined(tb_bits_get_ubits32_impl)
		return tb_bits_get_ubits32_impl(p, b, n);
#elif defined(TB_CONFIG_BINARY_SMALL)
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
#else	
		tb_uint32_t x = 0;
		tb_size_t 	i = b; b += n;

		if (b > 32)
		{
			x |= p[0] << (i + 24);
			x |= p[1] << (i + 16);
			x |= p[2] << (i + 8);
			x |= p[3] << (i + 0);
			x |= p[4] >> (8 - i);
		}
		else if (b > 24)
		{
#if 1
			x |= p[0] << (i + 24);
			x |= p[1] << (i + 16);
			x |= p[2] << (i + 8);
			x |= p[3] << (i + 0);
#else
			x = tb_bits_get_u32_be(p);
			x <<= i;
#endif
		}
		else if (b > 16)
		{
			x |= p[0] << (i + 24);
			x |= p[1] << (i + 16);
			x |= p[2] << (i + 8);
		}
		else if (b > 8)
		{
			x |= p[0] << (i + 24);
			x |= p[1] << (i + 16);
		}
		else x |= p[0] << (i + 24);
		return (n < 32)? (x >> (32 - n)) : x;
#endif
	}
}
tb_sint32_t tb_bits_get_sbits32(tb_byte_t const* p, tb_size_t b, tb_size_t n)
{
	tb_assert_and_check_return_val(p && n > 1 && n <= 32, 0);

	p += b >> 3; b &= 0x07;
	if (n < 32)
	{
		tb_sint32_t s = -tb_bits_get_ubits32(p, b, 1);
		return ((s << (n - 1)) | tb_bits_get_ubits32(p, b + 1, n - 1));
	}
	else return tb_bits_get_ubits32(p, b, n);
}
tb_void_t tb_bits_set_ubits32(tb_byte_t* p, tb_size_t b, tb_uint32_t x, tb_size_t n)
{
	tb_assert_and_check_return(p && n <= 32);
	if (!n) return ;

	p += b >> 3; b &= 0x07;
	if (!b && n == 1) tb_bits_set_u1(p, x);
	else if (!b && n == 8) tb_bits_set_u8(p, x);
	else if (!b && n == 16) tb_bits_set_u16_be(p, x);
	else if (!b && n == 24) tb_bits_set_u24_be(p, x);
	else if (!b && n == 32) tb_bits_set_u32_be(p, x);
	else
	{
#ifdef TB_CONFIG_BINARY_SMALL
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
#else
		tb_uint32_t m = 0xffffffff;
		tb_size_t 	i = b; b += n;

		if (n < 32) 
		{
			x <<= (32 - n);
			m <<= (32 - n);
		}
		if (b > 32)
		{
			p[0] &= ~(m >> (24 + i));
			p[0] |= (x >> (24 + i));
			p[1] = (x >> (16 + i));
			p[2] = (x >> (8 + i));	
			p[3] = (x >> i);
			p[4] = (x << (8 - i));
		}
		else if (b > 24)
		{
			if (i)
			{
				p[0] &= ~(m >> (24 + i));
				p[0] |= (x >> (24 + i));
				p[1] = (x >> (16 + i));
				p[2] = (x >> (8 + i));	
				p[3] = (x >> i);
			}
			else
			{	
				p[0] = x >> 24;
				p[1] = x >> 16;
				p[2] = x >> 8;
				p[3] = x;
			}
		}
		else if (b > 16)
		{
			if (i)
			{
				p[0] &= ~(m >> (24 + i));
				p[0] |= (x >> (24 + i));
				p[1] = (x >> (16 + i));
				p[2] = (x >> (8 + i));
			}
			else
			{	
				p[0] = x >> 24;
				p[1] = x >> 16;
				p[2] = x >> 8;
			}
		}
		else if (b > 8)
		{
			if (i)
			{
				p[0] &= ~(m >> (24 + i));
				p[0] |= (x >> (24 + i));
				p[1] = (x >> (16 + i));
			}
			else
			{	
				p[0] = x >> 24;
				p[1] = x >> 16;
			}
		}
		else 
		{
			if (i)
			{
				p[0] &= ~(m >> (24 + i));
				p[0] |= (x >> (24 + i));	
			}
			else p[0] = x >> 24;
		}
#endif
	}
}
tb_void_t tb_bits_set_sbits32(tb_byte_t* p, tb_size_t b, tb_sint32_t x, tb_size_t n)
{
	tb_assert_and_check_return(p && n > 1 && n <= 32);

	p += b >> 3; b &= 0x07;
	if (n < 32)
	{
		tb_bits_set_ubits32(p, b, (((tb_uint32_t)x) >> 31) & 0x01, 1);
		tb_bits_set_ubits32(p, b + 1, (((tb_uint32_t)x) & 0x7fffffff), n - 1);
	}
	else tb_bits_set_ubits32(p, b, x, n);
}
