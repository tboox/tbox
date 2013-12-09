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
 * @file		aicp.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 			"aicp_iocp"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../../ltimer.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// from mswsock.h
#define TB_IOCP_WSAID_ACCEPTEX 					{0xb5367df1, 0xcbac, 0x11cf, {0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}}
#define TB_IOCP_WSAID_TRANSMITFILE 				{0xb5367df0, 0xcbac, 0x11cf, {0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}}
#define TB_IOCP_WSAID_GETACCEPTEXSOCKADDRS 		{0xb5367df2, 0xcbac, 0x11cf, {0x95, 0xca, 0x00, 0x80, 0x5f, 0x48, 0xa1, 0x92}}
#define TB_IOCP_WSAID_CONNECTEX 				{0x25a207b9, 0xddf3, 0x4660, {0x8e, 0xe9, 0x76, 0xe5, 0x8c, 0x74, 0x06, 0x3e}}

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the acceptex func type from mswsock.h
typedef BOOL (WINAPI* tb_iocp_func_acceptex_t)( 	SOCKET sListenSocket
												,	SOCKET sAcceptSocket
												,	PVOID lpOutputBuffer
												,	DWORD dwReceiveDataLength
												,	DWORD dwLocalAddressLength
												,	DWORD dwRemoteAddressLength
												,	LPDWORD lpdwBytesReceived
												,	LPOVERLAPPED lpOverlapped);

// the connectex func type from mswsock.h
typedef BOOL (WINAPI* tb_iocp_func_connectex_t)( 	SOCKET s
												, 	struct sockaddr const*name
												,	tb_int_t namelen
												,	PVOID lpSendBuffer
												,	DWORD dwSendDataLength
												,	LPDWORD lpdwBytesSent
												,	LPOVERLAPPED lpOverlapped);

typedef BOOL (WINAPI* tb_iocp_func_transmitfile_t)( 	SOCKET hSocket
												,		HANDLE hFile
												,		DWORD nNumberOfBytesToWrite
												,		DWORD nNumberOfBytesPerSend
												,		LPOVERLAPPED lpOverlapped
												,		LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers
												,		DWORD dwReserved);

// the overlap type
typedef struct __tb_iocp_olap_t
{
	// the base
	OVERLAPPED 								base;
	
	// the data
	WSABUF 									data;

	//the size
	DWORD 									size;

	// the aice
	tb_aice_t 								aice;

}tb_iocp_olap_t;

// the iocp proactor type
typedef struct __tb_aicp_proactor_iocp_t
{
	// the proactor base
	tb_aicp_proactor_t 						base;

	// the i/o completion port
	HANDLE 									port;

	// the olap pool
	tb_handle_t 							pool;
	
	// the pool mutx
	tb_handle_t 							mutx;

	// the timer
	tb_handle_t 							timer;

	// the thread self for the timer spak
	tb_handle_t 							tself;

	// the acceptex func
	tb_iocp_func_acceptex_t 				acceptex;

	// the connectex func
	tb_iocp_func_connectex_t 				connectex;

	// the transmitfile func
	tb_iocp_func_transmitfile_t 			transmitfile;

}tb_aicp_proactor_iocp_t;

// the iocp aico type
typedef struct __tb_iocp_aico_t
{
	// the base
	tb_aico_t 								base;

	// the ptor
	tb_aicp_proactor_iocp_t* 				ptor;

	// the olap
	tb_iocp_olap_t* 						olap;

	// the task
	tb_handle_t 							task;

}tb_iocp_aico_t;

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
static tb_void_t tb_iocp_spak_timeout(tb_pointer_t data);

/* ///////////////////////////////////////////////////////////////////////
 * olap
 */
static tb_iocp_olap_t* tb_iocp_olap_init(tb_aicp_proactor_iocp_t* ptor)
{
	// check
	tb_assert_and_check_return_val(ptor, tb_null);

	// enter 
	if (ptor->mutx) tb_mutex_enter(ptor->mutx);

	// make data
	tb_pointer_t data = ptor->pool? tb_rpool_malloc0(ptor->pool) : tb_null;

	// leave 
	if (ptor->mutx) tb_mutex_leave(ptor->mutx);
	
	// ok?
	return (tb_iocp_olap_t*)data;
}
static tb_void_t tb_iocp_olap_exit(tb_aicp_proactor_iocp_t* ptor, tb_iocp_olap_t* data)
{
	// check
	tb_assert_and_check_return(ptor);

	// enter 
	if (ptor->mutx) tb_mutex_enter(ptor->mutx);

	// free data
	if (data) tb_rpool_free(ptor->pool, data);

	// leave 
	if (ptor->mutx) tb_mutex_leave(ptor->mutx);
}

/* ///////////////////////////////////////////////////////////////////////
 * func
 */
static tb_iocp_func_acceptex_t tb_iocp_func_acceptex()
{
	// done
	tb_long_t 				ok = -1;
	tb_handle_t 			sock = tb_null;
	tb_iocp_func_acceptex_t acceptex = tb_null;
	do
	{
		// init sock
		sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
		tb_assert_and_check_break(sock);

		// get the acceptex func address
		DWORD 	real = 0;
		GUID 	guid = TB_IOCP_WSAID_ACCEPTEX;
		ok = WSAIoctl( 	(SOCKET)sock - 1
					, 	SIO_GET_EXTENSION_FUNCTION_POINTER
					, 	&guid
					, 	sizeof(GUID)
					, 	&acceptex
					, 	sizeof(tb_iocp_func_acceptex_t)
					, 	&real
					, 	tb_null
					, 	tb_null);
		tb_assert_and_check_break(!ok && acceptex);

	} while (0);

	// exit sock
	if (sock) tb_socket_close(sock);

	// ok?
	return !ok? acceptex : tb_null;
}
static tb_iocp_func_connectex_t tb_iocp_func_connectex()
{
	// done
	tb_long_t 					ok = -1;
	tb_handle_t 				sock = tb_null;
	tb_iocp_func_connectex_t 	connectex = tb_null;
	do
	{
		// init sock
		sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
		tb_assert_and_check_break(sock);

		// get the acceptex func address
		DWORD 	real = 0;
		GUID 	guid = TB_IOCP_WSAID_CONNECTEX;
		ok = WSAIoctl( 	(SOCKET)sock - 1
					, 	SIO_GET_EXTENSION_FUNCTION_POINTER
					, 	&guid
					, 	sizeof(GUID)
					, 	&connectex
					, 	sizeof(tb_iocp_func_connectex_t)
					, 	&real
					, 	tb_null
					, 	tb_null);
		tb_assert_and_check_break(!ok && connectex);

	} while (0);

	// exit sock
	if (sock) tb_socket_close(sock);

	// ok?
	return !ok? connectex : tb_null;
}
static tb_iocp_func_transmitfile_t tb_iocp_func_transmitfile()
{
	// done
	tb_long_t 					ok = -1;
	tb_handle_t 				sock = tb_null;
	tb_iocp_func_transmitfile_t transmitfile = tb_null;
	do
	{
		// init sock
		sock = tb_socket_open(TB_SOCKET_TYPE_TCP);
		tb_assert_and_check_break(sock);

		// get the acceptex func address
		DWORD 	real = 0;
		GUID 	guid = TB_IOCP_WSAID_TRANSMITFILE;
		ok = WSAIoctl( 	(SOCKET)sock - 1
					, 	SIO_GET_EXTENSION_FUNCTION_POINTER
					, 	&guid
					, 	sizeof(GUID)
					, 	&transmitfile
					, 	sizeof(tb_iocp_func_transmitfile_t)
					, 	&real
					, 	tb_null
					, 	tb_null);
		tb_assert_and_check_break(!ok && transmitfile);

	} while (0);

	// exit sock
	if (sock) tb_socket_close(sock);

	// ok?
	return !ok? transmitfile : tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * post
 */
static tb_bool_t tb_iocp_post_acpt(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && ptor->acceptex && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_ACPT, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);
	
	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// trace
	tb_trace_impl("accept: ..");

	// done
	tb_bool_t 		ok = tb_false;
	tb_iocp_olap_t* olap = tb_null;
	tb_bool_t 		init_ok = tb_false;
	tb_bool_t 		acceptex_ok = tb_false;
	do
	{
		// make olap
		olap = tb_iocp_olap_init(ptor);
		tb_assert_and_check_break(olap);

		// init olap, hack: sizeof(tb_iocp_olap_t) >= olap->data.len
		olap->data.len 			= (sizeof(SOCKADDR_IN) + 16) << 1;
		olap->data.buf 			= (tb_byte_t*)tb_iocp_olap_init(ptor);
		olap->aice 				= *aice;
		olap->aice.u.acpt.sock 	= tb_socket_open(TB_SOCKET_TYPE_TCP);
		tb_assert_static(sizeof(tb_iocp_olap_t) >= ((sizeof(SOCKADDR_IN) + 16) << 1));
		tb_assert_and_check_break(olap->data.buf && olap->aice.u.acpt.sock);
		init_ok = tb_true;

		// done acceptex
		DWORD real = 0;
		acceptex_ok = ptor->acceptex( 	(SOCKET)aico->handle - 1
									, 	(SOCKET)olap->aice.u.acpt.sock - 1
									, 	olap->data.buf
									, 	0
									, 	sizeof(SOCKADDR_IN) + 16
									, 	sizeof(SOCKADDR_IN) + 16
									, 	&real
									, 	olap)? tb_true : tb_false;
		tb_trace_impl("acceptex: %d, error: %d", acceptex_ok, WSAGetLastError());
		tb_check_break(acceptex_ok);

		// post ok
		olap->aice.state = TB_AICE_STATE_OK;
		if (!PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) break;

		// ok
		ok = tb_true;

	} while (0);

	// acceptex failed? 
	if (init_ok && !acceptex_ok)
	{
		// pending? continue it
		if (WSA_IO_PENDING == WSAGetLastError()) 
		{
#if 0
			// add timeout task
			tb_long_t timeout = tb_aico_timeout_from_code(aico, aice->code);
			if (timeout >= 0)
			{
				// the iocp aico
				tb_iocp_aico_t* iocp_aico = (tb_iocp_aico_t*)aico; 
				iocp_aico->olap = olap;
				
				// exit the old task
				if (iocp_aico->task) tb_ltimer_task_del(ptor->timer, iocp_aico->task);

				// add the new task
				iocp_aico->task = tb_ltimer_task_add(ptor->timer, timeout, tb_false, tb_iocp_spak_timeout, aico);
			}
#endif
			// ok
			ok = tb_true;
		}
		// failed? remove olap
		else
		{
			// post failed
			olap->aice.state = TB_AICE_STATE_FAILED;
			if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) ok = tb_true;

			// trace
			tb_trace_impl("acceptex: unknown error: %d", WSAGetLastError());
		}
	}

	// failed? remove olap
	if (!ok)
	{
		// exit data
		if (olap->data.buf) tb_iocp_olap_exit(ptor, olap->data.buf);

		// exit sock
		if (olap->aice.u.acpt.sock) tb_socket_close(olap->aice.u.acpt.sock);

		// exit olap
		if (olap) tb_iocp_olap_exit(ptor, olap);
	}

	// ok?
	return ok;
}
static tb_bool_t tb_iocp_post_conn(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_CONN, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);
	tb_assert_and_check_return_val(aice->u.conn.host && aice->u.conn.port, tb_false);
	
	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// trace
	tb_trace_impl("connect: %s:%lu", aice->u.conn.host, aice->u.conn.port);

	// done
	tb_bool_t 		ok = tb_false;
	tb_iocp_olap_t* olap = tb_null;
	tb_bool_t 		init_ok = tb_false;
	tb_bool_t 		connectex_ok = tb_false;
	tb_bool_t 		bind_ok = tb_false;
	do
	{
		// make olap
		olap = tb_iocp_olap_init(ptor);
		tb_assert_and_check_break(olap);
		init_ok = tb_true;

		// init olap
		olap->aice = *aice;

		// bind it first for connectex
		SOCKADDR_IN local;
		local.sin_family = AF_INET;
		local.sin_addr.S_un.S_addr = INADDR_ANY;
		local.sin_port = 0;
		if (SOCKET_ERROR == bind((SOCKET)aico->handle - 1, (LPSOCKADDR)&local, sizeof(local))) break;
		bind_ok = tb_true;

		// done connectex
		DWORD real = 0;
		SOCKADDR_IN addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(aice->u.conn.port);
		addr.sin_addr.S_un.S_addr = inet_addr(aice->u.conn.host);
		connectex_ok = ptor->connectex( 	(SOCKET)aico->handle - 1
										, 	(struct sockaddr const*)&addr
										, 	sizeof(addr)
										, 	tb_null
										, 	0
										, 	&real
										, 	olap)? tb_true : tb_false;
		tb_trace_impl("connectex: %d, error: %d", connectex_ok, WSAGetLastError());
		tb_check_break(connectex_ok);

		// post ok
		olap->aice.state = TB_AICE_STATE_OK;
		if (!PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) break;

		// ok
		ok = tb_true;

	} while (0);

	// connectex failed?
	if (init_ok && bind_ok && !connectex_ok)
	{
		// pending? continue it
		if (WSA_IO_PENDING == WSAGetLastError()) ok = tb_true;
		// failed? remove olap
		else
		{
			// post failed
			olap->aice.state = TB_AICE_STATE_FAILED;
			if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) ok = tb_true;

			// trace
			tb_trace_impl("connectex: unknown error: %d", WSAGetLastError());
		}
	}

	// failed? remove olap
	if (!ok)
	{
		// exit olap
		if (olap) tb_iocp_olap_exit(ptor, olap);
	}

	// ok?
	return ok;
}
static tb_bool_t tb_iocp_post_recv(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{	
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RECV, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);
	tb_assert_and_check_return_val(aice->u.recv.data && aice->u.recv.size, tb_false);

	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// init olap
	tb_iocp_olap_t* olap = tb_iocp_olap_init(ptor);
	tb_assert_and_check_return_val(olap, tb_false);
	olap->data.buf 	= aice->u.recv.data;
	olap->data.len 	= aice->u.recv.size;
	olap->aice 		= *aice;

	// done recv
	DWORD 		flag = 0;
	tb_long_t 	real = WSARecv((SOCKET)aico->handle - 1, &olap->data, 1, &olap->size, &flag, olap, tb_null);
	tb_trace_impl("WSARecv: %ld, error: %d", real, WSAGetLastError());

	// pending? continue it
	if (!real || WSA_IO_PENDING == WSAGetLastError()) return tb_true;

	// ok?
	if (real > 0)
	{
		// post ok
		olap->aice.state = TB_AICE_STATE_OK;
		olap->aice.u.recv.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}
	else
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			olap->aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			olap->aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}

	// exit olap
	if (olap) tb_iocp_olap_exit(ptor, olap);

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_send(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_SEND, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);
	tb_assert_and_check_return_val(aice->u.send.data && aice->u.send.size, tb_false);

	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// init olap
	tb_iocp_olap_t* olap = tb_iocp_olap_init(ptor);
	tb_assert_and_check_return_val(olap, tb_false);
	olap->data.buf 	= aice->u.send.data;
	olap->data.len 	= aice->u.send.size;
	olap->aice 		= *aice;

	// done send
	tb_long_t real = WSASend((SOCKET)aico->handle - 1, &olap->data, 1, &olap->size, 0, olap, tb_null);
	tb_trace_impl("WSASend: %ld, error: %d", real, WSAGetLastError());

	// pending? continue it
	if (!real || WSA_IO_PENDING == WSAGetLastError()) return tb_true;

	// ok?
	if (real > 0)
	{
		// post ok
		olap->aice.state = TB_AICE_STATE_OK;
		olap->aice.u.send.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}
	else
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			olap->aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			olap->aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}

	// remove olap
	if (olap) tb_iocp_olap_exit(ptor, olap);

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_recvv(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{	
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RECVV, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);
	tb_assert_and_check_return_val(aice->u.recvv.list && aice->u.recvv.size, tb_false);

	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// init olap
	tb_iocp_olap_t* olap = tb_iocp_olap_init(ptor);
	tb_assert_and_check_return_val(olap, tb_false);
	olap->data.buf 	= aice->u.recvv.list[0].data;
	olap->data.len 	= aice->u.recvv.list[0].size;
	olap->aice 		= *aice;

	// done recv
	DWORD 		flag = 0;
	tb_long_t 	real = WSARecv((SOCKET)aico->handle - 1, &olap->data, 1, &olap->size, &flag, olap, tb_null);
	tb_trace_impl("WSARecv: %ld, error: %d", real, WSAGetLastError());

	// pending? continue it
	if (!real || WSA_IO_PENDING == WSAGetLastError()) return tb_true;

	// ok?
	if (real > 0)
	{
		// post ok
		olap->aice.state = TB_AICE_STATE_OK;
		olap->aice.u.recvv.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}
	else
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			olap->aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			olap->aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}

	// exit olap
	if (olap) tb_iocp_olap_exit(ptor, olap);

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_sendv(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_SENDV, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);
	tb_assert_and_check_return_val(aice->u.sendv.list && aice->u.sendv.size, tb_false);

	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// init olap
	tb_iocp_olap_t* olap = tb_iocp_olap_init(ptor);
	tb_assert_and_check_return_val(olap, tb_false);
	olap->data.buf 	= aice->u.sendv.list[0].data; // FIXME
	olap->data.len 	= aice->u.sendv.list[0].size;
	olap->aice 		= *aice;

	// done send
	tb_long_t real = WSASend((SOCKET)aico->handle - 1, &olap->data, 1, &olap->size, 0, olap, tb_null);
	tb_trace_impl("WSASend: %ld, error: %d", real, WSAGetLastError());

	// pending? continue it
	if (!real || WSA_IO_PENDING == WSAGetLastError()) return tb_true;

	// ok?
	if (real > 0)
	{
		// post ok
		olap->aice.state = TB_AICE_STATE_OK;
		olap->aice.u.sendv.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}
	else
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			olap->aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			olap->aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}

	// remove olap
	if (olap) tb_iocp_olap_exit(ptor, olap);

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_sendfile(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_SENDFILE, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);
	tb_assert_and_check_return_val(aice->u.sendfile.file && aice->u.sendfile.size, tb_false);

	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// init olap
	tb_iocp_olap_t* olap = tb_iocp_olap_init(ptor);
	tb_assert_and_check_return_val(olap, tb_false);
	olap->aice 			= *aice;
	olap->base.Offset 	= aice->u.sendfile.seek;

	// done send
	tb_long_t real = ptor->transmitfile((SOCKET)aico->handle - 1, (HANDLE)aice->u.sendfile.file, (DWORD)aice->u.sendfile.size, (1 << 16), olap, tb_null, 0);
	tb_trace_impl("sendfile: %ld, error: %d", real, WSAGetLastError());

	// pending? continue it
	if (!real || WSA_IO_PENDING == WSAGetLastError()) return tb_true;

	// ok?
	if (real > 0)
	{
		// post ok
		olap->aice.state = TB_AICE_STATE_OK;
		olap->aice.u.sendfile.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}
	else
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			olap->aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			olap->aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}

	// remove olap
	if (olap) tb_iocp_olap_exit(ptor, olap);

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_read(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{	
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_READ, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);
	tb_assert_and_check_return_val(aice->u.read.data && aice->u.read.size, tb_false);
	
	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// init olap
	tb_iocp_olap_t* olap = tb_iocp_olap_init(ptor);
	tb_assert_and_check_return_val(olap, tb_false);
	olap->aice 			= *aice;
	olap->base.Offset 	= aice->u.read.seek;

	// done read
	DWORD 		flag = 0;
	DWORD 		real = 0;
	BOOL 		ok = ReadFile((HANDLE)aico->handle, aice->u.read.data, (DWORD)aice->u.read.size, &real, olap);
	tb_trace_impl("ReadFile: %u, error: %d", real, GetLastError());

	// pending? continue it
	if (!real || ERROR_IO_PENDING == GetLastError()) return tb_true;

	// finished?
	if (ok || real > 0)
	{
		// post ok
		olap->aice.state = TB_AICE_STATE_OK;
		olap->aice.u.read.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}
	else 
	{
		// post failed
		olap->aice.state = TB_AICE_STATE_FAILED;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}

	// remove olap
	if (olap) tb_iocp_olap_exit(ptor, olap);

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_writ(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_WRIT, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);
	tb_assert_and_check_return_val(aice->u.writ.data && aice->u.writ.size, tb_false);

	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// init olap
	tb_iocp_olap_t* olap = tb_iocp_olap_init(ptor);
	tb_assert_and_check_return_val(olap, tb_false);
	olap->aice 			= *aice;
	olap->base.Offset 	= aice->u.writ.seek;

	// done writ
	DWORD 		flag = 0;
	DWORD 		real = 0;
	BOOL 		ok = WriteFile((HANDLE)aico->handle, aice->u.writ.data, (DWORD)aice->u.writ.size, &real, olap);
	tb_trace_impl("WriteFile: %u, error: %d", real, GetLastError());

	// pending? continue it
	if (!real || ERROR_IO_PENDING == GetLastError()) return tb_true;

	// finished?
	if (ok || real > 0)
	{
		// post ok
		olap->aice.state = TB_AICE_STATE_OK;
		olap->aice.u.writ.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}
	else
	{
		// post failed
		olap->aice.state = TB_AICE_STATE_FAILED;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}

	// remove olap
	if (olap) tb_iocp_olap_exit(ptor, olap);

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_readv(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{	
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_READV, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);
	tb_assert_and_check_return_val(aice->u.readv.list && aice->u.readv.size, tb_false);
	
	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// init olap
	tb_iocp_olap_t* olap = tb_iocp_olap_init(ptor);
	tb_assert_and_check_return_val(olap, tb_false);
	olap->aice 			= *aice;
	olap->base.Offset 	= aice->u.readv.seek;

	// done read
	DWORD 		flag = 0;
	DWORD 		real = 0;
	BOOL 		ok = ReadFile((HANDLE)aico->handle, aice->u.readv.list[0].data, (DWORD)aice->u.readv.list[0].size, &real, olap);
	tb_trace_impl("ReadFile: %u, error: %d", real, GetLastError());

	// pending? continue it
	if (!real || ERROR_IO_PENDING == GetLastError()) return tb_true;

	// finished?
	if (ok || real > 0)
	{
		// post ok
		olap->aice.state = TB_AICE_STATE_OK;
		olap->aice.u.readv.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}
	else 
	{
		// post failed
		olap->aice.state = TB_AICE_STATE_FAILED;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}

	// remove olap
	if (olap) tb_iocp_olap_exit(ptor, olap);

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_writv(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_WRITV, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);
	tb_assert_and_check_return_val(aice->u.writv.list && aice->u.writv.size, tb_false);

	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// init olap
	tb_iocp_olap_t* olap = tb_iocp_olap_init(ptor);
	tb_assert_and_check_return_val(olap, tb_false);
	olap->aice 			= *aice;
	olap->base.Offset 	= aice->u.writv.seek;

	// done writ
	DWORD 		flag = 0;
	DWORD 		real = 0;
	BOOL 		ok = WriteFile((HANDLE)aico->handle, aice->u.writv.list[0].data, (DWORD)aice->u.writv.list[0].size, &real, olap);
	tb_trace_impl("WriteFile: %u, error: %d", real, GetLastError());

	// pending? continue it
	if (!real || ERROR_IO_PENDING == GetLastError()) return tb_true;

	// finished?
	if (ok || real > 0)
	{
		// post ok
		olap->aice.state = TB_AICE_STATE_OK;
		olap->aice.u.writv.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}
	else
	{
		// post failed
		olap->aice.state = TB_AICE_STATE_FAILED;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;
	}

	// remove olap
	if (olap) tb_iocp_olap_exit(ptor, olap);

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_fsync(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_FSYNC, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);

	// the aico
	tb_aico_t const* aico = aice->aico;
	tb_assert_and_check_return_val(aico && aico->handle, tb_false);

	// init olap
	tb_iocp_olap_t* olap = tb_iocp_olap_init(ptor);
	tb_assert_and_check_return_val(olap, tb_false);
	olap->aice 			= *aice;

	// post ok
	olap->aice.state = TB_AICE_STATE_OK;
	if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, olap)) return tb_true;

	// remove olap
	if (olap) tb_iocp_olap_exit(ptor, olap);

	// failed
	return tb_false;
}
/* ///////////////////////////////////////////////////////////////////////
 * spak 
 */
static tb_void_t tb_iocp_spak_timeout(tb_pointer_t data)
{
	// the aico
	tb_aico_t* aico = data;
	tb_assert_and_check_return(aico && aico->handle);

	// trace
	tb_trace_impl("timeout: handle: %p", aico->handle);

	// cancel it
	switch (aico->type)
	{
	case TB_AICO_TYPE_SOCK:
		if (!CancelIo((SOCKET)aico->handle - 1))
		{
			tb_trace_impl("cancel: failed: %u", GetLastError());
		}
		break;
	case TB_AICO_TYPE_FILE:
		if (!CancelIo(aico->handle))
		{
			tb_trace_impl("cancel: failed: %u", GetLastError());
		}
		break;
	default:
		tb_assert(0);
		break;
	}
}
static tb_long_t tb_iocp_spak_acpt(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// spak resp
	*resp = olap->aice;

	// done
	tb_long_t ok = -1;
	if (wait) 
	{
		// save state
		resp->state = resp->u.acpt.sock? TB_AICE_STATE_OK : TB_AICE_STATE_FAILED;
		ok = 1;
	}
	// failed? done error
	else
	{
		// done error
		switch (GetLastError())
		{
		// unknown error
		default:
			tb_trace_impl("acpt: unknown error: %u", GetLastError());
			break;
		}
	}

	// failed? exit sock
	if (ok < 0)
	{
		if (resp->u.acpt.sock) tb_socket_close(resp->u.acpt.sock);
		resp->u.acpt.sock = tb_null;
	}

	// exit data
	if (olap->data.buf) tb_iocp_olap_exit(ptor, olap->data.buf);
	olap->data.buf = tb_null;
	olap->data.len = 0;

	// ok?
	return ok;
}
static tb_long_t tb_iocp_spak_conn(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// spak resp
	*resp = olap->aice;

	// ok
	if (wait) resp->state = TB_AICE_STATE_OK;
	// failed? done error
	else
	{
		// done error
		switch (GetLastError())
		{
			// refused?
		case ERROR_CONNECTION_REFUSED:
			// timeout?
		case ERROR_SEM_TIMEOUT:
			resp->state = TB_AICE_STATE_FAILED;
			break;
		// unknown error
		default:
			resp->state = TB_AICE_STATE_FAILED;
			tb_trace_impl("conn: unknown error: %u", GetLastError());
			break;
		}
	}

	// ok
	return 1;
}
static tb_long_t tb_iocp_spak_recv(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// spak resp
	*resp = olap->aice;

	// ok? spak the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// save state
		resp->state = real? TB_AICE_STATE_OK : TB_AICE_STATE_CLOSED;
		ok = 1;
	}
	// failed? done error
	else
	{
		// done error
		switch (GetLastError())
		{
		// closed?
		case ERROR_HANDLE_EOF:
		case ERROR_NETNAME_DELETED:
			resp->state = TB_AICE_STATE_CLOSED;
			ok = 1;
			break;
		// unknown error
		default:
			tb_trace_impl("recv: unknown error: %u", GetLastError());
			break;
		}
	}

	// spak the real size	
	resp->u.recv.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_iocp_spak_send(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// spak resp
	*resp = olap->aice;

	// ok? spak the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// save state
		resp->state = real? TB_AICE_STATE_OK : TB_AICE_STATE_CLOSED;
		ok = 1;
	}
	// failed? done error
	else
	{
		// done error
		switch (GetLastError())
		{
		// closed?
		case ERROR_HANDLE_EOF:
		case ERROR_NETNAME_DELETED:
			resp->state = TB_AICE_STATE_CLOSED;
			ok = 1;
			break;
		// unknown error
		default:
			tb_trace_impl("send: unknown error: %u", GetLastError());
			break;
		}
	}

	// spak the real size	
	resp->u.send.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_iocp_spak_recvv(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// spak resp
	*resp = olap->aice;

	// ok? spak the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// save state
		resp->state = real? TB_AICE_STATE_OK : TB_AICE_STATE_CLOSED;
		ok = 1;
	}
	// failed? done error
	else
	{
		// done error
		switch (GetLastError())
		{
		// closed?
		case ERROR_HANDLE_EOF:
		case ERROR_NETNAME_DELETED:
			resp->state = TB_AICE_STATE_CLOSED;
			ok = 1;
			break;
		// unknown error
		default:
			tb_trace_impl("recv: unknown error: %u", GetLastError());
			break;
		}
	}

	// spak the real size	
	resp->u.recvv.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_iocp_spak_sendv(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// spak resp
	*resp = olap->aice;

	// ok? spak the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// save state
		resp->state = real? TB_AICE_STATE_OK : TB_AICE_STATE_CLOSED;
		ok = 1;
	}
	// failed? done error
	else
	{
		// done error
		switch (GetLastError())
		{
		// closed?
		case ERROR_HANDLE_EOF:
		case ERROR_NETNAME_DELETED:
			resp->state = TB_AICE_STATE_CLOSED;
			ok = 1;
			break;
		// unknown error
		default:
			tb_trace_impl("send: unknown error: %u", GetLastError());
			break;
		}
	}

	// spak the real size	
	resp->u.sendv.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_iocp_spak_sendfile(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// spak resp
	*resp = olap->aice;

	// ok? spak the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// save state
		resp->state = real? TB_AICE_STATE_OK : TB_AICE_STATE_CLOSED;
		ok = 1;
	}
	// failed? done error
	else
	{
		// done error
		switch (GetLastError())
		{
		// closed?
		case ERROR_HANDLE_EOF:
		case ERROR_NETNAME_DELETED:
			resp->state = TB_AICE_STATE_CLOSED;
			ok = 1;
			break;
		// unknown error
		default:
			tb_trace_impl("sendfile: unknown error: %u", GetLastError());
			break;
		}
	}

	// spak the real size	
	resp->u.sendfile.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_iocp_spak_read(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// spak resp
	*resp = olap->aice;

	// ok? spak the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// save state
		resp->state = real? TB_AICE_STATE_OK : TB_AICE_STATE_CLOSED;
		ok = 1;
	}
	// failed? done error
	else
	{
		// done error
		switch (GetLastError())
		{
		// closed?
		case ERROR_HANDLE_EOF:
		case ERROR_NETNAME_DELETED:
			resp->state = TB_AICE_STATE_CLOSED;
			ok = 1;
			break;
		// unknown error
		default:
			tb_trace_impl("read: unknown error: %u", GetLastError());
			break;
		}
	}

	// spak the real size	
	resp->u.read.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_iocp_spak_writ(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// spak resp
	*resp = olap->aice;

	// ok? spak the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// save state
		resp->state = real? TB_AICE_STATE_OK : TB_AICE_STATE_CLOSED;
		ok = 1;
	}
	// failed? done error
	else
	{
		// done error
		switch (GetLastError())
		{
		// closed?
		case ERROR_HANDLE_EOF:
		case ERROR_NETNAME_DELETED:
			resp->state = TB_AICE_STATE_CLOSED;
			ok = 1;
			break;
		// unknown error
		default:
			tb_trace_impl("writ: unknown error: %u", GetLastError());
			break;
		}
	}

	// spak the real size	
	resp->u.writ.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_iocp_spak_readv(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// spak resp
	*resp = olap->aice;

	// ok? spak the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// save state
		resp->state = real? TB_AICE_STATE_OK : TB_AICE_STATE_CLOSED;
		ok = 1;
	}
	// failed? done error
	else
	{
		// done error
		switch (GetLastError())
		{
		// closed?
		case ERROR_HANDLE_EOF:
		case ERROR_NETNAME_DELETED:
			resp->state = TB_AICE_STATE_CLOSED;
			ok = 1;
			break;
		// unknown error
		default:
			tb_trace_impl("read: unknown error: %u", GetLastError());
			break;
		}
	}

	// spak the real size	
	resp->u.readv.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_iocp_spak_writv(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// spak resp
	*resp = olap->aice;

	// ok? spak the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// save state
		resp->state = real? TB_AICE_STATE_OK : TB_AICE_STATE_CLOSED;
		ok = 1;
	}
	// failed? done error
	else
	{
		// done error
		switch (GetLastError())
		{
		// closed?
		case ERROR_HANDLE_EOF:
		case ERROR_NETNAME_DELETED:
			resp->state = TB_AICE_STATE_CLOSED;
			ok = 1;
			break;
		// unknown error
		default:
			tb_trace_impl("writ: unknown error: %u", GetLastError());
			break;
		}
	}

	// spak the real size	
	resp->u.writv.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_iocp_spak_fsync(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// spak resp
	*resp = olap->aice;

	// ok? 
	tb_long_t ok = -1;
	if (wait) 
	{
		resp->state = TB_AICE_STATE_OK;
		ok = 1;
	}
	// failed? done error
	else
	{
		// done error
		switch (GetLastError())
		{
		// closed?
		case ERROR_HANDLE_EOF:
		case ERROR_NETNAME_DELETED:
			resp->state = TB_AICE_STATE_CLOSED;
			ok = 1;
			break;
		// unknown error
		default:
			tb_trace_impl("fsync: unknown error: %u", GetLastError());
			break;
		}
	}

	// ok?
	return ok;
}
static tb_long_t tb_iocp_spak_resp(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// init spak
	static tb_bool_t (*s_spak[])(tb_aicp_proactor_iocp_t* , tb_aice_t* , tb_iocp_olap_t* , tb_size_t , tb_bool_t ) = 
	{
		tb_null
	,	tb_iocp_spak_acpt
	,	tb_iocp_spak_conn
	,	tb_iocp_spak_recv
	,	tb_iocp_spak_send
	,	tb_iocp_spak_recvv
	,	tb_iocp_spak_sendv
	,	tb_iocp_spak_sendfile
	,	tb_iocp_spak_read
	,	tb_iocp_spak_writ
	,	tb_iocp_spak_readv
	,	tb_iocp_spak_writv
	,	tb_iocp_spak_fsync
	};
	tb_assert_and_check_return_val(olap->aice.code < tb_arrayn(s_spak), -1);

	// done spak
	return (s_spak[olap->aice.code])? s_spak[olap->aice.code](ptor, resp, olap, real, wait) : -1;
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_proactor_iocp_addo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp && aico && aico->handle, tb_false);

	// attach aico to port
	HANDLE port = CreateIoCompletionPort((HANDLE)aico->handle, ptor->port, (ULONG*)aico, 0);
	tb_assert_and_check_return_val(port == ptor->port, tb_false);
	
	// the iocp aico
	tb_iocp_aico_t* iocp_aico = (tb_iocp_aico_t*)aico;
	iocp_aico->ptor = ptor;

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_proactor_iocp_delo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp && aico && aico->handle, tb_false);
		
	// the iocp aico
	tb_iocp_aico_t* iocp_aico = (tb_iocp_aico_t*)aico;
	tb_assert_and_check_return_val(!iocp_aico->olap, tb_false);
	
	// del the timeout task
	if (iocp_aico->task) tb_ltimer_task_del(ptor->timer, iocp_aico->task);
	iocp_aico->task = tb_null;

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_proactor_iocp_post(tb_aicp_proactor_t* proactor, tb_aice_t const* list, tb_size_t size)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp && list && size, tb_false);

	// walk post
	tb_size_t i = 0;
	for (i = 0; i < size; i++)
	{
		// the aice
		tb_aice_t const* aice = &list[i];

		// init post
		static tb_bool_t (*s_post[])(tb_aicp_proactor_t* , tb_aice_t const*) = 
		{
			tb_null
		,	tb_iocp_post_acpt
		,	tb_iocp_post_conn
		,	tb_iocp_post_recv
		,	tb_iocp_post_send
		,	tb_iocp_post_recvv
		,	tb_iocp_post_sendv
		,	tb_iocp_post_sendfile
		,	tb_iocp_post_read
		,	tb_iocp_post_writ
		,	tb_iocp_post_readv
		,	tb_iocp_post_writv
		,	tb_iocp_post_fsync
		};
		tb_assert_and_check_return_val(aice->code < tb_arrayn(s_post) && s_post[aice->code], tb_false);

		// post aice
		if (!s_post[aice->code](proactor, aice)) return tb_false;
	}

	// ok
	return tb_true;
}
static tb_long_t tb_aicp_proactor_iocp_spak(tb_aicp_proactor_t* proactor, tb_aice_t* resp, tb_long_t timeout)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && ptor->timer && resp, -1);

	// the self
	tb_handle_t self = tb_thread_self();
	tb_assert_and_check_return_val(self, -1);

	// trace
	tb_trace_impl("spak[%lu]: ..", self);

	// save the first thread self for timer
	if (!ptor->tself) ptor->tself = self;

	// is the timer thread? 
	tb_bool_t btimer = ptor->tself == self? tb_true : tb_false;

	// update the timeout for the timer spak
	if (btimer) timeout = tb_ltimer_timeout(ptor->timer);

	// wait
	DWORD 			real = 0;
	tb_aioo_t* 		aico = tb_null;
	tb_iocp_olap_t* olap = tb_null;
	BOOL 			wait = GetQueuedCompletionStatus(ptor->port, (LPDWORD)&real, (LPDWORD)&aico, &olap, timeout < 0? INFINITE : timeout);

	// is the timer thread? spak timer
	if (btimer)
	{
		// spak ctime
		tb_ctime_spak();

		// spak timer
		if (!tb_ltimer_spak(ptor->timer)) return -1;
	}

	// trace
	tb_trace_impl("spak[%lu]: %d error: %u", tb_thread_self(), wait, GetLastError());

	// timeout?
	if (!wait && WAIT_TIMEOUT == GetLastError()) return 0;

	// killed?
	if (wait && !aico) return -1;

	// spak resp
	tb_long_t ok = tb_iocp_spak_resp(ptor, resp, olap, (tb_size_t)real, wait? tb_true : tb_false);

	// free olap
	tb_iocp_olap_exit(ptor, olap);

	// ok?
	return ok;
}
static tb_void_t tb_aicp_proactor_iocp_kill(tb_aicp_proactor_t* proactor)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return(ptor && ptor->port && ptor->timer && proactor->aicp);

	// clear timer
	tb_ltimer_clear(ptor->timer);

	// the worker size
	tb_size_t work = tb_atomic_get(&proactor->aicp->work);

	// trace
	tb_trace_impl("kill: %lu", work);

	// kill workers
	while (work--) PostQueuedCompletionStatus(ptor->port, 0, 0, tb_null);
}
static tb_void_t tb_aicp_proactor_iocp_exit(tb_aicp_proactor_t* proactor)
{
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	if (ptor)
	{
		// trace
		tb_trace_impl("exit");

		// exit port
		if (ptor->port) CloseHandle(ptor->port);
		ptor->port = tb_null;

		// exit pool
		if (ptor->mutx) tb_mutex_enter(ptor->mutx);
		if (ptor->pool) tb_rpool_exit(ptor->pool);
		ptor->pool = tb_null;
		if (ptor->mutx) tb_mutex_leave(ptor->mutx);

		// exit timer
		if (ptor->timer) tb_ltimer_exit(ptor->timer);
		ptor->timer = tb_null;

		// exit mutx
		if (ptor->mutx) tb_mutex_exit(ptor->mutx);
		ptor->mutx = tb_null;

		// free it
		tb_free(ptor);
	}
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp);
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->maxn, tb_null);

	// check iovec
	tb_assert_static(sizeof(tb_iovec_t) == sizeof(WSABUF));
	tb_assert_and_check_return_val(tb_memberof_eq(tb_iovec_t, data, WSABUF, buf), -1);
	tb_assert_and_check_return_val(tb_memberof_eq(tb_iovec_t, size, WSABUF, len), -1);

	// alloc proactor
	tb_aicp_proactor_iocp_t* ptor = tb_malloc0(sizeof(tb_aicp_proactor_iocp_t));
	tb_assert_and_check_return_val(ptor, tb_null);

	// init base
	ptor->base.aicp 	= aicp;
	ptor->base.step 	= sizeof(tb_iocp_aico_t);
	ptor->base.kill 	= tb_aicp_proactor_iocp_kill;
	ptor->base.exit 	= tb_aicp_proactor_iocp_exit;
	ptor->base.addo 	= tb_aicp_proactor_iocp_addo;
	ptor->base.delo 	= tb_aicp_proactor_iocp_delo;
	ptor->base.post 	= tb_aicp_proactor_iocp_post;
	ptor->base.spak 	= tb_aicp_proactor_iocp_spak;

	// init func
	ptor->acceptex 		= tb_iocp_func_acceptex();
	ptor->connectex 	= tb_iocp_func_connectex();
	ptor->transmitfile 	= tb_iocp_func_transmitfile();
	tb_assert_and_check_goto(ptor->acceptex && ptor->connectex && ptor->transmitfile, fail);

	// init mutx
	ptor->mutx 			= tb_mutex_init();
	tb_assert_and_check_goto(ptor->mutx, fail);

	// init port
	ptor->port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, tb_null, 0, 0);
	tb_assert_and_check_goto(ptor->port && ptor->port != INVALID_HANDLE_VALUE, fail);

	// init pool
	ptor->pool = tb_rpool_init((aicp->maxn << 1) + 16, sizeof(tb_iocp_olap_t), 0);
	tb_assert_and_check_goto(ptor->pool, fail);

	// init timer
	ptor->timer = tb_ltimer_init(aicp->maxn, TB_LTIMER_TICK_S, tb_true);
	tb_assert_and_check_goto(ptor->timer, fail);

	// ok
	return (tb_aicp_proactor_t*)ptor;

fail:
	if (ptor) tb_aicp_proactor_iocp_exit(ptor);
	return tb_null;
}

