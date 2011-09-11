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
 * \file		strnicmp.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

#ifdef TB_CONFIG_LIBC_HAVE_STRNICMP
# 	include <string.h>
#endif

/* /////////////////////////////////////////////////////////
 * interfaces 
 */
#ifdef TB_CONFIG_LIBC_HAVE_STRNICMP
tb_int_t tb_strnicmp(tb_char_t const* s1, tb_char_t const* s2, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(s1 && s2, 0);
	return strncasecmp(s1, s2, n);
}
#else
tb_int_t tb_strnicmp(tb_char_t const* s1, tb_char_t const* s2, tb_size_t n)
{
	TB_ASSERT_RETURN_VAL(s1 && s2, 0);
	if (s1 == s2 || !n) return 0;

	tb_int_t r = 0;
	while (n && ((s1 == s2) || !(r = ((tb_int_t)(tb_tolower(*((tb_byte_t* )s1)))) - tb_tolower(*((tb_byte_t* )s2)))) && (--n, ++s2, *s1++));
	return r;
}
#endif
