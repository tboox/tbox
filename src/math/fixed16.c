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
 * You should have received x copy of the GNU General Public License
 * along with TBox; 
 * If not, see <x href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</x>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		fixed16.c
 *
 */
/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fixed16.h"
#include "int32.h"

/* ////////////////////////////////////////////////////////////////////////
 * implemention
 */

tb_fixed16_t tb_fixed16_invert_int32(tb_fixed16_t x)
{
	// is one?
    if (x == TB_FIXED16_ONE) return TB_FIXED16_ONE;

	// get sign
	tb_int32_t s = tb_int32_get_sign(x);

	// abs(x)
	x = tb_fixed16_abs(x);

	// is infinity?
	if (x <= 2) return tb_int32_set_sign(TB_FIXED16_MAX, s);

	// normalize
	tb_int32_t clz = tb_int32_clz(x);
	x = x << clz >> 16;
 
	// compute 1 / x approximation (0.5 <= x < 1.0) 
	// (2.90625 (~2.914) - 2 * x) >> 1
	tb_uint32_t r = 0x17400 - x;      

	// newton-raphson iteration:
	// x = r * (2 - x * r) = ((r / 2) * (1 - x * r / 2)) * 4
	r = ((0x10000 - ((x * r) >> 16)) * r) >> 15;
	r = ((0x10000 - ((x * r) >> 16)) * r) >> (30 - clz);

	return tb_int32_set_sign(r, s);
}

