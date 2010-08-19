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
 * \file		bits.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "bits.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

/* /////////////////////////////////////////////////////////
 * interfaces
 */
void tb_bits_attach(tb_bits_t* bits, tb_byte_t const* data, tb_size_t size)
{
	TB_ASSERT(bits && data && size);

	bits->p 	= data;
	bits->b 	= 0;
	bits->e 	= data + size;
}
void tb_bits_goto(tb_bits_t* bits, tb_byte_t const* data)
{
	TB_ASSERT(bits && data && data >= bits->b && data <= bits->e);
	bits->b = 0;
	if (data >= bits->b && data <= bits->e) bits->p = data;
}
void tb_bits_sync(tb_bits_t* bits)
{
	if (bits->b) 
	{
		bits->p++;
		bits->b = 0;
	}
}
tb_byte_t const* tb_bits_tell(tb_bits_t* bits)
{
	TB_ASSERT(bits && bits->p <= bits->e);
	tb_bits_sync(bits);
	return bits->p;
}
tb_size_t tb_bits_left_bytes(tb_bits_t* bits)
{
	TB_ASSERT(bits && bits->p <= bits->e);
	tb_bits_sync(bits);
	return (bits->e - bits->p);
}
tb_size_t tb_bits_left_bits(tb_bits_t* bits)
{
	TB_ASSERT(bits && bits->p <= bits->e);
	return ((bits->e - bits->p) << 3) - bits->b;
}

tb_byte_t const* tb_bits_end(tb_bits_t* bits)
{
	TB_ASSERT(bits && bits->e);
	return bits->e;
}
void tb_bits_skip_bytes(tb_bits_t* bits, tb_size_t bytes_n)
{
	TB_ASSERT(bits && bits->p <= bits->e);
	tb_bits_sync(bits);
	bits->p += bytes_n;
}
void tb_bits_skip_bits(tb_bits_t* bits, tb_size_t bits_n)
{
	TB_ASSERT(bits && bits->p <= bits->e);
	bits->b += bits_n;
	while (bits->b > 7) 
	{
		bits->p++;
		bits->b -= 8;
	}
}
tb_char_t const* tb_bits_skip_string(tb_bits_t* bits)
{
	return tb_bits_get_string(bits);
}
tb_char_t const* tb_bits_get_string(tb_bits_t* bits)
{
	TB_ASSERT(bits && bits->p <= bits->e);
	tb_bits_sync(bits);

	// find '\0'
	tb_byte_t const* p = bits->p;
	while (*p && p < bits->e) p++;

	// is string with '\0' ?
	if ((*p)) return TB_NULL;

	tb_char_t const* s = (tb_char_t const*)bits->p;
	bits->p += p - bits->p + 1;

	return s;
}

tb_uint32_t tb_bits_get_ubits(tb_bits_t* bits, tb_size_t bits_n)
{
	if (!bits_n || !bits) return 0;

	tb_uint32_t val = 0;
	tb_uint8_t i = bits->b; 
	tb_uint8_t j = 24;

	bits->b += bits_n;
	while (bits->b > 7) 
	{
		val |= *(bits->p++) << (i + j);
		j -= 8;
		bits->b -= 8;
	}
	if (bits->b > 0) val |= *(bits->p) << (i + j);
	val >>= 1;
	if (val & 0x80000000) val &= 0x7fffffff;
	val >>= (31 - bits_n);

	return val;
}
tb_sint32_t tb_bits_get_sbits(tb_bits_t* bits, tb_size_t bits_n)
{
	if (!bits_n || !bits) return 0;

#if 1
	tb_sint32_t val = 0;
	tb_uint8_t i = bits->b; 
	tb_uint8_t j = 24;

	bits->b += bits_n;
	while (bits->b > 7) 
	{
		val |= *(bits->p++) << (i + j);
		j -= 8;
		bits->b -= 8;
	}

	if (bits->b > 0) val |= *(bits->p) << (i + j);
	val >>= (32 - bits_n);
	return val;
#else
	tb_sint32_t val = 0;
	val = -tb_bits_get_u1(bits);
	val = (val << (bits_n - 1)) | tb_bits_get_ubits(bits, bits_n - 1);
	return val;
#endif
}
tb_uint8_t tb_bits_get_u1(tb_bits_t* bits)
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
tb_uint8_t tb_bits_get_u8(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	return *(bits->p++);
}

tb_sint8_t tb_bits_get_s8(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	return *(bits->p++);
}

tb_uint16_t tb_bits_get_u16(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
#ifdef TPLAT_WORDS_BIGENDIAN
	tb_uint16_t val = (*(bits->p) | *(bits->p + 1) << 8);
#else
	tb_uint16_t val = *((tb_uint16_t*)bits->p);
#endif
	bits->p += 2;
	return val;
}

tb_sint16_t tb_bits_get_s16(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
#ifdef TPLAT_WORDS_BIGENDIAN
	tb_sint16_t val = (*(bits->p) | *(bits->p + 1) << 8);
#else
	tb_sint16_t val = *((tb_sint16_t*)bits->p);
#endif
	bits->p += 2;
	return val;
}

tb_uint32_t tb_bits_get_u32(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
#ifdef TPLAT_WORDS_BIGENDIAN
	tb_uint32_t val = (*(bits->p) | *(bits->p + 1) << 8 | *(bits->p + 2) << 16 | *(bits->p + 3) << 24);
#else
	tb_uint32_t val = *((tb_uint32_t*)bits->p);
#endif
	bits->p += 4;
	return val;
}

tb_sint32_t tb_bits_get_s32(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
#ifdef TPLAT_WORDS_BIGENDIAN
	tb_sint32_t val = (*(bits->p) | *(bits->p + 1) << 8 | *(bits->p + 2) << 16 | *(bits->p + 3) << 24);
#else
	tb_sint32_t val = *((tb_sint32_t*)bits->p);
#endif
	bits->p += 4;
	return val;
}
tb_uint16_t tb_bits_get_u16_be(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
	tb_uint16_t val = (*(bits->p) << 8 | *(bits->p + 1));
	bits->p += 2;
	return val;
}
tb_sint16_t tb_bits_get_s16_be(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
	tb_sint16_t val = (*(bits->p) << 8 | *(bits->p + 1));
	bits->p += 2;
	return val;
}
tb_uint16_t tb_bits_get_u16_le(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
	tb_uint16_t val = (*(bits->p + 1) << 8 | *(bits->p));
	bits->p += 2;
	return val;
}
tb_sint16_t tb_bits_get_s16_le(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
	tb_sint16_t val = (*(bits->p + 1) << 8 | *(bits->p));
	bits->p += 2;
	return val;
}
tb_uint32_t tb_bits_get_u32_be(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
	tb_uint32_t val = (*(bits->p) << 24 | *(bits->p + 1) << 16 | *(bits->p + 2) << 8 | *(bits->p + 3));
	bits->p += 4;
	return val;
}
tb_sint32_t tb_bits_get_s32_be(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
	tb_sint32_t val = (*(bits->p) << 24 | *(bits->p + 1) << 16 | *(bits->p + 2) << 8 | *(bits->p + 3));
	bits->p += 4;
	return val;
}
tb_uint32_t tb_bits_get_u32_le(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
	tb_uint32_t val = (*(bits->p +3) << 24 | *(bits->p + 2) << 16 | *(bits->p + 1) << 8 | *(bits->p));
	bits->p += 4;
	return val;
}
tb_sint32_t tb_bits_get_s32_le(tb_bits_t* bits)
{
	TB_ASSERT(!bits->b);
	
	tb_sint32_t val = (*(bits->p + 3) << 24 | *(bits->p + 2) << 16 | *(bits->p + 1) << 8 | *(bits->p));
	bits->p += 4;
	return val;
}

tb_uint32_t tb_bits_peek_ubits(tb_bits_t* bits, tb_size_t bits_n)
{
	if (!bits_n || !bits) return 0;

	// save status
	tb_byte_t const* p = bits->p;
	tb_size_t b = bits->b;

	// peek value
	tb_uint32_t val = tb_bits_get_ubits(bits, bits_n);

	// restore status
	bits->p = p;
	bits->b = b;

	return val;
}
tb_sint32_t tb_bits_peek_sbits(tb_bits_t* bits, tb_size_t bits_n)
{
	if (!bits_n || !bits) return 0;

	// save status
	tb_byte_t const* p = bits->p;
	tb_size_t b = bits->b;

	// peek value
	tb_sint32_t val = tb_bits_get_sbits(bits, bits_n);

	// restore status
	bits->p = p;
	bits->b = b;

	return val;
}

