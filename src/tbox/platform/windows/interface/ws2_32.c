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
 * @file        ws2_32.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "ws2_32.h"
#include "../../../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_ws2_32_instance_init(tb_handle_t instance)
{
    // check
    tb_ws2_32_ref_t ws2_32 = (tb_ws2_32_ref_t)instance;
    tb_assert_and_check_return_val(ws2_32, tb_false);

    // the ws2_32 module
    HANDLE module = GetModuleHandleA("ws2_32.dll");
    if (!module) module = tb_dynamic_init("ws2_32.dll");
    tb_assert_and_check_return_val(module, tb_false);

    // init interfaces
    TB_INTERFACE_LOAD(ws2_32, WSAStartup);
    TB_INTERFACE_LOAD(ws2_32, WSACleanup);
    TB_INTERFACE_LOAD(ws2_32, WSASocketA);
    TB_INTERFACE_LOAD(ws2_32, WSAIoctl);
    TB_INTERFACE_LOAD(ws2_32, WSAGetLastError);
    TB_INTERFACE_LOAD(ws2_32, WSASend);
    TB_INTERFACE_LOAD(ws2_32, WSARecv);
    TB_INTERFACE_LOAD(ws2_32, WSASendTo);
    TB_INTERFACE_LOAD(ws2_32, WSARecvFrom);
    TB_INTERFACE_LOAD(ws2_32, bind);
    TB_INTERFACE_LOAD(ws2_32, send);
    TB_INTERFACE_LOAD(ws2_32, recv);
    TB_INTERFACE_LOAD(ws2_32, sendto);
    TB_INTERFACE_LOAD(ws2_32, recvfrom);
    TB_INTERFACE_LOAD(ws2_32, accept);
    TB_INTERFACE_LOAD(ws2_32, listen);
    TB_INTERFACE_LOAD(ws2_32, select);
    TB_INTERFACE_LOAD(ws2_32, connect);
    TB_INTERFACE_LOAD(ws2_32, shutdown);
    TB_INTERFACE_LOAD(ws2_32, getsockname);
    TB_INTERFACE_LOAD(ws2_32, getsockopt);
    TB_INTERFACE_LOAD(ws2_32, setsockopt);
    TB_INTERFACE_LOAD(ws2_32, ioctlsocket);
    TB_INTERFACE_LOAD(ws2_32, closesocket);
    TB_INTERFACE_LOAD(ws2_32, gethostname);
    TB_INTERFACE_LOAD(ws2_32, __WSAFDIsSet);

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_ws2_32_ref_t tb_ws2_32()
{
    // init
    static tb_atomic_t      s_binited = 0;
    static tb_ws2_32_t      s_ws2_32 = {0};

    // init the static instance
    tb_bool_t ok = tb_singleton_static_init(&s_binited, &s_ws2_32, tb_ws2_32_instance_init);
    tb_assert(ok); tb_used(ok);

    // ok
    return &s_ws2_32;
}
