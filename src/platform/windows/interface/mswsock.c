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
 * @file        mswsock.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "mswsock.h"
#include "ws2_32.h"
#include "../../socket.h"
#include "../../../utils/singleton.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// from mswsock.h
#define TB_MSWSOCK_WSAID_ACCEPTEX                   {0xb5367df1, 0xcbac, 0x11cf, {0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}}
#define TB_MSWSOCK_WSAID_TRANSMITFILE               {0xb5367df0, 0xcbac, 0x11cf, {0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}}
#define TB_MSWSOCK_WSAID_GETACCEPTEXSOCKADDRS       {0xb5367df2, 0xcbac, 0x11cf, {0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}}
#define TB_MSWSOCK_WSAID_CONNECTEX                  {0x25a207b9, 0xddf3, 0x4660, {0x8e, 0xe9, 0x76, 0xe5, 0x8c, 0x74, 0x06, 0x3e}}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_mswsock_instance_init(tb_handle_t instance)
{
    // check
    tb_mswsock_t* mswsock = (tb_mswsock_t*)instance;
    tb_assert_and_check_return_val(mswsock, tb_false);

    // done
    tb_handle_t sock = tb_null;
    do
    {
        // init sock
        sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
        tb_assert_and_check_break(sock);

        // init AcceptEx
        DWORD   AcceptEx_real = 0;
        GUID    AcceptEx_guid = TB_MSWSOCK_WSAID_ACCEPTEX;
        tb_ws2_32()->WSAIoctl(  (SOCKET)sock - 1
                            ,   SIO_GET_EXTENSION_FUNCTION_POINTER
                            ,   &AcceptEx_guid
                            ,   sizeof(GUID)
                            ,   &mswsock->AcceptEx
                            ,   sizeof(tb_mswsock_AcceptEx_t)
                            ,   &AcceptEx_real
                            ,   tb_null
                            ,   tb_null);

        // init ConnectEx
        DWORD   ConnectEx_real = 0;
        GUID    ConnectEx_guid = TB_MSWSOCK_WSAID_CONNECTEX;
        tb_ws2_32()->WSAIoctl(  (SOCKET)sock - 1
                            ,   SIO_GET_EXTENSION_FUNCTION_POINTER
                            ,   &ConnectEx_guid
                            ,   sizeof(GUID)
                            ,   &mswsock->ConnectEx
                            ,   sizeof(tb_mswsock_ConnectEx_t)
                            ,   &ConnectEx_real
                            ,   tb_null
                            ,   tb_null);

        // init TransmitFile
        DWORD   TransmitFile_real = 0;
        GUID    TransmitFile_guid = TB_MSWSOCK_WSAID_TRANSMITFILE;
        tb_ws2_32()->WSAIoctl(  (SOCKET)sock - 1
                            ,   SIO_GET_EXTENSION_FUNCTION_POINTER
                            ,   &TransmitFile_guid
                            ,   sizeof(GUID)
                            ,   &mswsock->TransmitFile
                            ,   sizeof(tb_mswsock_TransmitFile_t)
                            ,   &TransmitFile_real
                            ,   tb_null
                            ,   tb_null);
    } while (0);

    // exit sock
    if (sock) tb_socket_clos(sock);

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_mswsock_t* tb_mswsock()
{
    // init
    static tb_atomic_t      s_binited = 0;
    static tb_mswsock_t     s_mswsock = {0};

    // init the static instance
    tb_bool_t ok = tb_singleton_static_init(&s_binited, &s_mswsock, tb_mswsock_instance_init);
    tb_assert(ok);

    // ok
    return &s_mswsock;
}
