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
 * @file        strncat.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#ifdef TB_CONFIG_LIBC_HAVE_STRNCAT
#   include <string.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */

#ifdef TB_CONFIG_LIBC_HAVE_STRNCAT
tb_char_t* tb_strncat(tb_char_t* s1, tb_char_t const* s2, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, tb_null);

    // append it
    return strncat(s1, s2, n);
}
#else
tb_char_t* tb_strncat(tb_char_t* s1, tb_char_t const* s2, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, tb_null);

    // append it
    tb_char_t* s = s1;
    while (*s++); --s;
    while (n-- && !(*s++ = *s2++));
    return s1;
}
#endif
