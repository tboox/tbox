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
 * \file		int64.h
 *
 */
#ifndef TB_MATH_INT64_H
#define TB_MATH_INT64_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "sint64.h"
#include "uint64.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros 
 */

#define tb_int32_to_int64(x) 		tb_sint32_to_sint64(x)
#define tb_int64_to_int32(x) 		tb_sint64_to_sint32(x)

#define tb_int64_add(x, y) 			tb_sint64_add(x, y)
#define tb_int64_sub(x, y) 			tb_sint64_sub(x, y)
#define tb_int64_mul(x, y) 			tb_sint64_mul(x, y)
#define tb_int64_div(x, y) 			tb_sint64_div(x, y)

#define tb_int64_add_int32(x, y) 	tb_sint64_add_sint32(x, y)
#define tb_int64_sub_int32(x, y) 	tb_sint64_sub_sint32(x, y)
#define tb_int64_mul_int32(x, y) 	tb_sint64_mul_sint32(x, y)
#define tb_int64_div_int32(x, y) 	tb_sint64_div_sint32(x, y)

#define tb_int64_lt(x, y) 			tb_sint64_lt(x, y)
#define tb_int64_bt(x, y) 			tb_sint64_bt(x, y)

#define tb_int64_lt_int32(x, y)		tb_sint64_lt_sint32(x, y)
#define tb_int64_bt_int32(x, y) 	tb_sint64_bt_sint32(x, y)

#endif

