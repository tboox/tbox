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
#include <ifaddrs.h>
#ifdef TB_CONFIG_OS_LINUX
#   include <linux/if.h>
#else
#   include <net/if.h>
#endif
#include <net/if_dl.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// Ethernet CSMACD
#if !defined(IFT_ETHER)
#   define IFT_ETHER                    (0x6)
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

    // query the list of interfaces.
    struct ifaddrs* list = tb_null;
    if (getifaddrs(&list) < 0 || !list) return tb_false;

    // find the given interface
    tb_bool_t       ok = tb_false;
    struct ifaddrs* interface = tb_null;
    for (interface = list; interface; interface = interface->ifa_next)
    {
        // is this address?
        if (    interface->ifa_addr
            &&  (interface->ifa_addr->sa_family == AF_LINK)
            && (((struct sockaddr_dl const*)interface->ifa_addr)->sdl_type == IFT_ETHER)) 
        {
            // is this interface?
            if (!interface_name || (interface->ifa_name && !tb_strcmp(interface->ifa_name, interface_name)))
            {
                // the address data
                struct sockaddr_dl const*   addr = (struct sockaddr_dl const*)interface->ifa_addr;
                tb_byte_t const*            base = (tb_byte_t const*)(addr->sdl_data + addr->sdl_nlen);

                // save the mac address
                if (addr->sdl_alen > 5)
                {
                    // copy it
                    tb_memcpy(mac_address, base, 6);

                    // ok
                    ok = tb_true;
                }

                // end
                break;
            }
        }
    }

    // exit the interface list
    freeifaddrs(list);

    // ok?
    return ok;
}

