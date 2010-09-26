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
 * along with TGraphic; 
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
typedef struct __tb_bits_t
{
	tb_byte_t* 			p; 	// the pointer to the current position
	tb_size_t 			b; 	// the bit offset < 8
	tb_byte_t const* 	e; 	// the pointer to the end

}tb_bits_t;

/* /////////////////////////////////////////////////////////
 * macros
 */

#define tb_bits_get_u16 			tb_bits_get_u16_ne
#define tb_bits_get_s16 			tb_bits_get_s16_ne
#define tb_bits_get_u24 			tb_bits_get_u24_ne
#define tb_bits_get_s24 			tb_bits_get_s24_ne
#define tb_bits_get_u32 			tb_bits_get_u32_ne
#define tb_bits_get_s32 			tb_bits_get_s32_ne

#define tb_bits_set_u16 			tb_bits_set_u16_ne
#define tb_bits_set_s16 			tb_bits_set_s16_ne
#define tb_bits_set_u24 			tb_bits_set_u24_ne
#define tb_bits_set_s24 			tb_bits_set_s24_ne
#define tb_bits_set_u32 			tb_bits_set_u32_ne
#define tb_bits_set_s32 			tb_bits_set_s32_ne

#define tb_pbits_get_u16 			tb_pbits_get_u16_ne
#define tb_pbits_get_s16 			tb_pbits_get_s16_ne
#define tb_pbits_get_u24 			tb_pbits_get_u24_ne
#define tb_pbits_get_s24 			tb_pbits_get_s24_ne
#define tb_pbits_get_u32 			tb_pbits_get_u32_ne
#define tb_pbits_get_s32 			tb_pbits_get_s32_ne

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init
void 				tb_bits_attach(tb_bits_t* bits, tb_byte_t* data, tb_size_t size);

// modifior
void 				tb_bits_goto(tb_bits_t* bits, tb_byte_t* data);
void 				tb_bits_sync(tb_bits_t* bits);

// position
tb_byte_t const* 	tb_bits_tell(tb_bits_t* bits);
tb_byte_t const* 	tb_bits_end(tb_bits_t* bits);

// left
tb_size_t 			tb_bits_left_bytes(tb_bits_t* bits);
tb_size_t 			tb_bits_left_bits(tb_bits_t* bits);

// skip
void 				tb_bits_skip_bytes(tb_bits_t* bits, tb_size_t bytes_n);
void 				tb_bits_skip_bits(tb_bits_t* bits, tb_size_t bits_n);
tb_char_t const* 	tb_bits_skip_string(tb_bits_t* bits);

// get
tb_uint8_t 			tb_bits_get_u1(tb_bits_t* bits);

tb_uint16_t 		tb_bits_get_u16_le(tb_bits_t* bits);
tb_sint16_t 		tb_bits_get_s16_le(tb_bits_t* bits);

tb_uint32_t 		tb_bits_get_u24_le(tb_bits_t* bits);
tb_sint32_t 		tb_bits_get_s24_le(tb_bits_t* bits);

tb_uint32_t 		tb_bits_get_u32_le(tb_bits_t* bits);
tb_sint32_t 		tb_bits_get_s32_le(tb_bits_t* bits);

tb_uint16_t 		tb_bits_get_u16_be(tb_bits_t* bits);
tb_sint16_t 		tb_bits_get_s16_be(tb_bits_t* bits);

tb_uint32_t 		tb_bits_get_u24_be(tb_bits_t* bits);
tb_sint32_t 		tb_bits_get_s24_be(tb_bits_t* bits);

tb_uint32_t 		tb_bits_get_u32_be(tb_bits_t* bits);
tb_sint32_t 		tb_bits_get_s32_be(tb_bits_t* bits);

tb_float_t 			tb_bits_get_float_le(tb_bits_t* bits);
tb_float_t 			tb_bits_get_float_be(tb_bits_t* bits);
tb_float_t 			tb_bits_get_float_ne(tb_bits_t* bits);

tb_float_t 			tb_bits_get_double_le(tb_bits_t* bits);
tb_float_t 			tb_bits_get_double_be(tb_bits_t* bits);
tb_float_t 			tb_bits_get_double_ne(tb_bits_t* bits);

tb_uint32_t 		tb_bits_get_ubits(tb_bits_t* bits, tb_size_t bits_n);
tb_sint32_t 		tb_bits_get_sbits(tb_bits_t* bits, tb_size_t bits_n);

tb_char_t const* 	tb_bits_get_string(tb_bits_t* bits);
tb_size_t 			tb_bits_get_data(tb_bits_t* bits, tb_byte_t* data, tb_size_t size);

// set
void 				tb_bits_set_u16_le(tb_bits_t* bits, tb_uint16_t val);
void 				tb_bits_set_s16_le(tb_bits_t* bits, tb_sint16_t val);

void 				tb_bits_set_u32_le(tb_bits_t* bits, tb_uint32_t val);
void 				tb_bits_set_s32_le(tb_bits_t* bits, tb_sint32_t val);

void 				tb_bits_set_u16_be(tb_bits_t* bits, tb_uint16_t val);
void 				tb_bits_set_s16_be(tb_bits_t* bits, tb_sint16_t val);

void 				tb_bits_set_u32_be(tb_bits_t* bits, tb_uint32_t val);
void 				tb_bits_set_s32_be(tb_bits_t* bits, tb_sint32_t val);

void 				tb_bits_set_ubits(tb_bits_t* bits, tb_uint32_t val, tb_size_t bits_n);
tb_size_t 			tb_bits_set_data(tb_bits_t* bits, tb_byte_t const* data, tb_size_t size);

// peek
tb_uint32_t 		tb_bits_peek_ubits(tb_bits_t* bits, tb_size_t bits_n);
tb_sint32_t 		tb_bits_peek_sbits(tb_bits_t* bits, tb_size_t bits_n);

// pbits
tb_char_t const* 	tb_pbits_get_string(tb_byte_t const* p, tb_size_t size);

// bits inline
static __tplat_inline__ tb_uint8_t tb_bits_get_u8(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	return *(bits->p++);
}
static __tplat_inline__ tb_sint8_t tb_bits_get_s8(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	return *(bits->p++);
}
static __tplat_inline__ tb_uint16_t tb_bits_get_u16_ne(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	// {
	tb_uint16_t val = *((tb_uint16_t*)bits->p);
	bits->p += 2;
	return val;
	// }
}
static __tplat_inline__ tb_sint16_t tb_bits_get_s16_ne(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	// {
	tb_sint16_t val = *((tb_sint16_t*)bits->p);
	bits->p += 2;
	return val;
	// }
}
static __tplat_inline__ tb_uint32_t tb_bits_get_u24_ne(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	// {
	tb_uint32_t val = *((tb_uint32_t*)bits->p) & 0x00ffffff;
	bits->p += 3;
	return val;
	// }
}
static __tplat_inline__ tb_sint32_t tb_bits_get_s24_ne(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	// {
	tb_sint32_t val = *((tb_sint32_t*)bits->p) & 0x80ffffff;
	bits->p += 3;
	return val;
	// }
}
static __tplat_inline__ tb_uint32_t tb_bits_get_u32_ne(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	// {
	tb_uint32_t val = *((tb_uint32_t*)bits->p);
	bits->p += 4;
	return val;
	// }
}
static __tplat_inline__ tb_sint32_t tb_bits_get_s32_ne(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	// {
	tb_sint32_t val = *((tb_sint32_t*)bits->p);
	bits->p += 4;
	return val;
	// }
}
static __tplat_inline__ void tb_bits_set_u8(tb_bits_t* bits, tb_uint8_t val)
{
	TB_ASSERT(!bits->b);
	*(bits->p++) = val;
}
static __tplat_inline__ void tb_bits_set_s8(tb_bits_t* bits, tb_sint8_t val)
{
	TB_ASSERT(!bits->b);
	*(bits->p++) = val;
}
static __tplat_inline__ void tb_bits_set_u16_ne(tb_bits_t* bits, tb_uint16_t val)
{
	TB_ASSERT(!bits->b);
	*((tb_uint16_t*)bits->p) = val;
	bits->p += 2;
}
static __tplat_inline__ void tb_bits_set_s16_ne(tb_bits_t* bits, tb_sint16_t val)
{
	TB_ASSERT(!bits->b);
	*((tb_sint16_t*)bits->p) = val;
	bits->p += 2;
}
static __tplat_inline__ void tb_bits_set_u32_ne(tb_bits_t* bits, tb_uint32_t val)
{
	TB_ASSERT(!bits->b);
	*((tb_uint32_t*)bits->p) = val;
	bits->p += 4;
}
static __tplat_inline__ void tb_bits_set_s32_ne(tb_bits_t* bits, tb_sint32_t val)
{
	TB_ASSERT(!bits->b);
	*((tb_sint32_t*)bits->p) = val;
	bits->p += 4;
}
// pbits inline
static __tplat_inline__ tb_uint8_t tb_pbits_get_u1(tb_byte_t const* p)
{
	return ((*p) >> 7) & 1;
}
static __tplat_inline__ tb_uint8_t tb_pbits_get_u8(tb_byte_t const* p)
{
	return *(p++);
}
static __tplat_inline__ tb_sint8_t tb_pbits_get_s8(tb_byte_t const* p)
{
	return *(p++);
}
static __tplat_inline__ tb_uint16_t tb_pbits_get_u16_le(tb_byte_t const* p)
{
	return (*(p + 1) << 8 | *(p));
}
static __tplat_inline__ tb_sint16_t tb_pbits_get_s16_le(tb_byte_t const* p)
{
	return (*(p + 1) << 8 | *(p));
}
static __tplat_inline__ tb_uint16_t tb_pbits_get_u16_be(tb_byte_t const* p)
{
	return (*(p) << 8 | *(p + 1));
}
static __tplat_inline__ tb_sint16_t tb_pbits_get_s16_be(tb_byte_t const* p)
{
	return (*(p) << 8 | *(p + 1));
}
static __tplat_inline__ tb_uint16_t tb_pbits_get_u16_ne(tb_byte_t const* p)
{
	return *((tb_uint16_t*)p);
}
static __tplat_inline__ tb_sint16_t tb_pbits_get_s16_ne(tb_byte_t const* p)
{
	return *((tb_sint16_t*)p);
}
static __tplat_inline__ tb_uint32_t tb_pbits_get_u24_le(tb_byte_t const* p)
{
	return (*(p + 2) << 16 | *(p + 1) << 8 | *(p));
}
static __tplat_inline__ tb_sint32_t tb_pbits_get_s24_le(tb_byte_t const* p)
{
	return (*(p + 2) << 16 | *(p + 1) << 8 | *(p));
}
static __tplat_inline__ tb_uint32_t tb_pbits_get_u24_be(tb_byte_t const* p)
{
	return (*(p) << 16 | *(p + 1) << 8 | *(p + 2));
}
static __tplat_inline__ tb_sint32_t tb_pbits_get_s24_be(tb_byte_t const* p)
{
	return (*(p) << 16 | *(p + 1) << 8 | *(p + 2));
}
static __tplat_inline__ tb_uint32_t tb_pbits_get_u24_ne(tb_byte_t const* p)
{
	return *((tb_uint32_t*)p) & 0x00ffffff;
}
static __tplat_inline__ tb_sint32_t tb_pbits_get_s24_ne(tb_byte_t const* p)
{
	return *((tb_sint32_t*)p) & 0x80ffffff;
}
static __tplat_inline__ tb_uint32_t tb_pbits_get_u32_le(tb_byte_t const* p)
{
	return (*(p + 3) << 24 | *(p + 2) << 16 | *(p + 1) << 8 | *(p));
}
static __tplat_inline__ tb_sint32_t tb_pbits_get_s32_le(tb_byte_t const* p)
{
	return (*(p + 3) << 24 | *(p + 2) << 16 | *(p + 1) << 8 | *(p));
}
static __tplat_inline__ tb_uint32_t tb_pbits_get_u32_be(tb_byte_t const* p)
{
	return (*(p) << 24 | *(p + 1) << 16 | *(p + 2) << 8 | *(p + 3));
}
static __tplat_inline__ tb_sint32_t tb_pbits_get_s32_be(tb_byte_t const* p)
{
	return (*(p) << 24 | *(p + 1) << 16 | *(p + 2) << 8 | *(p + 3));
}
static __tplat_inline__ tb_uint32_t tb_pbits_get_u32_ne(tb_byte_t const* p)
{
	return *((tb_uint32_t*)p);
}
static __tplat_inline__ tb_sint32_t tb_pbits_get_s32_ne(tb_byte_t const* p)
{
	return *((tb_sint32_t*)p);
}
static __tplat_inline__ tb_float_t tb_pbits_get_float_le(tb_byte_t const* p)
{
	union 
	{
		tb_uint32_t i;
		float 		f;

	} conv;

	conv.i = tb_pbits_get_u32_le(p);
	return (tb_float_t)conv.f;
}
static __tplat_inline__ tb_float_t tb_pbits_get_float_be(tb_byte_t const* p)
{
	union 
	{
		tb_uint32_t i;
		float 		f;

	} conv;

	conv.i = tb_pbits_get_u32_be(p);
	return (tb_float_t)conv.f;
}
static __tplat_inline__ tb_float_t tb_pbits_get_float_ne(tb_byte_t const* p)
{
	union 
	{
		tb_uint32_t i;
		float 		f;

	} conv;

	conv.i = tb_pbits_get_u32_ne(p);
	return (tb_float_t)conv.f;
}

static __tplat_inline__ tb_float_t tb_pbits_get_double_le(tb_byte_t const* p)
{
	union 
	{
		tb_uint32_t i[2];
		double 		f;

	} conv;

#ifdef TPLAT_FLOAT_BIGENDIAN
	conv.i[0] = tb_pbits_get_u32_le(p);
	conv.i[1] = tb_pbits_get_u32_le(p);
#else
	conv.i[1] = tb_pbits_get_u32_le(p);
	conv.i[0] = tb_pbits_get_u32_le(p);
#endif

	return (tb_float_t)conv.f;
}
static __tplat_inline__ tb_float_t tb_pbits_get_double_be(tb_byte_t const* p)
{
	union 
	{
		tb_uint32_t i[2];
		double 		f;

	} conv;

#ifdef TPLAT_FLOAT_BIGENDIAN
	conv.i[0] = tb_pbits_get_u32_be(p);
	conv.i[1] = tb_pbits_get_u32_be(p);
#else
	conv.i[1] = tb_pbits_get_u32_be(p);
	conv.i[0] = tb_pbits_get_u32_be(p);
#endif

	return (tb_float_t)conv.f;
}
static __tplat_inline__ tb_float_t tb_pbits_get_double_ne(tb_byte_t const* p)
{
	union 
	{
		tb_uint32_t i[2];
		double 		f;

	} conv;

#ifdef TPLAT_FLOAT_BIGENDIAN
	conv.i[0] = tb_pbits_get_u32_ne(p);
	conv.i[1] = tb_pbits_get_u32_ne(p);
#else
	conv.i[1] = tb_pbits_get_u32_ne(p);
	conv.i[0] = tb_pbits_get_u32_ne(p);
#endif

	return (tb_float_t)conv.f;
}


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

