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
#include "interface/interface.h"

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
    tb_bool_t           ok = tb_false;
    PIP_ADAPTER_INFO    adapter_info = tb_null;
    do
    {
        // make the adapter info 
        adapter_info = tb_malloc0_type(IP_ADAPTER_INFO);
        tb_assert_and_check_break(adapter_info);

        // get the real adapter info size
        ULONG size = sizeof(IP_ADAPTER_INFO);
        if (tb_iphlpapi()->GetAdaptersInfo(adapter_info, &size) == ERROR_BUFFER_OVERFLOW)
        {
            // grow the adapter info buffer
            adapter_info = (PIP_ADAPTER_INFO)tb_ralloc(adapter_info, size);
            tb_assert_and_check_break(adapter_info);

            // reclear it
            tb_memset(adapter_info, 0, size);
        }
    
        // get the adapter info 
        if (tb_iphlpapi()->GetAdaptersInfo(adapter_info, &size) != NO_ERROR) break;

        // find the given interface
        PIP_ADAPTER_INFO adapter = adapter_info;
        while (adapter && !ok)
        {
            if (adapter->Type == MIB_IF_TYPE_ETHERNET)
            {
                // trace name
                tb_trace_d("AdapterName: %s", adapter->AdapterName);
                tb_trace_d("Description: %s", adapter->Description);

                // is this interface?
                if (!interface_name || (adapter->AdapterName && !tb_strcmp(adapter->AdapterName, interface_name)))
                {
                    // save the mac address 
                    if (adapter->AddressLength > 5)
                    {
                        // copy it
                        tb_memcpy(mac_address, adapter->Address, 6);

                        // ok
                        ok = tb_true;
                    }

                    // end
                    break;
                }
            }
 
            // the next adapter
            adapter = adapter->Next;
        }

    } while (0);

    // exit the adapter info
    if (adapter_info) tb_free(adapter_info);
    adapter_info = tb_null;

    // ok?
    return ok;
}

