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
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "addr.h"
#include "ws2tcpip.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_socket_addr_save(tb_addr_ref_t addr, struct sockaddr_storage const* saddr)
{
    // check
    tb_assert_and_check_return_val(addr && saddr, 0);

    // clear address
    tb_addr_clear(addr);

    // done
    tb_size_t size = 0;
    switch (saddr->ss_family)
    {
    case AF_INET:
        {
            // the ipv4 addr
            struct sockaddr_in* addr4 = (struct sockaddr_in*)saddr;

            // save family
            tb_addr_family_set(addr, TB_ADDR_FAMILY_IPV4);

            // save ipv4
            addr->u.ipv4.u32 = (tb_uint32_t)addr4->sin_addr.s_addr;

            // save port
            tb_addr_port_set(addr, tb_bits_be_to_ne_u16(addr4->sin_port));

            // save size
            size = sizeof(struct sockaddr_in);
        }
        break;
    case AF_INET6:
        {
            // the ipv6 addr
            struct sockaddr_in6* addr6 = (struct sockaddr_in6*)saddr;

            // check
            tb_assert_static(sizeof(addr->u.ipv6.u8) == sizeof(addr6->sin6_addr.s6_addr));
            tb_assert_static(tb_arrayn(addr->u.ipv6.u8) == tb_arrayn(addr6->sin6_addr.s6_addr));

            // save family
            tb_addr_family_set(addr, TB_ADDR_FAMILY_IPV6);

            // save ipv6
            tb_memcpy(addr->u.ipv6.u8, addr6->sin6_addr.s6_addr, sizeof(addr->u.ipv6.u8));

            // save port
            tb_addr_port_set(addr, tb_bits_be_to_ne_u16(addr6->sin6_port));

            // save size
            size = sizeof(struct sockaddr_in6);
        }
        break;
    default:
        tb_assert_abort(0);
        break;
    }
    
    // ok?
    return size;
}
tb_size_t tb_socket_addr_load(struct sockaddr_storage* saddr, tb_addr_ref_t addr)
{
    // check
    tb_assert_and_check_return_val(saddr && addr, 0);

    // clear address
    tb_memset(saddr, 0, sizeof(struct sockaddr_storage));

    // done
    tb_size_t size = 0;
    switch (tb_addr_family(addr))
    {
    case TB_ADDR_FAMILY_IPV4:
        {
            // the ipv4 addr
            struct sockaddr_in* addr4 = (struct sockaddr_in*)saddr;

            // save family
            addr4->sin_family = AF_INET;

            // save ipv4
            addr4->sin_addr.s_addr = tb_addr_ip_is_empty(addr)? INADDR_ANY : addr->u.ipv4.u32;

            // save port
            addr4->sin_port = tb_bits_ne_to_be_u16(tb_addr_port(addr));

            // save size
            size = sizeof(struct sockaddr_in);
        }
        break;
    case TB_ADDR_FAMILY_IPV6:
        {
            // the ipv6 addr
            struct sockaddr_in6* addr6 = (struct sockaddr_in6*)saddr;

            // check
            tb_assert_static(sizeof(addr->u.ipv6.u8) == sizeof(addr6->sin6_addr.s6_addr));
            tb_assert_static(tb_arrayn(addr->u.ipv6.u8) == tb_arrayn(addr6->sin6_addr.s6_addr));

            // save family
            addr6->sin6_family = AF_INET6;

            // save ipv6
            if (tb_addr_ip_is_empty(addr)) addr6->sin6_addr = in6addr_any;
            else tb_memcpy(addr6->sin6_addr.s6_addr, addr->u.ipv6.u8, sizeof(addr6->sin6_addr.s6_addr));

            // save port
            addr6->sin6_port = tb_bits_ne_to_be_u16(tb_addr_port(addr));

            // save size
            size = sizeof(struct sockaddr_in6);
        }
        break;
    default:
        tb_assert_abort(0);
        break;
    }
    
    // ok?
    return size;
}

