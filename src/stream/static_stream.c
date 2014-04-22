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
 * @file		static_stream.c
 * @ingroup 	stream
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_stream.h"
#include "basic_stream.h"
#include "../asio/asio.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../memory/memory.h"
#include "../string/string.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation 
 */
tb_bool_t tb_static_stream_init(tb_static_stream_t* stream, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(stream && data, tb_false);

	// init
	stream->p 	= data;
	stream->b 	= 0;
	stream->n 	= size;
	stream->e 	= data + size;

	// ok
	return tb_true;
}
tb_void_t tb_static_stream_goto(tb_static_stream_t* stream, tb_byte_t* data)
{
	// check
	tb_assert_and_check_return(stream && data <= stream->e);

	// goto
	stream->b = 0;
	if (data <= stream->e) stream->p = data;
}
tb_void_t tb_static_stream_sync(tb_static_stream_t* stream)
{
	// check
	tb_assert_and_check_return(stream);

	// sync
	if (stream->b) 
	{
		stream->p++;
		stream->b = 0;
	}
}
tb_byte_t const* tb_static_stream_beg(tb_static_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, tb_null);

	// the head
	return stream->e? (stream->e - stream->n) : tb_null;
}
tb_byte_t const* tb_static_stream_pos(tb_static_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream && stream->p <= stream->e, tb_null);

	// sync
	tb_static_stream_sync(stream);

	// the position 
	return stream->p;
}
tb_byte_t const* tb_static_stream_end(tb_static_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, tb_null);

	// the end
	return stream->e;
}
tb_size_t tb_static_stream_size(tb_static_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, 0);

	// the size
	return stream->n;
}
tb_size_t tb_static_stream_offset(tb_static_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, 0);

	// sync
	tb_static_stream_sync(stream);

	// the offset
	return (((stream->p + stream->n) > stream->e)? (stream->p + stream->n - stream->e) : 0);
}
tb_size_t tb_static_stream_left(tb_static_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream && stream->p <= stream->e, 0);

	// sync
	tb_static_stream_sync(stream);

	// the left
	return (stream->e - stream->p);
}
tb_size_t tb_static_stream_left_bits(tb_static_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream, 0);

	// the left bits
	return ((stream->p < stream->e)? (((stream->e - stream->p) << 3) - stream->b) : 0);
}
tb_bool_t tb_static_stream_valid(tb_static_stream_t* stream)
{
	if (!stream) return tb_false;
	if (stream->p && stream->p > stream->e) return tb_false;
	return tb_true;
}
tb_void_t tb_static_stream_skip(tb_static_stream_t* stream, tb_size_t size)
{
	// check
	tb_assert_and_check_return(stream && stream->p <= stream->e);

	// sync
	tb_static_stream_sync(stream);

	// skip
	stream->p += size;
}
tb_void_t tb_static_stream_skip_bits(tb_static_stream_t* stream, tb_size_t nbits)
{
	// check
	tb_assert_and_check_return(stream && stream->p <= stream->e);

	stream->p += (stream->b + nbits) >> 3;
	stream->b = (stream->b + nbits) & 0x07;
}
tb_char_t const* tb_static_stream_skip_cstr(tb_static_stream_t* stream)
{
	return tb_static_stream_read_cstr(stream);
}
tb_uint32_t tb_static_stream_peek_ubits32(tb_static_stream_t* stream, tb_size_t nbits)
{
	// check
	tb_assert_and_check_return_val(stream, 0);

	// no nbits?
	tb_check_return_val(nbits, 0);

	// save status
	tb_byte_t* 	p = stream->p;
	tb_size_t 	b = stream->b;

	// peek value
	tb_uint32_t val = tb_static_stream_read_ubits32(stream, nbits);

	// restore status
	stream->p = p;
	stream->b = b;

	// ok?
	return val;
}
tb_sint32_t tb_static_stream_peek_sbits32(tb_static_stream_t* stream, tb_size_t nbits)
{
	// check
	tb_assert_and_check_return_val(stream, 0);

	// no nbits?
	tb_check_return_val(nbits, 0);

	// save status
	tb_byte_t* 	p = stream->p;
	tb_size_t 	b = stream->b;

	// peek value
	tb_sint32_t val = tb_static_stream_read_sbits32(stream, nbits);

	// restore status
	stream->p = p;
	stream->b = b;

	// ok?
	return val;
}
tb_uint8_t tb_static_stream_read_u1(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream, 0);
	tb_uint8_t val = ((*stream->p) >> (7 - stream->b)) & 1;
	stream->b++;
	if (stream->b >= 8) 
	{
		stream->p++;
		stream->b = 0;
	}
	return val;
}
tb_uint8_t tb_static_stream_read_u8(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	return *(stream->p++);
}
tb_sint8_t tb_static_stream_read_s8(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	return *(stream->p++);
}
tb_uint16_t tb_static_stream_read_u16_be(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_uint16_t val = tb_bits_get_u16_be(stream->p);
	stream->p += 2;
	return val;
	
}
tb_sint16_t tb_static_stream_read_s16_be(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_sint16_t val = tb_bits_get_s16_be(stream->p);
	stream->p += 2;
	return val;
	
}
tb_uint16_t tb_static_stream_read_u16_le(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_uint16_t val = tb_bits_get_u16_le(stream->p);
	stream->p += 2;
	return val;
	
}
tb_sint16_t tb_static_stream_read_s16_le(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_sint16_t val = tb_bits_get_s16_le(stream->p);
	stream->p += 2;
	return val;
	
}
tb_uint32_t tb_static_stream_read_u24_be(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_uint32_t val = tb_bits_get_u24_be(stream->p);
	stream->p += 3;
	return val;
	
}
tb_sint32_t tb_static_stream_read_s24_be(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_sint32_t val = tb_bits_get_s24_be(stream->p);
	stream->p += 3;
	return val;
	
}
tb_uint32_t tb_static_stream_read_u32_be(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_uint32_t val = tb_bits_get_u32_be(stream->p);;
	stream->p += 4;
	return val;
	
}
tb_sint32_t tb_static_stream_read_s32_be(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_sint32_t val = tb_bits_get_s32_be(stream->p);
	stream->p += 4;
	return val;
	
}
tb_uint64_t tb_static_stream_read_u64_be(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_uint64_t val = tb_bits_get_u64_be(stream->p);;
	stream->p += 8;
	return val;
	
}
tb_sint64_t tb_static_stream_read_s64_be(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_sint64_t val = tb_bits_get_s64_be(stream->p);
	stream->p += 8;
	return val;
	
}
tb_uint32_t tb_static_stream_read_u24_le(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_uint32_t val = tb_bits_get_u24_le(stream->p);
	stream->p += 3;
	return val;
}
tb_sint32_t tb_static_stream_read_s24_le(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_sint32_t val = tb_bits_get_s24_le(stream->p);
	stream->p += 3;
	return val;
	
}
tb_uint32_t tb_static_stream_read_u32_le(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_uint32_t val = tb_bits_get_u32_le(stream->p);
	stream->p += 4;
	return val;
	
}
tb_sint32_t tb_static_stream_read_s32_le(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_sint32_t val = tb_bits_get_s32_le(stream->p);
	stream->p += 4;
	return val;
	
}
tb_uint64_t tb_static_stream_read_u64_le(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_uint64_t val = tb_bits_get_u64_le(stream->p);
	stream->p += 8;
	return val;
	
}
tb_sint64_t tb_static_stream_read_s64_le(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_sint64_t val = tb_bits_get_s64_le(stream->p);
	stream->p += 8;
	return val;
	
}
tb_uint32_t tb_static_stream_read_ubits32(tb_static_stream_t* stream, tb_size_t nbits)
{
	if (!nbits || !stream) return 0;

	tb_uint32_t val = tb_bits_get_ubits32(stream->p, stream->b, nbits);
	stream->p += (stream->b + nbits) >> 3;
	stream->b = (stream->b + nbits) & 0x07;

	return val;
}
tb_sint32_t tb_static_stream_read_sbits32(tb_static_stream_t* stream, tb_size_t nbits)
{
	if (!nbits || !stream) return 0;

	tb_sint32_t val = tb_bits_get_sbits32(stream->p, stream->b, nbits);
	stream->p += (stream->b + nbits) >> 3;
	stream->b = (stream->b + nbits) & 0x07;

	return val;
}
tb_char_t const* tb_static_stream_read_cstr(tb_static_stream_t* stream)
{
	// check
	tb_assert_and_check_return_val(stream && stream->p <= stream->e, tb_null);

	// sync
	tb_static_stream_sync(stream);

	// find '\0'
	tb_byte_t const* p = stream->p;
	while (*p && p < stream->e) p++;

	// is string with '\0' ?
	if ((*p)) return tb_null;

	tb_char_t const* s = (tb_char_t const*)stream->p;
	stream->p += p - stream->p + 1;

	return s;
}
tb_size_t tb_static_stream_read_data(tb_static_stream_t* stream, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(stream->e >= stream->p, 0);

	// sync
	tb_static_stream_sync(stream);
	
	// the need
	tb_size_t need = size;
	if (stream->e - stream->p < need) need = stream->e - stream->p;

	// copy data
	if (need)
	{
		tb_memcpy(data, stream->p, need);
		stream->p += need;
	}

	// ok?
	return need;
}
tb_void_t tb_static_stream_writ_u1(tb_static_stream_t* stream, tb_uint8_t val)
{
	// check
	tb_assert_and_check_return(stream);

	// set
	*(stream->p) &= ~(0x1 << (7 - stream->b));
	*(stream->p) |= ((val & 0x1) << (7 - stream->b));

	// next
	stream->b++;
	if (stream->b >= 8) 
	{
		stream->p++;
		stream->b = 0;
	}
}
tb_void_t tb_static_stream_writ_u8(tb_static_stream_t* stream, tb_uint8_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	*(stream->p++) = val;
}
tb_void_t tb_static_stream_writ_s8(tb_static_stream_t* stream, tb_sint8_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	*(stream->p++) = val;
}
tb_void_t tb_static_stream_writ_u16_le(tb_static_stream_t* stream, tb_uint16_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_u16_le(stream->p, val);
	stream->p += 2;
}
tb_void_t tb_static_stream_writ_s16_le(tb_static_stream_t* stream, tb_sint16_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_s16_le(stream->p, val);
	stream->p += 2;
}
tb_void_t tb_static_stream_writ_u32_le(tb_static_stream_t* stream, tb_uint32_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_u32_le(stream->p, val);
	stream->p += 4;
}
tb_void_t tb_static_stream_writ_s32_le(tb_static_stream_t* stream, tb_sint32_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_s32_le(stream->p, val);
	stream->p += 4;
}
tb_void_t tb_static_stream_writ_u64_le(tb_static_stream_t* stream, tb_uint64_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_u64_le(stream->p, val);
	stream->p += 8;
}
tb_void_t tb_static_stream_writ_s64_le(tb_static_stream_t* stream, tb_sint64_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_s64_le(stream->p, val);
	stream->p += 8;
}
tb_void_t tb_static_stream_writ_u16_be(tb_static_stream_t* stream, tb_uint16_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_u16_be(stream->p, val);
	stream->p += 2;
}
tb_void_t tb_static_stream_writ_s16_be(tb_static_stream_t* stream, tb_sint16_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_s16_be(stream->p, val);
	stream->p += 2;
}
tb_void_t tb_static_stream_writ_u24_be(tb_static_stream_t* stream, tb_uint32_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_u24_be(stream->p, val);
	stream->p += 3;
}
tb_void_t tb_static_stream_writ_s24_be(tb_static_stream_t* stream, tb_sint32_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_s24_be(stream->p, val);
	stream->p += 3;
}
tb_void_t tb_static_stream_writ_u32_be(tb_static_stream_t* stream, tb_uint32_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_u32_be(stream->p, val);
	stream->p += 4;
}
tb_void_t tb_static_stream_writ_s32_be(tb_static_stream_t* stream, tb_sint32_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_s32_be(stream->p, val);
	stream->p += 4;
}
tb_void_t tb_static_stream_writ_u64_be(tb_static_stream_t* stream, tb_uint64_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_u64_be(stream->p, val);
	stream->p += 8;
}
tb_void_t tb_static_stream_writ_s64_be(tb_static_stream_t* stream, tb_sint64_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_s64_be(stream->p, val);
	stream->p += 8;
}
tb_void_t tb_static_stream_writ_ubits32(tb_static_stream_t* stream, tb_uint32_t val, tb_size_t nbits)
{
	// check
	tb_assert_and_check_return(stream);

	// no nbits?
	tb_check_return(nbits);

	// set bits
	tb_bits_set_ubits32(stream->p, stream->b, val, nbits);
	stream->p += (stream->b + nbits) >> 3;
	stream->b = (stream->b + nbits) & 0x07;
}

tb_void_t tb_static_stream_writ_sbits32(tb_static_stream_t* stream, tb_sint32_t val, tb_size_t nbits)
{
	// check
	tb_assert_and_check_return(stream);

	// no nbits?
	tb_check_return(nbits);

	// set bits
	tb_bits_set_sbits32(stream->p, stream->b, val, nbits);
	stream->p += (stream->b + nbits) >> 3;
	stream->b = (stream->b + nbits) & 0x07;
}
tb_size_t tb_static_stream_writ_data(tb_static_stream_t* stream, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(stream && stream->p && stream->e >= stream->p && data, 0);

	// no size?
	tb_check_return_val(size, 0);

	// sync
	tb_static_stream_sync(stream);

	// set data
	tb_size_t set_n = size;
	if (stream->e - stream->p < set_n) set_n = stream->e - stream->p;
	if (set_n)
	{
		tb_memcpy(stream->p, data, set_n);
		stream->p += set_n;
	}
	return (set_n > 0? set_n : 0);
}

tb_char_t* tb_static_stream_writ_cstr(tb_static_stream_t* stream, tb_char_t const* s)
{
	// check
	tb_assert_and_check_return_val(stream && stream->p && stream->e >= stream->p && s, tb_null);

	// sync
	tb_static_stream_sync(stream);

	// set string
	tb_char_t* 			b = (tb_char_t*)stream->p;
	tb_char_t* 			p = (tb_char_t*)stream->p;
	tb_char_t const* 	e = (tb_char_t const*)stream->e - 1;
	while (*s && p < e) *p++ = *s++;
	*p++ = '\0';
	stream->p = (tb_byte_t*)p;

	// ok?
	return b;
}
#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t tb_static_stream_read_float_le(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_float_t val = tb_bits_get_float_le(stream->p);
	stream->p += 4;
	return val;
}
tb_float_t tb_static_stream_read_float_be(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_float_t val = tb_bits_get_float_be(stream->p);
	stream->p += 4;
	return val;
}
tb_float_t tb_static_stream_read_float_ne(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_float_t val = tb_bits_get_float_ne(stream->p);
	stream->p += 4;
	return val;
}
tb_double_t tb_static_stream_read_double_ble(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_double_t val = tb_bits_get_double_ble(stream->p);
	stream->p += 8;
	return val;
}
tb_double_t tb_static_stream_read_double_bbe(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_double_t val = tb_bits_get_double_bbe(stream->p);
	stream->p += 8;
	return val;
}
tb_double_t tb_static_stream_read_double_bne(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_double_t val = tb_bits_get_double_bne(stream->p);
	stream->p += 8;
	return val;
}
tb_double_t tb_static_stream_read_double_lle(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_double_t val = tb_bits_get_double_lle(stream->p);
	stream->p += 8;
	return val;
}
tb_double_t tb_static_stream_read_double_lbe(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_double_t val = tb_bits_get_double_lbe(stream->p);
	stream->p += 8;
	return val;
}
tb_double_t tb_static_stream_read_double_lne(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_double_t val = tb_bits_get_double_lne(stream->p);
	stream->p += 8;
	return val;
}

tb_double_t tb_static_stream_read_double_nle(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_double_t val = tb_bits_get_double_nle(stream->p);
	stream->p += 8;
	return val;
}
tb_double_t tb_static_stream_read_double_nbe(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_double_t val = tb_bits_get_double_nbe(stream->p);
	stream->p += 8;
	return val;
}
tb_double_t tb_static_stream_read_double_nne(tb_static_stream_t* stream)
{
	tb_assert_and_check_return_val(stream && !stream->b, 0);
	tb_double_t val = tb_bits_get_double_nne(stream->p);
	stream->p += 8;
	return val;
}
tb_void_t tb_static_stream_writ_float_le(tb_static_stream_t* stream, tb_float_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_float_le(stream->p, val);
	stream->p += 4;
}
tb_void_t tb_static_stream_writ_float_be(tb_static_stream_t* stream, tb_float_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_float_be(stream->p, val);
	stream->p += 4;
}
tb_void_t tb_static_stream_writ_float_ne(tb_static_stream_t* stream, tb_float_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_float_ne(stream->p, val);
	stream->p += 4;
}
tb_void_t tb_static_stream_writ_double_ble(tb_static_stream_t* stream, tb_double_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_double_ble(stream->p, val);
	stream->p += 8;
}
tb_void_t tb_static_stream_writ_double_bbe(tb_static_stream_t* stream, tb_double_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_double_bbe(stream->p, val);
	stream->p += 8;
}
tb_void_t tb_static_stream_writ_double_bne(tb_static_stream_t* stream, tb_double_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_double_bne(stream->p, val);
	stream->p += 8;
}
tb_void_t tb_static_stream_writ_double_lle(tb_static_stream_t* stream, tb_double_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_double_lle(stream->p, val);
	stream->p += 8;
}
tb_void_t tb_static_stream_writ_double_lbe(tb_static_stream_t* stream, tb_double_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_double_lbe(stream->p, val);
	stream->p += 8;
}
tb_void_t tb_static_stream_writ_double_lne(tb_static_stream_t* stream, tb_double_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_double_lne(stream->p, val);
	stream->p += 8;
}
tb_void_t tb_static_stream_writ_double_nle(tb_static_stream_t* stream, tb_double_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_double_nle(stream->p, val);
	stream->p += 8;
}
tb_void_t tb_static_stream_writ_double_nbe(tb_static_stream_t* stream, tb_double_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_double_nbe(stream->p, val);
	stream->p += 8;
}
tb_void_t tb_static_stream_writ_double_nne(tb_static_stream_t* stream, tb_double_t val)
{
	tb_assert_and_check_return(stream && !stream->b);
	tb_bits_set_double_nne(stream->p, val);
	stream->p += 8;
}
#endif
