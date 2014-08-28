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

tb_void_t tb_ipv4_clr(tb_ipv4_t* ipv4)
{
    if (ipv4) ipv4->u32 = 0;
}
tb_uint32_t tb_ipv4_set(tb_ipv4_t* ipv4, tb_char_t const* ip)
{
    // done
    tb_uint32_t v4 = 0;
    if (ip)
    {
        tb_uint32_t         b = 0;
        tb_char_t           c = '\0';
        tb_size_t           i = 0;
        tb_char_t const*    p = ip;
        do
        {
            // the character
            c = *p++;

            // digit?
            if (c >= '0' && c <= '9' && b < 256)
            {
                b *= 10;
                b += (tb_uint32_t)(c - '0') & 0xff;
            }
            // '.'?
            else if ((c == '.' || !c) && b < 256)
            {
                v4 |= ((tb_uint32_t)b) << ((i++) << 3);
                b = 0;
            }
            // failed?
            else 
            {
                // trace
//                tb_trace_e("invalid addr: %s", ip);

                // clear it
                v4 = 0;
                break;
            }

        } while (c);
    }

    // save it
    if (ipv4) ipv4->u32 = v4;

    // ok?
    return v4;
}
tb_char_t const* tb_ipv4_get(tb_ipv4_t const* ipv4, tb_char_t* data, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(ipv4 && data && maxn > 15, tb_null);

    // format
    tb_size_t size = tb_snprintf(data, maxn, "%{ipv4}", ipv4);
    data[size] = '\0';

    // ok
    return data;
}

