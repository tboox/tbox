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
 * @file        ifaddrs.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../ifaddrs.h"
#include <ifaddrs.h>
#ifdef TB_CONFIG_OS_LINUX
#   include <linux/if.h>
#else
#   include <net/if.h>
#endif
#include <net/if_dl.h>
#include <netinet/in.h>
#include "sockaddr.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_ifaddrs_interface_exit(tb_item_func_t* func, tb_pointer_t buff)
{
    // check
    tb_ifaddrs_interface_ref_t interface = (tb_ifaddrs_interface_ref_t)buff;
    if (interface)
    {
        // exit the interface name
        if (interface->name) tb_free(interface->name);
        interface->name = tb_null;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_ifaddrs_ref_t tb_ifaddrs_init()
{
    // init it
    return (tb_ifaddrs_ref_t)tb_list_init(8, tb_item_func_mem(sizeof(tb_ifaddrs_interface_t), tb_ifaddrs_interface_exit, tb_null));
}
tb_void_t tb_ifaddrs_exit(tb_ifaddrs_ref_t ifaddrs)
{
    // exit it
    if (ifaddrs) tb_list_exit((tb_list_ref_t)ifaddrs);
}
tb_iterator_ref_t tb_ifaddrs_itor(tb_ifaddrs_ref_t ifaddrs, tb_bool_t reload)
{
    // check
    tb_list_ref_t interfaces = (tb_list_ref_t)ifaddrs;
    tb_assert_and_check_return_val(interfaces, tb_null);

    // uses the cached interfaces?
    tb_check_return_val(reload, (tb_iterator_ref_t)interfaces); 

    // clear interfaces first
    tb_list_clear(interfaces);

    // query the list of interfaces.
    struct ifaddrs* list = tb_null;
    if (!getifaddrs(&list) && list)
    {
        // done
        struct ifaddrs* item = tb_null;
        for (item = list; item; item = item->ifa_next)
        {
            // check
            tb_check_continue(item->ifa_addr && item->ifa_name);

            /* attempt to get the interface from the cached interfaces
             * and make a new interface if no the cached interface
             */
            tb_ifaddrs_interface_t      interface_new = {0};
            tb_ifaddrs_interface_ref_t  interface = tb_ifaddrs_interface_find((tb_iterator_ref_t)interfaces, item->ifa_name);
            if (!interface) interface = &interface_new;

            // check
            tb_assert_abort(interface == &interface_new || interface->name);

            // done
            switch (item->ifa_addr->sa_family)
            {
            case AF_INET:
                {
                    // the address
                    struct sockaddr_storage const* addr = (struct sockaddr_storage const*)item->ifa_addr;

                    // save ipaddr4
                    tb_ipaddr_t ipaddr4;
                    if (!tb_sockaddr_save(&ipaddr4, addr)) break;
                    interface->ipaddr4 = ipaddr4.u.ipv4;

                    // save flags
                    interface->flags |= TB_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR4;
                    if ((item->ifa_flags & IFF_LOOPBACK) || tb_ipaddr_ip_is_loopback(&ipaddr4)) 
                        interface->flags |= TB_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK;

                    // new interface? save it
                    if (interface == &interface_new)
                    {
                        // save interface name
                        interface->name = tb_strdup(item->ifa_name);
                        tb_assert_abort(interface->name);

                        // save interface
                        tb_list_insert_tail(interfaces, interface);
                    }
                }
                break;
            case AF_INET6:
                {
                    // the address
                    struct sockaddr_storage const* addr = (struct sockaddr_storage const*)item->ifa_addr;

                    // save ipaddr6
                    tb_ipaddr_t ipaddr6;
                    if (!tb_sockaddr_save(&ipaddr6, addr)) break;
                    interface->ipaddr6 = ipaddr6.u.ipv6;

                    // save flags
                    interface->flags |= TB_IFADDRS_INTERFACE_FLAG_HAVE_IPADDR6;
                    if ((item->ifa_flags & IFF_LOOPBACK) || tb_ipaddr_ip_is_loopback(&ipaddr6)) 
                        interface->flags |= TB_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK;

                    // new interface? save it
                    if (interface == &interface_new)
                    {
                        // save interface name
                        interface->name = tb_strdup(item->ifa_name);
                        tb_assert_abort(interface->name);

                        // save interface
                        tb_list_insert_tail(interfaces, interface);
                    }
                }
            case AF_LINK:
                {
                    // the address data
                    struct sockaddr_dl const*   addr = (struct sockaddr_dl const*)item->ifa_addr;
                    tb_byte_t const*            base = (tb_byte_t const*)(addr->sdl_data + addr->sdl_nlen);

                    // check
                    tb_check_break(addr->sdl_alen == sizeof(interface->hwaddr.u8));

                    // save flags
                    interface->flags |= TB_IFADDRS_INTERFACE_FLAG_HAVE_HWADDR;
                    if (item->ifa_flags & IFF_LOOPBACK) interface->flags |= TB_IFADDRS_INTERFACE_FLAG_IS_LOOPBACK;

                    // save hwaddr
                    tb_memcpy(interface->hwaddr.u8, base, sizeof(interface->hwaddr.u8));

                    // new interface? save it
                    if (interface == &interface_new)
                    {
                        // save interface name
                        interface->name = tb_strdup(item->ifa_name);
                        tb_assert_abort(interface->name);

                        // save interface
                        tb_list_insert_tail(interfaces, interface);
                    }
                }
                break;
            default:
                {
                    // trace
                    tb_trace_d("unknown family: %d", item->ifa_addr->sa_family);
                }
                break;
            }
        }

        // exit the interface list
        freeifaddrs(list);
    }

    // ok?
    return (tb_iterator_ref_t)interfaces;
}
