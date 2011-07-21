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
 * \file		bstream.h
 *
 */
#ifndef TB_STREAM_BSTREAM_H
#define TB_STREAM_BSTREAM_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "gstream.h"
#include "../utils/utils.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the bits stream type
typedef struct __tb_bstream_t
{
	tb_byte_t* 			p; 	// the pointer to the current position
	tb_size_t 			b; 	// the bit offset < 8
	tb_byte_t const* 	e; 	// the pointer to the end
	tb_size_t 			n; 	// the data size

}tb_bstream_t;

/* /////////////////////////////////////////////////////////
 * macros
 */

#define tb_bstream_get_u16 			tb_bstream_get_u16_ne
#define tb_bstream_get_s16 			tb_bstream_get_s16_ne
#define tb_bstream_get_u24 			tb_bstream_get_u24_ne
#define tb_bstream_get_s24 			tb_bstream_get_s24_ne
#define tb_bstream_get_u32 			tb_bstream_get_u32_ne
#define tb_bstream_get_s32 			tb_bstream_get_s32_ne

#define tb_bstream_set_u16 			tb_bstream_set_u16_ne
#define tb_bstream_set_s16 			tb_bstream_set_s16_ne
#define tb_bstream_set_u24 			tb_bstream_set_u24_ne
#define tb_bstream_set_s24 			tb_bstream_set_s24_ne
#define tb_bstream_set_u32 			tb_bstream_set_u32_ne
#define tb_bstream_set_s32 			tb_bstream_set_s32_ne


/* /////////////////////////////////////////////////////////
 * interfaces
 */

// attach
tb_bstream_t* 		tb_bstream_attach(tb_bstream_t* bst, tb_byte_t* data, tb_size_t size);

// load & save
tb_size_t 			tb_bstream_load(tb_bstream_t* bst, tb_gstream_t* ist);
tb_size_t 			tb_bstream_save(tb_bstream_t* bst, tb_gstream_t* ost);

// modifior
void 				tb_bstream_goto(tb_bstream_t* bst, tb_byte_t* data);
void 				tb_bstream_sync(tb_bstream_t* bst);

// position
tb_byte_t const* 	tb_bstream_beg(tb_bstream_t* bst);
tb_byte_t const* 	tb_bstream_pos(tb_bstream_t* bst);
tb_byte_t const* 	tb_bstream_end(tb_bstream_t* bst);

// size
tb_size_t 			tb_bstream_offset(tb_bstream_t* bst);
tb_size_t 			tb_bstream_size(tb_bstream_t* bst);
tb_size_t 			tb_bstream_left(tb_bstream_t* bst);
tb_size_t 			tb_bstream_left_bits(tb_bstream_t* bst);

// skip
void 				tb_bstream_skip(tb_bstream_t* bst, tb_size_t size);
void 				tb_bstream_skip_bits(tb_bstream_t* bst, tb_size_t nbits);
tb_char_t const* 	tb_bstream_skip_string(tb_bstream_t* bst);

// get
tb_uint8_t 			tb_bstream_get_u1(tb_bstream_t* bst);

tb_uint16_t 		tb_bstream_get_u16_le(tb_bstream_t* bst);
tb_sint16_t 		tb_bstream_get_s16_le(tb_bstream_t* bst);

tb_uint32_t 		tb_bstream_get_u24_le(tb_bstream_t* bst);
tb_sint32_t 		tb_bstream_get_s24_le(tb_bstream_t* bst);

tb_uint32_t 		tb_bstream_get_u32_le(tb_bstream_t* bst);
tb_sint32_t 		tb_bstream_get_s32_le(tb_bstream_t* bst);

tb_uint16_t 		tb_bstream_get_u16_be(tb_bstream_t* bst);
tb_sint16_t 		tb_bstream_get_s16_be(tb_bstream_t* bst);

tb_uint32_t 		tb_bstream_get_u24_be(tb_bstream_t* bst);
tb_sint32_t 		tb_bstream_get_s24_be(tb_bstream_t* bst);

tb_uint32_t 		tb_bstream_get_u32_be(tb_bstream_t* bst);
tb_sint32_t 		tb_bstream_get_s32_be(tb_bstream_t* bst);

#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t 			tb_bstream_get_float_le(tb_bstream_t* bst);
tb_float_t 			tb_bstream_get_float_be(tb_bstream_t* bst);
tb_float_t 			tb_bstream_get_float_ne(tb_bstream_t* bst);

tb_float_t 			tb_bstream_get_double_le(tb_bstream_t* bst);
tb_float_t 			tb_bstream_get_double_be(tb_bstream_t* bst);
tb_float_t 			tb_bstream_get_double_ne(tb_bstream_t* bst);
#endif

tb_uint32_t 		tb_bstream_get_ubits(tb_bstream_t* bst, tb_size_t nbits);
tb_sint32_t 		tb_bstream_get_sbits(tb_bstream_t* bst, tb_size_t nbits);

tb_char_t const* 	tb_bstream_get_string(tb_bstream_t* bst);
tb_size_t 			tb_bstream_get_data(tb_bstream_t* bst, tb_byte_t* data, tb_size_t size);

// set
void 				tb_bstream_set_u1(tb_bstream_t* bst, tb_uint8_t val);

void 				tb_bstream_set_u16_le(tb_bstream_t* bst, tb_uint16_t val);
void 				tb_bstream_set_s16_le(tb_bstream_t* bst, tb_sint16_t val);

void 				tb_bstream_set_u32_le(tb_bstream_t* bst, tb_uint32_t val);
void 				tb_bstream_set_s32_le(tb_bstream_t* bst, tb_sint32_t val);

void 				tb_bstream_set_u16_be(tb_bstream_t* bst, tb_uint16_t val);
void 				tb_bstream_set_s16_be(tb_bstream_t* bst, tb_sint16_t val);

void 				tb_bstream_set_u32_be(tb_bstream_t* bst, tb_uint32_t val);
void 				tb_bstream_set_s32_be(tb_bstream_t* bst, tb_sint32_t val);

void 				tb_bstream_set_ubits(tb_bstream_t* bst, tb_uint32_t val, tb_size_t nbits);
tb_size_t 			tb_bstream_set_data(tb_bstream_t* bst, tb_byte_t const* data, tb_size_t size);
tb_char_t*			tb_bstream_set_string(tb_bstream_t* bst, tb_char_t const* s);

// peek
tb_uint32_t 		tb_bstream_peek_ubits(tb_bstream_t* bst, tb_size_t nbits);
tb_sint32_t 		tb_bstream_peek_sbits(tb_bstream_t* bst, tb_size_t nbits);

// bst inline
static __tb_inline__ tb_uint8_t tb_bstream_get_u8(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	return *(bst->p++);
}
static __tb_inline__ tb_sint8_t tb_bstream_get_s8(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	return *(bst->p++);
}
static __tb_inline__ tb_uint16_t tb_bstream_get_u16_ne(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	tb_uint16_t val = tb_bits_get_u16_ne(bst->p);
	bst->p += 2;
	return val;
}
static __tb_inline__ tb_sint16_t tb_bstream_get_s16_ne(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	tb_sint16_t val = tb_bits_get_s16_ne(bst->p);
	bst->p += 2;
	return val;
}
static __tb_inline__ tb_uint32_t tb_bstream_get_u24_ne(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	tb_uint32_t val = tb_bits_get_u24_ne(bst->p);
	bst->p += 3;
	return val;
}
static __tb_inline__ tb_sint32_t tb_bstream_get_s24_ne(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	tb_sint32_t val = tb_bits_get_s24_ne(bst->p);
	bst->p += 3;
	return val;
}
static __tb_inline__ tb_uint32_t tb_bstream_get_u32_ne(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	tb_uint32_t val = tb_bits_get_u32_ne(bst->p);
	bst->p += 4;
	return val;
}
static __tb_inline__ tb_sint32_t tb_bstream_get_s32_ne(tb_bstream_t* bst)
{
	TB_ASSERT(!bst->b);
	tb_sint32_t val = tb_bits_get_s32_ne(bst->p);
	bst->p += 4;
	return val;
}
static __tb_inline__ void tb_bstream_set_u8(tb_bstream_t* bst, tb_uint8_t val)
{
	TB_ASSERT(!bst->b);
	*(bst->p++) = val;
}
static __tb_inline__ void tb_bstream_set_s8(tb_bstream_t* bst, tb_sint8_t val)
{
	TB_ASSERT(!bst->b);
	*(bst->p++) = val;
}
static __tb_inline__ void tb_bstream_set_u16_ne(tb_bstream_t* bst, tb_uint16_t val)
{
	TB_ASSERT(!bst->b);
	*((tb_uint16_t*)bst->p) = val;
	bst->p += 2;
}
static __tb_inline__ void tb_bstream_set_s16_ne(tb_bstream_t* bst, tb_sint16_t val)
{
	TB_ASSERT(!bst->b);
	*((tb_sint16_t*)bst->p) = val;
	bst->p += 2;
}
static __tb_inline__ void tb_bstream_set_u32_ne(tb_bstream_t* bst, tb_uint32_t val)
{
	TB_ASSERT(!bst->b);
	*((tb_uint32_t*)bst->p) = val;
	bst->p += 4;
}
static __tb_inline__ void tb_bstream_set_s32_ne(tb_bstream_t* bst, tb_sint32_t val)
{
	TB_ASSERT(!bst->b);
	*((tb_sint32_t*)bst->p) = val;
	bst->p += 4;
}

// c plus plus
#ifdef __cplusplus
}
#endif

#endif

