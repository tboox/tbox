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
 * \file		int32.c
 *
 */
/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "int32.h"

/* ////////////////////////////////////////////////////////////////////////
 * implemention
 */
tb_uint32_t tb_int32_sqrt(tb_uint32_t x)
{
#if 1
	// lookup + newton 
	static tb_uint32_t table[256] = 
	{ 
		0,    16,  22,  27,  32,  35,  39,  42,  45,  48,  50,  53,  55,  57, 
		59,   61,  64,  65,  67,  69,  71,  73,  75,  76,  78,  80,  81,  83, 
		84,   86,  87,  89,  90,  91,  93,  94,  96,  97,  98,  99, 101, 102, 
		103, 104, 106, 107, 108, 109, 110, 112, 113, 114, 115, 116, 117, 118, 
		119, 120, 121, 122, 123, 124, 125, 126, 128, 128, 129, 130, 131, 132, 
		133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 144, 145, 
		146, 147, 148, 149, 150, 150, 151, 152, 153, 154, 155, 155, 156, 157, 
		158, 159, 160, 160, 161, 162, 163, 163, 164, 165, 166, 167, 167, 168, 
		169, 170, 170, 171, 172, 173, 173, 174, 175, 176, 176, 177, 178, 178, 
		179, 180, 181, 181, 182, 183, 183, 184, 185, 185, 186, 187, 187, 188, 
		189, 189, 190, 191, 192, 192, 193, 193, 194, 195, 195, 196, 197, 197, 
		198, 199, 199, 200, 201, 201, 202, 203, 203, 204, 204, 205, 206, 206, 
		207, 208, 208, 209, 209, 210, 211, 211, 212, 212, 213, 214, 214, 215, 
		215, 216, 217, 217, 218, 218, 219, 219, 220, 221, 221, 222, 222, 223, 
		224, 224, 225, 225, 226, 226, 227, 227, 228, 229, 229, 230, 230, 231, 
		231, 232, 232, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238, 
		239, 240, 240, 241, 241, 242, 242, 243, 243, 244, 244, 245, 245, 246, 
		246, 247, 247, 248, 248, 249, 249, 250, 250, 251, 251, 252, 252, 253, 
		253, 254, 254, 255 
	}; 

	tb_uint32_t xn; 
	if (x >= 0x7FFEA810) return 0xB504; 
	if (x >= 0x10000) 
	{ 
		if (x >= 0x1000000)
		{ 
			// lookup table
			if (x >= 0x10000000)
			{ 
				if (x >= 0x40000000) xn = table[x >> 24] << 8; 
				else xn = table[x >> 22] << 7;
			} 
			else 
			{ 
				if (x >= 0x4000000) xn = table[x >> 20] << 6;  
				else xn = table[x >> 18] << 5;  
			} 

			// newton
			xn = (xn + 1 + (x / xn)) >> 1; 
			xn = (xn + 1 + (x / xn)) >> 1; 
			return (((xn * xn) > x)? --xn : xn); 

		}
		else 
		{ 
			if (x >= 0x100000) 
			{ 
				if (x >= 0x400000) xn = table[x >> 16] << 4; 
				else xn = table[x >> 14] << 3; 
			} 
			else 
			{ 
				if (x >= 0x40000) xn = table[x >> 12] << 2; 
				else xn = table[x >> 10] << 1;
			} 

			xn = (xn + 1 + (x / xn)) >> 1; 
			return (((xn * xn) > x)? --xn : xn); 
		} 
	}
	else 
	{ 
		if (x >= 0x100) 
		{ 
			if (x >= 0x1000) 
			{ 
				if (x >= 0x4000) xn = (table[x >> 8]) + 1; 
				else xn = (table[x >> 6] >> 1) + 1;
			} 
			else 
			{ 
				if (x >= 0x400) xn = (table[x >> 4] >> 2) + 1; 
				else xn = (table[x >> 2] >> 3) + 1; 
			} 
			return ((xn * xn) > x)? --xn : xn; 
		} 
		else 
		{ 
			if (x >= 0) return (table[x] >> 4);
		} 
	} 
	return ((tb_uint32_t)-1);
#elif 0
	// lookup bits
	tb_uint32_t t;
	tb_uint32_t n = 0;
	tb_uint32_t b = 0x8000;
	tb_uint32_t s = 15; 
	do 
	{ 
		if (x >= (t = (((n << 1) + b) << s--))) 
		{ 
			n += b; 
			x -= t; 
		} 
	} 
	while (b >>= 1); 
	return n;
#else
	// lookup bits
	tb_uint32_t i = 0;
	tb_uint32_t r = 0;
	tb_uint32_t n = 0;
	tb_uint32_t d = 0;
	for (i = 0; i < 16; i++)
	{
		n <<= 1;
		r = ((r << 2) + (x >> 30));
		x <<= 2;
		d = (n << 1) + 1;
		if (d <= r)
		{
			r -= d;
			n++;
		}
	}
	return n;
#endif
}

// floor
tb_uint32_t tb_int32_log2(tb_uint32_t x)
{
	static tb_uint32_t table[32] = 
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

	tb_int_t l = 0;
	tb_int_t m = 15;
	tb_int_t r = 32;
	while ((r - l) > 1)
	{
		tb_uint32_t v = table[m];
		if (x < v) r = m;
		else l = m;
		m = (l + r) >> 1;
	}
	return m;
}

// ceil
tb_uint32_t tb_int32_clog2(tb_uint32_t x)
{
	static tb_uint32_t table[32] = 
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

	tb_int_t l = 0;
	tb_int_t m = 16;
	tb_int_t r = 33;
	while ((r - l) > 1)
	{
		tb_uint32_t v = table[m];
		if (x < v) r = m;
		else l = m;
		m = (l + r) >> 1;
	}
	return m;
}

// round
tb_uint32_t tb_int32_rlog2(tb_uint32_t x)
{
	static tb_uint32_t table[32] = 
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

	tb_int_t l = 0;
	tb_int_t m = 15;
	tb_int_t r = 32;
	while ((r - l) > 1)
	{
		tb_uint32_t v = table[m];
		if (x < v) r = m;
		else l = m;
		m = (l + r) >> 1;
	}
	return m;
}
