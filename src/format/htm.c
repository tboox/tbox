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
 * \file		htm.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * types
 */


/* /////////////////////////////////////////////////////////
 * details
 */


/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_size_t tb_format_htm_probe(tb_stream_t* st)
{
	// compute the max need_n
	tb_size_t need_n = TB_STREAM_DATA_MAX;
	tb_size_t file_n = tb_stream_size(st);
	if (file_n) need_n = TB_MATH_MIN(file_n, TB_STREAM_DATA_MAX);

	// the score
	tb_size_t score = 0;

	// need it
	tb_byte_t const* p = tb_stream_need(st, need_n);
	if (!p) return score;

	// attach text
	tb_string_t string;
	tb_string_init(&string);
	tb_string_assign_c_string_with_size_by_ref(&string, p, need_n);

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
