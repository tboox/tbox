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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        wcstombs.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stdlib.h"
#ifdef TB_CONFIG_LIBC_HAVE_WCSTOMBS
#   include <stdlib.h>
#   ifdef TB_CONFIG_LIBC_HAVE_SETLOCALE
#       include <locale.h>
#   endif
#else
#   include "../../charset/charset.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */
#ifdef TB_CONFIG_LIBC_HAVE_WCSTOMBS
tb_size_t tb_wcstombs(tb_char_t* s1, tb_wchar_t const* s2, tb_size_t n)
{
    // set local locale
#ifdef TB_CONFIG_LIBC_HAVE_SETLOCALE
    setlocale(LC_ALL, "");
#endif

    // convert it
    n = wcstombs(s1, s2, n);

    // set default locale
#ifdef TB_CONFIG_LIBC_HAVE_SETLOCALE
    setlocale(LC_ALL, "C");
#endif

    // ok
    return n;
}
#else
tb_size_t tb_wcstombs(tb_char_t* s1, tb_wchar_t const* s2, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, 0);

    // init
    tb_long_t r = 0;
    tb_size_t l = tb_wcslen(s2);
    
    // atow
    if (l) 
    {
        tb_size_t e = (sizeof(tb_wchar_t) == 4)? TB_CHARSET_TYPE_UCS4 : TB_CHARSET_TYPE_UCS2;
        r = tb_charset_conv_data(e | TB_CHARSET_TYPE_LE, TB_CHARSET_TYPE_UTF8, (tb_byte_t const*)s2, l * sizeof(tb_wchar_t), (tb_byte_t*)s1, n);
    }

    // strip
    if (r >= 0) s1[r] = '\0';

    // ok?
    return r > 0? r : -1;
}
#endif
