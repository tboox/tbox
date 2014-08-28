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
 * @file        network.c
 * @defgroup    network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "network.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_long_t tb_network_printf_format_ipv4(tb_cpointer_t object, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(object && cstr && maxn, -1);

    // the ipv4
    tb_ipv4_t const* ipv4 = (tb_ipv4_t const*)object;

    // format
    tb_long_t size = tb_snprintf(cstr, maxn - 1, "%u.%u.%u.%u", ipv4->u8[0], ipv4->u8[1], ipv4->u8[2], ipv4->u8[3]);
    if (size >= 0) cstr[size] = '\0';

    // ok?
    return size;
}
    
/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_network_init()
{
    // init dns server
    if (!tb_dns_server_init()) return tb_false;

    // init dns cache
    if (!tb_dns_cache_init()) return tb_false;

    // register printf("%{ipv4}", &ipv4);
    tb_printf_object_register("ipv4", tb_network_printf_format_ipv4);

    // ok
    return tb_true;
}
tb_void_t tb_network_exit()
{
    // exit dns cache
    tb_dns_cache_exit();

    // exit dns server
    tb_dns_server_exit();
}
