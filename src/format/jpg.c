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
 * \file		jpg.c
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

tb_size_t tb_format_jpg_probe(tb_gstream_t* gst)
{
	tb_byte_t const* p = tb_gstream_need(gst, 4);
	if ( 	p 
		&& 	p[0] == 0xff 
		&& 	p[1] == 0xd8 
		&& 	p[2] == 0xff 
		&& (p[3] >= 0xe0 && p[3] <= 0xef))
		return 40;
	else return 0;
}
