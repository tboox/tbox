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
 * @file		iso8859.c
 * @ingroup 	charset
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../stream/stream.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_long_t tb_charset_iso8859_get(tb_bstream_t* bst, tb_bool_t be, tb_uint32_t* ch);
tb_long_t tb_charset_iso8859_get(tb_bstream_t* bst, tb_bool_t be, tb_uint32_t* ch)
{
	tb_byte_t b = tb_bstream_get_u8(bst);
	if (b < 0xa0) *ch = b;
	else
	{
		// @note: need lookup characters table
		tb_trace_d("iso8859: unknown character: %x", b);
		*ch = 0;
	}
	return 1;
}

tb_long_t tb_charset_iso8859_set(tb_bstream_t* bst, tb_bool_t be, tb_uint32_t ch);
tb_long_t tb_charset_iso8859_set(tb_bstream_t* bst, tb_bool_t be, tb_uint32_t ch)
{
	if (ch <= 0xa0) tb_bstream_set_u8(bst, (tb_uint8_t)ch);
	else 
	{
		// @note: need lookup characters table
		tb_trace_d("iso8859: unknown character: %x", ch);
	}
	return 1;
}

