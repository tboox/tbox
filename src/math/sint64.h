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
 * \file		sint64.h
 *
 */
#ifndef TB_MATH_SINT64_H
#define TB_MATH_SINT64_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros 
 */
#ifdef TB_CONFIG_TYPE_INT64
# 	define TB_SINT64_ZERO 				(tb_sint64_t)(0)
# 	define TB_SINT64_ONE 				(tb_sint64_t)(1)

# 	define tb_sint32_to_sint64(x) 		(tb_sint64_t)(x)
# 	define tb_sint64_to_sint32(x) 		(tb_sint32_t)(x)

# 	define tb_sint64_abs(x) 			((x) > 0? (x) : -(x))
# 	define tb_sint64_neg(x) 			(-(x))

# 	define tb_sint64_add(x, y) 			((x) + (y))
# 	define tb_sint64_sub(x, y) 			((x) - (y))
# 	define tb_sint64_mul(x, y) 			((x) * (y))
# 	define tb_sint64_div(x, y) 			((x) / (y))
# 	define tb_sint64_mod(x, y) 			((x) % (y))

# 	define tb_sint64_clz(x) 			tb_sint64_clz_inline(x)
# 	define tb_sint64_not(x) 			(!(x))
# 	define tb_sint64_or(x, y) 			((x) | (y))
# 	define tb_sint64_and(x, y) 			((x) & (y))
# 	define tb_sint64_xor(x, y) 			((x) ^ (y))

# 	define tb_sint64_lsh(x, b) 			((x) << (b))
# 	define tb_sint64_rsh(x, b) 			((x) >> (b))

# 	define tb_sint64_add_sint32(x, y) 	((x) + (tb_sint32_t)(y))
# 	define tb_sint64_sub_sint32(x, y) 	((x) - (tb_sint32_t)(y))
# 	define tb_sint64_mul_sint32(x, y) 	((x) * (tb_sint32_t)(y))
# 	define tb_sint64_div_sint32(x, y) 	((x) / (tb_sint32_t)(y))
# 	define tb_sint64_mod_sint32(x, y) 	((x) % (tb_sint32_t)(y))

# 	define tb_sint64_or_sint32(x, y) 	((x) | (tb_sint32_t)(y))
# 	define tb_sint64_and_sint32(x, y) 	((x) & (tb_sint32_t)(y))
# 	define tb_sint64_xor_sint32(x, y) 	((x) ^ (tb_sint32_t)(y))

# 	define tb_sint64_nz(x) 				(x)
# 	define tb_sint64_ez(x) 				!(x)
# 	define tb_sint64_lz(x) 				((x) < 0)
# 	define tb_sint64_gz(x) 				((x) > 0)
# 	define tb_sint64_et(x, y) 			((x) == (y))
# 	define tb_sint64_nt(x, y) 			((x) != (y))
# 	define tb_sint64_lt(x, y) 			((x) < (y))
# 	define tb_sint64_gt(x, y) 			((x) > (y))

# 	define tb_sint64_et_sint32(x, y)	((x) == (y))
# 	define tb_sint64_nt_sint32(x, y)	((x) != (y))
# 	define tb_sint64_lt_sint32(x, y)	((x) < (y))
# 	define tb_sint64_gt_sint32(x, y) 	((x) > (y))
#else
# 	define TB_SINT64_ZERO 				g_sint64_zero
# 	define TB_SINT64_ONE 				g_sint64_one

# 	define tb_sint32_to_sint64(x) 		tb_sint32_to_sint64_inline(x)
# 	define tb_sint64_to_sint32(x) 		tb_sint64_to_sint32_inline(x)

# 	define tb_sint64_abs(x) 			tb_sint64_abs_inline(x)
# 	define tb_sint64_neg(x) 			tb_sint64_neg_inline(x)

# 	define tb_sint64_add(x, y) 			tb_sint64_add_inline(x, y)
# 	define tb_sint64_sub(x, y) 			tb_sint64_sub_inline(x, y)
# 	define tb_sint64_mul(x, y) 			tb_sint64_mul_inline(x, y)
# 	define tb_sint64_div(x, y) 			tb_sint64_div_inline(x, y)
# 	define tb_sint64_mod(x, y) 			tb_sint64_mod_inline(x, y)

# 	define tb_sint64_clz(x) 			tb_sint64_clz_inline(x)
# 	define tb_sint64_not(x) 			tb_sint64_not_inline(x)
# 	define tb_sint64_or(x, y) 			tb_sint64_or_inline(x, y)
# 	define tb_sint64_and(x, y) 			tb_sint64_and_inline(x, y)
# 	define tb_sint64_xor(x, y) 			tb_sint64_xor_inline(x, y)

# 	define tb_sint64_lsh(x, b) 			tb_sint64_lsh_inline(x, b)
# 	define tb_sint64_rsh(x, b) 			tb_sint64_rsh_inline(x, b)

# 	define tb_sint64_add_sint32(x, y) 	tb_sint64_add_sint32_inline(x, y)
# 	define tb_sint64_sub_sint32(x, y) 	tb_sint64_sub_sint32_inline(x, y)
# 	define tb_sint64_mul_sint32(x, y) 	tb_sint64_mul_sint32_inline(x, y)
# 	define tb_sint64_div_sint32(x, y) 	tb_sint64_div_sint32_inline(x, y)
# 	define tb_sint64_mod_sint32(x, y) 	tb_sint64_mod_sint32_inline(x, y)

# 	define tb_sint64_or_sint32(x, y) 	tb_sint64_or_sint32_inline(x, y)
# 	define tb_sint64_and_sint32(x, y) 	tb_sint64_and_sint32_inline(x, y)
# 	define tb_sint64_xor_sint32(x, y) 	tb_sint64_xor_sint32_inline(x, y)

# 	define tb_sint64_nz(x) 				tb_sint64_nz_inline(x)
# 	define tb_sint64_ez(x) 				tb_sint64_ez_inline(x)
# 	define tb_sint64_lz(x) 				tb_sint64_lz_inline(x)
# 	define tb_sint64_gz(x) 				tb_sint64_gz_inline(x)
# 	define tb_sint64_et(x, y) 			tb_sint64_et_inline(x, y)
# 	define tb_sint64_nt(x, y) 			tb_sint64_nt_inline(x, y)
# 	define tb_sint64_lt(x, y) 			tb_sint64_lt_inline(x, y)
# 	define tb_sint64_gt(x, y) 			tb_sint64_gt_inline(x, y)

# 	define tb_sint64_et_sint32(x, y)	tb_sint64_et_sint32_inline(x, y)
# 	define tb_sint64_nt_sint32(x, y)	tb_sint64_nt_sint32_inline(x, y)
# 	define tb_sint64_lt_sint32(x, y)	tb_sint64_lt_sint32_inline(x, y)
# 	define tb_sint64_gt_sint32(x, y) 	tb_sint64_gt_sint32_inline(x, y)
#endif

/* ////////////////////////////////////////////////////////////////////////
 * globals 
 */

#ifndef TB_CONFIG_TYPE_INT64
extern tb_sint64_t const g_sint64_zero;
extern tb_sint64_t const g_sint64_one;
#endif

/* ////////////////////////////////////////////////////////////////////////
 * inline
 */
#ifndef TB_CONFIG_TYPE_INT64
static __tb_inline__ tb_sint64_t tb_sint32_to_sint64_inline(tb_sint32_t x)
{
	tb_sint64_t i;
	i.h = x >> 31; // 0 or -1
	i.l = x;
	return i;
}
static __tb_inline__ tb_sint32_t tb_sint64_to_sint32_inline(tb_sint64_t x)
{
	// is sint32?
	TB_ASSERT(x.h == ((tb_sint32_t)x.l >> 31));
	return (tb_sint32_t)x.l;
}
static __tb_inline__ tb_sint64_t tb_sint64_neg_inline(tb_sint64_t x)
{
	x.h = -x.h - tb_int32_nz(x.h);
	x.l = 0 - x.l;
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_abs_inline(tb_sint64_t x)
{
	return (x.h < 0)? tb_sint64_neg(x) : x;
}
static __tb_inline__ tb_sint64_t tb_sint64_add_inline(tb_sint64_t x, tb_sint64_t y)
{
	tb_uint32_t s = x.l + y.l;

	x.h += y.h + (s < x.l);
	x.l = s;

	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_sub_inline(tb_sint64_t x, tb_sint64_t y)
{
    x.h -= y.h + (x.l < y.l);
    x.l -= y.l;
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_mul_inline(tb_sint64_t x, tb_sint64_t y)
{
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_div_inline(tb_sint64_t x, tb_sint64_t y)
{
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_mod_inline(tb_sint64_t x, tb_sint64_t y)
{
	return x;
}
static __tb_inline__ tb_size_t tb_sint64_clz_inline(tb_sint64_t x)
{
	return x.h? tb_int32_clz(x.h) : (32 + tb_int32_clz(x.l));
}
static __tb_inline__ tb_sint64_t tb_sint64_not_inline(tb_sint64_t x)
{
	x.h = ~x.h;
	x.l = ~x.l;
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_and_inline(tb_sint64_t x, tb_sint64_t y)
{	
	x.h &= y.h;
	x.l &= y.l;
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_xor_inline(tb_sint64_t x, tb_sint64_t y)
{
	x.h ^= y.h;
	x.l ^= y.l;
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_or_inline(tb_sint64_t x, tb_sint64_t y)
{
	x.h |= y.h;
	x.l |= y.l;
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_add_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	tb_sint32_t h = y >> 31;
	tb_uint32_t s = x.l + (tb_uint32_t)y;

	x.h += h + (s < x.l);
	x.l = s;

	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_sub_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	tb_sint32_t h = y >> 31;
	tb_uint32_t s = x.l - (tb_uint32_t)y;

	x.h -= h + (s < x.l);
	x.l = s;

	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_mul_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_div_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_mod_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_or_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	x.l |= y;
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_and_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	x.h = 0;
	x.l &= y;
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_xor_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	x.l ^= y;
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_lsh_inline(tb_sint64_t x, tb_size_t b)
{
	TB_ASSERT(b < 64);
	//b &= 0x3f;
	if (b >= 32)
	{
		x.h = x.l << (b - 32);
		x.l = 0;
	}
	else if (b)
	{
		x.h = (x.h << b) | (x.l >> (32 - b));
		x.l <<= b;
	}
	return x;
}
static __tb_inline__ tb_sint64_t tb_sint64_rsh_inline(tb_sint64_t x, tb_size_t b)
{
	TB_ASSERT(b < 64);
	//b &= 0x3f;
	if (b >= 32)
	{
		x.l = x.h >> (b - 32);
		x.h >>= 31;
	}
	else if (b)
	{
		x.l = (x.h << (32 - b)) | (x.l >> b);
		x.h >>= b;
	}
	return x;
}
static __tb_inline__ tb_int_t tb_sint64_nz_inline(tb_sint64_t x)
{
 	return (x.h | x.l);
}
static __tb_inline__ tb_int_t tb_sint64_ez_inline(tb_sint64_t x)
{
 	return !(x.h | x.l);
}
static __tb_inline__ tb_int_t tb_sint64_lz_inline(tb_sint64_t x)
{
 	return ((tb_uint32_t)x.h >> 31);
}
static __tb_inline__ tb_int_t tb_sint64_gz_inline(tb_sint64_t x)
{
 	return ~(x.h >> 31) & (x.h | x.l);
}
static __tb_inline__ tb_int_t tb_sint64_et_inline(tb_sint64_t x, tb_sint64_t y)
{
 	return (x.h == y.h && x.l == y.l);
}
static __tb_inline__ tb_int_t tb_sint64_nt_inline(tb_sint64_t x, tb_sint64_t y)
{
 	return (x.h != y.h || x.l != y.l);
}
static __tb_inline__ tb_int_t tb_sint64_lt_inline(tb_sint64_t x, tb_sint64_t y)
{
 	return ((x.h < y.h) || (x.h == y.h && x.l < y.l));
}
static __tb_inline__ tb_int_t tb_sint64_gt_inline(tb_sint64_t x, tb_sint64_t y)
{
 	return ((x.h > y.h) || (x.h == y.h && x.l > y.l));
}
static __tb_inline__ tb_int_t tb_sint64_et_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
 	return (x.h == (y >> 31) && x.l == y);
}
static __tb_inline__ tb_int_t tb_sint64_nt_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
 	return (x.h != (y >> 31) || x.l != y);
}
static __tb_inline__ tb_int_t tb_sint64_lt_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
 	return ((x.h < (y >> 31)) || (x.h == (y >> 31) && x.l < y));
}
static __tb_inline__ tb_int_t tb_sint64_gt_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
 	return ((x.h > (y >> 31)) || (x.h == (y >> 31) && x.l > y));
}
#else
static __tb_inline__ tb_size_t tb_sint64_clz_inline(tb_sint64_t x)
{
	tb_sint32_t h = x >> 32;
	tb_uint32_t l = x & 0xffffffff;
	return h? tb_int32_clz(h) : (32 + tb_int32_clz(l));
}
#endif

#endif

