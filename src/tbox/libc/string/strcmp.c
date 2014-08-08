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
 * @file        strcmp.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#ifndef TB_CONFIG_LIBC_HAVE_STRCMP
#   if defined(TB_ARCH_x86)
#       include "opt/x86/strcmp.c"
#   elif defined(TB_ARCH_ARM)
#       include "opt/arm/strcmp.c"
#   elif defined(TB_ARCH_SH4)
#       include "opt/sh4/strcmp.c"
#   endif
#else
#   include <string.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation 
 */
#if defined(TB_CONFIG_LIBC_HAVE_STRCMP)
static tb_long_t tb_strcmp_impl(tb_char_t const* s1, tb_char_t const* s2)
{
    tb_assert_and_check_return_val(s1 && s2, 0);
    return strcmp(s1, s2);
}
#elif !defined(TB_LIBC_STRING_OPT_STRCMP)
static tb_long_t tb_strcmp_impl(tb_char_t const* s1, tb_char_t const* s2)
{
    tb_assert_and_check_return_val(s1 && s2, 0);
    if (s1 == s2) return 0;

    tb_long_t r = 0;
    while (((r = ((tb_long_t)(*((tb_byte_t *)s1))) - *((tb_byte_t *)s2++)) == 0) && *s1++);
    return r;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */
tb_long_t tb_strcmp(tb_char_t const* s1, tb_char_t const* s2)
{
    // check
#ifdef __tb_debug__
    {
        // check overflow? 
        tb_strlen(s1);
        tb_strlen(s2);
    }
#endif

    // done
    return tb_strcmp_impl(s1, s2);
}
