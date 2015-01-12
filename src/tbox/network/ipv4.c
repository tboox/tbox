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
 * @file        ipv4.c 
 * @ingroup     network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "ipv4"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "ipv4.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../string/string.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_void_t tb_ipv4_clear(tb_ipv4_ref_t ipv4)
{
    // check
    tb_assert_and_check_return(ipv4);

    // clear it
    ipv4->u32 = 0;
}
tb_bool_t tb_ipv4_is_any(tb_ipv4_ref_t ipv4)
{
    // check
    tb_assert_and_check_return_val(ipv4, tb_true);

    // is empty?
    return !ipv4->u32;
}
tb_bool_t tb_ipv4_is_loopback(tb_ipv4_ref_t ipv4)
{
    // check
    tb_assert_and_check_return_val(ipv4, tb_false);

    // is loopback?
    return (ipv4->u32 == 0x0100007f);
}
tb_bool_t tb_ipv4_is_equal(tb_ipv4_ref_t ipv4, tb_ipv4_ref_t other)
{
    // check
    tb_assert_and_check_return_val(ipv4 && other, tb_false);

    // is equal?
    return ipv4->u32 == other->u32;
}
tb_char_t const* tb_ipv4_cstr(tb_ipv4_ref_t ipv4, tb_char_t* data, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(ipv4 && data && maxn >= TB_IPV4_CSTR_MAXN, tb_null);

    // make it
    tb_long_t size = tb_snprintf(data, maxn - 1, "%u.%u.%u.%u", ipv4->u8[0], ipv4->u8[1], ipv4->u8[2], ipv4->u8[3]);
    if (size >= 0) data[size] = '\0';

    // ok
    return data;
}
tb_bool_t tb_ipv4_cstr_set(tb_ipv4_ref_t ipv4, tb_char_t const* cstr)
{
    // check
    tb_assert_and_check_return_val(cstr, tb_false);

    // done
    tb_uint32_t         r = 0;
    tb_uint32_t         v = 0;
    tb_char_t           c = '\0';
    tb_size_t           i = 0;
    tb_char_t const*    p = cstr;
    do
    {
        // the character
        c = *p++;

        // digit?
        if (tb_isdigit10(c) && v <= 0xff)
        {
            v *= 10;
            v += (tb_uint32_t)(c - '0') & 0xff;
        }
        // '.' or '\0'?
        else if ((c == '.' || !c) && v <= 0xff)
        {
            r |= ((tb_uint32_t)v) << ((i++) << 3);
            v = 0;
        }
        // failed?
        else 
        {
            // trace
            tb_trace_d("invalid ipv4 addr: %s", cstr);

            // clear it
            i = 0;
            break;
        }

    } while (c);

    // save it if ok
    if (ipv4) ipv4->u32 = r;

    // ok?
    return i == 4;
}
