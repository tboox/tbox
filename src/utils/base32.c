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
 * \file		base32.c
 *
 */

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "base32.h"
#include "conv.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_BASE32_OUTPUT_MIN(in)  ((((in) * 8) / 5) + (((in) % 5) != 0) + 1)

/* ////////////////////////////////////////////////////////////////////////
 * implemention
 */
tb_size_t tb_base32_encode(tb_byte_t const* ib, tb_size_t in, tb_char_t* ob, tb_size_t on)
{
	// table 
	static tb_char_t const table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

	// check
	TB_ASSERT_RETURN_VAL(!(in >= TB_MAXU32 / 4 || on < TB_BASE32_OUTPUT_MIN(in)), 0);

	// encode
	tb_size_t i = 0;
	tb_byte_t w = 0;
	tb_size_t idx = 0;
	tb_char_t* pb = ob;
	for ( ; i < in; )
	{
		if (idx > 3)
		{
			w = (ib[i] & (0xFF >> idx));
			idx = (idx + 5) & 0x07;
			w <<= idx;
			if (i < in - 1)
				w |= ib[i + 1] >> (8 - idx);
			i++;
		}
		else
		{
			w = (ib[i] >> (8 - (idx + 5))) & 0x1F;
			idx = (idx + 5) & 0x07;
			if (idx == 0) i++;
		}
		*pb++ = table[w];		
	}
	*pb = '\0';
	return (pb - ob);
}
tb_size_t tb_base32_decode(tb_byte_t const* ib, tb_size_t in, tb_char_t* ob, tb_size_t on)
{
	// table
	static tb_byte_t const table[43][2] =
	{
		{ '0', 0xFF }
	,	{ '1', 0xFF }
	,	{ '2', 0x1A }
	,	{ '3', 0x1B }
	,	{ '4', 0x1C }
	,	{ '5', 0x1D }
	,	{ '6', 0x1E }
	,	{ '7', 0x1F }
	,	{ '8', 0xFF }
	,	{ '9', 0xFF }
	,	{ ':', 0xFF }
	,	{ ';', 0xFF }
	,	{ '<', 0xFF }
	,	{ '=', 0xFF }
	,	{ '>', 0xFF }
	,	{ '?', 0xFF }
	,	{ '@', 0xFF }
	,	{ 'A', 0x00 }
	,	{ 'B', 0x01 }
	,	{ 'C', 0x02 }
	,	{ 'D', 0x03 }
	,	{ 'E', 0x04 }
	,	{ 'F', 0x05 }
	,	{ 'G', 0x06 }
	,	{ 'H', 0x07 }
	,	{ 'I', 0x08 }
	,	{ 'J', 0x09 }
	,	{ 'K', 0x0A }
	,	{ 'L', 0x0B }
	,	{ 'M', 0x0C }
	,	{ 'N', 0x0D }
	,	{ 'O', 0x0E }
	,	{ 'P', 0x0F }
	,	{ 'Q', 0x10 }
	,	{ 'R', 0x11 }
	,	{ 'S', 0x12 }
	,	{ 'T', 0x13 }
	,	{ 'U', 0x14 }
	,	{ 'V', 0x15 }
	,	{ 'W', 0x16 }
	,	{ 'X', 0x17 }
	,	{ 'Y', 0x18 }
	,	{ 'Z', 0x19 }
	};

	// check
	TB_ASSERT_RETURN_VAL(on > (in * 5) / 8, 0);

	// decode
	tb_size_t i = 0;
	tb_byte_t w = 0;
	tb_size_t idx = 0;
	tb_char_t* op = ob;
	for ( ; i < in; ++i)
	{
		// loopup
		tb_int_t lookup = TB_CONV_TOUPPER(ib[i]) - '0';
		if (lookup < 0 || lookup >= 43) w = 0xff;
		else w = table[lookup][1];
		if (w == 0xff) continue;

		if (idx <= 3)
		{
			idx = (idx + 5) & 0x07;
			if (idx == 0) *op++ |= w;
			else *op |= w << (8 - idx);
		}
		else
		{
			idx = (idx + 5) & 0x07;
			*op++ |= (w >> idx);
			*op |= w << (8 - idx);
		}
	}
	return (op - ob);
}
