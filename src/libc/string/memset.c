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
 * @file        memset.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "../../utils/utils.h"
#ifndef TB_CONFIG_LIBC_HAVE_MEMSET
#   if defined(TB_ARCH_x86)
#       include "opt/x86/memset.c"
#   elif defined(TB_ARCH_x64)
#       include "opt/x86/memset.c"
#   elif defined(TB_ARCH_ARM)
#       include "opt/arm/memset.c"
#   elif defined(TB_ARCH_SH4)
#       include "opt/sh4/memset.c"
#   endif
#else
#   include <string.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation 
 */
#if defined(TB_CONFIG_LIBC_HAVE_MEMSET)
static tb_pointer_t tb_memset_impl(tb_pointer_t s, tb_byte_t c, tb_size_t n)
{
    tb_assert_and_check_return_val(s, tb_null);
    return memset(s, c, n);
}
#elif !defined(TB_LIBC_STRING_OPT_MEMSET_U8)
static tb_pointer_t tb_memset_impl(tb_pointer_t s, tb_byte_t c, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

    // no size?
    tb_check_return_val(n, s);

    // init
    __tb_register__ tb_byte_t* p = s;

    // done
#ifdef __tb_small__
    while (n--) *p++ = c;
#else
    tb_size_t l = n & 0x3; n = (n - l) >> 2;
    while (n--)
    {
        p[0] = c;
        p[1] = c;
        p[2] = c;
        p[3] = c;
        p += 4;
    }

    while (l--) *p++ = c;
#endif
    return s;
}
#endif

#ifndef TB_LIBC_STRING_OPT_MEMSET_U16
static tb_pointer_t tb_memset_u16_impl(tb_pointer_t s, tb_uint16_t c, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

    // no size?
    tb_check_return_val(n, s);

    // must be aligned by 2-bytes 
    tb_assert(!(((tb_size_t)s) & 0x1));

    // init
    __tb_register__ tb_uint16_t* p = s;

    // done
#ifdef __tb_small__
    while (n--) *p++ = c;
#else
    tb_size_t l = n & 0x3; n = (n - l) >> 2;
    while (n--)
    {
        p[0] = c;
        p[1] = c;
        p[2] = c;
        p[3] = c;
        p += 4;
    }

    while (l--) *p++ = c;
#endif

    // ok?
    return s;
}
#endif

#ifndef TB_LIBC_STRING_OPT_MEMSET_U24
static tb_pointer_t tb_memset_u24_impl(tb_pointer_t s, tb_uint32_t c, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

    // no size?
    tb_check_return_val(n, s);

    // init
    __tb_register__ tb_byte_t* p = s;
    __tb_register__ tb_byte_t* e = p + (n * 3);

    // done
#ifdef __tb_small__
    for (; p < e; p += 3) tb_bits_set_u24_ne(p, c);
#else
    tb_size_t l = n & 0x3; n -= l;
    while (p < e)
    {
        tb_bits_set_u24_ne(p + 0, c);
        tb_bits_set_u24_ne(p + 3, c);
        tb_bits_set_u24_ne(p + 6, c);
        tb_bits_set_u24_ne(p + 9, c);
        p += 12;
    }

    while (l--)
    {
        tb_bits_set_u24_ne(p, c);
        p += 3;
    }
#endif

    // ok?
    return s;
}
#endif

#ifndef TB_LIBC_STRING_OPT_MEMSET_U32
static tb_pointer_t tb_memset_u32_impl(tb_pointer_t s, tb_uint32_t c, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

    // no size?
    tb_check_return_val(n, s);

    // must be aligned by 4-bytes 
    tb_assert(!(((tb_size_t)s) & 0x3));

    // init 
    __tb_register__ tb_uint32_t* p = s;

    // done
#ifdef __tb_small__
    while (n--) *p++ = c;
#else
    tb_size_t l = n & 0x3; n = (n - l) >> 2;
    while (n--)
    {
        p[0] = c;
        p[1] = c;
        p[2] = c;
        p[3] = c;
        p += 4;
    }

    while (l--) *p++ = c;
#endif

    // ok?
    return s;
}
#endif

#ifndef TB_LIBC_STRING_OPT_MEMSET_U64
static tb_pointer_t tb_memset_u64_impl(tb_pointer_t s, tb_uint64_t c, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

    // no size?
    tb_check_return_val(n, s);

    // must be aligned by 8-bytes 
    tb_assert(!(((tb_size_t)s) & 0x7));

    // init
    __tb_register__ tb_uint64_t* p = s;

    // done
#ifdef __tb_small__
    while (n--) *p++ = c;
#else
    tb_size_t l = n & 0x3; n = (n - l) >> 2;
    while (n--)
    {
        p[0] = c;
        p[1] = c;
        p[2] = c;
        p[3] = c;
        p += 4;
    }

    while (l--) *p++ = c;
#endif

    // ok?
    return s;
}
#endif
/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */
tb_pointer_t tb_memset(tb_pointer_t s, tb_byte_t c, tb_size_t n)
{
    // check
#ifdef __tb_debug__
    {
        // overflow?
        tb_size_t size = tb_memory_data_size(s);
        if (size && n > size)
        {
            tb_trace_i("[memset]: [overflow]: [%#x x %lu] => [%p, %lu]", c, n, s, size);
            tb_backtrace_dump("[memset]: [overflow]: ", tb_null, 10);
            tb_memory_data_dump(s, "\t[malloc]: [from]: ");
            tb_abort();
        }
    }
#endif

    // done
    return tb_memset_impl(s, c, n);
}
tb_pointer_t tb_memset_u16(tb_pointer_t s, tb_uint16_t c, tb_size_t n)
{
    // check
#ifdef __tb_debug__
    {
        // overflow?
        tb_size_t size = tb_memory_data_size(s);
        if (size && (n << 1) > size)
        {
            tb_trace_i("[memset_u16]: [overflow]: [%#x x %lu x 2] => [%p, %lu]", c, n, s, size);
            tb_backtrace_dump("[memset_u16]: [overflow]: ", tb_null, 10);
            tb_memory_data_dump(s, "\t[malloc]: [from]: ");
            tb_abort();
        }
    }
#endif

    // done
    return tb_memset_u16_impl(s, c, n);
}
tb_pointer_t tb_memset_u24(tb_pointer_t s, tb_uint32_t c, tb_size_t n)
{
    // check
#ifdef __tb_debug__
    {
        // overflow?
        tb_size_t size = tb_memory_data_size(s);
        if (size && (n * 3) > size)
        {
            tb_trace_i("[memset_u24]: [overflow]: [%#x x %lu x 3] => [%p, %lu]", c, n, s, size);
            tb_memory_data_dump(s, "\t[malloc]: [from]: ");
            tb_abort();
        }
    }
#endif

    // done
    return tb_memset_u24_impl(s, c, n);
}
tb_pointer_t tb_memset_u32(tb_pointer_t s, tb_uint32_t c, tb_size_t n)
{
    // check
#ifdef __tb_debug__
    {
        // overflow?
        tb_size_t size = tb_memory_data_size(s);
        if (size && (n << 2) > size)
        {
            tb_trace_i("[memset_u32]: [overflow]: [%#x x %lu x 4] => [%p, %lu]", c, n, s, size);
            tb_backtrace_dump("[memset_u32]: [overflow]: ", tb_null, 10);
            tb_memory_data_dump(s, "\t[malloc]: [from]: ");
            tb_abort();
        }
    }
#endif

    // done
    return tb_memset_u32_impl(s, c, n);
}
tb_pointer_t tb_memset_u64(tb_pointer_t s, tb_uint64_t c, tb_size_t n)
{
    // check
#ifdef __tb_debug__
    {
        // overflow?
        tb_size_t size = tb_memory_data_size(s);
        if (size && (n << 3) > size)
        {
            tb_trace_i("[memset_u64]: [overflow]: [%#llx x %lu x 4] => [%p, %lu]", c, n, s, size);
            tb_backtrace_dump("[memset_u64]: [overflow]: ", tb_null, 10);
            tb_memory_data_dump(s, "\t[malloc]: [from]: ");
            tb_abort();
        }
    }
#endif

    // done
    return tb_memset_u64_impl(s, c, n);
}
