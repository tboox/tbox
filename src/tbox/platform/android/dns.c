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
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../dns.h"
#include "../../network/network.h"
#include <sys/system_properties.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t tb_dns_init()
{
    // done
    tb_size_t count = 0;
    for (count = 0; count < 6; count++)
    {
        // init the dns property name
        tb_char_t prop_name[PROP_NAME_MAX] = {0};
        tb_snprintf(prop_name, sizeof(prop_name) - 1, "net.dns%lu", count + 1);
        
        // get dns address name
        tb_char_t dns[64] = {0};
        if (!__system_property_get(prop_name, dns)) break;

        // trace
        tb_trace_d("addr: %s", dns);

        // add server
        tb_dns_server_add(dns);
    }

    // ok
    return tb_true;
}
tb_void_t tb_dns_exit()
{
}

