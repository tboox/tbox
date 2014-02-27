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
#define TB_TRACE_IMPL_TAG 			"aicp_iocp"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "winerror.h"
#include "../../../ltimer.h"
#include "../../../../asio/pool.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the olap list maxn for GetQueuedCompletionStatusEx and adapter to queue 
#ifdef __tb_small__
# 	define TB_IOCP_OLAP_LIST_MAXN 					(63)
#else
# 	define TB_IOCP_OLAP_LIST_MAXN 					(255)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the iocp proactor type
typedef struct __tb_aicp_proactor_iocp_t
{
	// the proactor base
	tb_aicp_proactor_t 						base;

	// the i/o completion port
	HANDLE 									port;

	// the spak timer for task
	tb_handle_t								timer;

	// the spak low precision timer for timeout
	tb_handle_t								ltimer;

	// the timer loop
	tb_handle_t 							loop;

	// the timer event
	tb_handle_t 							event;

	// the AcceptEx func
	tb_api_AcceptEx_t 						AcceptEx;

	// the ConnectEx func
	tb_api_ConnectEx_t 						ConnectEx;

	// the TransmitFile func
	tb_api_TransmitFile_t 					TransmitFile;

	// the CancelIoEx func
	tb_api_CancelIoEx_t 					CancelIoEx;

	// the GetQueuedCompletionStatusEx func
	tb_api_GetQueuedCompletionStatusEx_t 	GetQueuedCompletionStatusEx;

}tb_aicp_proactor_iocp_t;

// the iocp olap type
typedef struct __tb_iocp_olap_t
{
	// the base
	OVERLAPPED 								base;
	
	// the aice
	tb_aice_t 								aice;

}tb_iocp_olap_t;

// the iocp aico type
typedef struct __tb_iocp_aico_t
{
	// the base
	tb_aico_t 								base;

	// the ptor
	tb_aicp_proactor_iocp_t* 				ptor;

	// the olap
	tb_iocp_olap_t 							olap;
	
	// the task
	tb_handle_t 							task;

	// is ltimer?
	tb_uint8_t 								bltimer : 1;

}tb_iocp_aico_t;

// the iocp loop type
typedef struct __tb_iocp_loop_t
{
	// the self
	tb_handle_t 							self;

	// the olap list
	OVERLAPPED_ENTRY 						list[TB_IOCP_OLAP_LIST_MAXN];

	// the aice spak 
	tb_queue_t* 							spak;					

}tb_iocp_loop_t;

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
static tb_void_t tb_iocp_spak_timeout(tb_bool_t killed, tb_pointer_t data);
static tb_void_t tb_iocp_spak_runtask_timeout(tb_bool_t killed, tb_pointer_t data);

/* ///////////////////////////////////////////////////////////////////////
 * timer 
 */
static tb_pointer_t tb_iocp_timer_loop(tb_pointer_t data)
{
	// check
	tb_aicp_proactor_iocp_t* 	ptor = (tb_aicp_proactor_iocp_t*)data;
	tb_aicp_t* 					aicp = ptor? ptor->base.aicp : tb_null;
	tb_assert_and_check_return_val(ptor && ptor->timer && ptor->ltimer && ptor->event && aicp, tb_null);

	// trace
	tb_trace_impl("loop: init");

	// loop 
	while (!tb_atomic_get(&aicp->kill))
	{
		// the delay
		tb_size_t delay = tb_timer_delay(ptor->timer);

		// the ldelay
		tb_size_t ldelay = tb_ltimer_delay(ptor->ltimer);
		tb_assert_and_check_break(ldelay != -1);

		// using the min delay
		if (ldelay < delay) delay = ldelay;

		// wait some time
		if (delay)
		{
			if (tb_event_wait(ptor->event, delay) < 0) break;
		}

		// spak ctime
		tb_ctime_spak();

		// spak timer
		if (!tb_timer_spak(ptor->timer)) break;

		// spak ltimer
		if (!tb_ltimer_spak(ptor->ltimer)) break;
	}

end:
	// trace
	tb_trace_impl("loop: exit");

	// kill
	tb_atomic_set(&aicp->kill, 1);

	// exit
	tb_thread_return(tb_null);
	return tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * post
 */
static tb_void_t tb_iocp_post_timeout(tb_aicp_proactor_t* proactor, tb_iocp_aico_t* aico)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->ltimer && aico && !aico->task, tb_false);
	
	// only for sock
	tb_check_return(aico->base.type == TB_AICO_TYPE_SOCK);

	// add timeout task
	tb_long_t timeout = tb_aico_timeout_from_code(aico, aico->olap.aice.code);
	if (timeout >= 0)
	{
		// add the new task
		aico->task = tb_ltimer_task_add(ptor->ltimer, timeout, tb_false, tb_iocp_spak_timeout, aico);
		aico->bltimer = 1;
	}
}
static tb_bool_t tb_iocp_post_acpt(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && ptor->AcceptEx && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_ACPT, tb_false);

	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// trace
	tb_trace_impl("accept: ..");

	// done
	tb_bool_t 		ok = tb_false;
	tb_bool_t 		init_ok = tb_false;
	tb_bool_t 		AcceptEx_ok = tb_false;
	do
	{
		// init olap
		tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

		// init aice, hack: sizeof(tb_iocp_olap_t) >= ((sizeof(SOCKADDR_IN) + 16) << 1)
		aico->olap.aice 				= *aice;
		aico->olap.aice.u.acpt.sock 	= tb_socket_open(TB_SOCKET_TYPE_TCP);
		aico->olap.aice.u.acpt.priv[0] = (tb_handle_t)tb_aico_pool_malloc0(aico, ((sizeof(SOCKADDR_IN) + 16) << 1));
		tb_assert_static(tb_arrayn(aico->olap.aice.u.acpt.priv));
		tb_assert_and_check_break(aico->olap.aice.u.acpt.priv[0] && aico->olap.aice.u.acpt.sock);
		init_ok = tb_true;

		// done AcceptEx
		DWORD real = 0;
		AcceptEx_ok = ptor->AcceptEx( 	(SOCKET)aico->base.handle - 1
									, 	(SOCKET)aico->olap.aice.u.acpt.sock - 1
									, 	(tb_byte_t*)aico->olap.aice.u.acpt.priv[0]
									, 	0
									, 	sizeof(SOCKADDR_IN) + 16
									, 	sizeof(SOCKADDR_IN) + 16
									, 	&real
									, 	&aico->olap)? tb_true : tb_false;
		tb_trace_impl("AcceptEx: %d, error: %d", AcceptEx_ok, WSAGetLastError());
		tb_check_break(AcceptEx_ok);

		// post ok
		aico->olap.aice.state = TB_AICE_STATE_OK;
		if (!PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) break;

		// ok
		ok = tb_true;

	} while (0);

	// AcceptEx failed? 
	if (init_ok && !AcceptEx_ok)
	{
		// pending? continue it
		if (WSA_IO_PENDING == WSAGetLastError()) 
		{
			// post timeout
			tb_iocp_post_timeout(proactor, aico);

			// ok
			ok = tb_true;
		}
		// failed? 
		else
		{
			// post failed
			aico->olap.aice.state = TB_AICE_STATE_FAILED;
			if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) ok = tb_true;

			// trace
			tb_trace_impl("AcceptEx: unknown error: %d", WSAGetLastError());
		}
	}

	// failed? 
	if (!ok)
	{
		// exit data
		if (aico->olap.aice.u.acpt.priv[0]) tb_aico_pool_free(aico, aico->olap.aice.u.acpt.priv[0]);
		aico->olap.aice.u.acpt.priv[0] = tb_null;

		// exit sock
		if (aico->olap.aice.u.acpt.sock) tb_socket_clos(aico->olap.aice.u.acpt.sock);
		aico->olap.aice.u.acpt.sock = tb_null;
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
	tb_assert_and_check_return_val(aice->u.conn.port, tb_false);
	
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// trace
	tb_trace_impl("connect: %u.%u.%u.%u: %lu", aice->u.conn.addr.u8[0], aice->u.conn.addr.u8[1], aice->u.conn.addr.u8[2], aice->u.conn.addr.u8[3], aice->u.conn.port);

	// done
	tb_bool_t 		ok = tb_false;
	tb_bool_t 		init_ok = tb_false;
	tb_bool_t 		ConnectEx_ok = tb_false;
	do
	{
		// init olap
		tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

		// init aice
		aico->olap.aice = *aice;

		// bind it first for ConnectEx
		SOCKADDR_IN local;
		local.sin_family = AF_INET;
		local.sin_addr.S_un.S_addr = INADDR_ANY;
		local.sin_port = 0;
		if (SOCKET_ERROR == bind((SOCKET)aico->base.handle - 1, (LPSOCKADDR)&local, sizeof(local))) break;
		init_ok = tb_true;

		// done ConnectEx
		DWORD real = 0;
		SOCKADDR_IN addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(aice->u.conn.port);
		addr.sin_addr.S_un.S_addr = aice->u.conn.addr.u32;
		ConnectEx_ok = ptor->ConnectEx( 	(SOCKET)aico->base.handle - 1
										, 	(struct sockaddr const*)&addr
										, 	sizeof(addr)
										, 	tb_null
										, 	0
										, 	&real
										, 	&aico->olap)? tb_true : tb_false;
		tb_trace_impl("ConnectEx: %d, error: %d", ConnectEx_ok, WSAGetLastError());
		tb_check_break(ConnectEx_ok);

		// post ok
		aico->olap.aice.state = TB_AICE_STATE_OK;
		if (!PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) break;

		// ok
		ok = tb_true;

	} while (0);

	// ConnectEx failed?
	if (init_ok && !ConnectEx_ok)
	{
		// pending? continue it
		if (WSA_IO_PENDING == WSAGetLastError()) 
		{	
			// post timeout
			tb_iocp_post_timeout(proactor, aico);

			// ok
			ok = tb_true;
		}
		// failed?
		else
		{
			// post failed
			aico->olap.aice.state = TB_AICE_STATE_FAILED;
			if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) ok = tb_true;

			// trace
			tb_trace_impl("ConnectEx: unknown error: %d", WSAGetLastError());
		}
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
	tb_assert_and_check_return_val(aice->u.recv.data && aice->u.recv.size, tb_false);
	
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

	// init aice
	aico->olap.aice = *aice;

	// done recv
	DWORD 		flag = 0;
	tb_long_t 	ok = WSARecv((SOCKET)aico->base.handle - 1, (WSABUF*)&aico->olap.aice.u.recv, 1, tb_null, &flag, &aico->olap, tb_null);
	tb_trace_impl("WSARecv: %ld, error: %d", ok, WSAGetLastError());

	// ok or pending? continue it
	if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == WSAGetLastError())))
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// error?
	if (ok == SOCKET_ERROR)
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			aico->olap.aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			aico->olap.aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

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
	tb_assert_and_check_return_val(aice->u.send.data && aice->u.send.size, tb_false);
	
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

	// init aice
	aico->olap.aice = *aice;

	// done send
	tb_long_t ok = WSASend((SOCKET)aico->base.handle - 1, (WSABUF*)&aico->olap.aice.u.send, 1, tb_null, 0, &aico->olap, tb_null);
	tb_trace_impl("WSASend: %ld, error: %d", ok, WSAGetLastError());

	// ok or pending? continue it
	if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == WSAGetLastError())))
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// error?
	if (ok == SOCKET_ERROR)
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			aico->olap.aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			aico->olap.aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_urecv(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{	
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_URECV, tb_false);
	tb_assert_and_check_return_val(aice->u.urecv.data && aice->u.urecv.size, tb_false);
	tb_assert_and_check_return_val(aice->u.urecv.addr.u32 && aice->u.urecv.port, tb_false);
	
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

	// init aice
	aico->olap.aice = *aice;

	// init addr
	SOCKADDR_IN 				addr = {0};
	addr.sin_family 			= AF_INET;
	addr.sin_port 				= htons(aice->u.urecv.port);
	addr.sin_addr.S_un.S_addr 	= aice->u.urecv.addr.u32;

	// done recv
	DWORD 		flag = 0;
	tb_int_t 	size = sizeof(addr);
	tb_long_t 	ok = WSARecvFrom((SOCKET)aico->base.handle - 1, (WSABUF*)&aico->olap.aice.u.urecv, 1, tb_null, &flag, &addr, &size, &aico->olap, tb_null);
	tb_trace_impl("WSARecv: %ld, error: %d", ok, WSAGetLastError());

	// ok or pending? continue it
	if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == WSAGetLastError())))
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// error?
	if (ok == SOCKET_ERROR)
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			aico->olap.aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			aico->olap.aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_usend(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_USEND, tb_false);
	tb_assert_and_check_return_val(aice->u.usend.data && aice->u.usend.size, tb_false);
	tb_assert_and_check_return_val(aice->u.usend.addr.u32 && aice->u.usend.port, tb_false);
	
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

	// init aice
	aico->olap.aice = *aice;

	// init addr
	SOCKADDR_IN 				addr = {0};
	addr.sin_family 			= AF_INET;
	addr.sin_port 				= htons(aice->u.usend.port);
	addr.sin_addr.S_un.S_addr 	= aice->u.usend.addr.u32;

	// done send
	tb_long_t ok = WSASendTo((SOCKET)aico->base.handle - 1, (WSABUF*)&aico->olap.aice.u.usend, 1, tb_null, 0, &addr, sizeof(addr), &aico->olap, tb_null);
	tb_trace_impl("WSASend: %ld, error: %d", ok, WSAGetLastError());

	// ok or pending? continue it
	if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == WSAGetLastError())))
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// error?
	if (ok == SOCKET_ERROR)
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			aico->olap.aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			aico->olap.aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

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
	tb_assert_and_check_return_val(aice->u.recvv.list && aice->u.recvv.size, tb_false);
	
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

	// init aice
	aico->olap.aice = *aice;

	// done recv
	DWORD 		flag = 0;
	tb_long_t 	ok = WSARecv((SOCKET)aico->base.handle - 1, (WSABUF*)aico->olap.aice.u.recvv.list, (DWORD)aico->olap.aice.u.recvv.size, tb_null, &flag, &aico->olap, tb_null);
	tb_trace_impl("WSARecv: %ld, error: %d", ok, WSAGetLastError());

	// ok or pending? continue it
	if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == WSAGetLastError())))
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// error?
	if (ok == SOCKET_ERROR)
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			aico->olap.aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			aico->olap.aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

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
	tb_assert_and_check_return_val(aice->u.sendv.list && aice->u.sendv.size, tb_false);
	
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

	// init aice
	aico->olap.aice = *aice;

	// done send
	tb_long_t ok = WSASend((SOCKET)aico->base.handle - 1, (WSABUF*)aico->olap.aice.u.sendv.list, (DWORD)aico->olap.aice.u.sendv.size, tb_null, 0, &aico->olap, tb_null);
	tb_trace_impl("WSASend: %ld, error: %d", ok, WSAGetLastError());

	// ok or pending? continue it
	if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == WSAGetLastError())))
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// error?
	if (ok == SOCKET_ERROR)
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			aico->olap.aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			aico->olap.aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_urecvv(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{	
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_URECVV, tb_false);
	tb_assert_and_check_return_val(aice->u.urecvv.list && aice->u.urecvv.size, tb_false);
	tb_assert_and_check_return_val(aice->u.urecvv.addr.u32 && aice->u.urecvv.port, tb_false);
	
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

	// init aice
	aico->olap.aice = *aice;

	// init addr
	SOCKADDR_IN 				addr = {0};
	addr.sin_family 			= AF_INET;
	addr.sin_port 				= htons(aice->u.urecvv.port);
	addr.sin_addr.S_un.S_addr 	= aice->u.urecv.addr.u32;

	// done recv
	DWORD 		flag = 0;
	tb_int_t 	size = sizeof(addr);
	tb_long_t 	ok = WSARecvFrom((SOCKET)aico->base.handle - 1, (WSABUF*)aico->olap.aice.u.urecvv.list, (DWORD)aico->olap.aice.u.urecvv.size, tb_null, &flag, &addr, &size, &aico->olap, tb_null);
	tb_trace_impl("WSARecvFrom: %ld, error: %d", ok, WSAGetLastError());

	// ok or pending? continue it
	if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == WSAGetLastError())))
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// error?
	if (ok == SOCKET_ERROR)
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			aico->olap.aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			aico->olap.aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_usendv(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_USENDV, tb_false);
	tb_assert_and_check_return_val(aice->u.usendv.list && aice->u.usendv.size, tb_false);
	tb_assert_and_check_return_val(aice->u.usendv.addr.u32 && aice->u.usendv.port, tb_false);
	
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

	// init aice
	aico->olap.aice = *aice;

	// init addr
	SOCKADDR_IN 				addr = {0};
	addr.sin_family 			= AF_INET;
	addr.sin_port 				= htons(aice->u.usendv.port);
	addr.sin_addr.S_un.S_addr 	= aice->u.urecv.addr.u32;

	// done send
	tb_long_t ok = WSASendTo((SOCKET)aico->base.handle - 1, (WSABUF*)aico->olap.aice.u.usendv.list, (DWORD)aico->olap.aice.u.usendv.size, tb_null, 0, &addr, sizeof(addr), &aico->olap, tb_null);
	tb_trace_impl("WSASendTo: %ld, error: %d", ok, WSAGetLastError());

	// ok or pending? continue it
	if (!ok || ((ok == SOCKET_ERROR) && (WSA_IO_PENDING == WSAGetLastError())))
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// error?
	if (ok == SOCKET_ERROR)
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			aico->olap.aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			aico->olap.aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

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
	tb_assert_and_check_return_val(aice->u.sendfile.file && aice->u.sendfile.size, tb_false);
	
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));
	aico->olap.base.Offset 	= aice->u.sendfile.seek;

	// init aice
	aico->olap.aice = *aice;

	// not supported?
	if (!ptor->TransmitFile)
	{
		// post not supported
		aico->olap.aice.state = TB_AICE_STATE_NOTSUPPORTED;
		return PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)? tb_true : tb_false;
	}

	// done send
	tb_long_t real = ptor->TransmitFile((SOCKET)aico->base.handle - 1, (HANDLE)aice->u.sendfile.file, (DWORD)aice->u.sendfile.size, (1 << 16), &aico->olap, tb_null, 0);
	tb_trace_impl("sendfile: %ld, error: %d", real, WSAGetLastError());

	// pending? continue it
	if (!real || WSA_IO_PENDING == WSAGetLastError()) 
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// ok?
	if (real > 0)
	{
		// post ok
		aico->olap.aice.state = TB_AICE_STATE_OK;
		aico->olap.aice.u.sendfile.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}
	else
	{
		// done error
		switch (WSAGetLastError())
		{
		// closed?
		case WSAECONNABORTED:
		case WSAECONNRESET:
			aico->olap.aice.state = TB_AICE_STATE_CLOSED;
			break;
		// failed?
		default:
			aico->olap.aice.state = TB_AICE_STATE_FAILED;
			break;
		}

		// post closed or failed
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

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
	tb_assert_and_check_return_val(aice->u.read.data && aice->u.read.size, tb_false);

	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));
	aico->olap.base.Offset 	= aice->u.read.seek;

	// init aice
	aico->olap.aice = *aice;

	// done read
	DWORD 		real = 0;
	BOOL 		ok = ReadFile((HANDLE)aico->base.handle, aice->u.read.data, (DWORD)aice->u.read.size, &real, &aico->olap);
	tb_trace_impl("ReadFile: real: %u, size: %lu, error: %d", real, aice->u.read.size, GetLastError());

	// pending? continue it
	if (!real || ERROR_IO_PENDING == GetLastError())
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// finished?
	if (ok || real > 0)
	{
		// post ok
		aico->olap.aice.state = TB_AICE_STATE_OK;
		aico->olap.aice.u.read.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}
	else 
	{
		// post failed
		aico->olap.aice.state = TB_AICE_STATE_FAILED;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

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
	tb_assert_and_check_return_val(aice->u.writ.data && aice->u.writ.size, tb_false);

	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));
	aico->olap.base.Offset 	= aice->u.writ.seek;

	// init aice
	aico->olap.aice = *aice;

	// done writ
	DWORD 		real = 0;
	BOOL 		ok = WriteFile((HANDLE)aico->base.handle, aice->u.writ.data, (DWORD)aice->u.writ.size, &real, &aico->olap);
	tb_trace_impl("WriteFile: real: %u, size: %lu, error: %d", real, aice->u.writ.size, GetLastError());

	// pending? continue it
	if (!real || ERROR_IO_PENDING == GetLastError())
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// finished?
	if (ok || real > 0)
	{
		// post ok
		aico->olap.aice.state = TB_AICE_STATE_OK;
		aico->olap.aice.u.writ.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}
	else
	{
		// post failed
		aico->olap.aice.state = TB_AICE_STATE_FAILED;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

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
	tb_assert_and_check_return_val(aice->u.readv.list && aice->u.readv.size, tb_false);

	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));
	aico->olap.base.Offset 	= aice->u.readv.seek;

	// init aice
	aico->olap.aice = *aice;

	// done read
	DWORD 		real = 0;
	BOOL 		ok = ReadFile((HANDLE)aico->base.handle, aice->u.readv.list[0].data, (DWORD)aice->u.readv.list[0].size, &real, &aico->olap);
	tb_trace_impl("ReadFile: %u, error: %d", real, GetLastError());

	// pending? continue it
	if (!real || ERROR_IO_PENDING == GetLastError())
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// finished?
	if (ok || real > 0)
	{
		// post ok
		aico->olap.aice.state = TB_AICE_STATE_OK;
		aico->olap.aice.u.readv.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}
	else 
	{
		// post failed
		aico->olap.aice.state = TB_AICE_STATE_FAILED;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

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
	tb_assert_and_check_return_val(aice->u.writv.list && aice->u.writv.size, tb_false);

	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));
	aico->olap.base.Offset 	= aice->u.writv.seek;

	// init aice
	aico->olap.aice = *aice;

	// done writ
	DWORD 		real = 0;
	BOOL 		ok = WriteFile((HANDLE)aico->base.handle, aice->u.writv.list[0].data, (DWORD)aice->u.writv.list[0].size, &real, &aico->olap);
	tb_trace_impl("WriteFile: %u, error: %d", real, GetLastError());

	// pending? continue it
	if (!real || ERROR_IO_PENDING == GetLastError())
	{
		// post timeout 
		tb_iocp_post_timeout(proactor, aico);

		// ok
		return tb_true;
	}

	// finished?
	if (ok || real > 0)
	{
		// post ok
		aico->olap.aice.state = TB_AICE_STATE_OK;
		aico->olap.aice.u.writv.real = real;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}
	else
	{
		// post failed
		aico->olap.aice.state = TB_AICE_STATE_FAILED;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}

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
	
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && aico->base.handle, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

	// init aice
	aico->olap.aice = *aice;

	// post ok
	aico->olap.aice.state = TB_AICE_STATE_OK;
	if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;

	// failed
	return tb_false;
}
static tb_bool_t tb_iocp_post_runtask(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && ptor->timer && ptor->ltimer && ptor->event && proactor->aicp, tb_false);

	// check aice
	tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RUNTASK, tb_false);
	tb_assert_and_check_return_val(aice->state == TB_AICE_STATE_PENDING, tb_false);
	tb_assert_and_check_return_val(aice->u.runtask.when, tb_false);
	
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico && !aico->task, tb_false);

	// init olap
	tb_memset(&aico->olap, 0, sizeof(tb_iocp_olap_t));

	// init aice
	aico->olap.aice = *aice;

	// now
	tb_hong_t now = tb_ctime_time();

	// timeout?
	if (aice->u.runtask.when <= now)
	{
		// trace
		tb_trace_impl("runtask: when: %llu, now: %lld: ok", aice->u.runtask.when, now);

		// post ok
		aico->olap.aice.state = TB_AICE_STATE_OK;
		if (PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)) return tb_true;
	}
	else
	{
		// trace
		tb_trace_impl("runtask: when: %llu, now: %lld: ..", aice->u.runtask.when, now);

		// add timeout task, is the higher precision timer?
		if (aico->base.handle)
		{
			// the top when
			tb_hize_t top = tb_timer_top(ptor->timer);

			// add task
			aico->task = tb_timer_task_add_at(ptor->timer, aice->u.runtask.when, 0, tb_false, tb_iocp_spak_runtask_timeout, aico);
			aico->bltimer = 0;

			// the top task is changed? spak the timer
			if (aico->task && aice->u.runtask.when < top)
				tb_event_post(ptor->event);
		}
		else
		{
			aico->task = tb_ltimer_task_add_at(ptor->ltimer, aice->u.runtask.when, 0, tb_false, tb_iocp_spak_runtask_timeout, aico);
			aico->bltimer = 1;
		}

		// pending
		return tb_true;
	}


	// failed
	return tb_false;
}


/* ///////////////////////////////////////////////////////////////////////
 * spak 
 */
static tb_void_t tb_iocp_spak_timeout(tb_bool_t killed, tb_pointer_t data)
{
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)data;
	tb_assert_and_check_return(aico);

	// the ptor
	tb_aicp_proactor_iocp_t* ptor = aico->ptor;
	tb_assert_and_check_return(ptor);

	// cancel it
	switch (aico->base.type)
	{
	case TB_AICO_TYPE_SOCK:
	case TB_AICO_TYPE_FILE:
		{
			// check
			tb_assert_and_check_break(aico->base.handle);

			// trace
			tb_trace_impl("spak: timeout[%p]: code: %lu", aico->base.handle, aico->olap.aice.code);

			// exists CancelIoEx?
			if (ptor->CancelIoEx)
			{
				// save state: timeout
				aico->olap.aice.state = killed? TB_AICE_STATE_KILLED : TB_AICE_STATE_TIMEOUT;

				// the handle
				HANDLE handle = aico->base.type == TB_AICO_TYPE_SOCK? (HANDLE)((SOCKET)aico->base.handle - 1) : aico->base.handle;

				// CancelIoEx it
				if (!ptor->CancelIoEx(handle, &aico->olap))
				{
					tb_trace_impl("cancel: failed: %u", GetLastError());
				}
			}
			else
			{
				// TODO
				tb_trace_impl("cancel: not impl");
			}
		}
		break;
	default:
		tb_assert(0);
		break;
	}
}
static tb_void_t tb_iocp_spak_runtask_timeout(tb_bool_t killed, tb_pointer_t data)
{
	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)data;
	tb_assert_and_check_return(aico);

	// the ptor
	tb_aicp_proactor_iocp_t* ptor = aico->ptor;
	tb_assert_and_check_return(ptor);

	// trace
	tb_trace_impl("runtask: timeout: when: %llu", aico->olap.aice.u.runtask.when);

	// post ok
	aico->olap.aice.state = killed? TB_AICE_STATE_KILLED : TB_AICE_STATE_OK;
	PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap);
}
static tb_long_t tb_iocp_spak_acpt(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_size_t real, tb_size_t error)
{
	// check?
	tb_assert_and_check_return_val(ptor && resp && resp->aico, -1);

	// done
	switch (error)
	{
		// ok or pending?
	case ERROR_SUCCESS:
	case WAIT_TIMEOUT:
	case ERROR_IO_PENDING:
		{
			// done state
			switch (resp->state)
			{
			case TB_AICE_STATE_OK:
			case TB_AICE_STATE_PENDING:
				resp->state = resp->u.acpt.sock? TB_AICE_STATE_OK : TB_AICE_STATE_FAILED;
				break;
			default:
				// using the self state here
				break;
			}
		}
		break;
		// canceled? timeout?
	case WSAEINTR:
	case ERROR_OPERATION_ABORTED:
		{
			resp->state = TB_AICE_STATE_TIMEOUT;
		}
		break;
		// unknown error
	default:
		{
			resp->state = TB_AICE_STATE_FAILED;
			tb_trace_impl("acpt: unknown error: %u", error);
		}
		break;
	}

	// failed? exit sock
	if (resp->state != TB_AICE_STATE_OK)
	{
		if (resp->u.acpt.sock) tb_socket_clos(resp->u.acpt.sock);
		resp->u.acpt.sock = tb_null;
	}

	// exit data
	if (resp->u.acpt.priv[0]) tb_aico_pool_free(resp->aico, resp->u.acpt.priv[0]);
	resp->u.acpt.priv[0] = tb_null;

	// ok
	return 1;
}
static tb_long_t tb_iocp_spak_conn(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_size_t real, tb_size_t error)
{
	// check?
	tb_assert_and_check_return_val(resp, -1);

	// done
	switch (error)
	{
		// ok or pending?
	case ERROR_SUCCESS:
	case WAIT_TIMEOUT:
	case ERROR_IO_PENDING:
		{
			// done state
			switch (resp->state)
			{
			case TB_AICE_STATE_OK:
			case TB_AICE_STATE_PENDING:
				resp->state = TB_AICE_STATE_OK;
				break;
			default:
				// using the self state here
				break;
			}
		}
		break;
		// failed?
	case WSAENOTCONN:
	case WSAECONNREFUSED:
	case ERROR_CONNECTION_REFUSED:
		{
			resp->state = TB_AICE_STATE_FAILED;
		}
		break;
		// timeout?
	case WSAEINTR:
	case ERROR_SEM_TIMEOUT:
	case ERROR_OPERATION_ABORTED:
		{
			resp->state = TB_AICE_STATE_TIMEOUT;
		}
		break;
		// unknown error
	default:
		{
			resp->state = TB_AICE_STATE_FAILED;
			tb_trace_impl("conn: unknown error: %u", error);
		}
		break;
	}

	// ok
	return 1;
}
static tb_long_t tb_iocp_spak_iorw(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_size_t real, tb_size_t error)
{
	// check?
	tb_assert_and_check_return_val(resp, -1);

	// ok?
	if (real)
	{
		// save the real size, @note: hack the real offset for the other io aice
		resp->u.recv.real = real;

		// ok
		resp->state = TB_AICE_STATE_OK;
		return 1;
	}

	// error? 
	switch (error)
	{		
		// ok or pending?
	case ERROR_SUCCESS:
	case WAIT_TIMEOUT:
	case ERROR_IO_PENDING:
		{
			// done state
			switch (resp->state)
			{
			case TB_AICE_STATE_OK:
			case TB_AICE_STATE_PENDING:
				resp->state = TB_AICE_STATE_CLOSED;
				break;
			default:
				// using the self state here
				break;
			}
		}
		break;
		// closed?
	case WSAECONNRESET:
	case ERROR_HANDLE_EOF:
	case ERROR_NETNAME_DELETED:
		{
			resp->state = TB_AICE_STATE_CLOSED;
		}
		break;
		// canceled? timeout 
	case WSAEINTR:
	case ERROR_OPERATION_ABORTED:
		{
			resp->state = TB_AICE_STATE_TIMEOUT;
		}
		break;
		// unknown error
	default:
		{
			resp->state = TB_AICE_STATE_FAILED;
			tb_trace_impl("iorw: code: %lu, unknown error: %u", resp->code, error);
		}
		break;
	}

	// ok
	return 1;
}
static tb_long_t tb_iocp_spak_fsync(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_size_t real, tb_size_t error)
{
	// check?
	tb_assert_and_check_return_val(resp, -1);

	// done 
	switch (error)
	{	
		// ok or pending?
	case ERROR_SUCCESS:
	case WAIT_TIMEOUT:
	case ERROR_IO_PENDING:
		{
			// done state
			switch (resp->state)
			{
			case TB_AICE_STATE_OK:
			case TB_AICE_STATE_PENDING:
				resp->state = TB_AICE_STATE_OK;
				break;
			default:
				break;
			}
		}
		break;
		// closed?
	case ERROR_HANDLE_EOF:
	case ERROR_NETNAME_DELETED:
		{
			resp->state = TB_AICE_STATE_CLOSED;
		}
		break;
		// unknown error
	default:
		{
			resp->state = TB_AICE_STATE_FAILED;
			tb_trace_impl("fsync: unknown error: %u", error);
		}
		break;
	}

	// ok
	return 1;
}
static tb_long_t tb_iocp_spak_runtask(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_size_t real, tb_size_t error)
{
	// check?
	tb_assert_and_check_return_val(resp, -1);

	// done 
	switch (error)
	{	
		// ok or pending?
	case ERROR_SUCCESS:
	case WAIT_TIMEOUT:
	case ERROR_IO_PENDING:
		{
			// done state
			switch (resp->state)
			{
			case TB_AICE_STATE_OK:
			case TB_AICE_STATE_PENDING:
				resp->state = TB_AICE_STATE_OK;
				break;
			default:
				break;
			}
		}
		break;
		// unknown error
	default:
		{
			resp->state = TB_AICE_STATE_FAILED;
			tb_trace_impl("runtask: unknown error: %u", error);
		}
		break;
	}

	// ok
	return 1;
}
static tb_long_t tb_iocp_spak_done(tb_aicp_proactor_iocp_t* ptor, tb_aice_t* resp, tb_size_t real, tb_size_t error)
{
	// check?
	tb_assert_and_check_return_val(resp && resp->aico, -1);

	// no pending? spak it directly
	tb_check_return_val(resp->state == TB_AICE_STATE_PENDING, 1);

	// killed?
	tb_aico_t* aico = (tb_aico_t*)resp->aico;
	if (tb_atomic_get(&aico->killed))
	{
		// save state
		resp->state = TB_AICE_STATE_KILLED;

		// trace
		tb_trace_impl("spak: code: %u: killed", resp->code);

		// ok
		return 1;
	}

	// init spak
	static tb_bool_t (*s_spak[])(tb_aicp_proactor_iocp_t* , tb_aice_t* , tb_size_t , tb_size_t ) = 
	{
		tb_null
	,	tb_iocp_spak_acpt
	,	tb_iocp_spak_conn
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_iorw
	,	tb_iocp_spak_fsync
	,	tb_iocp_spak_runtask
	};
	tb_assert_and_check_return_val(resp->code < tb_arrayn(s_spak), -1);

	// done spak
	return (s_spak[resp->code])? s_spak[resp->code](ptor, resp, real, error) : -1;
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_proactor_iocp_addo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && aico, tb_false);

	// done
	switch (aico->type)
	{
	case TB_AICO_TYPE_SOCK:
		{
			// check
			tb_assert_and_check_return_val(ptor->port && aico->handle, tb_false);

			// add aico to port
			HANDLE port = CreateIoCompletionPort((HANDLE)((SOCKET)aico->handle - 1), ptor->port, (ULONG*)aico, 0);
			tb_assert_and_check_return_val(port == ptor->port, tb_false);
		}
		break;
	case TB_AICO_TYPE_FILE:
		{
			// check
			tb_assert_and_check_return_val(ptor->port && aico->handle, tb_false);

			// add aico to port
			HANDLE port = CreateIoCompletionPort((HANDLE)aico->handle, ptor->port, (ULONG*)aico, 0);
			tb_assert_and_check_return_val(port == ptor->port, tb_false);
		}
		break;
	case TB_AICO_TYPE_TASK:
		{
		}
		break;
	default:
		tb_assert_and_check_return_val(0, tb_false);
		break;
	}
	
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
	tb_assert_and_check_return_val(ptor && aico, tb_false);
		
	// the iocp aico
	tb_iocp_aico_t* iocp_aico = (tb_iocp_aico_t*)aico;
	
	// exit the timeout task
	if (iocp_aico->task) 
	{
		if (iocp_aico->bltimer) tb_ltimer_task_del(ptor->ltimer, iocp_aico->task);
		else tb_timer_task_del(ptor->timer, iocp_aico->task);
		iocp_aico->bltimer = 0;
	}
	iocp_aico->task = tb_null;

	// exit olap
	tb_memset(&iocp_aico->olap, 0, sizeof(tb_iocp_olap_t));
	
	// exit ptor
	iocp_aico->ptor = tb_null;

	// ok
	return tb_true;
}
static tb_void_t tb_aicp_proactor_iocp_kilo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return(ptor && ptor->event && aico);
		
	// trace
	tb_trace_impl("kilo: type: %u", aico->type);

	// kill the task
	if (((tb_iocp_aico_t*)aico)->task) 
	{
		if (((tb_iocp_aico_t*)aico)->bltimer) tb_ltimer_task_kil(ptor->ltimer, ((tb_iocp_aico_t*)aico)->task);
		else tb_timer_task_kil(ptor->timer, ((tb_iocp_aico_t*)aico)->task);
	}

	// sock: kill
	if (aico->type == TB_AICO_TYPE_SOCK && aico->handle) tb_socket_kill(aico->handle, TB_SOCKET_KILL_RW);
	// file: kill
	else if (aico->type == TB_AICO_TYPE_FILE && aico->handle) tb_file_exit(aico->handle);

	/* the iocp will wait long time if the lastest task wait period is too long
	 * so spak the iocp manually for spak the timer
	 */
	tb_event_post(ptor->event);
}
static tb_bool_t tb_aicp_proactor_iocp_post(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && proactor->aicp && aice, tb_false);

	// the aico
	tb_iocp_aico_t* aico = (tb_iocp_aico_t*)aice->aico;
	tb_assert_and_check_return_val(aico, tb_false);

	// no pending? post it directly
	if (aice->state != TB_AICE_STATE_PENDING)
	{
		aico->olap.aice = *aice;
		return PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)? tb_true : tb_false;  
	}
	
	// killed?
	if (tb_atomic_get(&aico->base.killed) || tb_atomic_get(&proactor->aicp->kill))
	{
		// post the killed state
		aico->olap.aice = *aice;
		aico->olap.aice.state = TB_AICE_STATE_KILLED;
		return PostQueuedCompletionStatus(ptor->port, 0, (ULONG*)aico, &aico->olap)? tb_true : tb_false;  
	}

	// init post
	static tb_bool_t (*s_post[])(tb_aicp_proactor_t* , tb_aice_t const*) = 
	{
		tb_null
	,	tb_iocp_post_acpt
	,	tb_iocp_post_conn
	,	tb_iocp_post_recv
	,	tb_iocp_post_send
	,	tb_iocp_post_urecv
	,	tb_iocp_post_usend
	,	tb_iocp_post_recvv
	,	tb_iocp_post_sendv
	,	tb_iocp_post_urecvv
	,	tb_iocp_post_usendv
	,	tb_iocp_post_sendfile
	,	tb_iocp_post_read
	,	tb_iocp_post_writ
	,	tb_iocp_post_readv
	,	tb_iocp_post_writv
	,	tb_iocp_post_fsync
	,	tb_iocp_post_runtask
	};
	tb_assert_and_check_return_val(aice->code < tb_arrayn(s_post) && s_post[aice->code], tb_false);

	// post aice
	return s_post[aice->code](proactor, aice);
}
static tb_void_t tb_aicp_proactor_iocp_kill(tb_aicp_proactor_t* proactor)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return(ptor && ptor->port && ptor->event && proactor->aicp);

	// the workers
	tb_size_t work = tb_atomic_get(&proactor->aicp->work);
	
	// trace
	tb_trace_impl("kill: %lu", work);

	// post the timer event
	tb_event_post(ptor->event);

	// using GetQueuedCompletionStatusEx?
	if (ptor->GetQueuedCompletionStatusEx)
	{
		// kill workers
		while (work--) 
		{
			// post kill
			PostQueuedCompletionStatus(ptor->port, 0, 0, tb_null);
			
			// wait some time
			tb_msleep(200);
		}
	}
	else
	{
		// kill workers
		while (work--) PostQueuedCompletionStatus(ptor->port, 0, 0, tb_null);
	}
}
static tb_void_t tb_aicp_proactor_iocp_exit(tb_aicp_proactor_t* proactor)
{
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	if (ptor)
	{
		// trace
		tb_trace_impl("exit");

		// post the timer event
		if (ptor->event) tb_event_post(ptor->event);

		// exit loop
		if (ptor->loop)
		{
			if (!tb_thread_wait(ptor->loop, 5000))
				tb_thread_kill(ptor->loop);
			tb_thread_exit(ptor->loop);
			ptor->loop = tb_null;
		}

		// exit port
		if (ptor->port) CloseHandle(ptor->port);
		ptor->port = tb_null;

		// exit timer
		if (ptor->timer) tb_timer_exit(ptor->timer);
		ptor->timer = tb_null;

		// exit ltimer
		if (ptor->ltimer) tb_ltimer_exit(ptor->ltimer);
		ptor->ltimer = tb_null;

		// exit event
		if (ptor->event) tb_event_exit(ptor->event);
		ptor->event = tb_null;

		// free it
		tb_free(ptor);
	}
}
static tb_void_t tb_aicp_proactor_iocp_loop_exit(tb_aicp_proactor_t* proactor, tb_handle_t hloop)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return(ptor);

	// the loop
	tb_iocp_loop_t* loop = (tb_iocp_loop_t*)hloop;
	tb_assert_and_check_return(loop);

	// exit spak
	if (loop->spak) tb_queue_exit(loop->spak);
	loop->spak = tb_null;

	// exit self
	loop->self = tb_null;

	// exit loop
	tb_free(loop);
}
static tb_handle_t tb_aicp_proactor_iocp_loop_init(tb_aicp_proactor_t* proactor)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor, tb_null);

	// make loop
	tb_iocp_loop_t* loop = tb_malloc0(sizeof(tb_iocp_loop_t));
	tb_assert_and_check_return_val(loop, tb_null);

	// init self
	loop->self = tb_thread_self();
	tb_assert_and_check_goto(loop->self, fail);

	// init spak
	if (ptor->GetQueuedCompletionStatusEx)
	{
		loop->spak = tb_queue_init(TB_IOCP_OLAP_LIST_MAXN, tb_item_func_ifm(sizeof(OVERLAPPED_ENTRY), tb_null, tb_null));
		tb_assert_and_check_goto(loop->spak, fail);
	}

	// ok
	return (tb_handle_t)loop;
fail:
	tb_aicp_proactor_iocp_loop_exit(proactor, (tb_handle_t)loop);
	return tb_null;
}
static tb_long_t tb_aicp_proactor_iocp_loop_spak(tb_aicp_proactor_t* proactor, tb_handle_t hloop, tb_aice_t* resp, tb_long_t timeout)
{
	// check
	tb_aicp_proactor_iocp_t* ptor = (tb_aicp_proactor_iocp_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->port && ptor->timer && resp, -1);

	// the loop
	tb_iocp_loop_t* loop = (tb_iocp_loop_t*)hloop;
	tb_assert_and_check_return_val(loop, -1);

	// trace
	tb_trace_impl("spak[%lu]: ..", loop->self);

	// spak ctime
	tb_ctime_spak();

	// exists GetQueuedCompletionStatusEx? using it
	if (ptor->GetQueuedCompletionStatusEx)
	{
		// check
		tb_assert_and_check_return_val(loop->spak, -1);

		// exists olap? spak it first
		if (!tb_queue_null(loop->spak))
		{
			// the top entry
			LPOVERLAPPED_ENTRY entry = (LPOVERLAPPED_ENTRY)tb_queue_get(loop->spak);
			tb_assert_and_check_return_val(entry, -1);

			// pop the entry
			tb_queue_pop(loop->spak);
	
			// init 
			tb_size_t 			real = (tb_size_t)entry->dwNumberOfBytesTransferred;
			tb_iocp_aico_t* 	aico = (tb_iocp_aico_t* )entry->lpCompletionKey;
			tb_iocp_olap_t* 	olap = (tb_iocp_olap_t*)entry->lpOverlapped;
			tb_size_t 			error = tb_winerror_from_nsstatus((tb_size_t)entry->Internal);
			tb_trace("spak[%lu]: ntstatus: %lx, winerror: %lu", loop->self, (tb_size_t)entry->Internal, error);

			// check
			tb_assert_and_check_return_val(olap && aico, -1);

			// save resp
			*resp = olap->aice;

			// spak resp
			return tb_iocp_spak_done(ptor, resp, real, error);
		}
		else
		{
			// wait
			DWORD 		size = 0;
			BOOL 		wait = ptor->GetQueuedCompletionStatusEx(ptor->port, loop->list, tb_arrayn(loop->list), &size, timeout, FALSE);

			// the last error
			tb_size_t 	error = (tb_size_t)GetLastError();

			/* some error will be not clear immediately after success, .e.g ERROR_NETNAME_DELETED
			 * we need clear it manually before calling GetQueuedCompletionStatus next
			 */
			if (error != ERROR_SUCCESS) SetLastError(ERROR_SUCCESS);

			// trace
			tb_trace_impl("spak[%lu]: wait: %d, size: %u, error: %lu", loop->self, wait, size, error);

			// timeout?
			if (!wait && error == WAIT_TIMEOUT) return 0;

			// error?
			tb_assert_and_check_return_val(wait, -1);

			// put entries to the spak queue
			tb_size_t i = 0;
			for (i = 0; i < size; i++) 
			{
				// exit the aico task
				tb_iocp_aico_t* aico = (tb_iocp_aico_t* )loop->list[i].lpCompletionKey;
				if (aico)
				{
					// remove task
					if (aico->task) 
					{
						if (aico->bltimer) tb_ltimer_task_del(ptor->ltimer, aico->task);
						else tb_timer_task_del(ptor->timer, aico->task);
						aico->bltimer = 0;
					}
					aico->task = tb_null;
				}
				// killed?
				else return -1;

				// full?
				if (!tb_queue_full(loop->spak))
				{
					// put it
					tb_queue_put(loop->spak, &loop->list[i]);
				}
				else 
				{
					// full
					tb_assert_and_check_return_val(0, -1);
				}
			}

			// continue 
			return 0;
		}
	}
	else
	{
		// wait
		DWORD 				real = 0;
		tb_iocp_aico_t* 	aico = tb_null;
		tb_iocp_olap_t* 	olap = tb_null;
		BOOL 				wait = GetQueuedCompletionStatus(ptor->port, (LPDWORD)&real, (LPDWORD)&aico, &olap, timeout < 0? INFINITE : timeout);

		// the last error
		tb_size_t 			error = (tb_size_t)GetLastError();

		/* some error will be not clear immediately after success, .e.g ERROR_NETNAME_DELETED
		 * we need clear it manually before calling GetQueuedCompletionStatus next
		 */
		if (error != ERROR_SUCCESS) SetLastError(ERROR_SUCCESS);

		// trace
		tb_trace_impl("spak[%lu]: wait: %d, real: %u, error: %lu", loop->self, wait, real, error);

		// timeout?
		if (!wait && error == WAIT_TIMEOUT) return 0;

		// killed?
		if (wait && !aico) return -1;

		// exit the aico task
		if (aico)
		{
			// remove task
			if (aico->task) 
			{
				if (aico->bltimer) tb_ltimer_task_del(ptor->ltimer, aico->task);
				else tb_timer_task_del(ptor->timer, aico->task);
				aico->bltimer = 0;
			}
			aico->task = tb_null;
		}

		// check
		tb_assert_and_check_return_val(olap, -1);

		// save resp
		*resp = olap->aice;

		// spak resp
		return tb_iocp_spak_done(ptor, resp, (tb_size_t)real, error);
	}

	// failed
	return -1;
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
	ptor->base.aicp 		= aicp;
	ptor->base.step 		= sizeof(tb_iocp_aico_t);
	ptor->base.kill 		= tb_aicp_proactor_iocp_kill;
	ptor->base.exit 		= tb_aicp_proactor_iocp_exit;
	ptor->base.addo 		= tb_aicp_proactor_iocp_addo;
	ptor->base.delo 		= tb_aicp_proactor_iocp_delo;
	ptor->base.kilo 		= tb_aicp_proactor_iocp_kilo;
	ptor->base.post 		= tb_aicp_proactor_iocp_post;
	ptor->base.loop_init 	= tb_aicp_proactor_iocp_loop_init;
	ptor->base.loop_exit 	= tb_aicp_proactor_iocp_loop_exit;
	ptor->base.loop_spak 	= tb_aicp_proactor_iocp_loop_spak;

	// init func
	ptor->AcceptEx 						= tb_api_AcceptEx();
	ptor->ConnectEx 					= tb_api_ConnectEx();
	ptor->TransmitFile 					= tb_api_TransmitFile();
	ptor->CancelIoEx 					= tb_api_CancelIoEx();
	ptor->GetQueuedCompletionStatusEx 	= tb_api_GetQueuedCompletionStatusEx();
	tb_assert_and_check_goto(ptor->AcceptEx && ptor->ConnectEx, fail);

	// init port
	ptor->port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, tb_null, 0, 0);
	tb_assert_and_check_goto(ptor->port && ptor->port != INVALID_HANDLE_VALUE, fail);

	// init timer and using cache time
	ptor->timer = tb_timer_init(aicp->maxn >> 3, tb_true);
	tb_assert_and_check_goto(ptor->timer, fail);

	// init ltimer and using cache time
	ptor->ltimer = tb_ltimer_init(aicp->maxn, TB_LTIMER_TICK_S, tb_true);
	tb_assert_and_check_goto(ptor->ltimer, fail);

	// init the timer event
	ptor->event = tb_event_init();
	tb_assert_and_check_goto(ptor->event, fail);

	// init the timer loop
	ptor->loop = tb_thread_init(tb_null, tb_iocp_timer_loop, ptor, 0);
	tb_assert_and_check_goto(ptor->loop, fail);

	// ok
	return (tb_aicp_proactor_t*)ptor;

fail:
	if (ptor) tb_aicp_proactor_iocp_exit(ptor);
	return tb_null;
}

