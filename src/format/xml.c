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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		xml.c
 * @ingroup 	format
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../string/string.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_size_t tb_format_xml_probe(tb_gstream_t* gst)
{
	// get need size
	tb_size_t 	need = 0;
	tb_gstream_ctrl(gst, TB_GSTREAM_CMD_GET_CACHE, &need);
	tb_assert_and_check_return_val(need, 0);

	tb_hize_t size = tb_gstream_size(gst);
	if (size && size < need) need = (tb_size_t)size;
	if (need > 4096) need = 4096;

	// need it
	tb_byte_t* p = TB_NULL;
	if (!tb_gstream_bneed(gst, &p, need)) return 0;
	tb_assert_and_check_return_val(p, 0);

	// the score
	tb_size_t score = 0;

	// init string
	tb_sstring_t 	string;
	tb_char_t 		data[4096];
	if (!tb_sstring_init(&string, data, need)) return 0;

	// copy string
	tb_sstring_cstrncpy(&string, p, need - 1);

	// find <?xml ...>
	tb_long_t pos = tb_sstring_cstristr(&string, 0, "<?xml");
	if (pos >= 0) score += 50;

	// exit string
	tb_sstring_exit(&string);

	return score;
}
