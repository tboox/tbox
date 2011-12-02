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
 * \file		htm.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../string/string.h"

/* /////////////////////////////////////////////////////////
 * types
 */

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_size_t tb_format_htm_probe(tb_gstream_t* gst)
{
	// get need size
	tb_size_t 	need = 0;
	tb_gstream_ioctl1(gst, TB_GSTREAM_CMD_GET_CACHE, &need);
	tb_assert_and_check_return_val(need, 0);

	tb_uint64_t size = tb_gstream_size(gst);
	if (size && size < need) need = (tb_size_t)size;

	// the score
	tb_size_t score = 0;

	// need it
	tb_byte_t const* p = tb_gstream_need(gst, need);
	if (!p) return score;

	// attach text
	tb_string_t string;
	tb_string_init(&string);
	tb_string_assign_c_string_with_size_by_ref(&string, p, need);

	// find <!DOCTYPE html ... >
	tb_int_t pos = tb_string_find_c_string_nocase(&string, "<<!DOCTYPE html", 0);
	if (pos >= 0) score += 20;

	// find <html>
	pos = tb_string_find_c_string_nocase(&string, "<html>", 0);
	if (pos >= 0) score += 20;

	// find <head>
	pos = tb_string_find_c_string_nocase(&string, "<head>", 0);
	if (pos >= 0) score += 20;

	// find <body>
	pos = tb_string_find_c_string_nocase(&string, "<body>", 0);
	if (pos >= 0) score += 20;

	// detach it
	tb_string_uninit(&string);

	return score;
}
