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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		expi.c
 * @ingroup 	libm
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "math.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_double_t tb_expi(tb_long_t x)
{
	tb_assert(x >= -31 && x <= 31);

	// x = [-31, 31]
	static tb_double_t table[47] = 
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
