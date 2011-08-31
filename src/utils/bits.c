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
	TB_ASSERT_RETURN_VAL(p && n <= 32, 0);
	if (!n) return 0;

	tb_uint32_t x = 0;
	tb_uint8_t i = (tb_uint8_t)b; 
	tb_uint8_t j = 24;

	b += n;
	while (b > 7) 
	{
		x |= *(p++) << (i + j);
		j -= 8;
		b -= 8;
	}
	if (b > 0) x |= *(p) << (i + j);

	x >>= 1;
	if (x & 0x80000000) x &= 0x7fffffff;
	x >>= (31 - n);

	return x;
}
tb_sint32_t tb_bits_get_sbits32(tb_byte_t const* p, tb_size_t b, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(p && n <= 32, 0);
	if (!n) return 0;

	tb_sint32_t x = 0;
	tb_uint8_t i = (tb_uint8_t)b; 
	tb_uint8_t j = 24;

	b += n;
	while (b > 7) 
	{
		x |= *(p++) << (i + j);
		j -= 8;
		b -= 8;
	}
	if (b > 0) x |= *(p) << (i + j);

	x >>= (32 - n);
	return x;
}
tb_void_t tb_bits_set_ubits32(tb_byte_t* p, tb_size_t b, tb_uint32_t x, tb_size_t n)
{
	TB_ASSERT_RETURN(p && n <= 32);
	if (!n) return ;

	x <<= (32 - n);
	while (n--) 
	{
		*(p) &= ~(0x1 << (7 - b));
		*(p) |= (((x & 0x80000000) >> 31) << (7 - b));

		x <<= 1;
		if (++b > 7) 
		{
			b = 0;
			p++;
		}
	}
}
tb_void_t tb_bits_set_sbits32(tb_byte_t* p, tb_size_t b, tb_sint32_t x, tb_size_t n)
{
	TB_ASSERT_RETURN(p && n <= 32);
	if (!n) return ;


	TB_NOT_IMPLEMENT();
}
