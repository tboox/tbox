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
 * @file        wcsndup.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "string.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */

tb_wchar_t* tb_wcsndup(tb_wchar_t const* s, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

    // done
    n = tb_wcsnlen(s, n);
    __tb_register__ tb_wchar_t* p = (tb_wchar_t*)tb_malloc((n + 1) * sizeof(tb_wchar_t));
    if (p)
    {
        tb_memcpy(p, s, n * sizeof(tb_wchar_t));
        p[n] = L'\0';
    }

    return p;
}
