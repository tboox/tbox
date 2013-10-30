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
//#define TB_TRACE_IMPL_TAG 			"proactor"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

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
typedef BOOL (WINAPI* tb_iocp_acceptex_func_t)( 	SOCKET sListenSocket
												,	SOCKET sAcceptSocket
												,	PVOID lpOutputBuffer
												,	DWORD dwReceiveDataLength
												,	DWORD dwLocalAddressLength
												,	DWORD dwRemoteAddressLength
												,	LPDWORD lpdwBytesReceived
												,	LPOVERLAPPED lpOverlapped);

// the connectex func type from mswsock.h
typedef BOOL (WINAPI* tb_iocp_connectex_func_t)( 	SOCKET s
												, 	struct sockaddr const*name
												,	tb_int_t namelen
												,	PVOID lpSendBuffer
												,	DWORD dwSendDataLength
												,	LPDWORD lpdwBytesSent
												,	LPOVERLAPPED lpOverlapped);

// the overlap type
typedef struct __tb_iocp_olap_t
{
	// the base
	OVERLAPPED 					base;
	
	// the data
	WSABUF 						data;

	//the size
	DWORD 						size;

	// the aice
	tb_aice_t 					aice;

}tb_iocp_olap_t;

// the iocp proactor type
typedef struct __tb_aicp_proactor_iocp_t
{
	// the proactor base
	tb_aicp_proactor_t 			base;

	// the i/o completion port
	HANDLE 						port;

	// the olap pool
	tb_handle_t 				pool;
	
	// the pool mutx
	tb_handle_t 				mutx;

	// the acceptex func
	tb_iocp_acceptex_func_t 	acceptex;

	// the connectex func
	tb_iocp_connectex_func_t 	connectex;

}tb_aicp_proactor_iocp_t;

/* ///////////////////////////////////////////////////////////////////////
 * post
 */
static tb_pointer_t tb_aicp_proactor_iocp_malloc0(tb_aicp_proactor_iocp_t* ptor)
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
	return data;
}
static tb_void_t tb_aicp_proactor_iocp_free(tb_aicp_proactor_iocp_t* ptor, tb_cpointer_t data)
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
static tb_long_t tb_aicp_proactor_iocp_post_acpt(tb_aicp_proactor_t* proactor, tb_aice_t* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && ptor->acceptex && proactor->aicp, -1);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_ACPT, -1);
	
	// trace
	tb_trace_impl("accept: ..");

	// done
	tb_long_t 		ok = -1;
	tb_iocp_olap_t* olap = tb_null;
	tb_bool_t 		acceptex_ok = tb_false;
	do
	{
		// make olap
		olap = tb_aicp_proactor_iocp_malloc0(ptor);
		tb_assert_and_check_break(olap);

		// init olap, hack: sizeof(tb_iocp_olap_t) >= olap->data.len
		olap->data.len 			= (sizeof(SOCKADDR_IN) + 16) << 1;
		olap->data.buf 			= (tb_byte_t*)tb_aicp_proactor_iocp_malloc0(ptor);
		olap->aice 				= *aice;
		olap->aice.u.acpt.sock 	= tb_socket_open(TB_SOCKET_TYPE_TCP);
		tb_assert_static(sizeof(tb_iocp_olap_t) >= ((sizeof(SOCKADDR_IN) + 16) << 1));
		tb_assert_and_check_break(olap->data.buf && olap->aice.u.acpt.sock);

		// ok and done resp
		ok = 1;

		// done acceptex
		DWORD real = 0;
		acceptex_ok = ptor->acceptex( 	(SOCKET)aice->handle - 1
									, 	(SOCKET)olap->aice.u.acpt.sock - 1
									, 	olap->data.buf
									, 	0
									, 	sizeof(SOCKADDR_IN) + 16
									, 	sizeof(SOCKADDR_IN) + 16
									, 	&real
									, 	olap)? tb_true : tb_false;
		tb_trace_impl("acceptex: %d, error: %d", acceptex_ok, WSAGetLastError());
		tb_check_break(acceptex_ok);

		// finished? remove olap and done resp directly
		if (olap) tb_aicp_proactor_iocp_free(ptor, olap);

	} while (0);

	// error? remove olap
	if (ok < 0)
	{
		// exit data
		if (olap->data.buf) tb_aicp_proactor_iocp_free(ptor, olap->data.buf);

		// exit sock
		if (olap->aice.u.acpt.sock) tb_socket_close(olap->aice.u.acpt.sock);

		// exit olap
		if (olap) tb_aicp_proactor_iocp_free(ptor, olap);
	}
	else if (!acceptex_ok)
	{
		// pending? continue it
		if (WSA_IO_PENDING == WSAGetLastError()) ok = 0;
		// failed? remove olap
		else
		{
			// exit data
			if (olap->data.buf) tb_aicp_proactor_iocp_free(ptor, olap->data.buf);

			// exit sock
			if (olap->aice.u.acpt.sock) tb_socket_close(olap->aice.u.acpt.sock);

			// exit olap
			if (olap) tb_aicp_proactor_iocp_free(ptor, olap);

			// failed
			aice->state = TB_AICE_STATE_FAILED;

			// trace
			tb_trace_impl("acceptex: unknown error: %d", WSAGetLastError());
		}
	}

	// done resp?
	return ok;
}
static tb_long_t tb_aicp_proactor_iocp_post_conn(tb_aicp_proactor_t* proactor, tb_aice_t* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, -1);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_CONN, -1);
	tb_assert_and_check_return_val(aice->u.conn.host && aice->u.conn.port , -1);
	
	// trace
	tb_trace_impl("connect: %s:%lu", aice->u.conn.host, aice->u.conn.port);

	// done
	tb_long_t 		ok = -1;
	tb_iocp_olap_t* olap = tb_null;
	tb_bool_t 		connectex_ok = tb_false;
	tb_bool_t 		bind_ok = tb_false;
	do
	{
		// make olap
		olap = tb_aicp_proactor_iocp_malloc0(ptor);
		tb_assert_and_check_break(olap);

		// init olap
		olap->aice = *aice;

		// ok and done resp
		ok = 1;

		// bind it first for connectex
		SOCKADDR_IN local;
		local.sin_family = AF_INET;
		local.sin_addr.S_un.S_addr = INADDR_ANY;
		local.sin_port = 0;
		if (SOCKET_ERROR == bind((SOCKET)aice->handle - 1, (LPSOCKADDR)&local, sizeof(local))) break;
		bind_ok = tb_true;

		// done connectex
		DWORD real = 0;
		SOCKADDR_IN addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(aice->u.conn.port);
		addr.sin_addr.S_un.S_addr = inet_addr(aice->u.conn.host);
		connectex_ok = ptor->connectex( 	(SOCKET)aice->handle - 1
										, 	(struct sockaddr const*)&addr
										, 	sizeof(addr)
										, 	tb_null
										, 	0
										, 	&real
										, 	olap)? tb_true : tb_false;
		tb_trace_impl("connectex: %d, error: %d", connectex_ok, WSAGetLastError());
		tb_check_break(connectex_ok);

		// finished? remove olap and done resp directly
		if (olap) tb_aicp_proactor_iocp_free(ptor, olap);

	} while (0);

	// error? remove olap
	if (ok < 0)
	{
		// exit olap
		if (olap) tb_aicp_proactor_iocp_free(ptor, olap);
	}
	else if (!connectex_ok)
	{
		// pending? continue it
		if (WSA_IO_PENDING == WSAGetLastError()) ok = 0;
		// failed? remove olap
		else
		{
			// exit olap
			if (olap) tb_aicp_proactor_iocp_free(ptor, olap);

			// failed
			aice->state = TB_AICE_STATE_FAILED;

			// connectex: unknown error?
			if (bind_ok)
			{
				// trace
				tb_trace_impl("connectex: unknown error: %d", WSAGetLastError());
			}
		}
	}

	// done resp?
	return ok;
}
static tb_long_t tb_aicp_proactor_iocp_post_recv(tb_aicp_proactor_t* proactor, tb_aice_t* aice)
{	
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, -1);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_RECV, -1);
	tb_assert_and_check_return_val(aice->u.recv.data && aice->u.recv.size , -1);

	// init olap
	tb_iocp_olap_t* olap = tb_aicp_proactor_iocp_malloc0(ptor);
	tb_assert_and_check_return_val(olap, -1);
	olap->data.buf 	= aice->u.recv.data;
	olap->data.len 	= aice->u.recv.size;
	olap->aice 		= *aice;

	// done recv
	DWORD 		flag = 0;
	tb_long_t 	real = WSARecv((SOCKET)aice->handle - 1, &olap->data, 1, &olap->size, &flag, olap, tb_null);
	tb_trace_impl("WSARecv: %ld, error: %d", real, WSAGetLastError());

	// ok?
	if (real > 0)
	{
		// remove olap
		if (olap) tb_aicp_proactor_iocp_free(ptor, olap);

		// the real size
		aice->u.recv.real = real;

		// done resp
		return real;
	}

	// pending? continue it
	if (!real || WSA_IO_PENDING == WSAGetLastError()) return 0;

	// remove olap
	if (olap) tb_aicp_proactor_iocp_free(ptor, olap);

	// done error
	switch (WSAGetLastError())
	{
	// closed?
	case WSAECONNABORTED:
	case WSAECONNRESET:
		aice->state = TB_AICE_STATE_CLOSED;
		break;
	// failed?
	default:
		aice->state = TB_AICE_STATE_FAILED;
		break;
	}

	// done resp
	return 1;
}
static tb_long_t tb_aicp_proactor_iocp_post_read(tb_aicp_proactor_t* proactor, tb_aice_t* aice)
{	
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, -1);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_READ, -1);
	tb_assert_and_check_return_val(aice->u.read.data && aice->u.read.size , -1);

	// init olap
	tb_iocp_olap_t* olap = tb_aicp_proactor_iocp_malloc0(ptor);
	tb_assert_and_check_return_val(olap, -1);
	olap->aice 			= *aice;
	olap->base.Offset 	= aice->u.read.seek;

	// done read
	DWORD 		flag = 0;
	DWORD 		real = 0;
	BOOL 		ok = ReadFile((HANDLE)aice->handle, aice->u.read.data, (DWORD)aice->u.read.size, &real, olap);
	tb_trace_impl("ReadFile: %u, error: %d", real, GetLastError());

	// finished?
	if (ok || real > 0)
	{
		// remove olap
		if (olap) tb_aicp_proactor_iocp_free(ptor, olap);

		// the real size
		aice->u.read.real = real;

		// done resp
		return real;
	}

	// pending? continue it
	if (!real || ERROR_IO_PENDING == GetLastError()) return 0;

	// remove olap
	if (olap) tb_aicp_proactor_iocp_free(ptor, olap);

	// failed
	aice->state = TB_AICE_STATE_FAILED;

	// done resp
	return 1;
}
static tb_long_t tb_aicp_proactor_iocp_post_send(tb_aicp_proactor_t* proactor, tb_aice_t* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, -1);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_SEND, -1);
	tb_assert_and_check_return_val(aice->u.send.data && aice->u.send.size , -1);

	// init olap
	tb_iocp_olap_t* olap = tb_aicp_proactor_iocp_malloc0(ptor);
	tb_assert_and_check_return_val(olap, -1);
	olap->data.buf 	= aice->u.send.data;
	olap->data.len 	= aice->u.send.size;
	olap->aice 		= *aice;

	// done send
	tb_long_t real = WSASend((SOCKET)aice->handle - 1, &olap->data, 1, &olap->size, 0, olap, tb_null);
	tb_trace_impl("WSASend: %ld, error: %d", real, WSAGetLastError());

	// ok?
	if (real > 0)
	{
		// remove olap
		if (olap) tb_aicp_proactor_iocp_free(ptor, olap);

		// the real size
		aice->u.send.real = real;

		// done resp
		return real;
	}

	// pending? continue it
	if (!real || WSA_IO_PENDING == WSAGetLastError()) return 0;

	// remove olap
	if (olap) tb_aicp_proactor_iocp_free(ptor, olap);

	// done error
	switch (WSAGetLastError())
	{
	// closed?
	case WSAECONNABORTED:
	case WSAECONNRESET:
		aice->state = TB_AICE_STATE_CLOSED;
		break;
	// failed?
	default:
		aice->state = TB_AICE_STATE_FAILED;
		break;
	}

	// done resp
	return 1;
}
static tb_long_t tb_aicp_proactor_iocp_post_writ(tb_aicp_proactor_t* proactor, tb_aice_t* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, -1);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_WRIT, -1);
	tb_assert_and_check_return_val(aice->u.writ.data && aice->u.writ.size , -1);

	// init olap
	tb_iocp_olap_t* olap = tb_aicp_proactor_iocp_malloc0(ptor);
	tb_assert_and_check_return_val(olap, -1);
	olap->aice 			= *aice;
	olap->base.Offset 	= aice->u.writ.seek;

	// done writ
	DWORD 		flag = 0;
	DWORD 		real = 0;
	BOOL 		ok = WriteFile((HANDLE)aice->handle, aice->u.writ.data, (DWORD)aice->u.writ.size, &real, olap);
	tb_trace_impl("WriteFile: %u, error: %d", real, GetLastError());

	// finished?
	if (ok || real > 0)
	{
		// remove olap
		if (olap) tb_aicp_proactor_iocp_free(ptor, olap);

		// the real size
		aice->u.writ.real = real;

		// done resp
		return real;
	}

	// pending? continue it
	if (!real || ERROR_IO_PENDING == GetLastError()) return 0;

	// remove olap
	if (olap) tb_aicp_proactor_iocp_free(ptor, olap);

	// failed
	aice->state = TB_AICE_STATE_FAILED;

	// done resp
	return 1;
}

/* ///////////////////////////////////////////////////////////////////////
 * save resp
 */
static tb_long_t tb_aicp_proactor_iocp_save_acpt(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// save resp
	*resp = olap->aice;

	// done
	tb_long_t ok = -1;
	if (wait) 
	{
		// check
		tb_assert(resp->u.acpt.sock);
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
	if (olap->data.buf) tb_aicp_proactor_iocp_free(ptor, olap->data.buf);
	olap->data.buf = tb_null;
	olap->data.len = 0;

	// ok?
	return ok;
}
static tb_long_t tb_aicp_proactor_iocp_save_conn(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// save resp
	*resp = olap->aice;

	// failed? done error
	if (!wait)
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
static tb_long_t tb_aicp_proactor_iocp_save_recv(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// save resp
	*resp = olap->aice;

	// ok? save the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// peer closed?
		if (!real) resp->state = TB_AICE_STATE_CLOSED;
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

	// save the real size	
	resp->u.recv.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_aicp_proactor_iocp_save_send(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// save resp
	*resp = olap->aice;

	// ok? save the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// peer closed?
		if (!real) resp->state = TB_AICE_STATE_CLOSED;
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

	// save the real size	
	resp->u.send.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_aicp_proactor_iocp_save_read(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// save resp
	*resp = olap->aice;

	// ok? save the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// peer closed?
		if (!real) resp->state = TB_AICE_STATE_CLOSED;
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

	// save the real size	
	resp->u.read.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_aicp_proactor_iocp_save_writ(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// save resp
	*resp = olap->aice;

	// ok? save the size
	tb_long_t ok = -1;
	if (wait) 
	{
		// peer closed?
		if (!real) resp->state = TB_AICE_STATE_CLOSED;
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

	// save the real size	
	resp->u.writ.real = real;

	// ok?
	return ok;
}
static tb_long_t tb_aicp_proactor_iocp_save_resp(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_iocp_olap_t* olap, tb_size_t real, tb_bool_t wait)
{
	// check?
	tb_assert_and_check_return_val(resp && olap, -1);

	// save
	static tb_bool_t (*s_save[])(tb_aicp_proactor_iocp_t* , tb_aice_t* , tb_iocp_olap_t* , tb_size_t , tb_bool_t ) = 
	{
		tb_null
	,	tb_aicp_proactor_iocp_save_acpt
	,	tb_aicp_proactor_iocp_save_conn
	,	tb_aicp_proactor_iocp_save_recv
	,	tb_aicp_proactor_iocp_save_send
	,	tb_aicp_proactor_iocp_save_read
	,	tb_aicp_proactor_iocp_save_writ
	};
	tb_assert_and_check_return(olap->aice.code < tb_arrayn(s_save));

	// save aice
	return (s_save[olap->aice.code])? s_save[olap->aice.code](ptor, resp, olap, real, wait) : -1;
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_proactor_iocp_addo(tb_aicp_proactor_t* proactor, tb_handle_t handle, tb_size_t type)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp && handle && type, tb_false);

	// attach aico to port
	HANDLE port = CreateIoCompletionPort((HANDLE)handle, ptor->port, (ULONG*)type, 0);
	tb_assert_and_check_return_val(port == ptor->port, tb_false);

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_proactor_iocp_delo(tb_aicp_proactor_t* proactor, tb_handle_t handle)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp && handle, tb_false);

	// ok
	return tb_true;
}
static tb_long_t tb_aicp_proactor_iocp_post(tb_aicp_proactor_t* proactor, tb_aice_t* reqt)
{
	// post
	static tb_long_t (*s_post[])(tb_aicp_proactor_t* , tb_aice_t*) = 
	{
		tb_null
	,	tb_aicp_proactor_iocp_post_acpt
	,	tb_aicp_proactor_iocp_post_conn
	,	tb_aicp_proactor_iocp_post_recv
	,	tb_aicp_proactor_iocp_post_send
	,	tb_aicp_proactor_iocp_post_read
	,	tb_aicp_proactor_iocp_post_writ
	};
	tb_assert_and_check_return(reqt->code < tb_arrayn(s_post));

	// post reqt
	return (s_post[reqt->code])? s_post[reqt->code](proactor, reqt) : -1;
}

static tb_long_t tb_aicp_proactor_iocp_spak(tb_aicp_proactor_t* proactor, tb_aice_t* resp, tb_long_t timeout)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && resp, -1);

	// trace
	tb_trace_impl("spak[%lu]: ..", tb_thread_self());

	// wait
	DWORD 			real = 0;
	tb_size_t 		type = 0;
	tb_iocp_olap_t* olap = tb_null;
	BOOL 			wait = GetQueuedCompletionStatus(ptor->port, (LPDWORD)&real, (LPDWORD)&type, &olap, timeout < 0? INFINITE : timeout);

	// trace
	tb_trace_impl("spak[%lu]: %d error: %u", tb_thread_self(), wait, GetLastError());

	// timeout?
	if (!wait && WAIT_TIMEOUT == GetLastError()) return 0;

	// killed?
	if (wait && !type) return -1;

	// save resp
	tb_long_t ok = tb_aicp_proactor_iocp_save_resp(ptor, resp, olap, (tb_size_t)real, wait? tb_true : tb_false);

	// free olap
	tb_aicp_proactor_iocp_free(ptor, olap);

	// ok?
	return ok;
}
static tb_void_t tb_aicp_proactor_iocp_kill(tb_aicp_proactor_t* proactor)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return(ptor && ptor->port && proactor->aicp);

	// the worker size
	tb_size_t work = tb_atomic_get(&proactor->aicp->work);

	// trace
	tb_trace_impl("kill: %lu", work);

	// kill workers
	tb_size_t i = 0;
	for (i = 0; i < work; i++) PostQueuedCompletionStatus(ptor->port, 0, 0, tb_null);

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

		// exit mutx
		if (ptor->mutx) tb_mutex_exit(ptor->mutx);
		ptor->mutx = tb_null;

		// free it
		tb_free(ptor);
	}
}
static tb_iocp_acceptex_func_t tb_aicp_proactor_iocp_acceptex_func()
{
	// done
	tb_long_t 				ok = -1;
	tb_handle_t 			sock = tb_null;
	tb_iocp_acceptex_func_t acceptex = tb_null;
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
					, 	sizeof(tb_iocp_acceptex_func_t)
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
static tb_iocp_acceptex_func_t tb_aicp_proactor_iocp_connectex_func()
{
	// done
	tb_long_t 					ok = -1;
	tb_handle_t 				sock = tb_null;
	tb_iocp_connectex_func_t 	connectex = tb_null;
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
					, 	sizeof(tb_iocp_connectex_func_t)
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

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp);
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->maxn, tb_null);

	// alloc proactor
	tb_aicp_proactor_iocp_t* ptor = tb_malloc0(sizeof(tb_aicp_proactor_iocp_t));
	tb_assert_and_check_return_val(ptor, tb_null);

	// init base
	ptor->base.aicp 	= aicp;
	ptor->base.kill 	= tb_aicp_proactor_iocp_kill;
	ptor->base.exit 	= tb_aicp_proactor_iocp_exit;
	ptor->base.addo 	= tb_aicp_proactor_iocp_addo;
	ptor->base.delo 	= tb_aicp_proactor_iocp_delo;
	ptor->base.post 	= tb_aicp_proactor_iocp_post;
	ptor->base.spak 	= tb_aicp_proactor_iocp_spak;
	ptor->acceptex 		= tb_aicp_proactor_iocp_acceptex_func();
	ptor->connectex 	= tb_aicp_proactor_iocp_connectex_func();
	ptor->mutx 			= tb_mutex_init(tb_null);
	tb_assert_and_check_goto(ptor->acceptex && ptor->connectex, fail);

	// init port
	ptor->port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, tb_null, 0, 0);
	tb_assert_and_check_goto(ptor->port && ptor->port != INVALID_HANDLE_VALUE, fail);

	// init pool
	ptor->pool = tb_rpool_init((aicp->maxn << 1) + 16, sizeof(tb_iocp_olap_t), 0);
	tb_assert_and_check_goto(ptor->pool, fail);

	// ok
	return (tb_aicp_proactor_t*)ptor;

fail:
	if (ptor) tb_aicp_proactor_iocp_exit(ptor);
	return tb_null;
}

