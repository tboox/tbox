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
 * init 
 */
tb_bool_t tb_static_stream_init(tb_static_stream_t* sstream, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(sstream && data, tb_false);

	// init
	sstream->p 	= data;
	sstream->b 	= 0;
	sstream->n 	= size;
	sstream->e 	= data + size;

	// ok
	return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * modifiors
 */
tb_void_t tb_static_stream_goto(tb_static_stream_t* sstream, tb_byte_t* data)
{
	// check
	tb_assert_and_check_return(sstream && data <= sstream->e);

	// goto
	sstream->b = 0;
	if (data <= sstream->e) sstream->p = data;
}
tb_void_t tb_static_stream_sync(tb_static_stream_t* sstream)
{
	// check
	tb_assert_and_check_return(sstream);

	// sync
	if (sstream->b) 
	{
		sstream->p++;
		sstream->b = 0;
	}
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * position
 */
tb_byte_t const* tb_static_stream_beg(tb_static_stream_t* sstream)
{
	// check
	tb_assert_and_check_return_val(sstream, tb_null);

	// the head
	return sstream->e? (sstream->e - sstream->n) : tb_null;
}
tb_byte_t const* tb_static_stream_pos(tb_static_stream_t* sstream)
{
	// check
	tb_assert_and_check_return_val(sstream && sstream->p <= sstream->e, tb_null);

	// sync
	tb_static_stream_sync(sstream);

	// the position 
	return sstream->p;
}
tb_byte_t const* tb_static_stream_end(tb_static_stream_t* sstream)
{
	// check
	tb_assert_and_check_return_val(sstream, tb_null);

	// the end
	return sstream->e;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * attributes
 */
tb_size_t tb_static_stream_size(tb_static_stream_t* sstream)
{
	// check
	tb_assert_and_check_return_val(sstream, 0);

	// the size
	return sstream->n;
}
tb_size_t tb_static_stream_offset(tb_static_stream_t* sstream)
{
	// check
	tb_assert_and_check_return_val(sstream, 0);

	// sync
	tb_static_stream_sync(sstream);

	// the offset
	return (((sstream->p + sstream->n) > sstream->e)? (sstream->p + sstream->n - sstream->e) : 0);
}
tb_size_t tb_static_stream_left(tb_static_stream_t* sstream)
{
	// check
	tb_assert_and_check_return_val(sstream && sstream->p <= sstream->e, 0);

	// sync
	tb_static_stream_sync(sstream);

	// the left
	return (sstream->e - sstream->p);
}
tb_size_t tb_static_stream_left_bits(tb_static_stream_t* sstream)
{
	// check
	tb_assert_and_check_return_val(sstream, 0);

	// the left bits
	return ((sstream->p < sstream->e)? (((sstream->e - sstream->p) << 3) - sstream->b) : 0);
}
tb_bool_t tb_static_stream_valid(tb_static_stream_t* sstream)
{
	if (!sstream) return tb_false;
	if (sstream->p && sstream->p > sstream->e) return tb_false;
	return tb_true;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * skip
 */
tb_void_t tb_static_stream_skip(tb_static_stream_t* sstream, tb_size_t size)
{
	// check
	tb_assert_and_check_return(sstream && sstream->p <= sstream->e);

	// sync
	tb_static_stream_sync(sstream);

	// skip
	sstream->p += size;
}
tb_void_t tb_static_stream_skip_bits(tb_static_stream_t* sstream, tb_size_t nbits)
{
	// check
	tb_assert_and_check_return(sstream && sstream->p <= sstream->e);

	sstream->p += (sstream->b + nbits) >> 3;
	sstream->b = (sstream->b + nbits) & 0x07;
}
tb_char_t const* tb_static_stream_skip_string(tb_static_stream_t* sstream)
{
	return tb_static_stream_get_string(sstream);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * get
 */
tb_uint8_t tb_static_stream_get_u1(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream, 0);
	tb_uint8_t val = ((*sstream->p) >> (7 - sstream->b)) & 1;
	sstream->b++;
	if (sstream->b >= 8) 
	{
		sstream->p++;
		sstream->b = 0;
	}
	return val;
}
tb_uint8_t tb_static_stream_get_u8(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	return *(sstream->p++);
}
tb_sint8_t tb_static_stream_get_s8(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	return *(sstream->p++);
}
tb_uint16_t tb_static_stream_get_u16_be(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_uint16_t val = tb_bits_get_u16_be(sstream->p);
	sstream->p += 2;
	return val;
	
}
tb_sint16_t tb_static_stream_get_s16_be(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_sint16_t val = tb_bits_get_s16_be(sstream->p);
	sstream->p += 2;
	return val;
	
}
tb_uint16_t tb_static_stream_get_u16_le(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_uint16_t val = tb_bits_get_u16_le(sstream->p);
	sstream->p += 2;
	return val;
	
}
tb_sint16_t tb_static_stream_get_s16_le(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_sint16_t val = tb_bits_get_s16_le(sstream->p);
	sstream->p += 2;
	return val;
	
}
tb_uint32_t tb_static_stream_get_u24_be(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_uint32_t val = tb_bits_get_u24_be(sstream->p);
	sstream->p += 3;
	return val;
	
}
tb_sint32_t tb_static_stream_get_s24_be(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_sint32_t val = tb_bits_get_s24_be(sstream->p);
	sstream->p += 3;
	return val;
	
}
tb_uint32_t tb_static_stream_get_u32_be(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_uint32_t val = tb_bits_get_u32_be(sstream->p);;
	sstream->p += 4;
	return val;
	
}
tb_sint32_t tb_static_stream_get_s32_be(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_sint32_t val = tb_bits_get_s32_be(sstream->p);
	sstream->p += 4;
	return val;
	
}
tb_uint64_t tb_static_stream_get_u64_be(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_uint64_t val = tb_bits_get_u64_be(sstream->p);;
	sstream->p += 8;
	return val;
	
}
tb_sint64_t tb_static_stream_get_s64_be(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_sint64_t val = tb_bits_get_s64_be(sstream->p);
	sstream->p += 8;
	return val;
	
}
tb_uint32_t tb_static_stream_get_u24_le(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_uint32_t val = tb_bits_get_u24_le(sstream->p);
	sstream->p += 3;
	return val;
}
tb_sint32_t tb_static_stream_get_s24_le(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_sint32_t val = tb_bits_get_s24_le(sstream->p);
	sstream->p += 3;
	return val;
	
}
tb_uint32_t tb_static_stream_get_u32_le(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_uint32_t val = tb_bits_get_u32_le(sstream->p);
	sstream->p += 4;
	return val;
	
}
tb_sint32_t tb_static_stream_get_s32_le(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_sint32_t val = tb_bits_get_s32_le(sstream->p);
	sstream->p += 4;
	return val;
	
}
tb_uint64_t tb_static_stream_get_u64_le(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_uint64_t val = tb_bits_get_u64_le(sstream->p);
	sstream->p += 8;
	return val;
	
}
tb_sint64_t tb_static_stream_get_s64_le(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_sint64_t val = tb_bits_get_s64_le(sstream->p);
	sstream->p += 8;
	return val;
	
}

#ifdef TB_CONFIG_TYPE_FLOAT
tb_double_t tb_static_stream_get_double_le(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_double_t val = tb_bits_get_float_le(sstream->p);
	sstream->p += 4;
	return val;
}
tb_double_t tb_static_stream_get_double_be(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_double_t val = tb_bits_get_float_be(sstream->p);
	sstream->p += 4;
	return val;
}

tb_double_t tb_static_stream_get_double_ble(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_double_t val = tb_bits_get_double_ble(sstream->p);
	sstream->p += 8;
	return val;
}
tb_double_t tb_static_stream_get_double_bbe(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_double_t val = tb_bits_get_double_bbe(sstream->p);
	sstream->p += 8;
	return val;
}
tb_double_t tb_static_stream_get_double_bne(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_double_t val = tb_bits_get_double_bne(sstream->p);
	sstream->p += 8;
	return val;
}

tb_double_t tb_static_stream_get_double_lle(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_double_t val = tb_bits_get_double_lle(sstream->p);
	sstream->p += 8;
	return val;
}
tb_double_t tb_static_stream_get_double_lbe(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_double_t val = tb_bits_get_double_lbe(sstream->p);
	sstream->p += 8;
	return val;
}
tb_double_t tb_static_stream_get_double_lne(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_double_t val = tb_bits_get_double_lne(sstream->p);
	sstream->p += 8;
	return val;
}

tb_double_t tb_static_stream_get_double_nle(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_double_t val = tb_bits_get_double_nle(sstream->p);
	sstream->p += 8;
	return val;
}
tb_double_t tb_static_stream_get_double_nbe(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_double_t val = tb_bits_get_double_nbe(sstream->p);
	sstream->p += 8;
	return val;
}
tb_double_t tb_static_stream_get_double_nne(tb_static_stream_t* sstream)
{
	tb_assert_and_check_return_val(sstream && !sstream->b, 0);
	tb_double_t val = tb_bits_get_double_nne(sstream->p);
	sstream->p += 8;
	return val;
}

#endif

tb_uint32_t tb_static_stream_get_ubits32(tb_static_stream_t* sstream, tb_size_t nbits)
{
	if (!nbits || !sstream) return 0;

	tb_uint32_t val = tb_bits_get_ubits32(sstream->p, sstream->b, nbits);
	sstream->p += (sstream->b + nbits) >> 3;
	sstream->b = (sstream->b + nbits) & 0x07;

	return val;
}
tb_sint32_t tb_static_stream_get_sbits32(tb_static_stream_t* sstream, tb_size_t nbits)
{
	if (!nbits || !sstream) return 0;

	tb_sint32_t val = tb_bits_get_sbits32(sstream->p, sstream->b, nbits);
	sstream->p += (sstream->b + nbits) >> 3;
	sstream->b = (sstream->b + nbits) & 0x07;

	return val;
}
tb_char_t const* tb_static_stream_get_string(tb_static_stream_t* sstream)
{
	// check
	tb_assert_and_check_return_val(sstream && sstream->p <= sstream->e, tb_null);

	// sync
	tb_static_stream_sync(sstream);

	// find '\0'
	tb_byte_t const* p = sstream->p;
	while (*p && p < sstream->e) p++;

	// is string with '\0' ?
	if ((*p)) return tb_null;

	tb_char_t const* s = (tb_char_t const*)sstream->p;
	sstream->p += p - sstream->p + 1;

	return s;
}
tb_size_t tb_static_stream_get_data(tb_static_stream_t* sstream, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(sstream->e >= sstream->p, 0);

	// sync
	tb_static_stream_sync(sstream);
	
	// the need
	tb_size_t need = size;
	if (sstream->e - sstream->p < need) need = sstream->e - sstream->p;

	// copy data
	if (need)
	{
		tb_memcpy(data, sstream->p, need);
		sstream->p += need;
	}

	// ok?
	return need;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * set
 */
tb_void_t tb_static_stream_set_u1(tb_static_stream_t* sstream, tb_uint8_t val)
{
	// check
	tb_assert_and_check_return(sstream);

	// set
	*(sstream->p) &= ~(0x1 << (7 - sstream->b));
	*(sstream->p) |= ((val & 0x1) << (7 - sstream->b));

	// next
	sstream->b++;
	if (sstream->b >= 8) 
	{
		sstream->p++;
		sstream->b = 0;
	}
}
tb_void_t tb_static_stream_set_u8(tb_static_stream_t* sstream, tb_uint8_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	*(sstream->p++) = val;
}
tb_void_t tb_static_stream_set_s8(tb_static_stream_t* sstream, tb_sint8_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	*(sstream->p++) = val;
}
tb_void_t tb_static_stream_set_u16_le(tb_static_stream_t* sstream, tb_uint16_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_u16_le(sstream->p, val);
	sstream->p += 2;
}
tb_void_t tb_static_stream_set_s16_le(tb_static_stream_t* sstream, tb_sint16_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_s16_le(sstream->p, val);
	sstream->p += 2;
}
tb_void_t tb_static_stream_set_u32_le(tb_static_stream_t* sstream, tb_uint32_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_u32_le(sstream->p, val);
	sstream->p += 4;
}
tb_void_t tb_static_stream_set_s32_le(tb_static_stream_t* sstream, tb_sint32_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_s32_le(sstream->p, val);
	sstream->p += 4;
}
tb_void_t tb_static_stream_set_u64_le(tb_static_stream_t* sstream, tb_uint64_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_u64_le(sstream->p, val);
	sstream->p += 8;
}
tb_void_t tb_static_stream_set_s64_le(tb_static_stream_t* sstream, tb_sint64_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_s64_le(sstream->p, val);
	sstream->p += 8;
}
tb_void_t tb_static_stream_set_u16_be(tb_static_stream_t* sstream, tb_uint16_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_u16_be(sstream->p, val);
	sstream->p += 2;
}
tb_void_t tb_static_stream_set_s16_be(tb_static_stream_t* sstream, tb_sint16_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_s16_be(sstream->p, val);
	sstream->p += 2;
}
tb_void_t tb_static_stream_set_u24_be(tb_static_stream_t* sstream, tb_uint32_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_u24_be(sstream->p, val);
	sstream->p += 3;
}
tb_void_t tb_static_stream_set_s24_be(tb_static_stream_t* sstream, tb_sint32_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_s24_be(sstream->p, val);
	sstream->p += 3;
}
tb_void_t tb_static_stream_set_u32_be(tb_static_stream_t* sstream, tb_uint32_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_u32_be(sstream->p, val);
	sstream->p += 4;
}
tb_void_t tb_static_stream_set_s32_be(tb_static_stream_t* sstream, tb_sint32_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_s32_be(sstream->p, val);
	sstream->p += 4;
}
tb_void_t tb_static_stream_set_u64_be(tb_static_stream_t* sstream, tb_uint64_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_u64_be(sstream->p, val);
	sstream->p += 8;
}
tb_void_t tb_static_stream_set_s64_be(tb_static_stream_t* sstream, tb_sint64_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_s64_be(sstream->p, val);
	sstream->p += 8;
}
#ifdef TB_CONFIG_TYPE_FLOAT
tb_void_t tb_static_stream_set_double_le(tb_static_stream_t* sstream, tb_double_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_float_le(sstream->p, val);
	sstream->p += 4;
}
tb_void_t tb_static_stream_set_double_be(tb_static_stream_t* sstream, tb_double_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_float_be(sstream->p, val);
	sstream->p += 4;
}
tb_void_t tb_static_stream_set_double_ble(tb_static_stream_t* sstream, tb_double_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_double_ble(sstream->p, val);
	sstream->p += 8;
}
tb_void_t tb_static_stream_set_double_bbe(tb_static_stream_t* sstream, tb_double_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_double_bbe(sstream->p, val);
	sstream->p += 8;
}
tb_void_t tb_static_stream_set_double_bne(tb_static_stream_t* sstream, tb_double_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_double_bne(sstream->p, val);
	sstream->p += 8;
}
tb_void_t tb_static_stream_set_double_lle(tb_static_stream_t* sstream, tb_double_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_double_lle(sstream->p, val);
	sstream->p += 8;
}
tb_void_t tb_static_stream_set_double_lbe(tb_static_stream_t* sstream, tb_double_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_double_lbe(sstream->p, val);
	sstream->p += 8;
}
tb_void_t tb_static_stream_set_double_lne(tb_static_stream_t* sstream, tb_double_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_double_lne(sstream->p, val);
	sstream->p += 8;
}
tb_void_t tb_static_stream_set_double_nle(tb_static_stream_t* sstream, tb_double_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_double_nle(sstream->p, val);
	sstream->p += 8;
}
tb_void_t tb_static_stream_set_double_nbe(tb_static_stream_t* sstream, tb_double_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_double_nbe(sstream->p, val);
	sstream->p += 8;
}
tb_void_t tb_static_stream_set_double_nne(tb_static_stream_t* sstream, tb_double_t val)
{
	tb_assert_and_check_return(sstream && !sstream->b);
	tb_bits_set_double_nne(sstream->p, val);
	sstream->p += 8;
}
#endif

tb_void_t tb_static_stream_set_ubits32(tb_static_stream_t* sstream, tb_uint32_t val, tb_size_t nbits)
{
	// check
	tb_assert_and_check_return(sstream);

	// no nbits?
	tb_check_return(nbits);

	// set bits
	tb_bits_set_ubits32(sstream->p, sstream->b, val, nbits);
	sstream->p += (sstream->b + nbits) >> 3;
	sstream->b = (sstream->b + nbits) & 0x07;
}

tb_void_t tb_static_stream_set_sbits32(tb_static_stream_t* sstream, tb_sint32_t val, tb_size_t nbits)
{
	// check
	tb_assert_and_check_return(sstream);

	// no nbits?
	tb_check_return(nbits);

	// set bits
	tb_bits_set_sbits32(sstream->p, sstream->b, val, nbits);
	sstream->p += (sstream->b + nbits) >> 3;
	sstream->b = (sstream->b + nbits) & 0x07;
}
tb_size_t tb_static_stream_set_data(tb_static_stream_t* sstream, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(sstream && sstream->p && sstream->e >= sstream->p && data, 0);

	// no size?
	tb_check_return_val(size, 0);

	// sync
	tb_static_stream_sync(sstream);

	// set data
	tb_size_t set_n = size;
	if (sstream->e - sstream->p < set_n) set_n = sstream->e - sstream->p;
	if (set_n)
	{
		tb_memcpy(sstream->p, data, set_n);
		sstream->p += set_n;
	}
	return (set_n > 0? set_n : 0);
}

tb_char_t* tb_static_stream_set_string(tb_static_stream_t* sstream, tb_char_t const* s)
{
	// check
	tb_assert_and_check_return_val(sstream && sstream->p && sstream->e >= sstream->p && s, tb_null);

	// sync
	tb_static_stream_sync(sstream);

	// set string
	tb_char_t* 			b = sstream->p;
	tb_char_t* 			p = sstream->p;
	tb_char_t const* 	e = sstream->e - 1;
	while (*s && p < e) *p++ = *s++;
	*p++ = '\0';
	sstream->p = p;

	// ok?
	return b;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * peek
 */
tb_uint32_t tb_static_stream_peek_ubits32(tb_static_stream_t* sstream, tb_size_t nbits)
{
	// check
	tb_assert_and_check_return_val(sstream, 0);

	// no nbits?
	tb_check_return_val(nbits, 0);

	// save status
	tb_byte_t* 	p = sstream->p;
	tb_size_t 	b = sstream->b;

	// peek value
	tb_uint32_t val = tb_static_stream_get_ubits32(sstream, nbits);

	// restore status
	sstream->p = p;
	sstream->b = b;

	// ok?
	return val;
}
tb_sint32_t tb_static_stream_peek_sbits32(tb_static_stream_t* sstream, tb_size_t nbits)
{
	// check
	tb_assert_and_check_return_val(sstream, 0);

	// no nbits?
	tb_check_return_val(nbits, 0);

	// save status
	tb_byte_t* 	p = sstream->p;
	tb_size_t 	b = sstream->b;

	// peek value
	tb_sint32_t val = tb_static_stream_get_sbits32(sstream, nbits);

	// restore status
	sstream->p = p;
	sstream->b = b;

	// ok?
	return val;
}

