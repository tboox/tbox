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
 * @file		wcsicmp.c
 * @ingroup 	libc
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#ifdef TB_CONFIG_LIBC_HAVE_WCSICMP
# 	include <wchar.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * interfaces 
 */
#ifdef TB_CONFIG_LIBC_HAVE_WCSICMP
tb_long_t tb_wcsicmp(tb_wchar_t const* s1, tb_wchar_t const* s2)
{
	tb_assert_and_check_return_val(s1 && s2, 0);
	return wcscasecmp(s1, s2);
}
#else
tb_long_t tb_wcsicmp(tb_wchar_t const* s1, tb_wchar_t const* s2)
{
	tb_assert_and_check_return_val(s1 && s2, 0);
	tb_trace_noimpl();
	return 0;
}
#endif
