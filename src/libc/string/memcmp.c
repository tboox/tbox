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
 * @file        memcmp.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "../../memory/memory.h"

#ifndef TB_CONFIG_LIBC_HAVE_MEMCMP
#   if defined(TB_ARCH_x86)
#       include "opt/x86/memcmp.c"
#   elif defined(TB_ARCH_ARM)
#       include "opt/arm/memcmp.c"
#   elif defined(TB_ARCH_SH4)
#       include "opt/sh4/memcmp.c"
#   endif
#else
#   include <string.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation 
 */
#if defined(TB_CONFIG_LIBC_HAVE_MEMCMP)
static tb_long_t tb_memcmp_impl(tb_cpointer_t s1, tb_cpointer_t s2, tb_size_t n)
{
    tb_assert_and_check_return_val(s1 && s2, 0);
    return memcmp(s1, s2, n);
}
#elif !defined(TB_LIBC_STRING_OPT_MEMCMP)
static tb_long_t tb_memcmp_impl(tb_cpointer_t s1, tb_cpointer_t s2, tb_size_t n)
{
    tb_assert_and_check_return_val(s1 && s2, 0);
    if (s1 == s2 || !n) return 0;

    tb_long_t r = 0;
    tb_byte_t const* p1 = (tb_byte_t const *)s1;
    tb_byte_t const* p2 = (tb_byte_t const *)s2;
    while (n-- && ((r = ((tb_long_t)(*p1++)) - *p2++) == 0)) ;
    return r;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */
tb_long_t tb_memcmp(tb_cpointer_t s1, tb_cpointer_t s2, tb_size_t n)
{
    // check
#ifdef __tb_debug__
    {
        // overflow?
        tb_size_t n1 = tb_memory_data_size(s1);
        if (n1 && n > n1)
        {
            tb_trace_i("[memcmp]: [overflow]: [%p, %lu] ?= [%p, %lu]", s2, n, s1, n1);
            tb_backtrace_dump("[memcmp]: [overflow]: ", tb_null, 10);
            tb_memory_data_dump(s1, "\t[malloc]: [from]: ");
            tb_abort();
        }

        // overflow?
        tb_size_t n2 = tb_memory_data_size(s2);
        if (n2 && n > n2)
        {
            tb_trace_i("[memcmp]: [overflow]: [%p, %lu] ?= [%p, %lu]", s2, n, s1, n1);
            tb_backtrace_dump("[memcmp]: [overflow]: ", tb_null, 10);
            tb_memory_data_dump(s2, "\t[malloc]: [from]: ");
            tb_abort();
        }
    }
#endif

    // done
    return tb_memcmp_impl(s1, s2, n);
}
