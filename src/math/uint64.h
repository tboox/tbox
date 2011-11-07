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
 * \file		uint64.h
 *
 */
#ifndef TB_MATH_UINT64_H
#define TB_MATH_UINT64_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "int32.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros 
 */

#ifdef TB_CONFIG_TYPE_INT64
# 	define TB_UINT64_ZERO 				(tb_uint64_t)(0)
# 	define TB_UINT64_ONE 				(tb_uint64_t)(1)

# 	define tb_uint32_to_uint64(x) 		(tb_uint64_t)(x)
# 	define tb_uint64_to_uint32(x) 		(tb_uint32_t)(x)
# 	define tb_uint64_make(h, l) 		(tb_uint64_t)(((tb_uint64_t)(h) << 32) + (tb_uint64_t)(l))

# 	define tb_uint64_add(x, y) 			((x) + (y))
# 	define tb_uint64_sub(x, y) 			((x) - (y))
# 	define tb_uint64_mul(x, y) 			((x) * (y))
# 	define tb_uint64_div(x, y) 			((x) / (y))
# 	define tb_uint64_mod(x, y) 			((x) % (y))

# 	define tb_uint64_clz(x) 			tb_uint64_clz_inline(x)
# 	define tb_uint64_not(x) 			(~(x))
# 	define tb_uint64_or(x, y) 			((x) | (y))
# 	define tb_uint64_and(x, y) 			((x) & (y))
# 	define tb_uint64_xor(x, y) 			((x) ^ (y))

# 	define tb_uint64_lsh(x, b) 			((x) << (b))
# 	define tb_uint64_rsh(x, b) 			((x) >> (b))

# 	define tb_uint64_add_uint32(x, y) 	((x) + (tb_uint32_t)(y))
# 	define tb_uint64_sub_uint32(x, y) 	((x) - (tb_uint32_t)(y))
# 	define tb_uint64_mul_uint32(x, y) 	((x) * (tb_uint32_t)(y))
# 	define tb_uint64_div_uint32(x, y) 	((x) / (tb_uint32_t)(y))
# 	define tb_uint64_mod_uint32(x, y) 	((x) % (tb_uint32_t)(y))

# 	define tb_uint64_or_uint32(x, y) 	((x) | (tb_uint32_t)(y))
# 	define tb_uint64_and_uint32(x, y) 	((x) & (tb_uint32_t)(y))
# 	define tb_uint64_xor_uint32(x, y) 	((x) ^ (tb_uint32_t)(y))

# 	define tb_uint64_nz(x) 				(x)
# 	define tb_uint64_ez(x) 				!(x)
# 	define tb_uint64_et(x, y) 			((x) == (y))
# 	define tb_uint64_nt(x, y) 			((x) != (y))
# 	define tb_uint64_lt(x, y) 			((x) < (y))
# 	define tb_uint64_gt(x, y) 			((x) > (y))

# 	define tb_uint64_et_uint32(x, y)	((x) == (tb_uint32_t)(y))
# 	define tb_uint64_nt_uint32(x, y)	((x) != (tb_uint32_t)(y))
# 	define tb_uint64_lt_uint32(x, y)	((x) < (tb_uint32_t)(y))
# 	define tb_uint64_gt_uint32(x, y) 	((x) > (tb_uint32_t)(y))
#else
# 	define TB_UINT64_ZERO 				g_uint64_zero
# 	define TB_UINT64_ONE 				g_uint64_one

# 	define tb_uint32_to_uint64(x) 		tb_uint32_to_uint64_inline(x)
# 	define tb_uint64_to_uint32(x) 		tb_uint64_to_uint32_inline(x)
# 	define tb_uint64_make(h, l) 		tb_uint64_make_inline(h, l)

# 	define tb_uint64_add(x, y) 			tb_uint64_add_inline(x, y)
# 	define tb_uint64_sub(x, y) 			tb_uint64_sub_inline(x, y)
# 	define tb_uint64_mul(x, y) 			tb_uint64_mul_inline(x, y)
# 	define tb_uint64_div(x, y) 			tb_uint64_div_inline(x, y)
# 	define tb_uint64_mod(x, y) 			tb_uint64_mod_inline(x, y)

# 	define tb_uint64_clz(x) 			tb_uint64_clz_inline(x)
# 	define tb_uint64_not(x) 			tb_uint64_not_inline(x)
# 	define tb_uint64_or(x, y) 			tb_uint64_or_inline(x, y)
# 	define tb_uint64_and(x, y) 			tb_uint64_and_inline(x, y)
# 	define tb_uint64_xor(x, y) 			tb_uint64_xor_inline(x, y)

# 	define tb_uint64_lsh(x, b) 			tb_uint64_lsh_inline(x, b)
# 	define tb_uint64_rsh(x, b) 			tb_uint64_rsh_inline(x, b)

# 	define tb_uint64_add_uint32(x, y) 	tb_uint64_add_uint32_inline(x, y)
# 	define tb_uint64_sub_uint32(x, y) 	tb_uint64_sub_uint32_inline(x, y)
# 	define tb_uint64_mul_uint32(x, y) 	tb_uint64_mul_uint32_inline(x, y)
# 	define tb_uint64_div_uint32(x, y) 	tb_uint64_div_uint32_inline(x, y)
# 	define tb_uint64_mod_uint32(x, y) 	tb_uint64_mod_uint32_inline(x, y)

# 	define tb_uint64_or_uint32(x, y) 	tb_uint64_or_uint32_inline(x, y)
# 	define tb_uint64_and_uint32(x, y) 	tb_uint64_and_uint32_inline(x, y)
# 	define tb_uint64_xor_uint32(x, y) 	tb_uint64_xor_uint32_inline(x, y)

# 	define tb_uint64_nz(x) 				tb_uint64_nz_inline(x)
# 	define tb_uint64_ez(x) 				tb_uint64_ez_inline(x)
# 	define tb_uint64_et(x, y) 			tb_uint64_et_inline(x, y)
# 	define tb_uint64_nt(x, y) 			tb_uint64_nt_inline(x, y)
# 	define tb_uint64_lt(x, y) 			tb_uint64_lt_inline(x, y)
# 	define tb_uint64_gt(x, y) 			tb_uint64_gt_inline(x, y)

# 	define tb_uint64_et_uint32(x, y)	tb_uint64_et_uint32_inline(x, y)
# 	define tb_uint64_nt_uint32(x, y)	tb_uint64_nt_uint32_inline(x, y)
# 	define tb_uint64_lt_uint32(x, y)	tb_uint64_lt_uint32_inline(x, y)
# 	define tb_uint64_gt_uint32(x, y) 	tb_uint64_gt_uint32_inline(x, y)
#endif

/* ////////////////////////////////////////////////////////////////////////
 * globals 
 */

#ifndef TB_CONFIG_TYPE_INT64
extern tb_uint64_t const g_uint64_zero;
extern tb_uint64_t const g_uint64_one;
#endif

/* ////////////////////////////////////////////////////////////////////////
 * inline
 */
#ifndef TB_CONFIG_TYPE_INT64
static __tb_inline__ tb_uint64_t tb_uint32_to_uint64_inline(tb_uint32_t x)
{
	tb_uint64_t i;
	i.h = 0;
	i.l = x;
	return i;
}
static __tb_inline__ tb_uint32_t tb_uint64_to_uint32_inline(tb_uint64_t x)
{
	// is uint32?
	tb_assert(!x.h);
	return x.l;
}
static __tb_inline__ tb_uint64_t tb_uint64_make_inline(tb_uint32_t h, tb_uint32_t l)
{
	tb_uint64_t x;
	x.h = h;
	x.l = l;
	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_add_inline(tb_uint64_t x, tb_uint64_t y)
{
	tb_uint32_t s = x.l + y.l;

	x.h += y.h + (s < x.l);
	x.l = s;

	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_sub_inline(tb_uint64_t x, tb_uint64_t y)
{
    x.h -= y.h + (x.l < y.l);
    x.l -= y.l;

	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_mul_uint32_x_uint32(tb_uint32_t x, tb_uint32_t y)
{
	tb_uint64_t r;
	tb_uint32_t xh = x >> 16;
	tb_uint32_t xl = x & 0xffff;
	tb_uint32_t yh = y >> 16;
	tb_uint32_t yl = y & 0xffff;

	tb_uint32_t a = xh * yh;
	tb_uint32_t b = xh * yl + xl * yh;
	tb_uint32_t c = xl * yl;

	r.l = c + (b << 16);
	r.h = a + (b >> 16) + (r.l < c);

	return r;
}
static __tb_inline__ tb_uint64_t tb_uint64_mul_inline(tb_uint64_t x, tb_uint64_t y)
{
	// (x.h << 32 + x.l) * (y.h << 32 + y.l)
	// x.h * y.h << 64 + x.l * y.h << 32 + x.h * y.l << 32 + x.l * y.l
	// (x.l * y.h + x.h * y.l) << 32 + x.l * y.l
	tb_uint32_t h = x.h * y.l + x.l * y.h;
	x = tb_uint64_mul_uint32_x_uint32(x.l, y.l);
	x.h += h;
	tb_assert(x.h >= h);

	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_div_inline(tb_uint64_t x, tb_uint64_t y)
{
	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_mod_inline(tb_uint64_t x, tb_uint64_t y)
{
	return x;
}
static __tb_inline__ tb_size_t tb_uint64_clz_inline(tb_uint64_t x)
{
	return x.h? tb_int32_clz(x.h) : (32 + tb_int32_clz(x.l));
}
static __tb_inline__ tb_uint64_t tb_uint64_not_inline(tb_uint64_t x)
{
	x.h = ~x.h;
	x.l = ~x.l;
	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_and_inline(tb_uint64_t x, tb_uint64_t y)
{	
	x.h &= y.h;
	x.l &= y.l;
	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_xor_inline(tb_uint64_t x, tb_uint64_t y)
{
	x.h ^= y.h;
	x.l ^= y.l;
	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_or_inline(tb_uint64_t x, tb_uint64_t y)
{
	x.h |= y.h;
	x.l |= y.l;
	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_add_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
	tb_uint32_t s = x.l + y;

	x.h += (s < x.l);
	x.l = s;

	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_sub_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
    x.h -= x.l < y;
    x.l -= y;

	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_mul_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
#if 0
	// (x.h << 32 + x.l) * y
	// (x.h * y) << 32 + x.l * y
	tb_uint32_t h = x.h * y;
	x = tb_uint64_mul_uint32_x_uint32(x.l, y);
	x.h += h;
	tb_assert(x.h >= h);
	return x;
#else
	tb_uint64_t r;
	tb_uint32_t xlh = x.l >> 16;
	tb_uint32_t xll = x.l & 0xffff;	
	tb_uint32_t xhh = x.h >> 16;
	tb_uint32_t xhl = x.h & 0xffff;
	tb_uint32_t ylh = y >> 16;
	tb_uint32_t yll = y & 0xffff;

	tb_uint32_t a = xll * yll;
	tb_uint32_t b = xlh * yll + xll * ylh;
	tb_uint32_t c = xhl * yll + xlh * ylh;
	tb_uint32_t d = xhh * yll + xhl * ylh;

	r.l = (b << 16) + a;
	r.h = (d << 16) + c + (b >> 16) + (r.l < a);
	tb_assert(r.h >= c);

	return r;
#endif
}
static __tb_inline__ tb_uint64_t tb_uint64_div_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_mod_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_or_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
	x.l |= y;
	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_and_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
	x.h = 0;
	x.l &= y;
	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_xor_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
	x.l ^= y;
	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_lsh_inline(tb_uint64_t x, tb_size_t b)
{
	tb_assert(b < 64);
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
static __tb_inline__ tb_uint64_t tb_uint64_rsh_inline(tb_uint64_t x, tb_size_t b)
{
	tb_assert(b < 64);
	//b &= 0x3f;
	if (b >= 32)
	{
		x.l = x.h >> (b - 32);
		x.h = 0;
	}
	else if (b)
	{
		x.l = (x.h << (32 - b)) | (x.l >> b);
		x.h >>= b;
	}
	return x;
}
static __tb_inline__ tb_int_t tb_uint64_nz_inline(tb_uint64_t x)
{
 	return (x.h || x.l);
}
static __tb_inline__ tb_int_t tb_uint64_ez_inline(tb_uint64_t x)
{
 	return !(x.h | x.l);
}
static __tb_inline__ tb_int_t tb_uint64_et_inline(tb_uint64_t x, tb_uint64_t y)
{
 	return (x.h == y.h && x.l == y.l);
}
static __tb_inline__ tb_int_t tb_uint64_nt_inline(tb_uint64_t x, tb_uint64_t y)
{
 	return (x.h != y.h || x.l != y.l);
}
static __tb_inline__ tb_int_t tb_uint64_lt_inline(tb_uint64_t x, tb_uint64_t y)
{
 	return ((x.h < y.h) || (x.h == y.h && x.l < y.l));
}
static __tb_inline__ tb_int_t tb_uint64_gt_inline(tb_uint64_t x, tb_uint64_t y)
{
 	return ((x.h > y.h) || (x.h == y.h && x.l > y.l));
}
static __tb_inline__ tb_int_t tb_uint64_et_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
 	return (!x.h && x.l == y);
}
static __tb_inline__ tb_int_t tb_uint64_nt_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
 	return (x.h || x.l != y);
}
static __tb_inline__ tb_int_t tb_uint64_lt_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
 	return (!x.h && x.l < y);
}
static __tb_inline__ tb_int_t tb_uint64_gt_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
 	return (x.h || (x.l > y));
}
#else
static __tb_inline__ tb_size_t tb_uint64_clz_inline(tb_uint64_t x)
{
	tb_uint32_t h = x >> 32;
	tb_uint32_t l = x & 0xffffffff;
	return h? tb_int32_clz(h) : (32 + tb_int32_clz(l));
}
#endif

#endif

