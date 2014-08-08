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

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_network_init()
{
    // init dns server
    if (!tb_dns_server_init()) return tb_false;

    // init dns cache
    if (!tb_dns_cache_init()) return tb_false;

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
