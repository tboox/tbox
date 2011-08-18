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

/* ////////////////////////////////////////////////////////////////////////
 * macros 
 */

#ifdef TB_CONFIG_TYPE_INT64
# 	define tb_uint32_to_uint64(x) 		(tb_uint64_t)(x)
# 	define tb_uint64_to_uint32(x) 		(tb_uint32_t)(x)

# 	define tb_uint64_add(x, y) 			((x) + (y))
# 	define tb_uint64_sub(x, y) 			((x) - (y))
# 	define tb_uint64_mul(x, y) 			((x) * (y))
# 	define tb_uint64_div(x, y) 			((x) / (y))

# 	define tb_uint64_add_uint32(x, y) 	((x) + (tb_uint32_t)(y))
# 	define tb_uint64_sub_uint32(x, y) 	((x) - (tb_uint32_t)(y))
# 	define tb_uint64_mul_uint32(x, y) 	((x) * (tb_uint32_t)(y))
# 	define tb_uint64_div_uint32(x, y) 	((x) / (tb_uint32_t)(y))

# 	define tb_uint64_et(x, y) 			((x) == (y))
# 	define tb_uint64_lt(x, y) 			((x) < (y))
# 	define tb_uint64_bt(x, y) 			((x) > (y))

# 	define tb_uint64_et_uint32(x, y)	((tb_uint32_t)(x) == (y))
# 	define tb_uint64_lt_uint32(x, y)	((tb_uint32_t)(x) < (y))
# 	define tb_uint64_bt_uint32(x, y) 	((tb_uint32_t)(x) > (y))
#else
# 	define tb_uint32_to_uint64(x) 		tb_uint32_to_uint64_inline(x)
# 	define tb_uint64_to_uint32(x) 		tb_uint64_to_uint32_inline(x)

# 	define tb_uint64_add(x, y) 			tb_uint64_add_inline(x, y)
# 	define tb_uint64_sub(x, y) 			tb_uint64_sub_inline(x, y)
# 	define tb_uint64_mul(x, y) 			tb_uint64_mul_inline(x, y)
# 	define tb_uint64_div(x, y) 			tb_uint64_div_inline(x, y)

# 	define tb_uint64_add_uint32(x, y) 	tb_uint64_add_uint32_inline(x, y)
# 	define tb_uint64_sub_uint32(x, y) 	tb_uint64_sub_uint32_inline(x, y)
# 	define tb_uint64_mul_uint32(x, y) 	tb_uint64_mul_uint32_inline(x, y)
# 	define tb_uint64_div_uint32(x, y) 	tb_uint64_div_uint32_inline(x, y)

# 	define tb_uint64_et(x, y) 			tb_uint64_et_inline(x, y)
# 	define tb_uint64_lt(x, y) 			tb_uint64_lt_inline(x, y)
# 	define tb_uint64_bt(x, y) 			tb_uint64_bt_inline(x, y)

# 	define tb_uint64_et_uint32(x, y)	tb_uint64_et_uint32_inline(x, y)
# 	define tb_uint64_lt_uint32(x, y)	tb_uint64_lt_uint32_inline(x, y)
# 	define tb_uint64_bt_uint32(x, y) 	tb_uint64_bt_uint32_inline(x, y)
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
static __tb_inline__ tb_uint32_t tb_uint64_to_int32_inline(tb_uint64_t x)
{
	// is uint32?
	TB_ASSERT(!x.h);
	return x.l;
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
//#error
static __tb_inline__ tb_uint64_t tb_uint64_mul_inline(tb_uint64_t x, tb_uint64_t y)
{
	TB_ASSERT(!x.h && !y.h);

	tb_uint32_t xlh = x.l >> 16;
	tb_uint32_t xll = x.l & 0xffff;
	tb_uint32_t ylh = y.l >> 16;
	tb_uint32_t yll = y.l & 0xffff;

	tb_uint32_t a = xlh * ylh;
	tb_uint32_t b = xlh * yll + xll * ylh;
	tb_uint32_t c = xll * yll;

	x.l = c + (b << 16);
	x.h = a + (b >> 16) + (x.l < c);

	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_div_inline(tb_uint64_t x, tb_uint64_t y)
{
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

	return x;
}
static __tb_inline__ tb_uint64_t tb_uint64_div_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
	return x;
}
static __tb_inline__ tb_int_t tb_uint64_et_inline(tb_uint64_t x, tb_uint64_t y)
{
 	return (x.h == y.h && x.l == y.l);
}
static __tb_inline__ tb_int_t tb_uint64_lt_inline(tb_uint64_t x, tb_uint64_t y)
{
 	return ((x.h < y.h) || (x.h == y.h && x.l < y.l));
}
static __tb_inline__ tb_int_t tb_uint64_bt_inline(tb_uint64_t x, tb_uint64_t y)
{
 	return ((x.h > y.h) || (x.h == y.h && x.l > y.l));
}
static __tb_inline__ tb_int_t tb_uint64_et_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
 	return (!x.h && x.l == y);
}
static __tb_inline__ tb_int_t tb_uint64_lt_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
 	return (!x.h && x.l < y);
}
static __tb_inline__ tb_int_t tb_uint64_bt_uint32_inline(tb_uint64_t x, tb_uint32_t y)
{
 	return (x.h || (x.l > y));
}
#endif

#endif

