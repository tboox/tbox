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
 * @file		mbstowcs.c
 * @ingroup 	libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stdlib.h"
#ifdef TB_CONFIG_LIBC_HAVE_MBSTOWCS
# 	include <stdlib.h>
#else
# 	include "../../charset/charset.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */

#ifdef TB_CONFIG_LIBC_HAVE_MBSTOWCS
tb_size_t tb_mbstowcs(tb_wchar_t* s1, tb_char_t const* s2, tb_size_t n)
{
	return mbstowcs(s1, s2, n);
}
#else
tb_size_t tb_mbstowcs(tb_wchar_t* s1, tb_char_t const* s2, tb_size_t n)
{
	// check
	tb_assert_and_check_return_val(s1 && s2, 0);

	// init
	tb_size_t e = (sizeof(tb_wchar_t) == 4)? TB_CHARSET_TYPE_UCS4 : TB_CHARSET_TYPE_UCS2;
	tb_long_t r = tb_charset_conv_cstr(TB_CHARSET_TYPE_UTF8, e | TB_CHARSET_TYPE_LE, s2, (tb_byte_t*)s1, n * sizeof(tb_wchar_t));
	if (r > 0) r /= sizeof(tb_wchar_t);
	
	// strip
	if (r >= 0) s1[r] = L'\0';

	// ok?
	return r > 0? r : 0;
}
#endif
