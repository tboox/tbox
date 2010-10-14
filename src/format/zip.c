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
 * \file		zip.c
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

tb_size_t tb_format_zip_probe(tb_stream_t* st)
{
	tb_byte_t const* p = tb_stream_need(st, 4);
	if (p && tb_pbits_get_u32_le(p) == 0x04034b50)
		return 40;
	else return 0;
}
