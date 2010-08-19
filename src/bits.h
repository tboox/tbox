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
	tb_byte_t const* 	p; 	// the pointer to the current position
	tb_size_t 			b; 	// the bit offset < 8
	tb_byte_t const* 	e; 	// the pointer to the end

}tb_bits_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init
void 				tb_bits_attach(tb_bits_t* bits, tb_byte_t const* data, tb_size_t size);

// modifior
void 				tb_bits_goto(tb_bits_t* bits, tb_byte_t const* data);
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
tb_char_t const* 	tb_bits_get_string(tb_bits_t* bits);
tb_uint32_t 		tb_bits_get_ubits(tb_bits_t* bits, tb_size_t bits_n);
tb_sint32_t 		tb_bits_get_sbits(tb_bits_t* bits, tb_size_t bits_n);

tb_uint8_t 			tb_bits_get_u1(tb_bits_t* bits);

tb_uint8_t 			tb_bits_get_u8(tb_bits_t* bits);
tb_sint8_t 			tb_bits_get_s8(tb_bits_t* bits);

tb_uint16_t 		tb_bits_get_u16(tb_bits_t* bits);
tb_sint16_t 		tb_bits_get_s16(tb_bits_t* bits);

tb_uint32_t 		tb_bits_get_u32(tb_bits_t* bits);
tb_sint32_t 		tb_bits_get_s32(tb_bits_t* bits);

tb_uint16_t 		tb_bits_get_u16_be(tb_bits_t* bits);
tb_sint16_t 		tb_bits_get_s16_be(tb_bits_t* bits);

tb_uint32_t 		tb_bits_get_u32_be(tb_bits_t* bits);
tb_sint32_t 		tb_bits_get_s32_be(tb_bits_t* bits);

tb_uint16_t 		tb_bits_get_u16_le(tb_bits_t* bits);
tb_sint16_t 		tb_bits_get_s16_le(tb_bits_t* bits);

tb_uint32_t 		tb_bits_get_u32_le(tb_bits_t* bits);
tb_sint32_t 		tb_bits_get_s32_le(tb_bits_t* bits);

// peek
tb_uint32_t 		tb_bits_peek_ubits(tb_bits_t* bits, tb_size_t bits_n);
tb_sint32_t 		tb_bits_peek_sbits(tb_bits_t* bits, tb_size_t bits_n);

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

static __tplat_inline__ tb_uint16_t tb_pbits_get_u16(tb_byte_t const* p)
{
#ifdef TPLAT_WORDS_BIGENDIAN
	return (*(p) | *(p + 1) << 8);
#else
	return *((tb_uint16_t*)p);
#endif
}

static __tplat_inline__ tb_sint16_t tb_pbits_get_s16(tb_byte_t const* p)
{
#ifdef TPLAT_WORDS_BIGENDIAN
	return (*(p) | *(p + 1) << 8);
#else
	return *((tb_sint16_t*)p);
#endif
}

static __tplat_inline__ tb_uint32_t tb_pbits_get_u32(tb_byte_t const* p)
{
#ifdef TPLAT_WORDS_BIGENDIAN
	return (*(p) | *(p + 1) << 8 | *(p + 2) << 16 | *(p + 3) << 24);
#else
	return *((tb_uint32_t*)p);
#endif
}

static __tplat_inline__ tb_sint32_t tb_pbits_get_s32(tb_byte_t const* p)
{
#ifdef TPLAT_WORDS_BIGENDIAN
	return (*(p) | *(p + 1) << 8 | *(p + 2) << 16 | *(p + 3) << 24);
#else
	return *((tb_sint32_t*)p);
#endif
}
static __tplat_inline__ tb_uint16_t tb_pbits_get_u16_be(tb_byte_t const* p)
{
	return (*(p) << 8 | *(p + 1));
}
static __tplat_inline__ tb_sint16_t tb_pbits_get_s16_be(tb_byte_t const* p)
{
	return (*(p) << 8 | *(p + 1));
}
static __tplat_inline__ tb_uint16_t tb_pbits_get_u16_le(tb_byte_t const* p)
{
	return (*(p + 1) << 8 | *(p));
}
static __tplat_inline__ tb_sint16_t tb_pbits_get_s16_le(tb_byte_t const* p)
{
	return (*(p + 1) << 8 | *(p));
}
static __tplat_inline__ tb_uint32_t tb_pbits_get_u32_be(tb_byte_t const* p)
{
	return (*(p) << 24 | *(p + 1) << 16 | *(p + 2) << 8 | *(p + 3));
}
static __tplat_inline__ tb_sint32_t tb_pbits_get_s32_be(tb_byte_t const* p)
{
	return (*(p) << 24 | *(p + 1) << 16 | *(p + 2) << 8 | *(p + 3));
}
static __tplat_inline__ tb_uint32_t tb_pbits_get_u32_le(tb_byte_t const* p)
{
	return (*(p + 3) << 24 | *(p + 2) << 16 | *(p + 1) << 8 | *(p));
}
static __tplat_inline__ tb_sint32_t tb_pbits_get_s32_le(tb_byte_t const* p)
{
	return (*(p + 3) << 24 | *(p + 2) << 16 | *(p + 1) << 8 | *(p));
}

static __tplat_inline__ tb_char_t const* tb_pbits_get_string(tb_byte_t const* p, tb_size_t size)
{
	TB_ASSERT(p && size);
	tb_char_t const* s = (tb_char_t const*)p;

	// find '\0'
	while (*p && size--) p++;

	// is string with '\0' ?
	if ((*p)) return TB_NULL;
	return s;
}


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

