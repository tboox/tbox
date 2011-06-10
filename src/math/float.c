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
 * \file		float.c
 *
 */
/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "float.h"
#include "int32.h"

/* ////////////////////////////////////////////////////////////////////////
 * implemention
 */

tb_float_t tb_float_sqrt_generic(tb_float_t x)
{
	TB_ASSERT(x > 0);
	return sqrt(x);
}
tb_float_t tb_float_sin_generic(tb_float_t x)
{
	return sin(x);
}
tb_float_t tb_float_cos_generic(tb_float_t x)
{
	return cos(x);
}
tb_float_t tb_float_tan_generic(tb_float_t x)
{
	return tan(x);
}
tb_float_t tb_float_asin_generic(tb_float_t x)
{
	return asin(x);
}
tb_float_t tb_float_acos_generic(tb_float_t x)
{
	return acos(x);
}
tb_float_t tb_float_atan_generic(tb_float_t x)
{
	return atan(x);
}
tb_float_t tb_float_exp_generic(tb_float_t x)
{
#if 1
	tb_float_t a = x - (tb_int_t)x;
	return (tb_float_expi(((tb_int_t)x)) * tb_float_exp1(a));
#else
	return exp(x);
#endif
}
tb_float_t tb_float_expi_generic(tb_uint16_t x)
{
	// x = [-31, 31]
	static tb_float_t table[47] = 
	{
		// [-15, -1]
		0.000000
	,	0.000001
	,	0.000002
	,	0.000006
	,	0.000017
	,	0.000045
	,	0.000123
	,	0.000335
	,	0.000912
	,	0.002479
	,	0.006738
	,	0.018316
	,	0.049787
	,	0.135335
	,	0.367879

		// 0
	,	1.000000

		// [1, 31]
	, 	2.718282
	,	7.389056
	,	20.085537
	,	54.598150
	,	148.413159
	,	403.428793
	,	1096.633158
	,	2980.957987
	,	8103.083928
	,	22026.465795
	,	59874.141715
	,	162754.791419
	,	442413.392009
	,	1202604.284165
	,	3269017.372472
	,	8886110.520508
	,	24154952.753575
	,	65659969.137331
	,	178482300.963187
	,	485165195.409790
	,	1318815734.483215
	,	3584912846.131592
	,	9744803446.248903
	,	26489122129.843472
	,	72004899337.385880
	,	195729609428.838776
	,	532048240601.798645
	,	1446257064291.475098
	,	3931334297144.041992
	,	10686474581524.462891
	,	29048849665247.425781

	};
	return table[((x) + 15) & 0x3f];
}
tb_uint32_t tb_float_ilog2_generic(tb_float_t x)
{
	TB_ASSERT(x > 0);
	return tb_int32_log2(tb_float_round(x));
}
tb_uint32_t tb_float_iclog2_generic(tb_float_t x)
{
	TB_ASSERT(x > 0);
	return tb_int32_clog2(tb_float_round(x));
}
tb_uint32_t tb_float_irlog2_generic(tb_float_t x)
{
	TB_ASSERT(x > 0);
	return tb_int32_rlog2(tb_float_round(x));
}
