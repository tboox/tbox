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
 * @file        wcslcpy.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#ifdef TB_CONFIG_LIBC_HAVE_WCSLCPY
#   include <wchar.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */
#ifdef TB_CONFIG_LIBC_HAVE_WCSLCPY
tb_size_t tb_wcslcpy(tb_wchar_t* s1, tb_wchar_t const* s2, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, 0);
    return wcslcpy(s1, s2, n);
}
#else
tb_size_t tb_wcslcpy(tb_wchar_t* s1, tb_wchar_t const* s2, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, 0);

    // no size or same? 
    tb_check_return_val(n && s1 != s2, tb_wcslen(s1));

    // copy
#if 0
    tb_wchar_t const* s = s2; --n;
    while (*s1 = *s2) 
    {
        if (n) 
        {
            --n;
            ++s1;
        }
        ++s2;
    }
    return s2 - s;
#else
    tb_size_t sn = tb_wcslen(s2);
    tb_memcpy(s1, s2, tb_min(sn + 1, n) * sizeof(tb_wchar_t));
    return tb_min(sn, n);
#endif
}
#endif
