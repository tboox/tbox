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
 * @file		strnirchr.c
 * @ingroup 	libc
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces 
 */

tb_char_t* tb_strnirchr(tb_char_t const* s, tb_size_t n, tb_char_t c)
{
	// check
	tb_assert_and_check_return_val(s, tb_null);

	// done
	tb_byte_t const* 	p = s + n - 1;
	tb_byte_t 			b = tb_tolower((tb_byte_t)c);
	while (p >= s && *p)
	{
		if (tb_tolower(*p) == b) return (tb_char_t*)p;
		p--;
	}
	return tb_null;
}
