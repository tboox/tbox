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
 * @file		ucs4.c
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
tb_long_t tb_charset_ucs4_get(tb_bits_stream_t* bst, tb_bool_t be, tb_uint32_t* ch);
tb_long_t tb_charset_ucs4_get(tb_bits_stream_t* bst, tb_bool_t be, tb_uint32_t* ch)
{
	*ch = be? tb_bits_stream_get_u32_be(bst) : tb_bits_stream_get_u32_le(bst);
	return 1;
}

tb_long_t tb_charset_ucs4_set(tb_bits_stream_t* bst, tb_bool_t be, tb_uint32_t ch);
tb_long_t tb_charset_ucs4_set(tb_bits_stream_t* bst, tb_bool_t be, tb_uint32_t ch)
{
	if (be) tb_bits_stream_set_u32_be(bst, ch);
	else tb_bits_stream_set_u32_le(bst, ch);
	return 1;
}

