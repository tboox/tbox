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
 * @file		bstream.h
 * @ingroup 	stream
 *
 */
#ifndef TB_STREAM_BSTREAM_H
#define TB_STREAM_BSTREAM_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "gstream.h"
#include "../utils/utils.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef TB_WORDS_BIGENDIAN
# 	define tb_bstream_get_u16_ne(bst) 			tb_bstream_get_u16_be(bst)
# 	define tb_bstream_get_s16_ne(bst) 			tb_bstream_get_s16_be(bst)
# 	define tb_bstream_get_u24_ne(bst) 			tb_bstream_get_u24_be(bst)
# 	define tb_bstream_get_s24_ne(bst) 			tb_bstream_get_s24_be(bst)
# 	define tb_bstream_get_u32_ne(bst) 			tb_bstream_get_u32_be(bst)
# 	define tb_bstream_get_s32_ne(bst) 			tb_bstream_get_s32_be(bst)
# 	define tb_bstream_get_u64_ne(bst) 			tb_bstream_get_u64_be(bst)
# 	define tb_bstream_get_s64_ne(bst) 			tb_bstream_get_s64_be(bst)

# 	define tb_bstream_set_u16_ne(bst, val) 		tb_bstream_set_u16_be(bst, val)
# 	define tb_bstream_set_s16_ne(bst, val)		tb_bstream_set_s16_be(bst, val)
# 	define tb_bstream_set_u24_ne(bst, val) 		tb_bstream_set_u24_be(bst, val)
# 	define tb_bstream_set_s24_ne(bst, val)		tb_bstream_set_s24_be(bst, val)
# 	define tb_bstream_set_u32_ne(bst, val)		tb_bstream_set_u32_be(bst, val)
# 	define tb_bstream_set_s32_ne(bst, val) 		tb_bstream_set_s32_be(bst, val)
# 	define tb_bstream_set_u64_ne(bst, val)		tb_bstream_set_u64_be(bst, val)
# 	define tb_bstream_set_s64_ne(bst, val) 		tb_bstream_set_s64_be(bst, val)

#else
# 	define tb_bstream_get_u16_ne(bst) 			tb_bstream_get_u16_le(bst)
# 	define tb_bstream_get_s16_ne(bst) 			tb_bstream_get_s16_le(bst)
# 	define tb_bstream_get_u24_ne(bst) 			tb_bstream_get_u24_le(bst)
# 	define tb_bstream_get_s24_ne(bst) 			tb_bstream_get_s24_le(bst)
# 	define tb_bstream_get_u32_ne(bst) 			tb_bstream_get_u32_le(bst)
# 	define tb_bstream_get_s32_ne(bst) 			tb_bstream_get_s32_le(bst)
# 	define tb_bstream_get_u64_ne(bst) 			tb_bstream_get_u64_le(bst)
# 	define tb_bstream_get_s64_ne(bst) 			tb_bstream_get_s64_le(bst)

# 	define tb_bstream_set_u16_ne(bst, val) 		tb_bstream_set_u16_le(bst, val)
# 	define tb_bstream_set_s16_ne(bst, val)		tb_bstream_set_s16_le(bst, val)
# 	define tb_bstream_set_u24_ne(bst, val) 		tb_bstream_set_u24_le(bst, val)
# 	define tb_bstream_set_s24_ne(bst, val)		tb_bstream_set_s24_le(bst, val)
# 	define tb_bstream_set_u32_ne(bst, val)		tb_bstream_set_u32_le(bst, val)
# 	define tb_bstream_set_s32_ne(bst, val) 		tb_bstream_set_s32_le(bst, val)
# 	define tb_bstream_set_u64_ne(bst, val)		tb_bstream_set_u64_le(bst, val)
# 	define tb_bstream_set_s64_ne(bst, val) 		tb_bstream_set_s64_le(bst, val)

#endif

/* ///////////////////////////////////////////////////////////////////////
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


/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// attach
tb_bstream_t* 		tb_bstream_attach(tb_bstream_t* bst, tb_byte_t* data, tb_size_t size);

// load & save
tb_size_t 			tb_bstream_load(tb_bstream_t* bst, tb_gstream_t* ist);
tb_size_t 			tb_bstream_save(tb_bstream_t* bst, tb_gstream_t* ost);

// modifior
tb_void_t 			tb_bstream_goto(tb_bstream_t* bst, tb_byte_t* data);
tb_void_t 			tb_bstream_sync(tb_bstream_t* bst);

// position
tb_byte_t const* 	tb_bstream_beg(tb_bstream_t* bst);
tb_byte_t const* 	tb_bstream_pos(tb_bstream_t* bst);
tb_byte_t const* 	tb_bstream_end(tb_bstream_t* bst);

// attributes
tb_size_t 			tb_bstream_offset(tb_bstream_t* bst);
tb_size_t 			tb_bstream_size(tb_bstream_t* bst);
tb_size_t 			tb_bstream_left(tb_bstream_t* bst);
tb_size_t 			tb_bstream_left_bits(tb_bstream_t* bst);
tb_bool_t 			tb_bstream_valid(tb_bstream_t* bst);

// skip
tb_void_t 			tb_bstream_skip(tb_bstream_t* bst, tb_size_t size);
tb_void_t 			tb_bstream_skip_bits(tb_bstream_t* bst, tb_size_t nbits);
tb_char_t const* 	tb_bstream_skip_string(tb_bstream_t* bst);

// get
tb_uint8_t 			tb_bstream_get_u1(tb_bstream_t* bst);

tb_uint8_t 			tb_bstream_get_u8(tb_bstream_t* bst);
tb_sint8_t 			tb_bstream_get_s8(tb_bstream_t* bst);

tb_uint16_t 		tb_bstream_get_u16_le(tb_bstream_t* bst);
tb_sint16_t 		tb_bstream_get_s16_le(tb_bstream_t* bst);

tb_uint32_t 		tb_bstream_get_u24_le(tb_bstream_t* bst);
tb_sint32_t 		tb_bstream_get_s24_le(tb_bstream_t* bst);

tb_uint32_t 		tb_bstream_get_u32_le(tb_bstream_t* bst);
tb_sint32_t 		tb_bstream_get_s32_le(tb_bstream_t* bst);

tb_uint64_t 		tb_bstream_get_u64_le(tb_bstream_t* bst);
tb_sint64_t 		tb_bstream_get_s64_le(tb_bstream_t* bst);

tb_uint16_t 		tb_bstream_get_u16_be(tb_bstream_t* bst);
tb_sint16_t 		tb_bstream_get_s16_be(tb_bstream_t* bst);

tb_uint32_t 		tb_bstream_get_u24_be(tb_bstream_t* bst);
tb_sint32_t 		tb_bstream_get_s24_be(tb_bstream_t* bst);

tb_uint32_t 		tb_bstream_get_u32_be(tb_bstream_t* bst);
tb_sint32_t 		tb_bstream_get_s32_be(tb_bstream_t* bst);

tb_uint64_t 		tb_bstream_get_u64_be(tb_bstream_t* bst);
tb_sint64_t 		tb_bstream_get_s64_be(tb_bstream_t* bst);

#ifdef TB_CONFIG_TYPE_FLOAT
tb_double_t 		tb_bstream_get_double_le(tb_bstream_t* bst);
tb_double_t 		tb_bstream_get_double_be(tb_bstream_t* bst);

tb_double_t 		tb_bstream_get_double_ble(tb_bstream_t* bst);
tb_double_t 		tb_bstream_get_double_bbe(tb_bstream_t* bst);
tb_double_t 		tb_bstream_get_double_bne(tb_bstream_t* bst);

tb_double_t 		tb_bstream_get_double_lle(tb_bstream_t* bst);
tb_double_t 		tb_bstream_get_double_lbe(tb_bstream_t* bst);
tb_double_t 		tb_bstream_get_double_lne(tb_bstream_t* bst);

tb_double_t 		tb_bstream_get_double_nle(tb_bstream_t* bst);
tb_double_t 		tb_bstream_get_double_nbe(tb_bstream_t* bst);
tb_double_t 		tb_bstream_get_double_nne(tb_bstream_t* bst);
#endif

tb_uint32_t 		tb_bstream_get_ubits32(tb_bstream_t* bst, tb_size_t nbits);
tb_sint32_t 		tb_bstream_get_sbits32(tb_bstream_t* bst, tb_size_t nbits);

tb_char_t const* 	tb_bstream_get_string(tb_bstream_t* bst);
tb_size_t 			tb_bstream_get_data(tb_bstream_t* bst, tb_byte_t* data, tb_size_t size);

// set
tb_void_t 			tb_bstream_set_u1(tb_bstream_t* bst, tb_uint8_t val);

tb_void_t 			tb_bstream_set_u8(tb_bstream_t* bst, tb_uint8_t val);
tb_void_t 			tb_bstream_set_s8(tb_bstream_t* bst, tb_sint8_t val);

tb_void_t 			tb_bstream_set_u16_le(tb_bstream_t* bst, tb_uint16_t val);
tb_void_t 			tb_bstream_set_s16_le(tb_bstream_t* bst, tb_sint16_t val);

tb_void_t 			tb_bstream_set_u24_le(tb_bstream_t* bst, tb_uint32_t val);
tb_void_t 			tb_bstream_set_s24_le(tb_bstream_t* bst, tb_sint32_t val);

tb_void_t 			tb_bstream_set_u32_le(tb_bstream_t* bst, tb_uint32_t val);
tb_void_t 			tb_bstream_set_s32_le(tb_bstream_t* bst, tb_sint32_t val);

tb_void_t 			tb_bstream_set_u64_le(tb_bstream_t* bst, tb_uint64_t val);
tb_void_t 			tb_bstream_set_s64_le(tb_bstream_t* bst, tb_sint64_t val);

tb_void_t 			tb_bstream_set_u16_be(tb_bstream_t* bst, tb_uint16_t val);
tb_void_t 			tb_bstream_set_s16_be(tb_bstream_t* bst, tb_sint16_t val);

tb_void_t 			tb_bstream_set_u24_be(tb_bstream_t* bst, tb_uint32_t val);
tb_void_t 			tb_bstream_set_s24_be(tb_bstream_t* bst, tb_sint32_t val);

tb_void_t 			tb_bstream_set_u32_be(tb_bstream_t* bst, tb_uint32_t val);
tb_void_t 			tb_bstream_set_s32_be(tb_bstream_t* bst, tb_sint32_t val);

tb_void_t 			tb_bstream_set_u64_be(tb_bstream_t* bst, tb_uint64_t val);
tb_void_t 			tb_bstream_set_s64_be(tb_bstream_t* bst, tb_sint64_t val);

tb_void_t 			tb_bstream_set_ubits32(tb_bstream_t* bst, tb_uint32_t val, tb_size_t nbits);
tb_void_t 			tb_bstream_set_sbits32(tb_bstream_t* bst, tb_sint32_t val, tb_size_t nbits);

tb_size_t 			tb_bstream_set_data(tb_bstream_t* bst, tb_byte_t const* data, tb_size_t size);
tb_char_t*			tb_bstream_set_string(tb_bstream_t* bst, tb_char_t const* s);

#ifdef TB_CONFIG_TYPE_FLOAT
tb_void_t 			tb_bstream_set_double_le(tb_bstream_t* bst, tb_double_t val);
tb_void_t 			tb_bstream_set_double_be(tb_bstream_t* bst, tb_double_t val);

tb_void_t 			tb_bstream_set_double_ble(tb_bstream_t* bst, tb_double_t val);
tb_void_t 			tb_bstream_set_double_bbe(tb_bstream_t* bst, tb_double_t val);
tb_void_t 			tb_bstream_set_double_bne(tb_bstream_t* bst, tb_double_t val);

tb_void_t 			tb_bstream_set_double_lle(tb_bstream_t* bst, tb_double_t val);
tb_void_t 			tb_bstream_set_double_lbe(tb_bstream_t* bst, tb_double_t val);
tb_void_t 			tb_bstream_set_double_lne(tb_bstream_t* bst, tb_double_t val);

tb_void_t 			tb_bstream_set_double_nle(tb_bstream_t* bst, tb_double_t val);
tb_void_t 			tb_bstream_set_double_nbe(tb_bstream_t* bst, tb_double_t val);
tb_void_t 			tb_bstream_set_double_nne(tb_bstream_t* bst, tb_double_t val);
#endif

// peek
tb_uint32_t 		tb_bstream_peek_ubits32(tb_bstream_t* bst, tb_size_t nbits);
tb_sint32_t 		tb_bstream_peek_sbits32(tb_bstream_t* bst, tb_size_t nbits);


#endif

