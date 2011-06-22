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
 * globals
 */
static tb_fixed16_t const tb_fixed16_cordic_atan2i_table[16] =
{ 
	0xc90f	// 44.999253
,	0x76b1	// 26.564518
,	0x3eb6	// 14.035438
,	0x1fd5	// 7.124379
,	0xffa	// 3.575741
,	0x7ff	// 1.789619
,	0x3ff	// 0.894372
,	0x1ff	// 0.446749
,	0xff	// 0.222937
,	0x7f	// 0.111032
,	0x3f	// 0.055079
,	0x1f	// 0.027102
,	0xf		// 0.013114
,	0x7		// 0.006120
,	0x3		// 0.002623
,	0x1		// 0.000874
};

/* ////////////////////////////////////////////////////////////////////////
 * details
 */

static void tb_fixed16_cordic(tb_fixed16_t* x0, tb_fixed16_t* y0, tb_fixed16_t* z0) 
{
	tb_int_t i = 0;
	tb_fixed16_t x = *x0;
	tb_fixed16_t y = *y0;
	tb_fixed16_t z = *z0;
	tb_fixed16_t const* patan2i = tb_fixed16_cordic_atan2i_table;

	do 
	{
		tb_fixed16_t xi = x >> i;
		tb_fixed16_t yi = y >> i;
		tb_fixed16_t atan2i = *patan2i++;
		//tb_int32_t atan2i = tb_float_to_fixed16(atan(1. / (1 << i)));
		//tb_printf(",\t0x%x\t// %f\n", atan2i, tb_fixed16_to_float(atan2i) * 180 / TB_FLOAT_PI);
		if (z >= 0) 
		{
			x -= yi;
			y += xi;
			z -= atan2i;
		} 
		else 
		{
			x += yi;
			y -= xi;
			z += atan2i;
		}

	} while (++i < 16);

	*x0 = x;
	*y0 = y;
	*z0 = z;
}
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

void tb_fixed16_sincos_int32(tb_fixed16_t x, tb_fixed16_t* s, tb_fixed16_t* c)
{
    tb_fixed16_t cos = 0x9b74; // k = 0.607252935
	tb_fixed16_t sin = 0;
	tb_fixed16_t ang = x;

#error angle > 90
    tb_fixed16_cordic(&cos, &sin, &ang);

	if (s) *s = sin;
	if (c) *c = cos;
}
