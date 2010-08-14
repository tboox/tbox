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

// peek
tb_uint32_t 		tb_bits_peek_ubits(tb_bits_t* bits, tb_size_t bits_n);
tb_sint32_t 		tb_bits_peek_sbits(tb_bits_t* bits, tb_size_t bits_n);


/* the swf file format uses 8-bit, 16-bit, 32-bit, 64-bit, 
 * signed, and unsigned integer types. 
 * all integer values are stored in the swf file by using little-endian byte order
 *
 * \note
 * all integer types must be byte-aligned. 
 * that is, the first bit of an integer value must be stored
 * in the first bit of a byte in the swf file.
 */
static __tb_inline__ tb_uint8_t tb_bits_get_u1(tb_bits_t* bits)
{
	tb_uint8_t val = ((*bits->p) >> (7 - bits->b)) & 1;
	bits->b++;
	if (bits->b >= 8) 
	{
		bits->p++;
		bits->b = 0;
	}
	return val;
}
static __tb_inline__ tb_uint8_t tb_bits_get_u8(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	return *(bits->p++);
}

static __tb_inline__ tb_sint8_t tb_bits_get_s8(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	return *(bits->p++);
}

static __tb_inline__ tb_uint16_t tb_bits_get_u16(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
#ifdef TB_WORDS_BIGENDIAN
	tb_uint16_t val = (*(bits->p) | *(bits->p + 1) << 8);
#else
	tb_uint16_t val = *((tb_uint16_t*)bits->p);
#endif
	bits->p += 2;
	return val;
}

static __tb_inline__ tb_sint16_t tb_bits_get_s16(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
#ifdef TB_WORDS_BIGENDIAN
	tb_sint16_t val = (*(bits->p) | *(bits->p + 1) << 8);
#else
	tb_sint16_t val = *((tb_sint16_t*)bits->p);
#endif
	bits->p += 2;
	return val;
}

static __tb_inline__ tb_uint32_t tb_bits_get_u32(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
#ifdef TB_WORDS_BIGENDIAN
	tb_uint32_t val = (*(bits->p) | *(bits->p + 1) << 8 | *(bits->p + 2) << 16 | *(bits->p + 3) << 24);
#else
	tb_uint32_t val = *((tb_uint32_t*)bits->p);
#endif
	bits->p += 4;
	return val;
}

static __tb_inline__ tb_sint32_t tb_bits_get_s32(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
#ifdef TB_WORDS_BIGENDIAN
	tb_sint32_t val = (*(bits->p) | *(bits->p + 1) << 8 | *(bits->p + 2) << 16 | *(bits->p + 3) << 24);
#else
	tb_sint32_t val = *((tb_sint32_t*)bits->p);
#endif
	bits->p += 4;
	return val;
}


static __tb_inline__ tb_uint8_t tb_pbits_get_u1(tb_byte_t const* p)
{
	return ((*p) >> 7) & 1;
}
static __tb_inline__ tb_uint8_t tb_pbits_get_u8(tb_byte_t const* p)
{
	return *(p++);
}

static __tb_inline__ tb_sint8_t tb_pbits_get_s8(tb_byte_t const* p)
{
	return *(p++);
}

static __tb_inline__ tb_uint16_t tb_pbits_get_u16(tb_byte_t const* p)
{
#ifdef TB_WORDS_BIGENDIAN
	return (*(p) | *(p + 1) << 8);
#else
	return *((tb_uint16_t*)p);
#endif
}

static __tb_inline__ tb_sint16_t tb_pbits_get_s16(tb_byte_t const* p)
{
#ifdef TB_WORDS_BIGENDIAN
	return (*(p) | *(p + 1) << 8);
#else
	return *((tb_sint16_t*)p);
#endif
}

static __tb_inline__ tb_uint32_t tb_pbits_get_u32(tb_byte_t const* p)
{
#ifdef TB_WORDS_BIGENDIAN
	return (*(p) | *(p + 1) << 8 | *(p + 2) << 16 | *(p + 3) << 24);
#else
	return *((tb_uint32_t*)p);
#endif
}

static __tb_inline__ tb_sint32_t tb_pbits_get_s32(tb_byte_t const* p)
{
#ifdef TB_WORDS_BIGENDIAN
	return (*(p) | *(p + 1) << 8 | *(p + 2) << 16 | *(p + 3) << 24);
#else
	return *((tb_sint32_t*)p);
#endif
}

static __tb_inline__ tb_char_t const* tb_pbits_get_string(tb_byte_t const* p, tb_size_t size)
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

