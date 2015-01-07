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
 * @file        addr.c
 * @ingroup     network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "addr.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static __tb_inline__ tb_bool_t tb_addr_ipv6_to_ipv4(tb_ipv6_ref_t ipv6, tb_ipv4_ref_t ipv4)
{
    // check
    tb_assert_abort(ipv6 && ipv4);

    // is ipv4?
    if (!ipv6->u32[0] && !ipv6->u32[1] && ipv6->u32[2] == 0xffff0000)
    {
        // make ipv4
        ipv4->u32 = ipv6->u32[3];

        // ok
        return tb_true;
    }

    // failed
    return tb_false;
}
static __tb_inline__ tb_bool_t tb_addr_ipv4_to_ipv6(tb_ipv4_ref_t ipv4, tb_ipv6_ref_t ipv6)
{
    // check
    tb_assert_abort(ipv6 && ipv4);

    // make ipv6
    ipv6->u32[0] = 0;
    ipv6->u32[1] = 0;
    ipv6->u32[2] = 0xffff0000;
    ipv6->u32[3] = ipv4->u32;

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_addr_clear(tb_addr_ref_t addr)
{
    // check
    tb_assert_and_check_return(addr);

    // clear it
    tb_memset(addr, 0, sizeof(tb_addr_t));
}
tb_char_t const* tb_addr_ip_cstr(tb_addr_ref_t addr, tb_char_t* data, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(addr && data && maxn, tb_null);

    // done
    tb_char_t const* cstr = tb_null;
    switch (addr->family)
    {
    case TB_ADDR_FAMILY_IPV4:
        cstr = tb_ipv4_cstr(&addr->u.ipv4, data, maxn);
        break;
    case TB_ADDR_FAMILY_IPV6:
        cstr = tb_ipv6_cstr(&addr->u.ipv6, data, maxn);
        break;
    default:
        tb_assert_abort(0);
        break;
    }

    // ok?
    return cstr;
}
tb_bool_t tb_addr_set_ip_cstr(tb_addr_ref_t addr, tb_char_t const* cstr, tb_uint8_t family)
{
    // check
    tb_assert_and_check_return_val(cstr, tb_false);

    // done
    tb_bool_t ok = tb_false;
    tb_addr_t temp;
    switch (family)
    {
    case TB_ADDR_FAMILY_IPV4:
        {
            // make ipv4
            ok = tb_ipv4_set_cstr(&temp.u.ipv4, cstr);

            // make family
            if (ok) temp.family = family;
        }
        break;
    case TB_ADDR_FAMILY_IPV6:
        {
            // make ipv6
            ok = tb_ipv6_set_cstr(&temp.u.ipv6, cstr);

            // make family
            if (ok) temp.family = family;
        }
        break;
    default:
        {
            // attempt to make ipv4
            if ((ok = tb_ipv4_set_cstr(&temp.u.ipv4, cstr))) temp.family = TB_ADDR_FAMILY_IPV4;
            // make ipv6
            else if ((ok = tb_ipv6_set_cstr(&temp.u.ipv6, cstr))) temp.family = TB_ADDR_FAMILY_IPV6;
        }
        break;
    }

    // ok? save it
    if (ok && addr) 
    {
        // save port
        temp.port = addr->port;

        // save addr
        *addr = temp;
    }

    // ok?
    return ok;
}
tb_ipv4_ref_t tb_addr_ipv4(tb_addr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(addr, tb_null);

    // done
    tb_ipv4_ref_t ipv4 = tb_null;
    switch (addr->family)
    {
    case TB_ADDR_FAMILY_IPV4:
        ipv4 = &addr->u.ipv4;
        break;
    case TB_ADDR_FAMILY_IPV6:
        {
            tb_ipv4_t temp;
            if (tb_addr_ipv6_to_ipv4(&addr->u.ipv6, &temp))
            {
                addr->family = TB_ADDR_FAMILY_IPV4;
                addr->u.ipv4 = temp;
                ipv4 = &addr->u.ipv4;
            }
        }
        break;
    default:
        tb_assert_abort(0);
        break;
    }

    // ok?
    return ipv4;
}
tb_void_t tb_addr_set_ipv4(tb_addr_ref_t addr, tb_ipv4_ref_t ipv4)
{
    // check
    tb_assert_and_check_return(addr && ipv4);

    // save it
    addr->family = TB_ADDR_FAMILY_IPV4;
    addr->u.ipv4 = *ipv4;
}
tb_ipv6_ref_t tb_addr_ipv6(tb_addr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(addr, tb_null);

    // done
    tb_ipv6_ref_t ipv6 = tb_null;
    switch (addr->family)
    {
    case TB_ADDR_FAMILY_IPV4:
        {
            tb_ipv6_t temp;
            if (tb_addr_ipv4_to_ipv6(&addr->u.ipv4, &temp))
            {
                addr->family = TB_ADDR_FAMILY_IPV6;
                addr->u.ipv6 = temp;
                ipv6 = &addr->u.ipv6;
            }
        }
        break;
    case TB_ADDR_FAMILY_IPV6:
        ipv6 = &addr->u.ipv6;
        break;
    default:
        tb_assert_abort(0);
        break;
    }

    // ok?
    return ipv6;
}
tb_void_t tb_addr_set_ipv6(tb_addr_ref_t addr, tb_ipv6_ref_t ipv6)
{
    // check
    tb_assert_and_check_return(addr && ipv6);

    // save it
    addr->family = TB_ADDR_FAMILY_IPV6;
    addr->u.ipv6 = *ipv6;
}
tb_size_t tb_addr_family(tb_addr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(addr, TB_ADDR_FAMILY_NONE);

    // the family
    return addr->family;
}
tb_void_t tb_addr_set_family(tb_addr_ref_t addr, tb_size_t family)
{
    // check
    tb_assert_and_check_return(addr);

    // ipv4 => ipv6?
    if (addr->family == TB_ADDR_FAMILY_IPV4 && family == TB_ADDR_FAMILY_IPV6)
    {
        tb_ipv6_t temp;
        if (tb_addr_ipv4_to_ipv6(&addr->u.ipv4, &temp))
        {
            addr->family = TB_ADDR_FAMILY_IPV6;
            addr->u.ipv6 = temp;
        }
        else
        {
            // check
            tb_assert_abort(0);
        }
    }
    // ipv6 => ipv4?
    else if (addr->family == TB_ADDR_FAMILY_IPV4 && family == TB_ADDR_FAMILY_IPV6)
    {
        tb_ipv4_t temp;
        if (tb_addr_ipv6_to_ipv4(&addr->u.ipv6, &temp))
        {
            addr->family = TB_ADDR_FAMILY_IPV4;
            addr->u.ipv4 = temp;
        }
        else
        {
            // check
            tb_assert_abort(0);
        }
    }
    else addr->family = family;
}
tb_uint16_t tb_addr_port(tb_addr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(addr, 0);

    // the port
    return addr->port;
}
tb_void_t tb_addr_set_port(tb_addr_ref_t addr, tb_uint16_t port)
{
    // check
    tb_assert_and_check_return(addr && port);

    // set port
    addr->port = port;
}
