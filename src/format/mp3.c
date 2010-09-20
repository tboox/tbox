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
 * \file		mp3.c
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

tb_size_t tb_format_mp3_probe(tb_stream_t* st)
{
	tb_byte_t const* p = tb_stream_need(st, 5);

	// only for idv2.3 now
	if (p && p[0] == 'I' && p[1] == 'D' && p[2] == '3' && p[3] == 3 && p[4] == 0)
		return TB_FORMAT_SCORE_MAX;
	else return 0;
}
