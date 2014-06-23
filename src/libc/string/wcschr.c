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
 * @file        wcschr.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#ifdef TB_CONFIG_LIBC_HAVE_WCSCHR
#   include <string.h>
#endif
/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */

#ifdef TB_CONFIG_LIBC_HAVE_WCSCHR
tb_wchar_t* tb_wcschr(tb_wchar_t const* s, tb_wchar_t c)
{
    tb_assert_and_check_return_val(s1 && s2, tb_object_null);
    return wcschr(s1, c);
}
#else
tb_wchar_t* tb_wcschr(tb_wchar_t const* s, tb_wchar_t c)
{
    tb_assert_and_check_return_val(s, tb_object_null);

    while (*s)
    {
        if (*s == c) return (tb_wchar_t* )s;
        s++;

    }
    return tb_object_null;
}
#endif

