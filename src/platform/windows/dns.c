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
 * @file        dns.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../dynamic.h"
#include "interface/interface.h"
#include "../../network/network.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_dns_init()
{
    // done
    FIXED_INFO*             info = tb_null;
    ULONG                   size = 0;
    tb_size_t               count = 0;
    do 
    {
        // init func
        tb_iphlpapi_GetNetworkParams_t pGetNetworkParams = tb_iphlpapi()->GetNetworkParams;
        tb_assert_and_check_break(pGetNetworkParams);

        // init info
        info = tb_malloc0_type(FIXED_INFO);
        tb_assert_and_check_break(info);

        // get the info size
        size = sizeof(FIXED_INFO);
        if (pGetNetworkParams(info, &size) == ERROR_BUFFER_OVERFLOW) 
        {
            // grow info
            info = (FIXED_INFO *)tb_ralloc(info, size);
            tb_assert_and_check_break(info);
        }
        
        // get the info
        if (pGetNetworkParams(info, &size) != NO_ERROR) break;

        // trace
//      tb_trace_d("host: %s",  info->HostName);
//      tb_trace_d("domain: %s", info->DomainName);
        tb_trace_d("server: %s", info->DnsServerList.IpAddress.String);

        // add the first dns address
        if (info->DnsServerList.IpAddress.String)
        {
            tb_dns_server_add(info->DnsServerList.IpAddress.String);
            count++;
        }

        // walk dns address
        IP_ADDR_STRING* addr = info->DnsServerList.Next;
        for (; addr; addr = addr->Next) 
        {
            // trace
            tb_trace_d("server: %s", addr->IpAddress.String);
            
            // add the dns address
            if (addr->IpAddress.String)
            {
                tb_dns_server_add(addr->IpAddress.String);
                count++;
            }
        }

    } while (0);

    // exit info
    if (info) tb_free(info);
    info = tb_null;

    // ok
    return tb_true;
}
tb_void_t tb_dns_exit()
{
}

