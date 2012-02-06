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
 * \author		ruki
 * \file		7z.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_size_t tb_format_7z_probe(tb_gstream_t* gst)
{
	tb_byte_t* p = TB_NULL;
	if (!tb_gstream_bneed(gst, &p, 6)) return 0;
	tb_assert_and_check_return_val(p, 0);

	if ( 	p[0] == '7' 
		&& 	p[1] == 'z' 
		&& 	p[2] == 0xbc
		&& 	p[3] == 0xaf
		&& 	p[4] == 0x27
		&& 	p[5] == 0x1c)
		return 60;
	else return 0;
}
