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
tb_char_t const* tb_bits_get_string(tb_byte_t const* p, tb_size_t size)
{
	TB_ASSERT(p && size);
	// {
	tb_char_t const* s = (tb_char_t const*)p;

	// find '\0'
	while (*p && size--) p++;

	// is string with '\0' ?
	if ((*p)) return TB_NULL;
	return s;
	// }
}
tb_float_t tb_bits_get_float_le(tb_byte_t const* p)
{
	union 
	{
		tb_uint32_t i;
		float 		f;

	} conv;

	conv.i = tb_bits_get_u32_le(p);
	return (tb_float_t)conv.f;
}
tb_float_t tb_bits_get_float_be(tb_byte_t const* p)
{
	union 
	{
		tb_uint32_t i;
		float 		f;

	} conv;

	conv.i = tb_bits_get_u32_be(p);
	return (tb_float_t)conv.f;
}
tb_float_t tb_bits_get_float_ne(tb_byte_t const* p)
{
	union 
	{
		tb_uint32_t i;
		float 		f;

	} conv;

	conv.i = tb_bits_get_u32_ne(p);
	return (tb_float_t)conv.f;
}

tb_float_t tb_bits_get_double_le(tb_byte_t const* p)
{
	union 
	{
		tb_uint32_t i[2];
		double 		f;

	} conv;

#ifdef TPLAT_FLOAT_BIGENDIAN
	conv.i[0] = tb_bits_get_u32_le(p);
	conv.i[1] = tb_bits_get_u32_le(p);
#else
	conv.i[1] = tb_bits_get_u32_le(p);
	conv.i[0] = tb_bits_get_u32_le(p);
#endif

	return (tb_float_t)conv.f;
}
tb_float_t tb_bits_get_double_be(tb_byte_t const* p)
{
	union 
	{
		tb_uint32_t i[2];
		double 		f;

	} conv;

#ifdef TPLAT_FLOAT_BIGENDIAN
	conv.i[0] = tb_bits_get_u32_be(p);
	conv.i[1] = tb_bits_get_u32_be(p);
#else
	conv.i[1] = tb_bits_get_u32_be(p);
	conv.i[0] = tb_bits_get_u32_be(p);
#endif

	return (tb_float_t)conv.f;
}
tb_float_t tb_bits_get_double_ne(tb_byte_t const* p)
{
	union 
	{
		tb_uint32_t i[2];
		double 		f;

	} conv;

#ifdef TPLAT_FLOAT_BIGENDIAN
	conv.i[0] = tb_bits_get_u32_ne(p);
	conv.i[1] = tb_bits_get_u32_ne(p);
#else
	conv.i[1] = tb_bits_get_u32_ne(p);
	conv.i[0] = tb_bits_get_u32_ne(p);
#endif

	return (tb_float_t)conv.f;
}

void tb_bits_swap_u16(tb_uint16_t* p)
{
	tb_byte_t* q = (tb_byte_t*)p;
	tb_byte_t b = q[0];
	q[0] = q[1];
	q[1] = b;
}
void tb_bits_swap_u32(tb_uint32_t* p)
{
	tb_byte_t* q = (tb_byte_t*)p;
	tb_byte_t b0 = q[0];
	tb_byte_t b1 = q[1];
	q[0] = q[3];
	q[1] = q[2];
	q[3] = b0;
	q[2] = b1;
}
