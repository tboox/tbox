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
 * \file		fixed.h
 *
 */
#ifndef TB_FIXED_H
#define TB_FIXED_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "type.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

/* fixed-point numbers
 *
 * the swf file format supports two types of fixed-point numbers: 32-bit and 16-bit.
 * the 32-bit fixed-point numbers are 16.16. that is, the high 16 bits represent the number
 * before the decimal point, and the low 16 bits represent the number after the decimal point.
 * fixed values are stored like 32-bit integers in the swf file (using little-endian byte order)
 * and must be byte aligned.
 */
#define TB_FIXED8_FACTOR 			(0xff)
#define TB_FIXED16_FACTOR 			(0xffff)
#define TB_FIXED32_FACTOR 			(0xffffffff)

#define TB_FIXED32_2_FLOAT(x) 		((tg_float_t)(x) / 0xffffffff)
#define TB_FLOAT_2_FIXED32(x) 		((tg_fixed32_t)((x) * 0xffffffff))
	
#define TB_FIXED16_2_FLOAT(x) 		((tg_float_t)(x) / 0xffff)
#define TB_FLOAT_2_FIXED16(x) 		((tg_fixed16_t)((x) * 0xffff))

#define TB_FIXED8_2_FLOAT(x) 		((tg_float_t)(x) / 0xff)
#define TB_FLOAT_2_FIXED8(x) 		((tg_fixed8_t)((x) * 0xff))

#define TB_FIXED32_2_INT(x) 		((x) >> 32)
#define TB_INT_2_FIXED32(x) 		((x) << 32)

#define TB_FIXED16_2_INT(x) 		((x) >> 16)
#define TB_INT_2_FIXED16(x) 		((x) << 16)
#define TB_FIXED16_2_RINT(x) 		((((x) >= 0)? ((x) + (TB_FIXED16_FACTOR >> 1)) : ((x) - (TB_FIXED16_FACTOR >> 1))) >> 16)

#define TB_FIXED8_2_INT(x) 			((x) >> 8)
#define TB_INT_2_FIXED8(x) 			((x) << 8)

#define TB_FIXED16_2_FIXED32(x) 	((x) << 16)
#define TB_FIXED32_2_FIXED16(x) 	((x) >> 16)

#define TB_FIXED8_2_FIXED16(x) 		((x) << 8)
#define TB_FIXED16_2_FIXED8(x) 		((x) >> 8)

/* /////////////////////////////////////////////////////////
 * basic types
 */
typedef tb_int16_t 			tb_fixed8_t;
typedef tb_int32_t 			tb_fixed16_t;


// c plus plus
#ifdef __cplusplus
}
#endif

#endif
