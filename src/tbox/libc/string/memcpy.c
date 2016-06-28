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
 * @file        memcpy.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "../../memory/impl/prefix.h"
#ifndef TB_CONFIG_LIBC_HAVE_MEMCPY
#   if defined(TB_ARCH_x86)
#       include "impl/x86/memcpy.c"
#   elif defined(TB_ARCH_ARM)
#       include "impl/arm/memcpy.c"
#   elif defined(TB_ARCH_SH4)
#       include "impl/sh4/memcpy.c"
#   endif
#else
#   include <string.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_LIBC_HAVE_MEMCPY)
static tb_pointer_t tb_memcpy_impl(tb_pointer_t s1, tb_cpointer_t s2, tb_size_t n)
{
    tb_assert_and_check_return_val(s1 && s2, tb_null);
    return memcpy(s1, s2, n);
}
#elif !defined(TB_LIBC_STRING_IMPL_MEMCPY)
static tb_pointer_t tb_memcpy_impl(tb_pointer_t s1, tb_cpointer_t s2, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, tb_null);

#ifdef __tb_small__
    __tb_register__ tb_byte_t*          p1 = (tb_byte_t*)s1;
    __tb_register__ tb_byte_t const*    p2 = (tb_byte_t const*)s2;
    if (p1 == p2 || !n) return s1;
    while (n--) *p1++ = *p2++;
    return s1;
#else
    __tb_register__ tb_byte_t*          p1 = (tb_byte_t*)s1;
    __tb_register__ tb_byte_t const*    p2 = (tb_byte_t const*)s2;
    if (p1 == p2 || !n) return s1;
    
    tb_size_t l = n & 0x3; n = (n - l) >> 2;
    while (n--)
    {
        p1[0] = p2[0];
        p1[1] = p2[1];
        p1[2] = p2[2];
        p1[3] = p2[3];
        p1 += 4;
        p2 += 4;
    }
    while (l--) *p1++ = *p2++;
    return s1;
#endif /* __tb_small__ */
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_pointer_t tb_memcpy_(tb_pointer_t s1, tb_cpointer_t s2, tb_size_t n)
{
    // done
    return tb_memcpy_impl(s1, s2, n);
}
tb_pointer_t tb_memcpy(tb_pointer_t s1, tb_cpointer_t s2, tb_size_t n)
{
    // check
#ifdef __tb_debug__
    {
        // overflow dst?
        tb_size_t n1 = tb_pool_data_size(s1);
        if (n1 && n > n1)
        {
            tb_trace_i("[memcpy]: [overflow]: [%p, %lu] => [%p, %lu]", s2, n, s1, n1);
            tb_backtrace_dump("[memcpy]: [overflow]: [dst]: ", tb_null, 10);
            tb_pool_data_dump(s1, tb_true, "\t[malloc]: [from]: ");
            tb_abort();
        }

        // overflow src?
        tb_size_t n2 = tb_pool_data_size(s2);
        if (n2 && n > n2)
        {
            tb_trace_i("[memcpy]: [overflow]: [%p, %lu(%lu)] => [%p, %lu]", s2, n, n2, s1, n1);
            tb_backtrace_dump("[memcpy]: [overflow]: [src] ", tb_null, 10);
            tb_pool_data_dump(s2, tb_true, "\t[malloc]: [from]: ");
            tb_abort();
        }

        // overlap?
        if (((tb_byte_t*)s2 >= (tb_byte_t*)s1 && (tb_byte_t*)s2 < (tb_byte_t*)s1 + n)
            || ((tb_byte_t*)s1 >= (tb_byte_t*)s2 && (tb_byte_t*)s1 < (tb_byte_t*)s2 + n))
        {
            tb_trace_i("[memcpy]: [overlap]: [%p, %lu] => [%p, %lu]", s2, n, s1, n);
            tb_backtrace_dump("[memcpy]: [overlap]: ", tb_null, 10);
            tb_pool_data_dump(s1, tb_true, "\t[malloc]: [from]: ");
            tb_abort();
        }
    }
#endif

    // done
    return tb_memcpy_impl(s1, s2, n);
}
