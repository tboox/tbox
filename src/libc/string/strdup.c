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
 * @file        strdup.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */

tb_char_t* tb_strdup(tb_char_t const* s)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

    // make 
    __tb_register__ tb_size_t   n = tb_strlen(s);
    __tb_register__ tb_char_t*  p = (tb_char_t*)tb_malloc(n + 1);
    tb_assert_and_check_return_val(p, tb_null);

    // copy
    tb_memcpy(p, s, n);

    // end
    p[n] = '\0';

    // ok
    return p;
}
