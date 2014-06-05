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
 * @file        mswsock.h
 *
 */
#ifndef TB_PLATFORM_WINDOWS_INTERFACE_MSWSOCK_H
#define TB_PLATFORM_WINDOWS_INTERFACE_MSWSOCK_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the AcceptEx func type 
typedef BOOL (WINAPI* tb_mswsock_AcceptEx_t)(   SOCKET sListenSocket
                                            ,   SOCKET sAcceptSocket
                                            ,   PVOID lpOutputBuffer
                                            ,   DWORD dwReceiveDataLength
                                            ,   DWORD dwLocalAddressLength
                                            ,   DWORD dwRemoteAddressLength
                                            ,   LPDWORD lpdwBytesReceived
                                            ,   LPOVERLAPPED lpOverlapped);

// the ConnectEx func type 
typedef BOOL (WINAPI* tb_mswsock_ConnectEx_t)(  SOCKET s
                                            ,   struct sockaddr const*name
                                            ,   tb_int_t namelen
                                            ,   PVOID lpSendBuffer
                                            ,   DWORD dwSendDataLength
                                            ,   LPDWORD lpdwBytesSent
                                            ,   LPOVERLAPPED lpOverlapped);

// the TransmitFile func type 
typedef BOOL (WINAPI* tb_mswsock_TransmitFile_t)(   SOCKET hSocket
                                                ,   HANDLE hFile
                                                ,   DWORD nNumberOfBytesToWrite
                                                ,   DWORD nNumberOfBytesPerSend
                                                ,   LPOVERLAPPED lpOverlapped
                                                ,   LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers
                                                ,   DWORD dwReserved);

// the mswsock interfaces type
typedef struct __tb_mswsock_t
{
    // AcceptEx
    tb_mswsock_AcceptEx_t                       AcceptEx;

    // ConnectEx
    tb_mswsock_ConnectEx_t                      ConnectEx;

    // TransmitFile
    tb_mswsock_TransmitFile_t                   TransmitFile;

}tb_mswsock_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* the mswsock interfaces
 *
 * @return      the mswsock interfaces pointer
 */
tb_mswsock_t*   tb_mswsock(tb_noarg_t);


#endif
