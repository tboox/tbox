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
 * @file		wcsnlen.c
 * @ingroup 	libc
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#ifdef TB_CONFIG_LIBC_HAVE_WCSNLEN
# 	include <wchar.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * interfaces 
 */
#ifdef TB_CONFIG_LIBC_HAVE_WCSNLEN
tb_size_t tb_wcsnlen(tb_wchar_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(s, 0);
	return wcsnlen(s, n);
}
#else
tb_size_t tb_wcsnlen(tb_wchar_t const* s, tb_size_t n)
{
	tb_assert_and_check_return_val(s, 0);
	if (!n) return 0;

	__tb_register__ tb_wchar_t const* p = s;

#ifdef __tb_small__
	while (n-- && *p) ++p;
	return p - s;
#else
	tb_size_t l = n & 0x3; n = (n - l) >> 2;
	while (n--)
	{
		if (!p[0]) return (p - s + 0);
		if (!p[1]) return (p - s + 1);
		if (!p[2]) return (p - s + 2);
		if (!p[3]) return (p - s + 3);
		p += 4;
	}

	while (l-- && *p) ++p;
	return p - s;
#endif
}
#endif
