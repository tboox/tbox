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
 * \file		pow.c
 *
 */
/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pow.h"

/* ////////////////////////////////////////////////////////////////////////
 * globals
 */

tb_float_t g_tb_math_fpow2i_table[53] = 
{
	// [-21, -1]
	0.000000
,	0.000001
,	0.000002
,	0.000004
,	0.000008
,	0.000015
,	0.000031
,	0.000061
,	0.000122
,	0.000244
,	0.000488
,	0.000977
,	0.001953
,	0.003906
,	0.007812
,	0.015625
,	0.031250
,	0.062500
,	0.125000
,	0.250000
,	0.500000
	// 0
,	1.000000
	// [1, 31]
,	2.000000
,	4.000000
,	8.000000
,	16.000000
,	32.000000
,	64.000000
,	128.000000
,	256.000000
,	512.000000
,	1024.000000
,	2048.000000
,	4096.000000
,	8192.000000
,	16384.000000
,	32768.000000
,	65536.000000
,	131072.000000
,	262144.000000
,	524288.000000
,	1048576.000000
,	2097152.000000
,	4194304.000000
,	8388608.000000
,	16777216.000000
,	33554432.000000
,	67108864.000000
,	134217728.000000
,	268435456.000000
,	536870912.000000
,	1073741824.000000
,	2147483648.000000


};

