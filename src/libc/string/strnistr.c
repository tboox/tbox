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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		strnistr.c
 * @ingroup 	libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation 
 */
tb_char_t* tb_strnistr(tb_char_t const* s1, tb_size_t n1, tb_char_t const* s2)
{
	// check
	tb_assert_and_check_return_val(s1 && s2 && n1, tb_null);

	// init
	__tb_register__ tb_char_t const* s = s1;
	__tb_register__ tb_char_t const* p = s2;
	__tb_register__ tb_size_t 		 n = n1;

	// done
	do 
	{
		if (!*p) return (tb_char_t* )s1;
		if (n && ((*p == *s) || (tb_tolower(*((tb_byte_t*)p)) == tb_tolower(*((tb_byte_t*)s))))) 
		{
			++p;
			++s;
			--n;
		} 
		else 
		{
			p = s2;
			if (!*s || !n) return tb_null;
			s = ++s1;
			n = --n1;
		}

	} while (1);

	// no found
	return tb_null;
}

