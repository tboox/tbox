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
 * @file        ipv6.c 
 * @ingroup     network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "ipv6"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "ipv6.h"
#include "ipv4.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../string/string.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_void_t tb_ipv6_clear(tb_ipv6_ref_t ipv6)
{
    // check
    tb_assert_and_check_return(ipv6);

    // clear it
    tb_memset(ipv6, 0, sizeof(tb_ipv6_t));
}
tb_char_t const* tb_ipv6_cstr(tb_ipv6_ref_t ipv6, tb_char_t* data, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(ipv6 && data && maxn >= TB_IPV6_CSTR_MAXN, tb_null);

    // make it
    tb_long_t size = tb_snprintf(   data
                                ,   maxn - 1
                                ,   "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x"
                                ,   ipv6->u16[0]
                                ,   ipv6->u16[1]
                                ,   ipv6->u16[2]
                                ,   ipv6->u16[3]
                                ,   ipv6->u16[4]
                                ,   ipv6->u16[5]
                                ,   ipv6->u16[6]
                                ,   ipv6->u16[7]);
    if (size >= 0) data[size] = '\0';

    // ok
    return data;
}
tb_bool_t tb_ipv6_set_cstr(tb_ipv6_ref_t ipv6, tb_char_t const* cstr)
{
    // check
    tb_assert_and_check_return_val(cstr, tb_false);

    // is ipv4?
    if (!tb_strnicmp(cstr, "::ffff:", 7))
    {
        // attempt to make ipv6 from ipv4
        tb_ipv4_t ipv4;
        if (tb_ipv4_set_cstr(&ipv4, cstr + 7))
        {
            // make ipv6
            ipv6->u32[0] = 0;
            ipv6->u32[1] = 0;
            ipv6->u32[2] = 0xffff0000;
            ipv6->u32[3] = ipv4.u32;

            // ok
            return tb_true;
        }
    }

    // done
    tb_uint32_t         v = 0;
    tb_size_t           i = 0;
    tb_char_t           c = '\0';
    tb_char_t const*    p = cstr;
    tb_bool_t           ok = tb_true;
    tb_bool_t           stub = tb_false;
    tb_char_t           prev = '\0';
    tb_ipv6_t           temp;
    do
    {
        // save previous character
        prev = c;

        // the character
        c = *p++;

        // digit?
        if (tb_isdigit16(c) && v <= 0xffff)
        {
            // update value
            if (tb_isdigit10(c))
                v = (v << 4) + (c - '0');
            else if (c > ('a' - 1) && c < ('f' + 1))
                v = (v << 4) + (c - 'a') + 10;
            else if (c > ('A' - 1) && c < ('F' + 1))
                v = (v << 4) + (c - 'A') + 10;
            else 
            {
                // abort
                tb_assert_abort(0);

                // failed
                ok = tb_false;
                break;
            }
        }
        // "::"?
        else if (c == ':' && *p == ':' && p[1] != ':' && !stub)
        {
            // save value
            temp.u16[i++] = v;

            // clear value
            v = 0;

            // clear previous value
            prev = '\0';

            // find the left count of ':'
            tb_long_t           n = 0;
            tb_char_t const*    q = p;
            while (*q)
            {
                if (*q == ':') n++;
                q++;
            }
            tb_check_break_state(n <= 6, ok, tb_false);

            // compute the stub count
            n = 8 - n - i;
            tb_check_break_state(n > 0, ok, tb_false);

            // save the stub value
            while (n-- > 0) temp.u16[i++] = 0;

            // only one "::"
            stub = tb_true;

            // skip ':'
            p++;
        }
        // ':' or '\0'?
        else if (i < 8 && ((c == ':' && *p != ':') || !c) && v <= 0xffff && prev)
        {
            // save value
            temp.u16[i++] = v;

            // clear value
            v = 0;

            // clear previous value
            prev = '\0';
        }
        // failed?
        else 
        {
            ok = tb_false;
            break;
        }

    } while (c);

    // failed
    if (i != 8) ok = tb_false;

    // save it if ok
    if (ok && ipv6) *ipv6 = temp;

    // trace
//    tb_assertf(ok, "invalid addr: %s", cstr);

    // ok?
    return ok;
}

