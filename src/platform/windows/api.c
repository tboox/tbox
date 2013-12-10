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
 * @file		api.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "api.h"
#include "../socket.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// from mswsock.h
#define TB_API_WSAID_ACCEPTEX 					{0xb5367df1, 0xcbac, 0x11cf, {0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}}
#define TB_API_WSAID_TRANSMITFILE 				{0xb5367df0, 0xcbac, 0x11cf, {0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}}
#define TB_API_WSAID_GETACCEPTEXSOCKADDRS 		{0xb5367df2, 0xcbac, 0x11cf, {0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}}
#define TB_API_WSAID_CONNECTEX 					{0x25a207b9, 0xddf3, 0x4660, {0x8e, 0xe9, 0x76, 0xe5, 0x8c, 0x74, 0x06, 0x3e}}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_api_AcceptEx_t tb_api_AcceptEx()
{
	// done
	tb_long_t 			ok = -1;
	tb_handle_t 		sock = tb_null;
	tb_api_AcceptEx_t 	AcceptEx_func = tb_null;
	do
	{
		// init sock
		sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
		tb_assert_and_check_break(sock);

		// get the AcceptEx func address
		DWORD 	real = 0;
		GUID 	guid = TB_API_WSAID_ACCEPTEX;
		ok = WSAIoctl( 	(SOCKET)sock - 1
					, 	SIO_GET_EXTENSION_FUNCTION_POINTER
					, 	&guid
					, 	sizeof(GUID)
					, 	&AcceptEx_func
					, 	sizeof(tb_api_AcceptEx_t)
					, 	&real
					, 	tb_null
					, 	tb_null);
		tb_assert_and_check_break(!ok && AcceptEx_func);

	} while (0);

	// exit sock
	if (sock) tb_socket_close(sock);

	// ok?
	return !ok? AcceptEx_func : tb_null;
}
tb_api_ConnectEx_t tb_api_ConnectEx()
{
	// done
	tb_long_t 			ok = -1;
	tb_handle_t 		sock = tb_null;
	tb_api_ConnectEx_t 	ConnectEx_func = tb_null;
	do
	{
		// init sock
		sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
		tb_assert_and_check_break(sock);

		// get the ConnectEx func address
		DWORD 	real = 0;
		GUID 	guid = TB_API_WSAID_CONNECTEX;
		ok = WSAIoctl( 	(SOCKET)sock - 1
					, 	SIO_GET_EXTENSION_FUNCTION_POINTER
					, 	&guid
					, 	sizeof(GUID)
					, 	&ConnectEx_func
					, 	sizeof(tb_api_ConnectEx_t)
					, 	&real
					, 	tb_null
					, 	tb_null);
		tb_assert_and_check_break(!ok && ConnectEx_func);

	} while (0);

	// exit sock
	if (sock) tb_socket_close(sock);

	// ok?
	return !ok? ConnectEx_func : tb_null;
}
tb_api_TransmitFile_t tb_api_TransmitFile()
{
	// done
	tb_long_t 				ok = -1;
	tb_handle_t 			sock = tb_null;
	tb_api_TransmitFile_t 	TransmitFile_func = tb_null;
	do
	{
		// init sock
		sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
		tb_assert_and_check_break(sock);

		// get the TransmitFile func address
		DWORD 	real = 0;
		GUID 	guid = TB_API_WSAID_TRANSMITFILE;
		ok = WSAIoctl( 	(SOCKET)sock - 1
					, 	SIO_GET_EXTENSION_FUNCTION_POINTER
					, 	&guid
					, 	sizeof(GUID)
					, 	&TransmitFile_func
					, 	sizeof(tb_api_TransmitFile_t)
					, 	&real
					, 	tb_null
					, 	tb_null);
		tb_assert_and_check_break(!ok && TransmitFile_func);

	} while (0);

	// exit sock
	if (sock) tb_socket_close(sock);

	// ok?
	return !ok? TransmitFile_func : tb_null;
}
tb_api_CancelIoEx_t tb_api_CancelIoEx()
{
	// the kernel32 module
	HANDLE module = GetModuleHandleA("kernel32.dll");

	// the CancelIoEx func
	tb_api_CancelIoEx_t CancelIoEx_func = tb_null;
	if (module) CancelIoEx_func = GetProcAddress(module, "CancelIoEx");

	// ok?
	return CancelIoEx_func;
}
tb_api_GetQueuedCompletionStatusEx_t tb_api_GetQueuedCompletionStatusEx()
{
	// the kernel32 module
	HANDLE module = GetModuleHandleA("kernel32.dll");

	// the GetQueuedCompletionStatusEx func
	tb_api_GetQueuedCompletionStatusEx_t GetQueuedCompletionStatusEx_func = tb_null;
	if (module) GetQueuedCompletionStatusEx_func = GetProcAddress(module, "GetQueuedCompletionStatusEx");

	// ok?
	return GetQueuedCompletionStatusEx_func;
}
