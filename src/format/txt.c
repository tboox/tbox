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
 * interfaces
 */

tb_size_t tb_format_txt_probe(tb_gstream_t* gst)
{
	// get need size
	tb_size_t 	need = 0;
	tb_gstream_ioctl1(gst, TB_GSTREAM_CMD_GET_CACHE, &need);
	tb_assert_and_check_return_val(need, 0);

	tb_uint64_t size = tb_gstream_size(gst);
	if (size && size < need) need = (tb_size_t)size;

	// need it
	tb_byte_t* p = TB_NULL;
	if (!tb_gstream_bneed(gst, &p, need)) return 0;
	tb_assert_and_check_return_val(p, 0);

	// filter space
	tb_byte_t const* e = p + need;
	while (p < e && *p) p++;

	// is utf-9 or ascii?
	if (p == e) return 1;
	else return 0;
}
