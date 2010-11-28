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
 * \file		log.c
 *
 */
/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "log.h"

/* ////////////////////////////////////////////////////////////////////////
 * globals
 */
tb_uint32_t g_tb_math_ilog2i_table[32] = 
{
	1 			// 0
,	2 			// 1
,	4 			// 2
,	8 			// 3
,	16 			// 4
,	32 			// 5
,	64 			// 6
,	128 		// 7
,	256 		// 8
,	512 		// 9
,	1024 		// 10
,	2048 		// 11
,	4096 		// 12
,	8192 		// 13
,	16384 		// 14
,	32768 		// 15
,	65536 		// 16
,	131072 		// 17
,	262144 		// 18
,	524288 		// 19
,	1048576 	// 20
,	2097152 	// 21
,	4194304 	// 22
,	8388608 	// 23
,	16777216 	// 24
,	33554432 	// 25
,	67108864 	// 26
,	134217728 	// 27
,	268435456 	// 28
,	536870912 	// 29
,	1073741824 	// 30
,	2147483648 	// 31
};

tb_uint32_t g_tb_math_irlog2i_table[32] = 
{
	0 			// 0
,	2 			// 1
,	3 			// 2
,	6 			// 3
,	12 			// 4
,	23 			// 5
,	46 			// 6
,	91 			// 7
,	182 		// 8
,	363 		// 9
,	725 		// 10
,	1449 		// 11
,	2897 		// 12
,	5793 		// 13
,	11586 		// 14
,	23171 		// 15
,	46341 		// 16
,	92682 		// 17
,	185364 		// 18
,	370728 		// 19
,	741456 		// 20
,	1482911 	// 21
,	2965821 	// 22
,	5931642 	// 23
,	11863284 	// 24
,	23726567 	// 25
,	47453133 	// 26
,	94906266 	// 27
,	189812532 	// 28
,	379625063 	// 29
,	759250125 	// 30
,	1518500250 	// 31
};

tb_uint32_t g_tb_math_iclog2i_table[33] = 
{
	0 			// 0
,	2 			// 1
,	3 			// 2
,	5 			// 3
,	9 			// 4
,	17 			// 5
,	33 			// 6
,	65 			// 7
,	129 		// 8
,	257 		// 9
,	513 		// 10
,	1025 		// 11
,	2049 		// 12
,	4097 		// 13
,	8193 		// 14
,	16385 		// 15
,	32769 		// 16
,	65537 		// 17
,	131073 		// 18
,	262145 		// 19
,	524289 		// 20
,	1048577 	// 21
,	2097153 	// 22
,	4194305 	// 23
,	8388609 	// 24
,	16777217 	// 25
,	33554433 	// 26
,	67108865 	// 27
,	134217729 	// 28
,	268435457 	// 29
,	536870913 	// 30
,	1073741825 	// 31
,	2147483649 	// 32
};

