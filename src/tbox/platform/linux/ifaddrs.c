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
 * @file        hostmac.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifndef AF_LINK
#   define AF_LINK     AF_PACKET
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_hostmac(tb_char_t const* interface_name, tb_byte_t mac_address[6])
{
    // check
    tb_assert_and_check_return_val(mac_address, tb_false);

    // clear the mac address
    tb_memset(mac_address, 0, 6);

    // done
    tb_bool_t ok = tb_false;
    tb_long_t sock = 0;
    do
    {
        // query the list of interfaces.
        struct ifaddrs* list = tb_null;
        if (getifaddrs(&list) < 0 || !list) break;

        // find the given interface
        struct ifaddrs* interface = tb_null;
        for (interface = list; interface && !ok; interface = interface->ifa_next)
        {
            // is this address?
            if (    interface->ifa_addr
                &&  interface->ifa_addr->sa_family == AF_INET
                &&  !(interface->ifa_flags & IFF_LOOPBACK))
            {
                // is this interface?
                if (!interface_name || (interface->ifa_name && !tb_strcmp(interface->ifa_name, interface_name)))
                {
                    // make socket
                    sock = socket(AF_INET, SOCK_DGRAM, 0);

                    // init interfaces
                    struct ifreq ifr;
                    tb_memset(&ifr, 0, sizeof(struct ifreq));
                    tb_strcpy(ifr.ifr_name, interface->ifa_name);

                    // get the mac address
                    if (ioctl(sock, SIOCGIFHWADDR, &ifr) < 0) break;

                    // save the mac address
                    tb_memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);

                    // ok
                    ok = tb_true;

                    // end
                    break;
                }
            }
        }

        // exit the interface list
        freeifaddrs(list);

    } while (0);

    // exit socket
    if (sock) close(sock);
    sock = 0;

    // ok?
    return ok;
}

