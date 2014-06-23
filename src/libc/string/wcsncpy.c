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
 * @author      ruki
 * @file        wcsncpy.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#ifdef TB_CONFIG_LIBC_HAVE_WCSNCPY
#   include <wchar.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */
#ifdef TB_CONFIG_LIBC_HAVE_WCSNCPY
tb_wchar_t* tb_wcsncpy(tb_wchar_t* s1, tb_wchar_t const* s2, tb_size_t n)
{
    tb_assert_and_check_return_val(s1 && s2, tb_object_null);
    return wcsncpy(s1, s2, n);
}
#else
tb_wchar_t* tb_wcsncpy(tb_wchar_t* s1, tb_wchar_t const* s2, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, s1);

    // no size or same? 
    tb_check_return_val(n && s1 != s2, s1);

    // copy
#if 0
    tb_wchar_t* s = s1;
    while (n) 
    {
        if (*s = *s2) s2++;
        ++s;
        --n;
    }
    return s1;
#else
    tb_size_t sn = tb_wcslen(s2);
    tb_size_t cn = tb_min(sn, n);
    tb_size_t fn = sn < n? n - sn : 0;
    tb_memcpy(s1, s2, cn * sizeof(tb_wchar_t));
    if (fn) tb_memset(s1 + cn, 0, fn * sizeof(tb_wchar_t));
    return s1;
#endif
}
#endif
