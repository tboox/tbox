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
 * @file        hwaddr.c 
 * @ingroup     network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "hwaddr"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "hwaddr.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../string/string.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_void_t tb_hwaddr_clear(tb_hwaddr_ref_t hwaddr)
{
    // check
    tb_assert_and_check_return(hwaddr);

    // clear it
    tb_memset(hwaddr->u8, 0, sizeof(hwaddr->u8));
}
tb_void_t tb_hwaddr_copy(tb_hwaddr_ref_t hwaddr, tb_hwaddr_ref_t copied)
{
    // check
    tb_assert_and_check_return(hwaddr && copied);

    // copy it
    hwaddr->u8[0] = copied->u8[0];
    hwaddr->u8[1] = copied->u8[1];
    hwaddr->u8[2] = copied->u8[2];
    hwaddr->u8[3] = copied->u8[3];
    hwaddr->u8[4] = copied->u8[4];
    hwaddr->u8[5] = copied->u8[5];
}
tb_bool_t tb_hwaddr_is_equal(tb_hwaddr_ref_t hwaddr, tb_hwaddr_ref_t other)
{
    // check
    tb_assert_and_check_return_val(hwaddr && other, tb_false);

    // is equal?
    return !tb_memcmp(hwaddr->u8, other->u8, sizeof(hwaddr->u8));
}
tb_char_t const* tb_hwaddr_cstr(tb_hwaddr_ref_t hwaddr, tb_char_t* data, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(hwaddr && data && maxn >= TB_HWADDR_CSTR_MAXN, tb_null);

    // make it
    tb_long_t size = tb_snprintf(data, maxn - 1, "%02x:%02x:%02x:%02x:%02x:%02x", hwaddr->u8[0], hwaddr->u8[1], hwaddr->u8[2], hwaddr->u8[3], hwaddr->u8[4], hwaddr->u8[5]);
    if (size >= 0) data[size] = '\0';

    // ok
    return data;
}
tb_bool_t tb_hwaddr_cstr_set(tb_hwaddr_ref_t hwaddr, tb_char_t const* cstr)
{
    // check
    tb_assert_and_check_return_val(cstr, tb_false);

    // done
    tb_uint32_t         v = 0;
    tb_char_t           c = '\0';
    tb_size_t           i = 0;
    tb_char_t const*    p = cstr;
    tb_bool_t           ok = tb_true;
    tb_hwaddr_t         temp;
    do
    {
        // the character
        c = *p++;

        // digit?
        if (tb_isdigit16(c) && v <= 0xff)
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
        // ':' or "-" or '\0'?
        else if (i < 6 && (c == ':' || c == '-' || !c) && v <= 0xff)
        {
            // save value
            temp.u8[i++] = v;

            // clear value
            v = 0;
        }
        // failed?
        else 
        {
            ok = tb_false;
            break;
        }

    } while (c);

    // failed
    if (i != 6) ok = tb_false;

    // save it if ok
    if (ok && hwaddr) *hwaddr = temp;

    // trace
//    tb_assertf(ok, "invalid hwaddr: %s", cstr);

    // ok?
    return ok;
}

