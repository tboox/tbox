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
 * @file		iocp.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_IOCP_WORK_MAXN 			(64)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the overlap type
typedef struct __tb_iocp_olap_t
{
	// the base
	OVERLAPPED 				base;
	
	// the data
	WSABUF 					data;

	//the size
	DWORD 					size;

	// the aice
	tb_aice_t 				aice;

}tb_iocp_olap_t;

// the iocp reactor type
typedef struct __tb_aicp_reactor_iocp_t
{
	// the reactor base
	tb_aicp_reactor_t 		base;

	// the i/o completion port
	HANDLE 					port;

	// the olap pool, FIXME: lock
	tb_handle_t 			pool;

}tb_aicp_reactor_iocp_t;

/* ///////////////////////////////////////////////////////////////////////
 * post
 */
static tb_long_t tb_aicp_reactor_iocp_post_acpt(tb_aicp_reactor_t* reactor, tb_aice_t* aice)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_ACPT, -1);
	
	// trace
	tb_trace_impl("accept: ..");

	// post acpt
	tb_long_t ok = -1;
	tb_aioo_t aioo = {0};
	tb_aioo_seto(&aioo, aice->handle, aice->otype, TB_AIOO_ETYPE_ACPT, tb_null);
	if ((ok = tb_aioo_wait(&aioo, aice->u.acpt.timeout)) > 0)
	{
		aice->u.acpt.sock = tb_socket_accept(aice->handle);
		if (!aice->u.acpt.sock) ok = -1;
	}

	// ok?
	aice->state = (ok > 0? TB_AICE_STATE_OK : (!ok? TB_AICE_STATE_TIMEOUT : TB_AICE_STATE_FAILED));

	// done resp
	return 1;
}
static tb_long_t tb_aicp_reactor_iocp_post_conn(tb_aicp_reactor_t* reactor, tb_aice_t* aice)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_CONN, -1);
	tb_assert_and_check_return_val(aice->u.conn.host && aice->u.conn.port , -1);
	
	// trace
	tb_trace_impl("connect: %s:%lu", aice->u.conn.host, aice->u.conn.port);

	// post conn
	tb_long_t ok = -1;
	while (!(ok = tb_socket_connect(aice->handle, aice->u.conn.host, aice->u.conn.port)))
	{
		// wait
		tb_aioo_t aioo = {0};
		tb_aioo_seto(&aioo, aice->handle, aice->otype, TB_AIOO_ETYPE_CONN, tb_null);
		if (tb_aioo_wait(&aioo, aice->u.conn.timeout) <= 0) break;

		// killed?
		tb_check_break(!tb_atomic_get(&reactor->aicp->kill));
	}

	// ok?
	aice->state = (ok > 0? TB_AICE_STATE_OK : (!ok? TB_AICE_STATE_TIMEOUT : TB_AICE_STATE_FAILED));

	// done resp
	return 1;
}
static tb_long_t tb_aicp_reactor_iocp_post_recv(tb_aicp_reactor_t* reactor, tb_aice_t* aice)
{	
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_RECV, -1);
	tb_assert_and_check_return_val(aice->u.recv.data && aice->u.recv.size , -1);

	// init olap
	tb_iocp_olap_t* olap = tb_rpool_malloc0(rtor->pool);
	tb_assert_and_check_return_val(olap, -1);
	olap->data.buf 	= aice->u.recv.data;
	olap->data.len 	= aice->u.recv.size;
	olap->aice 		= *aice;

	// post recv
	DWORD 		flag = 0;
	tb_long_t 	real = WSARecv((SOCKET)aice->handle - 1, &olap->data, 1, &olap->size, &flag, olap, tb_null);
	tb_trace_impl("WSARecv: %ld, error: %d", real, WSAGetLastError());

	// ok?
	if (real > 0)
	{
		// remove olap
		if (olap) tb_rpool_free(rtor->pool, olap);

		// the real size
		aice->u.recv.real = real;

		// done resp
		return real;
	}

	// pending? continue it
	if (!real || WSA_IO_PENDING == WSAGetLastError()) return 0;

	// remove olap
	if (olap) tb_rpool_free(rtor->pool, olap);

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
static tb_long_t tb_aicp_reactor_iocp_post_read(tb_aicp_reactor_t* reactor, tb_aice_t* aice)
{	
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_READ, -1);
	tb_assert_and_check_return_val(aice->u.read.data && aice->u.read.size , -1);

	// init olap
	tb_iocp_olap_t* olap = tb_rpool_malloc0(rtor->pool);
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
		if (olap) tb_rpool_free(rtor->pool, olap);

		// the real size
		aice->u.read.real = real;

		// done resp
		return real;
	}

	// pending? continue it
	if (!real || ERROR_IO_PENDING == GetLastError()) return 0;

	// remove olap
	if (olap) tb_rpool_free(rtor->pool, olap);

	// failed
	aice->state = TB_AICE_STATE_FAILED;

	// done resp
	return 1;
}
static tb_long_t tb_aicp_reactor_iocp_post_send(tb_aicp_reactor_t* reactor, tb_aice_t* aice)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_SEND, -1);
	tb_assert_and_check_return_val(aice->u.send.data && aice->u.send.size , -1);

	// init olap
	tb_iocp_olap_t* olap = tb_rpool_malloc0(rtor->pool);
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
		if (olap) tb_rpool_free(rtor->pool, olap);

		// the real size
		aice->u.send.real = real;

		// done resp
		return real;
	}

	// pending? continue it
	if (!real || WSA_IO_PENDING == WSAGetLastError()) return 0;

	// remove olap
	if (olap) tb_rpool_free(rtor->pool, olap);

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
static tb_long_t tb_aicp_reactor_iocp_post_writ(tb_aicp_reactor_t* reactor, tb_aice_t* aice)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);

	// check aice
	tb_assert_and_check_return_val(aice && aice->handle && aice->code == TB_AICE_CODE_WRIT, -1);
	tb_assert_and_check_return_val(aice->u.writ.data && aice->u.writ.size , -1);

	// init olap
	tb_iocp_olap_t* olap = tb_rpool_malloc0(rtor->pool);
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
		if (olap) tb_rpool_free(rtor->pool, olap);

		// the real size
		aice->u.writ.real = real;

		// done resp
		return real;
	}

	// pending? continue it
	if (!real || ERROR_IO_PENDING == GetLastError()) return 0;

	// remove olap
	if (olap) tb_rpool_free(rtor->pool, olap);

	// failed
	aice->state = TB_AICE_STATE_FAILED;

	// done resp
	return 1;
}
static tb_long_t tb_aicp_reactor_iocp_post_aice(tb_aicp_reactor_t* reactor, tb_aice_t* aice)
{
	// post
	static tb_bool_t (*s_post[])(tb_aicp_reactor_t* , tb_aice_t const*) = 
	{
		tb_null
	,	tb_aicp_reactor_iocp_post_acpt
	,	tb_aicp_reactor_iocp_post_conn
	,	tb_aicp_reactor_iocp_post_recv
	,	tb_aicp_reactor_iocp_post_send
	,	tb_aicp_reactor_iocp_post_read
	,	tb_aicp_reactor_iocp_post_writ
	};
	tb_assert_and_check_return(aice->code < tb_arrayn(s_post));

	// post aice
	return (s_post[aice->code])? s_post[aice->code](reactor, aice) : -1;
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_reactor_iocp_addo(tb_aicp_reactor_t* reactor, tb_handle_t handle, tb_size_t otype)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp && handle && otype, tb_false);

	// attach aico to port
	HANDLE port = CreateIoCompletionPort((HANDLE)handle, rtor->port, (ULONG*)otype, 0);
	tb_assert_and_check_return_val(port == rtor->port, tb_false);

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_reactor_iocp_delo(tb_aicp_reactor_t* reactor, tb_handle_t handle)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp && handle, tb_false);

	// ok
	return tb_true;
}
static tb_long_t tb_aicp_reactor_iocp_spak(tb_aicp_reactor_t* reactor, tb_aice_t* resp, tb_long_t timeout)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && resp, -1);

	// the aicp
	tb_aicp_t* 	aicp = reactor->aicp;
	tb_assert_and_check_return_val(aicp && aicp->post, -1);

	// enter 
	if (aicp->mutx.post) tb_mutex_enter(aicp->mutx.post);

	// init reqt
	tb_aice_t reqt = {0};
	if (!tb_queue_null(aicp->post)) 
	{
		// get aice
		tb_aice_t* aice = tb_queue_get(aicp->post);
		if (aice) reqt = *aice;
	
		// pop aice
		tb_queue_pop(aicp->post);
	}

	// leave 
	if (aicp->mutx.post) tb_mutex_leave(aicp->mutx.post);

	// post reqt
	tb_long_t ok = 0;
	if (reqt.code)
	{
		// post reqt
		ok = tb_aicp_reactor_iocp_post_aice(reactor, &reqt);

		// save it for freeing
		*resp = reqt;

		// failed? exit loop
		tb_assert_and_check_return_val(ok >= 0, -1);
		
		// continue waiting?
		tb_check_return_val(!ok, ok);
	}

	// wait
	DWORD 			real = 0;
	tb_size_t 		otype = 0;
	tb_iocp_olap_t* olap = tb_null;
	BOOL 			wait = GetQueuedCompletionStatus(rtor->port, (LPDWORD)&real, (LPDWORD)&otype, &olap, timeout < 0? INFINITE : timeout);
	tb_trace_impl("GetQueuedCompletionStatus: %d error: %u", wait, GetLastError());

	// timeout?
	if (!wait && WAIT_TIMEOUT == GetLastError()) return 0;

	// check?
	tb_assert_and_check_return_val(olap && otype == olap->aice.otype, -1);

	// save resp
	*resp = olap->aice;

	// ok? save the size
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
			tb_trace_impl("GetQueuedCompletionStatus: unknown error: %u", GetLastError());
			ok = -1;
			break;
		}
	}

	// done aice
	switch (olap->aice.code)
	{
	case TB_AICE_CODE_RECV:
	case TB_AICE_CODE_SEND:
	case TB_AICE_CODE_READ:
	case TB_AICE_CODE_WRIT:
		resp->u.real.real = real;
		break;
	default:
		ok = -1;
		tb_assert(0);
		break;
	}

	// free olap
	tb_rpool_free(rtor->pool, olap);

	// ok
	return ok;
}
static tb_void_t tb_aicp_reactor_iocp_exit(tb_aicp_reactor_t* reactor)
{
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	if (rtor)
	{
		// exit port
		if (rtor->port) 
		{
			// post it
			PostQueuedCompletionStatus(rtor->port, 0xffffffff, 0, tb_null);
			
			// close it
			CloseHandle(rtor->port);
			rtor->port = tb_null;
		}

		// exit pool
		if (rtor->pool) tb_rpool_exit(rtor->pool);
		rtor->pool = tb_null;

		// free it
		tb_free(rtor);
	}
}
static tb_aicp_reactor_t* tb_aicp_reactor_iocp_init(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->maxn, tb_null);

	// alloc reactor
	tb_aicp_reactor_iocp_t* rtor = tb_malloc0(sizeof(tb_aicp_reactor_iocp_t));
	tb_assert_and_check_return_val(rtor, tb_null);

	// init base
	rtor->base.aicp = aicp;
	rtor->base.exit = tb_aicp_reactor_iocp_exit;
	rtor->base.addo = tb_aicp_reactor_iocp_addo;
	rtor->base.delo = tb_aicp_reactor_iocp_delo;
	rtor->base.spak = tb_aicp_reactor_iocp_spak;

	// init port
	rtor->port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, tb_null, 0, 0);
	tb_assert_and_check_goto(rtor->port && rtor->port != INVALID_HANDLE_VALUE, fail);

	// init pool
	rtor->pool = tb_rpool_init(TB_IOCP_WORK_MAXN << 2, sizeof(tb_iocp_olap_t), 0);
	tb_assert_and_check_goto(rtor->pool, fail);

	// ok
	return (tb_aicp_reactor_t*)rtor;

fail:
	if (rtor) tb_aicp_reactor_iocp_exit(rtor);
	return tb_null;
}

