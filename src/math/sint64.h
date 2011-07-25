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
# 	define tb_sint32_to_sint64(x) 		(tb_sint64_t)(x)
# 	define tb_sint64_to_sint32(x) 		(tb_sint32_t)(x)

# 	define tb_sint64_add(x, y) 			((x) + (y))
# 	define tb_sint64_sub(x, y) 			((x) - (y))
# 	define tb_sint64_mul(x, y) 			((x) * (y))
# 	define tb_sint64_div(x, y) 			((x) / (y))

# 	define tb_sint64_add_sint32(x, y) 	((x) + (tb_sint32_t)(y))
# 	define tb_sint64_sub_sint32(x, y) 	((x) - (tb_sint32_t)(y))
# 	define tb_sint64_mul_sint32(x, y) 	((x) * (tb_sint32_t)(y))
# 	define tb_sint64_div_sint32(x, y) 	((x) / (tb_sint32_t)(y))

# 	define tb_sint64_lt(x, y) 			((x) < (y))
# 	define tb_sint64_bt(x, y) 			((x) > (y))

# 	define tb_sint64_lt_sint32(x, y)	((x) < (tb_sint32_t)(y))
# 	define tb_sint64_bt_sint32(x, y) 	((x) > (tb_sint32_t)(y))
#else
# 	define tb_sint32_to_sint64(x) 		tb_sint32_to_sint64_inline(x)
# 	define tb_sint64_to_sint32(x) 		tb_sint64_to_sint32_inline(x)

# 	define tb_sint64_add(x, y) 			tb_sint64_add_inline(x, y)
# 	define tb_sint64_sub(x, y) 			tb_sint64_sub_inline(x, y)
# 	define tb_sint64_mul(x, y) 			tb_sint64_mul_inline(x, y)
# 	define tb_sint64_div(x, y) 			tb_sint64_div_inline(x, y)

# 	define tb_sint64_add_sint32(x, y) 	tb_sint64_add_sint32_inline(x, y)
# 	define tb_sint64_sub_sint32(x, y) 	tb_sint64_sub_sint32_inline(x, y)
# 	define tb_sint64_mul_sint32(x, y) 	tb_sint64_mul_sint32_inline(x, y)
# 	define tb_sint64_div_sint32(x, y) 	tb_sint64_div_sint32_inline(x, y)

# 	define tb_sint64_lt(x, y) 			tb_sint64_lt_inline(x, y)
# 	define tb_sint64_bt(x, y) 			tb_sint64_bt_inline(x, y)

# 	define tb_sint64_lt_sint32(x, y)	tb_sint64_lt_sint32_inline(x, y)
# 	define tb_sint64_bt_sint32(x, y) 	tb_sint64_bt_sint32_inline(x, y)
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

static __tb_inline__ tb_sint64_t tb_sint64_add_inline(tb_sint64_t x, tb_sint64_t y)
{
	tb_sint64_t a;
	return a;
}
static __tb_inline__ tb_sint64_t tb_sint64_sub_inline(tb_sint64_t x, tb_sint64_t y)
{
	tb_sint64_t a = {0};
	return a;
}
static __tb_inline__ tb_sint64_t tb_sint64_mul_inline(tb_sint64_t x, tb_sint64_t y)
{
	tb_sint64_t a;
	return a;
}
static __tb_inline__ tb_sint64_t tb_sint64_div_inline(tb_sint64_t x, tb_sint64_t y)
{
	tb_sint64_t a;
	return a;
}
static __tb_inline__ tb_sint64_t tb_sint64_add_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	tb_sint64_t a;
	return a;
}
static __tb_inline__ tb_sint64_t tb_sint64_sub_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	tb_sint64_t a;
	return a;
}
static __tb_inline__ tb_sint64_t tb_sint64_mul_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	tb_sint64_t a;
	return a;
}
static __tb_inline__ tb_sint64_t tb_sint64_div_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	tb_sint64_t a;
	return a;
}
static __tb_inline__ tb_bool_t tb_sint64_lt_inline(tb_sint64_t x, tb_sint64_t y)
{
	return TB_FALSE;
}
static __tb_inline__ tb_bool_t tb_sint64_bt_inline(tb_sint64_t x, tb_sint64_t y)
{
	return TB_FALSE;
}
static __tb_inline__ tb_bool_t tb_sint64_lt_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	return TB_FALSE;
}
static __tb_inline__ tb_bool_t tb_sint64_bt_sint32_inline(tb_sint64_t x, tb_sint32_t y)
{
	return TB_FALSE;
}
#endif

#endif

