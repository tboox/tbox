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
 * @file        strnlen.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "../../memory/memory.h"
#ifndef TB_CONFIG_LIBC_HAVE_STRNLEN
#   if defined(TB_ARCH_x86)
#       include "opt/x86/strnlen.c"
#   elif defined(TB_ARCH_ARM)
#       include "opt/arm/strnlen.c"
#   elif defined(TB_ARCH_SH4)
#       include "opt/sh4/strnlen.c"
#   endif
#else
#   include <string.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation 
 */
#if defined(TB_CONFIG_LIBC_HAVE_STRNLEN)
static tb_size_t tb_strnlen_impl(tb_char_t const* s, tb_size_t n)
{
    tb_assert_and_check_return_val(s, 0);
    return strnlen(s, n);
}
#elif !defined(TB_LIBC_STRING_OPT_STRNLEN)
static tb_size_t tb_strnlen_impl(tb_char_t const* s, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s, 0);
    if (!n) return 0;

    __tb_register__ tb_char_t const* p = s;

#ifdef __tb_small__
    while (n-- && *p) ++p;
    return p - s;
#else
    tb_size_t l = n & 0x3; n = (n - l) >> 2;
    while (n--)
    {
        if (!p[0]) return (p - s + 0);
        if (!p[1]) return (p - s + 1);
        if (!p[2]) return (p - s + 2);
        if (!p[3]) return (p - s + 3);
        p += 4;
    }

    while (l-- && *p) ++p;
    return p - s;
#endif
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */
tb_size_t tb_strnlen(tb_char_t const* s, tb_size_t n)
{
    // check
#ifdef __tb_debug__
    {
        // overflow? 
        tb_size_t size = tb_memory_data_size(s);
        if (size)
        {
            // no '\0'?
            tb_size_t real = tb_strnlen_impl(s, size);
            if (s[real])
            {
                tb_trace_i("[strnlen]: [overflow]: [%p, %lu]", s, size);
                tb_backtrace_dump("[strnlen]: [overflow]: ", tb_object_null, 10);
                tb_memory_data_dump(s, "\t[malloc]: [from]: ");
                tb_abort();
            }
        }
    }
#endif

    // done
    return tb_strnlen_impl(s, n);
}
