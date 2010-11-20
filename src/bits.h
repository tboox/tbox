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
 * \file		bits.h
 *
 */
#ifndef TB_BITS_H
#define TB_BITS_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * types
 */

/* /////////////////////////////////////////////////////////
 * macros
 */

// 1-bits
#define tb_bits_get_u1(p) 		(((*(p)) >> 7) & 1)

// 8-bits
#define tb_bits_get_u8(p) 		(*(p))
#define tb_bits_get_s8(p) 		(*(p))

// 16-bits
#define tb_bits_get_u16_le(p) 	(*((p) + 1) << 8 | *(p))
#define tb_bits_get_s16_le(p) 	(*((p) + 1) << 8 | *(p))
#define tb_bits_get_u16_be(p) 	(*((p)) << 8 | *((p) + 1))
#define tb_bits_get_s16_be(p) 	(*((p)) << 8 | *((p) + 1))
#define tb_bits_get_u16_ne(p) 	(*((tb_uint16_t*)(p)))
#define tb_bits_get_s16_ne(p) 	(*((tb_sint16_t*)(p)))
#define tb_bits_get_u16(p) 		tb_bits_get_u16_ne(p)
#define tb_bits_get_s16(p) 		tb_bits_get_s16_ne(p)

// 24-bits
#define tb_bits_get_u24_le(p) 	(*((p) + 2) << 16 | *((p) + 1) << 8 | *(p))
#define tb_bits_get_s24_le(p) 	(*((p) + 2) << 16 | *((p) + 1) << 8 | *(p))
#define tb_bits_get_u24_be(p) 	(*(p) << 16 | *((p) + 1) << 8 | *((p) + 2))
#define tb_bits_get_s24_be(p) 	(*(p) << 16 | *((p) + 1) << 8 | *((p) + 2))
#define tb_bits_get_u24_ne(p) 	(*((tb_uint32_t*)(p)) & 0x00ffffff)
#define tb_bits_get_s24_ne(p) 	(*((tb_sint32_t*)(p)) & 0x80ffffff)
#define tb_bits_get_u24(p) 		tb_bits_get_u24_ne(p)
#define tb_bits_get_s24(p) 		tb_bits_get_s24_ne(p)

// 32-bits
#define tb_bits_get_u32_le(p) 	(*((p) + 3) << 24 | *((p) + 2) << 16 | *((p) + 1) << 8 | *(p))
#define tb_bits_get_s32_le(p) 	(*((p) + 3) << 24 | *((p) + 2) << 16 | *((p) + 1) << 8 | *(p))
#define tb_bits_get_u32_be(p) 	(*(p) << 24 | *((p) + 1) << 16 | *((p) + 2) << 8 | *((p) + 3))
#define tb_bits_get_s32_be(p) 	(*(p) << 24 | *((p) + 1) << 16 | *((p) + 2) << 8 | *((p) + 3))
#define tb_bits_get_u32_ne(p) 	(*((tb_uint32_t*)(p)))
#define tb_bits_get_s32_ne(p) 	(*((tb_sint32_t*)(p)))
#define tb_bits_get_u32(p) 		tb_bits_get_u32_ne(p)
#define tb_bits_get_s32(p) 		tb_bits_get_s32_ne(p)

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// string
tb_char_t const* 	tb_bits_get_string(tb_byte_t const* p, tb_size_t size);

// float
tb_float_t 			tb_bits_get_float_le(tb_byte_t const* p);
tb_float_t 			tb_bits_get_float_be(tb_byte_t const* p);
tb_float_t 			tb_bits_get_float_ne(tb_byte_t const* p);

// double
tb_float_t 			tb_bits_get_double_le(tb_byte_t const* p);
tb_float_t 			tb_bits_get_double_be(tb_byte_t const* p);
tb_float_t 			tb_bits_get_double_ne(tb_byte_t const* p);


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

