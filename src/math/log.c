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
	0 			// 0
,	2 			// 1
,	4 			// 2
,	9 			// 3
,	16 			// 4
,	32 			// 5
,	65 			// 6
,	129 		// 7
,	256 		// 8
,	512 		// 9
,	1024 		// 10
,	2048 		// 11
,	4097 		// 12
,	8193 		// 13
,	16385 		// 14
,	32769 		// 15
,	65536 		// 16
,	131072 		// 17
,	262144 		// 18
,	524288 		// 19
,	1048576 	// 20
,	2097152 	// 21
,	4194304 	// 22
,	8388608 	// 23
,	16777217 	// 24
,	33554432 	// 25
,	67108865 	// 26
,	134217728 	// 27
,	268435457 	// 28
,	536870912 	// 29
,	1073741825 	// 30
,	4294967294 	// 31
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

