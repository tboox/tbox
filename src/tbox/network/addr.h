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
 * @file        address.h
 * @ingroup     network
 *
 */
#ifndef TB_NETWORK_ADDR_H
#define TB_NETWORK_ADDR_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "ipv4.h"
#include "ipv6.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the address family enum
typedef enum __tb_addr_family_e
{
    TB_ADDR_FAMILY_NONE     = 0
,   TB_ADDR_FAMILY_IPV4     = 1
,   TB_ADDR_FAMILY_IPV6     = 2

}tb_addr_family_e;

/// the address type
typedef struct __tb_addr_t
{
    /// the family
    tb_uint8_t              family;

    /// the port
    tb_uint16_t             port;

    /// the address
    union
    {
        /// the ipv4
        tb_ipv4_t           ipv4;

        /// the ipv6
        tb_ipv6_t           ipv6;

    }u;

}tb_addr_t, *tb_addr_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! clear the address
 *
 * @param addr      the address
 */
tb_void_t           tb_addr_clear(tb_addr_ref_t addr);

/*! get the ip address string
 *
 * @param addr      the address
 * @param data      the address string data
 * @param maxn      the address string data maxn
 *
 * @return          the address string
 */
tb_char_t const*    tb_addr_ip_cstr(tb_addr_ref_t addr, tb_char_t* data, tb_size_t maxn);

/*! set the ip address from string
 *
 * @param addr      the address, only analyze format if be null
 * @param cstr      the address string
 * @param family    the address family, will analyze family automaticly if be none
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_addr_set_ip_cstr(tb_addr_ref_t addr, tb_char_t const* cstr, tb_uint8_t family);

/*! get the ipv4 address
 *
 * @param addr      the address
 *
 * @return          the ipv4
 */
tb_ipv4_ref_t       tb_addr_ipv4(tb_addr_ref_t addr);

/*! set the address from ipv4
 *
 * @param addr      the address
 * @param ipv4      the ipv4
 */
tb_void_t           tb_addr_set_ipv4(tb_addr_ref_t addr, tb_ipv4_ref_t ipv4);

/*! get the ipv6 address
 *
 * @param addr      the address
 *
 * @return          the ipv6
 */
tb_ipv6_ref_t       tb_addr_ipv6(tb_addr_ref_t addr);

/*! set the address from ipv6
 *
 * @param addr      the address
 * @param ipv6      the ipv6
 */
tb_void_t           tb_addr_set_ipv6(tb_addr_ref_t addr, tb_ipv6_ref_t ipv6);

/*! get the address family
 *
 * @param addr      the address
 *
 * @return          the family
 */
tb_uint8_t          tb_addr_family(tb_addr_ref_t addr);

/*! set the address family
 *
 * @param addr      the address
 * @param family    the family
 */
tb_void_t           tb_addr_set_family(tb_addr_ref_t addr, tb_uint8_t family);

/*! get the address port
 *
 * @param addr      the address
 *
 * @return          the port
 */
tb_uint16_t         tb_addr_port(tb_addr_ref_t addr);

/*! set the address family
 *
 * @param addr      the address
 * @param port      the port
 */
tb_void_t           tb_addr_set_port(tb_addr_ref_t addr, tb_uint16_t port);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
