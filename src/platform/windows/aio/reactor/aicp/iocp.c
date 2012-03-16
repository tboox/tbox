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
 * \author		ruki
 * \file		iocp.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the iocp reactor type
typedef struct __tb_aicp_reactor_iocp_t
{
	// the reactor base
	tb_aicp_reactor_t 		base;

	// the i/o completion port
	HANDLE 					port;

}tb_aicp_reactor_iocp_t;

/* ///////////////////////////////////////////////////////////////////////
 * post
 */
tb_void_t tb_aicp_reactor_iocp_post_resv(tb_aicp_reactor_t* reactor, tb_aico_t const* aico, tb_aice_t const* aice)
{
}
tb_void_t tb_aicp_reactor_iocp_post_conn(tb_aicp_reactor_t* reactor, tb_aico_t const* aico, tb_aice_t const* aice)
{
}
tb_void_t tb_aicp_reactor_iocp_post_read(tb_aicp_reactor_t* reactor, tb_aico_t const* aico, tb_aice_t const* aice)
{
	OVERLAPPED olap;
	DWORD 	size = 0;
	DWORD 	flag = 0;
	WSABUF 	b;
	b.buf = aice->u.read.data;
	b.len = aice->u.read.size;
	WSARecv((tb_long_t)aico->aioo.handle - 1, &b, 1, &size, &flag, &olap, TB_NULL);
}
tb_void_t tb_aicp_reactor_iocp_post_writ(tb_aicp_reactor_t* reactor, tb_aico_t const* aico, tb_aice_t const* aice)
{
}
tb_void_t tb_aicp_reactor_iocp_post_sync(tb_aicp_reactor_t* reactor, tb_aico_t const* aico, tb_aice_t const* aice)
{
}
tb_void_t tb_aicp_reactor_iocp_post_seek(tb_aicp_reactor_t* reactor, tb_aico_t const* aico, tb_aice_t const* aice)
{
}
tb_void_t tb_aicp_reactor_iocp_post_skip(tb_aicp_reactor_t* reactor, tb_aico_t const* aico, tb_aice_t const* aice)
{
}
/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
static tb_bool_t tb_aicp_reactor_iocp_addo(tb_aicp_reactor_t* reactor, tb_aico_t const* aico)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, TB_FALSE);

	// attach aico to port
	HANDLE port = CreateIoCompletionPort((HANDLE)aico->aioo.handle, rtor->port, aico->self, 0);
	tb_assert_and_check_return_val(port == rtor->port, TB_FALSE);

	// ok
	return TB_TRUE;
}
static tb_bool_t tb_aicp_reactor_iocp_delo(tb_aicp_reactor_t* reactor, tb_aico_t const* aico)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, TB_FALSE);

	// ok
	return TB_TRUE;
}
static tb_void_t tb_aicp_reactor_iocp_post(tb_aicp_reactor_t* reactor, tb_aico_t const* aico, tb_aice_t const* aice)
{
	// post
	static tb_void_t (*s_post[])(tb_aicp_reactor_t* , tb_aico_t const* , tb_aice_t const* ) = 
	{
		TB_NULL
	, 	tb_aicp_reactor_iocp_post_resv
	,	tb_aicp_reactor_iocp_post_conn
	,	tb_aicp_reactor_iocp_post_read
	,	tb_aicp_reactor_iocp_post_writ
	,	tb_aicp_reactor_iocp_post_sync
	,	tb_aicp_reactor_iocp_post_seek
	,	tb_aicp_reactor_iocp_post_skip
	};
	tb_assert_and_check_return(aice->code < tb_arrayn(s_post));

	// post aice
	if (s_post[aice->code]) s_post[aice->code](reactor, aico, aice);
}

static tb_long_t tb_aicp_reactor_iocp_wait(tb_aicp_reactor_t* reactor, tb_long_t timeout)
{
	// check
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->port && reactor->aicp, -1);

	tb_print("waitb:");

	// wait
	LPOVERLAPPED 	olap = TB_NULL;
	DWORD 			size = 0;
	tb_handle_t 	aioo = TB_NULL;
	BOOL 			r = GetQueuedCompletionStatus(rtor->port, (LPDWORD)&size, (LPDWORD)&aioo, &olap, timeout < 0? INFINITY : timeout);
	tb_print("waite: %d %d", r, size);

	// error?
	tb_check_return_val(r && aioo, -1);


	// ok
	return 1;
}

static tb_void_t tb_aicp_reactor_iocp_exit(tb_aicp_reactor_t* reactor)
{
	tb_aicp_reactor_iocp_t* rtor = (tb_aicp_reactor_iocp_t*)reactor;
	if (rtor)
	{
		// exit port
		if (rtor->port) CloseHandle(rtor->port);

		// free it
		tb_free(rtor);
	}
}
static tb_aicp_reactor_t* tb_aicp_reactor_iocp_init(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->maxn, TB_NULL);
	tb_assert_and_check_return_val(aicp->type == TB_AIOO_OTYPE_FILE || aicp->type == TB_AIOO_OTYPE_SOCK, TB_NULL);

	// alloc reactor
	tb_aicp_reactor_iocp_t* rtor = tb_calloc(1, sizeof(tb_aicp_reactor_iocp_t));
	tb_assert_and_check_return_val(rtor, TB_NULL);

	// init base
	rtor->base.aicp = aicp;
	rtor->base.exit = tb_aicp_reactor_iocp_exit;
	rtor->base.addo = tb_aicp_reactor_iocp_addo;
	rtor->base.delo = tb_aicp_reactor_iocp_delo;
	rtor->base.post = tb_aicp_reactor_iocp_post;
	rtor->base.wait = tb_aicp_reactor_iocp_wait;

	// init port
	rtor->port = CreateIoCompletionPort(INVALID_HANDLE_VALUE, TB_NULL, 0, 0);
	tb_assert_and_check_goto(rtor->port && rtor->port != INVALID_HANDLE_VALUE, fail);

	// ok
	return (tb_aicp_reactor_t*)rtor;

fail:
	if (rtor) tb_aicp_reactor_iocp_exit(rtor);
	return TB_NULL;
}

