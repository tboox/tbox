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
 * @file		int32.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "int32.h"
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_int32_t tb_int32_div(tb_int32_t x, tb_int32_t y, tb_int_t nbits)
{
	tb_assert(y);
	tb_check_return_val(x, 0);

	// get sign
	tb_int32_t s = tb_int32_get_sign(x ^ y);
	x = tb_abs(x);
	y = tb_abs(y);

	tb_int_t xbits = (tb_int_t)tb_bits_cl0_u32_be(x) - 1;
	tb_int_t ybits = (tb_int_t)tb_bits_cl0_u32_be(y) - 1;
    tb_int_t bits = nbits - xbits + ybits;
 
	// underflow?
	if (bits < 0) return 0; 

	// overflow?
    if (bits > 31) return tb_int32_set_sign(TB_MAXS32, s);

	x <<= xbits;
	y <<= ybits;

    // do the first one
	tb_int32_t r = 0;
    if ((x -= y) >= 0) r = 1;
    else x += y;
    
    // now fall into our switch statement if there are more bits to compute
    if (bits > 0) 
	{
		// make room for the rest of the answer bits
		r <<= bits;
		switch (bits) 
		{
#define TB_INT32_DIV_CASE(n) \
		case n: \
			if ((x = (x << 1) - y) >= 0) \
			r |= 1 << (n - 1); else x += y

			TB_INT32_DIV_CASE(31); TB_INT32_DIV_CASE(30); TB_INT32_DIV_CASE(29);
			TB_INT32_DIV_CASE(28); TB_INT32_DIV_CASE(27); TB_INT32_DIV_CASE(26);
			TB_INT32_DIV_CASE(25); TB_INT32_DIV_CASE(24); TB_INT32_DIV_CASE(23);
			TB_INT32_DIV_CASE(22); TB_INT32_DIV_CASE(21); TB_INT32_DIV_CASE(20);
			TB_INT32_DIV_CASE(19); TB_INT32_DIV_CASE(18); TB_INT32_DIV_CASE(17);
			TB_INT32_DIV_CASE(16); TB_INT32_DIV_CASE(15); TB_INT32_DIV_CASE(14);
			TB_INT32_DIV_CASE(13); TB_INT32_DIV_CASE(12); TB_INT32_DIV_CASE(11);
			TB_INT32_DIV_CASE(10); TB_INT32_DIV_CASE( 9); TB_INT32_DIV_CASE( 8);
			TB_INT32_DIV_CASE( 7); TB_INT32_DIV_CASE( 6); TB_INT32_DIV_CASE( 5);
			TB_INT32_DIV_CASE( 4); TB_INT32_DIV_CASE( 3); TB_INT32_DIV_CASE( 2);

			// we merge these last two together, makes gcc make better arm
		default:
			TB_INT32_DIV_CASE(1);
		}
	}

    if (r < 0) r = TB_MAXS32;
    return tb_int32_set_sign(r, s);
}
