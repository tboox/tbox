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
 * @author		ruki
 * @file		api.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "api.h"
#include "../atomic.h"
#include "../socket.h"
#include "../dynamic.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// from mswsock.h
#define TB_API_WSAID_ACCEPTEX 					{0xb5367df1, 0xcbac, 0x11cf, {0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}}
#define TB_API_WSAID_FLTRSMITFILE 				{0xb5367df0, 0xcbac, 0x11cf, {0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}}
#define TB_API_WSAID_GETACCEPTEXSOCKADDRS 		{0xb5367df2, 0xcbac, 0x11cf, {0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}}
#define TB_API_WSAID_CONNECTEX 					{0x25a207b9, 0xddf3, 0x4660, {0x8e, 0xe9, 0x76, 0xe5, 0x8c, 0x74, 0x06, 0x3e}}

/* //////////////////////////////////////////////////////////////////////////////////////
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
	if (sock) tb_socket_clos(sock);

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
	if (sock) tb_socket_clos(sock);

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
		GUID 	guid = TB_API_WSAID_FLTRSMITFILE;
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
	if (sock) tb_socket_clos(sock);

	// ok?
	return !ok? TransmitFile_func : tb_null;
}
tb_api_CancelIoEx_t tb_api_CancelIoEx()
{
	// init
	static tb_atomic_t s_pCancelIoEx = 0;

	// try getting it
	tb_api_CancelIoEx_t pCancelIoEx = tb_null;
	if (!(pCancelIoEx = (tb_api_CancelIoEx_t)tb_atomic_get(&s_pCancelIoEx))) 
	{
		// the kernel32 module
		HANDLE module = GetModuleHandleA("kernel32.dll");

		// the CancelIoEx func
		if (module) 
		{
			// get it
			pCancelIoEx = GetProcAddress(module, "CancelIoEx");

			// save it
			tb_atomic_set(&s_pCancelIoEx, pCancelIoEx);
		}
	}

	// ok?
	return pCancelIoEx;
}
tb_api_GetQueuedCompletionStatusEx_t tb_api_GetQueuedCompletionStatusEx()
{
	// init
	static tb_atomic_t s_pGetQueuedCompletionStatusEx = 0;

	// try getting it
	tb_api_GetQueuedCompletionStatusEx_t pGetQueuedCompletionStatusEx = tb_null;
	if (!(pGetQueuedCompletionStatusEx = (tb_api_GetQueuedCompletionStatusEx_t)tb_atomic_get(&s_pGetQueuedCompletionStatusEx))) 
	{
		// the kernel32 module
		HANDLE module = GetModuleHandleA("kernel32.dll");

		// the GetQueuedCompletionStatusEx func
		if (module) 
		{
			// get it
			pGetQueuedCompletionStatusEx = GetProcAddress(module, "GetQueuedCompletionStatusEx");

			// save it
			tb_atomic_set(&s_pGetQueuedCompletionStatusEx, pGetQueuedCompletionStatusEx);
		}
	}

	// ok?
	return pGetQueuedCompletionStatusEx;
}
tb_api_CaptureStackBackTrace_t tb_api_CaptureStackBackTrace()
{
	// init
	static tb_atomic_t s_pCaptureStackBackTrace = 0;

	// try getting it
	tb_api_CaptureStackBackTrace_t pCaptureStackBackTrace = tb_null;
	if (!(pCaptureStackBackTrace = (tb_api_CaptureStackBackTrace_t)tb_atomic_get(&s_pCaptureStackBackTrace))) 
	{
		// the kernel32 module
		HANDLE module = GetModuleHandleA("kernel32.dll");

		// the CaptureStackBackTrace func
		if (module) 
		{
			// get it
			pCaptureStackBackTrace = GetProcAddress(module, "CaptureStackBackTrace");

			// save it
			tb_atomic_set(&s_pCaptureStackBackTrace, pCaptureStackBackTrace);
		}
	}

	// ok?
	return pCaptureStackBackTrace;
}
tb_api_GetFileSizeEx_t tb_api_GetFileSizeEx()
{
	// init
	static tb_atomic_t s_pGetFileSizeEx = 0;

	// try getting it
	tb_api_GetFileSizeEx_t pGetFileSizeEx = tb_null;
	if (!(pGetFileSizeEx = (tb_api_GetFileSizeEx_t)tb_atomic_get(&s_pGetFileSizeEx))) 
	{
		// the kernel32 module
		HANDLE module = GetModuleHandleA("kernel32.dll");

		// the GetFileSizeEx func
		if (module) 
		{
			// get it
			pGetFileSizeEx = GetProcAddress(module, "GetFileSizeEx");

			// save it
			tb_atomic_set(&s_pGetFileSizeEx, pGetFileSizeEx);
		}
	}

	// ok?
	return pGetFileSizeEx;
}
tb_api_SymInitialize_t tb_api_SymInitialize()
{
	// init
	static tb_atomic_t s_pSymInitialize = 0;

	// try getting it
	tb_api_SymInitialize_t pSymInitialize = tb_null;
	if (!(pSymInitialize = (tb_api_SymInitialize_t)tb_atomic_get(&s_pSymInitialize))) 
	{
		// the dbghelp module
		HANDLE module = LoadLibraryExA("dbghelp.dll", tb_null, LOAD_WITH_ALTERED_SEARCH_PATH);

		// the SymInitialize func
		if (module) 
		{
			// get it
			pSymInitialize = GetProcAddress(module, "SymInitialize");

			// save it
			tb_atomic_set(&s_pSymInitialize, pSymInitialize);
		}
	}

	// ok?
	return pSymInitialize;
}
tb_api_GetNetworkParams_t tb_api_GetNetworkParams()
{
	// init
	static tb_atomic_t s_pGetNetworkParams = 0;

	// try getting it
	tb_api_GetNetworkParams_t pGetNetworkParams = tb_null;
	if (!(pGetNetworkParams = (tb_api_GetNetworkParams_t)tb_atomic_get(&s_pGetNetworkParams))) 
	{
		// the iphlpapi module
		HANDLE module = tb_dynamic_init("iphlpapi.dll");

		// the GetNetworkParams func
		if (module) 
		{
			// get it
			pGetNetworkParams = GetProcAddress(module, "GetNetworkParams");

			// save it
			tb_atomic_set(&s_pGetNetworkParams, pGetNetworkParams);
		}
	}

	// ok?
	return pGetNetworkParams;
}
