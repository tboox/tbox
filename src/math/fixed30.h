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
 * \file		fixed30.h
 *
 */
#ifndef TB_MATH_FIXED30_H
#define TB_MATH_FIXED30_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

// constant
#define TB_FIXED30_ONE 					(1 << 30)
#define TB_FIXED30_HALF 				(1 << 29)
#define TB_FIXED30_MAX 					(TB_MAXS32)
#define TB_FIXED30_MIN 					(-TB_FIXED30_MAX)
#define TB_FIXED30_NAN 					((tb_int_t)0x80000000)
#define TB_FIXED30_INF 					(TB_MAXS32)
#define TB_FIXED30_PI 					(0xc90fdaa2)
#define TB_FIXED30_SQRT2 				(0x5a827999)

// conversion
#ifdef TB_CONFIG_TYPE_FLOAT
# 	ifndef tb_fixed30_to_float
# 		define tb_fixed30_to_float(x) 	(((x) * 0.00000000093132257f))
# 	endif
# 	ifndef tb_float_to_fixed30
# 		define tb_float_to_fixed30(x) 	((tb_fixed30_t)((x) * TB_FIXED30_ONE))
# 	endif
#endif


#endif

