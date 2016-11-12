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
 * @file        network.c
 * @ingroup     network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "network.h"
#include "../network.h"
#include "../../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_long_t tb_network_printf_format_ipv4(tb_cpointer_t object, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(object && cstr && maxn, -1);

    // the ipv4
    tb_ipv4_ref_t ipv4 = (tb_ipv4_ref_t)object;

    // make it
    cstr = (tb_char_t*)tb_ipv4_cstr(ipv4, cstr, maxn);

    // ok?
    return cstr? tb_strlen(cstr) : -1;
}
static tb_long_t tb_network_printf_format_ipv6(tb_cpointer_t object, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(object && cstr && maxn, -1);

    // the ipv6
    tb_ipv6_ref_t ipv6 = (tb_ipv6_ref_t)object;

    // make it
    cstr = (tb_char_t*)tb_ipv6_cstr(ipv6, cstr, maxn);

    // ok?
    return cstr? tb_strlen(cstr) : -1;
}
static tb_long_t tb_network_printf_format_ipaddr(tb_cpointer_t object, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(object && cstr && maxn, -1);

    // the ipaddr
    tb_ipaddr_ref_t ipaddr = (tb_ipaddr_ref_t)object;

    // make it
    cstr = (tb_char_t*)tb_ipaddr_cstr(ipaddr, cstr, maxn);

    // ok?
    return cstr? tb_strlen(cstr) : -1;
}
static tb_long_t tb_network_printf_format_hwaddr(tb_cpointer_t object, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(object && cstr && maxn, -1);

    // the hwaddr
    tb_hwaddr_ref_t hwaddr = (tb_hwaddr_ref_t)object;

    // make it
    cstr = (tb_char_t*)tb_hwaddr_cstr(hwaddr, cstr, maxn);

    // ok?
    return cstr? tb_strlen(cstr) : -1;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_network_init_env()
{
    // init dns server
    if (!tb_dns_server_init()) return tb_false;

    // init dns cache
    if (!tb_dns_cache_init()) return tb_false;

    // register printf("%{ipv4}", &ipv4);
    tb_printf_object_register("ipv4", tb_network_printf_format_ipv4);

    // register printf("%{ipv6}", &ipv6);
    tb_printf_object_register("ipv6", tb_network_printf_format_ipv6);

    // register printf("%{ipaddr}", &ipaddr);
    tb_printf_object_register("ipaddr", tb_network_printf_format_ipaddr);

    // register printf("%{hwaddr}", &hwaddr);
    tb_printf_object_register("hwaddr", tb_network_printf_format_hwaddr);

    // ok
    return tb_true;
}
tb_void_t tb_network_exit_env()
{
    // exit dns cache
    tb_dns_cache_exit();

    // exit dns server
    tb_dns_server_exit();
}
