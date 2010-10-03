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
 * \file		txt.c
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

tb_size_t tb_format_txt_probe(tb_stream_t* st)
{
	// compute the max need_n
	tb_size_t need_n = TB_STREAM_DATA_MAX;
	tb_size_t file_n = tb_stream_size(st);
	if (file_n) need_n = TB_MATH_MIN(file_n, TB_STREAM_DATA_MAX);

	// need it
	tb_byte_t const* p = tb_stream_need(st, need_n);
	if (!p) return 0;

	// filter space
	tb_byte_t const* e = p + need_n;
	while (p < e && *p) p++;

	// is utf-9 or ascii?
	if (p == e) return 1;
	else return 0;
}
