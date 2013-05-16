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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		isinf.c
 * @ingroup 	libm
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "math.h"

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */

tb_long_t tb_isin(tb_double_t x)
{
	tb_ieee_double_t e; e.d = x;
	tb_int32_t 		t = e.i.l | ((e.i.h & 0x7fffffff) ^ 0x7ff00000);
	t |= -t;
	return (tb_long_t)(~(t >> 31) & (e.i.h >> 30));
}
