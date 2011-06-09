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
 * \file		xml.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../string/string.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_size_t tb_format_xml_probe(tb_gstream_t* gst)
{
	// compute the max need
	tb_size_t need = TB_GSTREAM_CACHE_SIZE;
	tb_size_t file = tb_gstream_size(gst);
	if (file) need = tb_min(file, TB_GSTREAM_CACHE_SIZE);

	// the score
	tb_size_t score = 0;

	// need it
	tb_byte_t const* p = tb_gstream_need(gst, need);
	if (!p) return score;

	// attach text
	tb_string_t string;
	tb_string_init(&string);
	tb_string_assign_c_string_with_size_by_ref(&string, p, need);

	// find <?xml ...>
	tb_int_t pos = tb_string_find_c_string(&string, "<?xml", 0);
	if (pos >= 0) score += 50;

	// detach it
	tb_string_uninit(&string);

	return score;
}
