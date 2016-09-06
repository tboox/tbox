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
 * @file        strlcpy.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "../../memory/impl/prefix.h"
#ifndef TB_CONFIG_LIBC_HAVE_STRLCPY
#   if defined(TB_ARCH_x86)
#       include "impl/x86/strlcpy.c"
#   elif defined(TB_ARCH_ARM)
#       include "impl/arm/strlcpy.c"
#   elif defined(TB_ARCH_SH4)
#       include "impl/sh4/strlcpy.c"
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
    // check
    tb_assert_and_check_return_val(s1 && s2, 0);

    // copy it
    return strlcpy(s1, s2, n);
}
#elif !defined(TB_LIBC_STRING_IMPL_STRLCPY)
/* copy s2 to s1 of size n
 *
 * - at most n - 1 characters will be copied.
 * - always null terminates (unless n == 0).
 * 
 * returns strlen(s2); if retval >= n, truncation occurred.
 */
static tb_size_t tb_strlcpy_impl(tb_char_t* s1, tb_char_t const* s2, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, 0);

    // init
    tb_char_t*          d = s1;
    tb_char_t const*    s = s2;
    tb_size_t           m = n;

    // copy as many bytes as will fit 
    if (m != 0 && --m != 0)
    {
        do 
        {
            if ((*d++ = *s++) == 0) break;

        } while (--m != 0);
    }

    // not enough room in dst, add null and traverse rest of src 
    if (m == 0)
    {
        if (n != 0) *d = '\0';      
        while (*s++) ;
    }

    // count does not include null
    return (s - s2 - 1);
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
        tb_size_t n1 = tb_pool_data_size(s1);
        if (n1 && tb_min(n2 + 1, n) > n1)
        {
            tb_trace_i("[strlcpy]: [overflow]: [%p, %lu] => [%p, %lu]", s2, tb_min(n2 + 1, n), s1, n1);
            tb_backtrace_dump("[strlcpy]: [overflow]: ", tb_null, 10);
            tb_pool_data_dump(s2, tb_true, "\t[malloc]: [from]: ");
            tb_abort();
        }
    }
#endif

    // done
    return tb_strlcpy_impl(s1, s2, n);
}
