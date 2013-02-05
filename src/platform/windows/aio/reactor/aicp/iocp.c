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
	
	// the flag
	DWORD 					flag;

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
static tb_bool_t tb_aicp_reactor_iocp_post_conn(tb_aicp_reactor_t* reactor, tb_aice_t const* aice)
{

	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_reactor_iocp_post_read(tb_aicp_reactor_t* reactor, tb_aice_t const* aice)
{	
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, tb_false);

	// init olap
	tb_iocp_olap_t* olap = tb_rpool_malloc0(rtor->pool);
	tb_assert_and_check_return_val(olap, tb_false);
	olap->data.buf 	= aice->u.read.data;
	olap->data.len 	= aice->u.read.size;
	olap->aice 		= *aice;

#if 0
	// post recv
	LONG n = WSARecv((tb_long_t)aice->aico->aioo.handle - 1, &olap->data, 1, &olap->size, &olap->flag, olap, tb_null);
	tb_check_goto((SOCKET_ERROR != n) || (WSA_IO_PENDING == WSAGetLastError()), fail);
#endif

	// ok
	return tb_true;

fail:
	if (olap) tb_rpool_free(rtor->pool, olap);
	return tb_false;
}
static tb_bool_t tb_aicp_reactor_iocp_post_writ(tb_aicp_reactor_t* reactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, tb_false);

	// init olap
	tb_iocp_olap_t* olap = tb_rpool_malloc0(rtor->pool);
	tb_assert_and_check_return_val(olap, tb_false);
	olap->data.buf 	= aice->u.writ.data;
	olap->data.len 	= aice->u.writ.size;
	olap->aice 		= *aice;

#if 0
	// post send
	LONG n = WSASend((tb_long_t)aice->aico->aioo.handle - 1, &olap->data, 1, &olap->size, &olap->flag, olap, tb_null);
	tb_check_goto((SOCKET_ERROR != n) || (WSA_IO_PENDING == WSAGetLastError()), fail);
#endif

	// ok
	return tb_true;

fail:
	if (olap) tb_rpool_free(rtor->pool, olap);
	return tb_false;
}
static tb_bool_t tb_aicp_reactor_iocp_post_sync(tb_aicp_reactor_t* reactor, tb_aice_t const* aice)
{

	// ok
	return tb_true;
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
static tb_bool_t tb_aicp_reactor_iocp_post(tb_aicp_reactor_t* reactor, tb_aice_t const* aice)
{
	// post
	static tb_bool_t (*s_post[])(tb_aicp_reactor_t* , tb_aice_t const* ) = 
	{
		tb_null
	,	tb_aicp_reactor_iocp_post_sync
	,	tb_aicp_reactor_iocp_post_conn
	,	tb_aicp_reactor_iocp_post_read
	,	tb_aicp_reactor_iocp_post_writ
	};
	tb_assert_and_check_return(aice->code < tb_arrayn(s_post));

	// post aice
	return (s_post[aice->code])? s_post[aice->code](reactor, aice) : tb_false;
}

static tb_long_t tb_aicp_reactor_iocp_wait(tb_aicp_reactor_t* reactor, tb_long_t timeout)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);
#if 0
	// wait
	tb_iocp_olap_t* olap = tb_null;
	DWORD 			size = 0;
	tb_aico_t* 		aico = tb_null;
	BOOL 			r = GetQueuedCompletionStatus(rtor->port, (LPDWORD)&size, (LPDWORD)&aico, &olap, timeout < 0? INFINITY : timeout);

	// error?
	tb_check_return_val(r && aico && olap && aico == olap->aice.aico, -1);

	tb_long_t ok = 1;

	// done
	olap->aice.ok = size;

	// do aicb
	if (aico->aicb) ok = aico->aicb(reactor->aicp, aico, &olap->aice)? 1 : -1;

	// free olap
	tb_rpool_free(rtor->pool, olap);

	// ok
	return ok;
#else
	return -1;
#endif
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
//	rtor->base.post = tb_aicp_reactor_iocp_post;
//	rtor->base.wait = tb_aicp_reactor_iocp_wait;

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

