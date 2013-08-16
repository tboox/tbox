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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		dns.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 		"dns"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <winsock2.h>
#include <iphlpapi.h>
#include "../dynamic.h"
#include "../../network/network.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */
typedef DWORD (*tb_GetNetworkParams_t)(PFIXED_INFO pinfo, PULONG psize);

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_void_t tb_dns_local_init()
{
	// done
	FIXED_INFO* 			info = tb_null;
	ULONG 					size = 0;
	tb_handle_t 			hdll = tb_null;
	tb_GetNetworkParams_t 	func = tb_null;
	do 
	{
		// init dynamic
		hdll = tb_dynamic_init("iphlpapi.dll");
		tb_assert_and_check_break(hdll);

		// init func
		func = (tb_GetNetworkParams_t)tb_dynamic_func(hdll, "GetNetworkParams");
		tb_assert_and_check_break(func);

		// init info
		info = tb_malloc0(sizeof(FIXED_INFO));
		tb_assert_and_check_break(info);

		// get the info size
		size = sizeof(FIXED_INFO);
		if (func(info, &size) == ERROR_BUFFER_OVERFLOW) 
		{
			// grow info
			info = (FIXED_INFO *)tb_ralloc(info, size);
			tb_assert_and_check_break(info);
		}
		
		// get the info
		if (func(info, &size) != NO_ERROR) break;

		// trace
//		tb_trace_impl("host: %s", 	info->HostName);
//		tb_trace_impl("domain: %s", info->DomainName);
		tb_trace_impl("dns: %s", 	info->DnsServerList.IpAddress.String);

		// add the first dns address
		if (info->DnsServerList.IpAddress.String)
			tb_dns_list_adds(info->DnsServerList.IpAddress.String);

		// walk dns address
        IP_ADDR_STRING* addr = info->DnsServerList.Next;
        for (; addr; addr = addr->Next) 
		{
			// trace
			tb_trace_impl("dns: %s", addr->IpAddress.String);
			
			// add the dns address
			if (addr->IpAddress.String)
				tb_dns_list_adds(addr->IpAddress.String);
        }

	} while (0);

	// exit info
	if (info) tb_free(info);
	info = tb_null;

	// exit dll
	if (hdll) tb_dynamic_exit(hdll);
	hdll = tb_null;
}

