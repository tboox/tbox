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
 * @file        iphlpapi.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "iphlpapi.h"
#include "../../../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_iphlpapi_instance_init(tb_handle_t instance)
{
    // check
    tb_iphlpapi_ref_t iphlpapi = (tb_iphlpapi_ref_t)instance;
    tb_assert_and_check_return_val(iphlpapi, tb_false);

    // the iphlpapi module
    HANDLE module = GetModuleHandleA("iphlpapi.dll");
    if (!module) module = tb_dynamic_init("iphlpapi.dll");
    tb_check_return_val(module, tb_false);

    // init interfaces
    TB_INTERFACE_LOAD(iphlpapi, GetNetworkParams);

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_iphlpapi_ref_t tb_iphlpapi()
{
    // init
    static tb_atomic_t      s_binited = 0;
    static tb_iphlpapi_t    s_iphlpapi = {0};

    // init the static instance
    tb_bool_t ok = tb_singleton_static_init(&s_binited, &s_iphlpapi, tb_iphlpapi_instance_init);
    tb_assert(ok); tb_used(ok);

    // ok
    return &s_iphlpapi;
}
