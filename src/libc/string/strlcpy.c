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
 * @file        strlcpy.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "../../memory/memory.h"
#ifndef TB_CONFIG_LIBC_HAVE_STRLCPY
#   if defined(TB_ARCH_x86)
#       include "opt/x86/strlcpy.c"
#   elif defined(TB_ARCH_ARM)
#       include "opt/arm/strlcpy.c"
#   elif defined(TB_ARCH_SH4)
#       include "opt/sh4/strlcpy.c"
#   endif
#else
#   include <string.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation 
 */
#if defined(TB_CONFIG_LIBC_HAVE_STRLCPY)
static tb_size_t tb_strlcpy_impl(tb_char_t* s1, tb_char_t const* s2, tb_size_t n)
{
    tb_assert_and_check_return_val(s1 && s2, 0);
    return strlcpy(s1, s2, n);
}
#elif !defined(TB_LIBC_STRING_OPT_STRLCPY)
static tb_size_t tb_strlcpy_impl(tb_char_t* s1, tb_char_t const* s2, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, 0);

    // no size or same? 
    tb_check_return_val(n && s1 != s2, tb_strlen(s1));

    // copy
#if 0
    tb_char_t const* s = s2; --n;
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
    tb_size_t sn = tb_strlen(s2);
    tb_memcpy(s1, s2, tb_min(sn + 1, n));
    return tb_min(sn, n);
#endif
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */
tb_size_t tb_strlcpy(tb_char_t* s1, tb_char_t const* s2, tb_size_t n)
{
    // check
#ifdef __tb_debug__
    {
        // overflow dst? 
        tb_size_t n2 = tb_strlen(s2);

        // strlcpy overflow? 
        tb_size_t n1 = tb_memory_data_size(s1);
        if (n1 && tb_min(n2, n) + 1 > n1)
        {
            tb_trace_i("[strlcpy]: [overflow]: [%p, %lu] => [%p, %lu]", s2, tb_min(n2, n), s1, n1);
            tb_backtrace_dump("[strlcpy]: [overflow]: ", tb_object_null, 10);
            tb_memory_data_dump(s2, "\t[malloc]: [from]: ");
            tb_abort();
        }
    }
#endif

    // done
    return tb_strlcpy_impl(s1, s2, n);
}
