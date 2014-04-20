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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		static_stream.h
 * @ingroup 	stream
 *
 */
#ifndef TB_STREAM_STATIC_STREAM_H
#define TB_STREAM_STATIC_STREAM_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "basic_stream.h"
#include "../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef TB_WORDS_BIGENDIAN
# 	define tb_static_stream_get_u16_ne(sstream) 			tb_static_stream_get_u16_be(sstream)
# 	define tb_static_stream_get_s16_ne(sstream) 			tb_static_stream_get_s16_be(sstream)
# 	define tb_static_stream_get_u24_ne(sstream) 			tb_static_stream_get_u24_be(sstream)
# 	define tb_static_stream_get_s24_ne(sstream) 			tb_static_stream_get_s24_be(sstream)
# 	define tb_static_stream_get_u32_ne(sstream) 			tb_static_stream_get_u32_be(sstream)
# 	define tb_static_stream_get_s32_ne(sstream) 			tb_static_stream_get_s32_be(sstream)
# 	define tb_static_stream_get_u64_ne(sstream) 			tb_static_stream_get_u64_be(sstream)
# 	define tb_static_stream_get_s64_ne(sstream) 			tb_static_stream_get_s64_be(sstream)

# 	define tb_static_stream_set_u16_ne(sstream, val) 		tb_static_stream_set_u16_be(sstream, val)
# 	define tb_static_stream_set_s16_ne(sstream, val)		tb_static_stream_set_s16_be(sstream, val)
# 	define tb_static_stream_set_u24_ne(sstream, val) 		tb_static_stream_set_u24_be(sstream, val)
# 	define tb_static_stream_set_s24_ne(sstream, val)		tb_static_stream_set_s24_be(sstream, val)
# 	define tb_static_stream_set_u32_ne(sstream, val)		tb_static_stream_set_u32_be(sstream, val)
# 	define tb_static_stream_set_s32_ne(sstream, val) 		tb_static_stream_set_s32_be(sstream, val)
# 	define tb_static_stream_set_u64_ne(sstream, val)		tb_static_stream_set_u64_be(sstream, val)
# 	define tb_static_stream_set_s64_ne(sstream, val) 		tb_static_stream_set_s64_be(sstream, val)

#else
# 	define tb_static_stream_get_u16_ne(sstream) 			tb_static_stream_get_u16_le(sstream)
# 	define tb_static_stream_get_s16_ne(sstream) 			tb_static_stream_get_s16_le(sstream)
# 	define tb_static_stream_get_u24_ne(sstream) 			tb_static_stream_get_u24_le(sstream)
# 	define tb_static_stream_get_s24_ne(sstream) 			tb_static_stream_get_s24_le(sstream)
# 	define tb_static_stream_get_u32_ne(sstream) 			tb_static_stream_get_u32_le(sstream)
# 	define tb_static_stream_get_s32_ne(sstream) 			tb_static_stream_get_s32_le(sstream)
# 	define tb_static_stream_get_u64_ne(sstream) 			tb_static_stream_get_u64_le(sstream)
# 	define tb_static_stream_get_s64_ne(sstream) 			tb_static_stream_get_s64_le(sstream)

# 	define tb_static_stream_set_u16_ne(sstream, val) 		tb_static_stream_set_u16_le(sstream, val)
# 	define tb_static_stream_set_s16_ne(sstream, val)		tb_static_stream_set_s16_le(sstream, val)
# 	define tb_static_stream_set_u24_ne(sstream, val) 		tb_static_stream_set_u24_le(sstream, val)
# 	define tb_static_stream_set_s24_ne(sstream, val)		tb_static_stream_set_s24_le(sstream, val)
# 	define tb_static_stream_set_u32_ne(sstream, val)		tb_static_stream_set_u32_le(sstream, val)
# 	define tb_static_stream_set_s32_ne(sstream, val) 		tb_static_stream_set_s32_le(sstream, val)
# 	define tb_static_stream_set_u64_ne(sstream, val)		tb_static_stream_set_u64_le(sstream, val)
# 	define tb_static_stream_set_s64_ne(sstream, val) 		tb_static_stream_set_s64_le(sstream, val)

#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the bits stream type
typedef struct __tb_static_stream_t
{
	tb_byte_t* 			p; 	// the pointer to the current position
	tb_size_t 			b; 	// the bit offset < 8
	tb_byte_t* 			e; 	// the pointer to the end
	tb_size_t 			n; 	// the data size

}tb_static_stream_t;


/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init
tb_bool_t 			tb_static_stream_init(tb_static_stream_t* sstream, tb_byte_t* data, tb_size_t size);

// modifior
tb_void_t 			tb_static_stream_goto(tb_static_stream_t* sstream, tb_byte_t* data);
tb_void_t 			tb_static_stream_sync(tb_static_stream_t* sstream);

// position
tb_byte_t const* 	tb_static_stream_beg(tb_static_stream_t* sstream);
tb_byte_t const* 	tb_static_stream_pos(tb_static_stream_t* sstream);
tb_byte_t const* 	tb_static_stream_end(tb_static_stream_t* sstream);

// attributes
tb_size_t 			tb_static_stream_offset(tb_static_stream_t* sstream);
tb_size_t 			tb_static_stream_size(tb_static_stream_t* sstream);
tb_size_t 			tb_static_stream_left(tb_static_stream_t* sstream);
tb_size_t 			tb_static_stream_left_bits(tb_static_stream_t* sstream);
tb_bool_t 			tb_static_stream_valid(tb_static_stream_t* sstream);

// skip
tb_void_t 			tb_static_stream_skip(tb_static_stream_t* sstream, tb_size_t size);
tb_void_t 			tb_static_stream_skip_bits(tb_static_stream_t* sstream, tb_size_t nbits);
tb_char_t const* 	tb_static_stream_skip_string(tb_static_stream_t* sstream);

// get
tb_uint8_t 			tb_static_stream_get_u1(tb_static_stream_t* sstream);

tb_uint8_t 			tb_static_stream_get_u8(tb_static_stream_t* sstream);
tb_sint8_t 			tb_static_stream_get_s8(tb_static_stream_t* sstream);

tb_uint16_t 		tb_static_stream_get_u16_le(tb_static_stream_t* sstream);
tb_sint16_t 		tb_static_stream_get_s16_le(tb_static_stream_t* sstream);

tb_uint32_t 		tb_static_stream_get_u24_le(tb_static_stream_t* sstream);
tb_sint32_t 		tb_static_stream_get_s24_le(tb_static_stream_t* sstream);

tb_uint32_t 		tb_static_stream_get_u32_le(tb_static_stream_t* sstream);
tb_sint32_t 		tb_static_stream_get_s32_le(tb_static_stream_t* sstream);

tb_uint64_t 		tb_static_stream_get_u64_le(tb_static_stream_t* sstream);
tb_sint64_t 		tb_static_stream_get_s64_le(tb_static_stream_t* sstream);

tb_uint16_t 		tb_static_stream_get_u16_be(tb_static_stream_t* sstream);
tb_sint16_t 		tb_static_stream_get_s16_be(tb_static_stream_t* sstream);

tb_uint32_t 		tb_static_stream_get_u24_be(tb_static_stream_t* sstream);
tb_sint32_t 		tb_static_stream_get_s24_be(tb_static_stream_t* sstream);

tb_uint32_t 		tb_static_stream_get_u32_be(tb_static_stream_t* sstream);
tb_sint32_t 		tb_static_stream_get_s32_be(tb_static_stream_t* sstream);

tb_uint64_t 		tb_static_stream_get_u64_be(tb_static_stream_t* sstream);
tb_sint64_t 		tb_static_stream_get_s64_be(tb_static_stream_t* sstream);

#ifdef TB_CONFIG_TYPE_FLOAT
tb_double_t 		tb_static_stream_get_double_le(tb_static_stream_t* sstream);
tb_double_t 		tb_static_stream_get_double_be(tb_static_stream_t* sstream);

tb_double_t 		tb_static_stream_get_double_ble(tb_static_stream_t* sstream);
tb_double_t 		tb_static_stream_get_double_bbe(tb_static_stream_t* sstream);
tb_double_t 		tb_static_stream_get_double_bne(tb_static_stream_t* sstream);

tb_double_t 		tb_static_stream_get_double_lle(tb_static_stream_t* sstream);
tb_double_t 		tb_static_stream_get_double_lbe(tb_static_stream_t* sstream);
tb_double_t 		tb_static_stream_get_double_lne(tb_static_stream_t* sstream);

tb_double_t 		tb_static_stream_get_double_nle(tb_static_stream_t* sstream);
tb_double_t 		tb_static_stream_get_double_nbe(tb_static_stream_t* sstream);
tb_double_t 		tb_static_stream_get_double_nne(tb_static_stream_t* sstream);
#endif

tb_uint32_t 		tb_static_stream_get_ubits32(tb_static_stream_t* sstream, tb_size_t nbits);
tb_sint32_t 		tb_static_stream_get_sbits32(tb_static_stream_t* sstream, tb_size_t nbits);

tb_char_t const* 	tb_static_stream_get_string(tb_static_stream_t* sstream);
tb_size_t 			tb_static_stream_get_data(tb_static_stream_t* sstream, tb_byte_t* data, tb_size_t size);

// set
tb_void_t 			tb_static_stream_set_u1(tb_static_stream_t* sstream, tb_uint8_t val);

tb_void_t 			tb_static_stream_set_u8(tb_static_stream_t* sstream, tb_uint8_t val);
tb_void_t 			tb_static_stream_set_s8(tb_static_stream_t* sstream, tb_sint8_t val);

tb_void_t 			tb_static_stream_set_u16_le(tb_static_stream_t* sstream, tb_uint16_t val);
tb_void_t 			tb_static_stream_set_s16_le(tb_static_stream_t* sstream, tb_sint16_t val);

tb_void_t 			tb_static_stream_set_u24_le(tb_static_stream_t* sstream, tb_uint32_t val);
tb_void_t 			tb_static_stream_set_s24_le(tb_static_stream_t* sstream, tb_sint32_t val);

tb_void_t 			tb_static_stream_set_u32_le(tb_static_stream_t* sstream, tb_uint32_t val);
tb_void_t 			tb_static_stream_set_s32_le(tb_static_stream_t* sstream, tb_sint32_t val);

tb_void_t 			tb_static_stream_set_u64_le(tb_static_stream_t* sstream, tb_uint64_t val);
tb_void_t 			tb_static_stream_set_s64_le(tb_static_stream_t* sstream, tb_sint64_t val);

tb_void_t 			tb_static_stream_set_u16_be(tb_static_stream_t* sstream, tb_uint16_t val);
tb_void_t 			tb_static_stream_set_s16_be(tb_static_stream_t* sstream, tb_sint16_t val);

tb_void_t 			tb_static_stream_set_u24_be(tb_static_stream_t* sstream, tb_uint32_t val);
tb_void_t 			tb_static_stream_set_s24_be(tb_static_stream_t* sstream, tb_sint32_t val);

tb_void_t 			tb_static_stream_set_u32_be(tb_static_stream_t* sstream, tb_uint32_t val);
tb_void_t 			tb_static_stream_set_s32_be(tb_static_stream_t* sstream, tb_sint32_t val);

tb_void_t 			tb_static_stream_set_u64_be(tb_static_stream_t* sstream, tb_uint64_t val);
tb_void_t 			tb_static_stream_set_s64_be(tb_static_stream_t* sstream, tb_sint64_t val);

tb_void_t 			tb_static_stream_set_ubits32(tb_static_stream_t* sstream, tb_uint32_t val, tb_size_t nbits);
tb_void_t 			tb_static_stream_set_sbits32(tb_static_stream_t* sstream, tb_sint32_t val, tb_size_t nbits);

tb_size_t 			tb_static_stream_set_data(tb_static_stream_t* sstream, tb_byte_t const* data, tb_size_t size);
tb_char_t*			tb_static_stream_set_string(tb_static_stream_t* sstream, tb_char_t const* s);

#ifdef TB_CONFIG_TYPE_FLOAT
tb_void_t 			tb_static_stream_set_double_le(tb_static_stream_t* sstream, tb_double_t val);
tb_void_t 			tb_static_stream_set_double_be(tb_static_stream_t* sstream, tb_double_t val);

tb_void_t 			tb_static_stream_set_double_ble(tb_static_stream_t* sstream, tb_double_t val);
tb_void_t 			tb_static_stream_set_double_bbe(tb_static_stream_t* sstream, tb_double_t val);
tb_void_t 			tb_static_stream_set_double_bne(tb_static_stream_t* sstream, tb_double_t val);

tb_void_t 			tb_static_stream_set_double_lle(tb_static_stream_t* sstream, tb_double_t val);
tb_void_t 			tb_static_stream_set_double_lbe(tb_static_stream_t* sstream, tb_double_t val);
tb_void_t 			tb_static_stream_set_double_lne(tb_static_stream_t* sstream, tb_double_t val);

tb_void_t 			tb_static_stream_set_double_nle(tb_static_stream_t* sstream, tb_double_t val);
tb_void_t 			tb_static_stream_set_double_nbe(tb_static_stream_t* sstream, tb_double_t val);
tb_void_t 			tb_static_stream_set_double_nne(tb_static_stream_t* sstream, tb_double_t val);
#endif

// peek
tb_uint32_t 		tb_static_stream_peek_ubits32(tb_static_stream_t* sstream, tb_size_t nbits);
tb_sint32_t 		tb_static_stream_peek_sbits32(tb_static_stream_t* sstream, tb_size_t nbits);


#endif

