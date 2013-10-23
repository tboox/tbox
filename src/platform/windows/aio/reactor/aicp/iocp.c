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

	// the olap pool
	tb_handle_t 			pool;

}tb_aicp_reactor_iocp_t;

/* ///////////////////////////////////////////////////////////////////////
 * post
 */
static tb_long_t tb_aicp_reactor_iocp_post_sync(tb_aicp_reactor_t* reactor, tb_aice_t* aice, tb_long_t timeout)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);
	tb_assert_and_check_return_val(aice && aice->aico && aice->aico->aioo.handle, -1);

	// ok
	aice->u.sync.ok = tb_true;
	return 1;
}
static tb_long_t tb_aicp_reactor_iocp_post_acpt(tb_aicp_reactor_t* reactor, tb_aice_t* aice, tb_long_t timeout)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);
	tb_assert_and_check_return_val(aice && aice->aico && aice->aico->aioo.handle, -1);

	// post acpt
	tb_aioo_t aioo = aice->aico->aioo; aioo.etype = TB_AIOO_ETYPE_ACPT;
	if (tb_aioo_wait(&aioo, timeout) > 0)
		aice->u.acpt.sock = tb_socket_accept(aice->aico->aioo.handle);
	else aice->u.acpt.sock = tb_null;

	// ok
	return 1;
}
static tb_long_t tb_aicp_reactor_iocp_post_clos(tb_aicp_reactor_t* reactor, tb_aice_t* aice, tb_long_t timeout)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);
	tb_assert_and_check_return_val(aice && aice->aico && aice->aico->aioo.handle, -1);

	// post clos
	aice->u.clos.ok = tb_socket_close(aice->aico->aioo.handle);

	// ok
	return 1;
}
static tb_long_t tb_aicp_reactor_iocp_post_conn(tb_aicp_reactor_t* reactor, tb_aice_t* aice, tb_long_t timeout)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);
	tb_assert_and_check_return_val(aice && aice->aico && aice->aico->aioo.handle, -1);
	
	tb_print("tb_aicp_reactor_iocp_post_conn b");
	// post conn
	tb_long_t ok = -1;
	tb_char_t host[16] = {0};
	if (tb_ipv4_get(&aice->u.conn.host, host, 16))
	{
		while (!(ok = tb_socket_connect(aice->aico->aioo.handle, host, aice->u.conn.port)) && !tb_atomic_get(&reactor->aicp->kill)) 
		{
			tb_aioo_t aioo = aice->aico->aioo; aioo.etype = TB_AIOO_ETYPE_CONN;
			if (tb_aioo_wait(&aioo, timeout) <= 0) break;
		}
	}
	tb_print("tb_aicp_reactor_iocp_post_conn e");

	// ok?
	aice->u.conn.ok = ok;
	return 1;
}
static tb_long_t tb_aicp_reactor_iocp_post_read(tb_aicp_reactor_t* reactor, tb_aice_t* aice, tb_long_t timeout)
{	
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);
	tb_assert_and_check_return_val(aice && aice->aico && aice->aico->aioo.handle, -1);

	tb_print("tb_aicp_reactor_iocp_post_read b: %p %lu", aice->u.read.data, aice->u.read.maxn);
	// init olap
	tb_iocp_olap_t* olap = tb_rpool_malloc0(rtor->pool);
	tb_assert_and_check_return_val(olap, -1);
	olap->data.buf 	= aice->u.read.data;
	olap->data.len 	= aice->u.read.maxn;
	olap->aice 		= *aice;

	// post recv
	DWORD 	flag = 0;
	LONG 	real = WSARecv((SOCKET)aice->aico->aioo.handle - 1, &olap->data, 1, &olap->size, &flag, olap, tb_null);
	tb_print("real: %ld %d", real, WSAGetLastError());

	// finished?
	if (real == aice->u.read.maxn)
	{
		// remove olap
		if (olap) tb_rpool_free(rtor->pool, olap);

		// the real size
		aice->u.read.size = real;
		return real;
	}

	// pending? continue it
	if (!real || WSA_IO_PENDING == WSAGetLastError()) return 0;
	tb_print("error: %ld", WSAGetLastError());

	// remove olap
	if (olap) tb_rpool_free(rtor->pool, olap);

	// error?
	if (real == SOCKET_ERROR)
	{
		// closed?
		if (WSAECONNABORTED == WSAGetLastError())
		{
			aice->code = TB_AICE_CODE_CLOS;
			aice->u.clos.ok = tb_socket_close(aice->aico->aioo.handle);
		}
		// unknown error
		else 
		{
			aice->code = TB_AICE_CODE_ERRO;
			aice->u.erro.code = TB_AICE_ERROR_UNKNOWN;
		}

		// ok
		return 1;
	}

	tb_print("tb_aicp_reactor_iocp_post_read e");
	// failed
	aice->u.read.size = -1;
	return -1;
}
static tb_long_t tb_aicp_reactor_iocp_post_writ(tb_aicp_reactor_t* reactor, tb_aice_t* aice, tb_long_t timeout)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);
	tb_assert_and_check_return_val(aice && aice->aico && aice->aico->aioo.handle, -1);

	tb_print("tb_aicp_reactor_iocp_post_writ b: %p %lu", aice->u.writ.data, aice->u.writ.maxn);
	// init olap
	tb_iocp_olap_t* olap = tb_rpool_malloc0(rtor->pool);
	tb_assert_and_check_return_val(olap, -1);
	olap->data.buf 	= aice->u.writ.data;
	olap->data.len 	= aice->u.writ.maxn;
	olap->aice 		= *aice;

	// post send
	LONG real = WSASend((SOCKET)aice->aico->aioo.handle - 1, &olap->data, 1, &olap->size, 0, olap, tb_null);
	tb_print("real: %ld %d", real, WSAGetLastError());

	// finished?
	if (real == aice->u.writ.maxn)
	{
		// remove olap
		if (olap) tb_rpool_free(rtor->pool, olap);

		// the real size
		aice->u.writ.size = real;
		return real;
	}

	// pending? continue it
	if (!real || WSA_IO_PENDING == WSAGetLastError()) return 0;
	tb_print("error: %ld", WSAGetLastError());

	// remove olap
	if (olap) tb_rpool_free(rtor->pool, olap);

	// error?
	if (real == SOCKET_ERROR)
	{
		// closed?
		if (WSAECONNABORTED == WSAGetLastError())
		{
			aice->code = TB_AICE_CODE_CLOS;
			aice->u.clos.ok = tb_socket_close(aice->aico->aioo.handle);
		}
		// unknown error
		else 
		{
			aice->code = TB_AICE_CODE_ERRO;
			aice->u.erro.code = TB_AICE_ERROR_UNKNOWN;
		}

		// ok
		return 1;
	}

	tb_print("tb_aicp_reactor_iocp_post_writ e");
	// failed
	aice->u.writ.size = -1;
	return -1;
}
static tb_long_t tb_aicp_reactor_iocp_post_aice(tb_aicp_reactor_t* reactor, tb_aice_t* aice, tb_long_t timeout)
{
	// post
	static tb_bool_t (*s_post[])(tb_aicp_reactor_t* , tb_aice_t const* , tb_long_t) = 
	{
		tb_null
	,	tb_aicp_reactor_iocp_post_sync
	,	tb_aicp_reactor_iocp_post_acpt
	,	tb_aicp_reactor_iocp_post_conn
	,	tb_aicp_reactor_iocp_post_clos
	,	tb_aicp_reactor_iocp_post_read
	,	tb_aicp_reactor_iocp_post_writ
	};
	tb_assert_and_check_return(aice->code < tb_arrayn(s_post));

	// post aice
	return (s_post[aice->code])? s_post[aice->code](reactor, aice, timeout) : -1;
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_reactor_iocp_addo(tb_aicp_reactor_t* reactor, tb_aico_t const* aico)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, tb_false);

	// attach aico to port
	HANDLE port = CreateIoCompletionPort((HANDLE)aico->aioo.handle, rtor->port, aico, 0);
	tb_assert_and_check_return_val(port == rtor->port, tb_false);

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_reactor_iocp_delo(tb_aicp_reactor_t* reactor, tb_aico_t const* aico)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, tb_false);

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
		ok = tb_aicp_reactor_iocp_post_aice(reactor, &reqt, timeout);
		tb_assert_and_check_return_val(ok >= 0, -1);

		// finished?
		if (ok > 0) *resp = reqt;
		
		// continue waiting?
		tb_check_return_val(!ok, ok);
	}

	// wait
	tb_iocp_olap_t* olap = tb_null;
	DWORD 			real = 0;
	tb_aico_t* 		aico = tb_null;
	BOOL 			wait = GetQueuedCompletionStatus(rtor->port, (LPDWORD)&real, (LPDWORD)&aico, &olap, timeout < 0? INFINITE : timeout);
	tb_print("GetQueuedCompletionStatus: %d %d", wait, GetLastError());

	// timeout?
	if (!wait && WAIT_TIMEOUT == GetLastError()) return 0;

	// closed?
	tb_check_return_val(wait, -1);

	// check?
	tb_assert_and_check_return_val(aico && olap && aico == olap->aice.aico, -1);

	// done aice
	switch (olap->aice.code)
	{
	case TB_AICE_CODE_READ:
		{
			*resp = olap->aice;
			resp->u.read.size = real;
			ok = 1;
		}
		break;
	case TB_AICE_CODE_WRIT:
		{
			*resp = olap->aice;
			resp->u.writ.size = real;
			ok = 1;
		}
		break;
	default:
		{
			ok = -1;
			tb_assert(0);
		}
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
	tb_assert_and_check_return_val(aicp->type == TB_AIOO_OTYPE_FILE || aicp->type == TB_AIOO_OTYPE_SOCK, tb_null);

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

