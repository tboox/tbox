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
 * \file		stricmp.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

#ifdef TB_CONFIG_LIBC_HAVE_STRICMP
# 	include <string.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * interfaces 
 */
#ifdef TB_CONFIG_LIBC_HAVE_STRICMP
tb_long_t tb_stricmp(tb_char_t const* s1, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(s1 && s2, 0);
	return strcasecmp(s1, s2);
}
#else
tb_long_t tb_stricmp(tb_char_t const* s1, tb_char_t const* s2)
{
	tb_assert_and_check_return_val(s1 && s2, 0);
	if (s1 == s2) return 0;

	tb_long_t r = 0;
	while (((s1 == s2) || !(r = ((tb_long_t)(tb_tolower(*((tb_byte_t* )s1)))) - tb_tolower(*((tb_byte_t* )s2)))) && (++s2, *s1++));
	return r;
}
#endif
