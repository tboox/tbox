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
 * @ingroup 	aio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aicp.h"
#include "aioo.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */

tb_aicp_reactor_t* tb_aicp_reactor_file_init(tb_aicp_t* aicp);
tb_aicp_reactor_t* tb_aicp_reactor_sock_init(tb_aicp_t* aicp);

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_aicp_t* tb_aicp_init(tb_size_t type, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, TB_NULL);

	// alloc aicp
	tb_aicp_t* aicp = tb_malloc0(sizeof(tb_aicp_t));
	tb_assert_and_check_return_val(aicp, TB_NULL);

	// init aicp
	aicp->type = type;
	aicp->maxn = maxn;

	// reactors
	static tb_aicp_reactor_t* (*s_init[])(tb_aicp_t*) = 
	{
		TB_NULL
	, 	tb_aicp_reactor_file_init
	,	tb_aicp_reactor_sock_init
	
	};
	tb_assert_and_check_goto(type < tb_arrayn(s_init), fail);

	// init mutex
	aicp->mutx = tb_mutex_init(TB_NULL);
	tb_assert_and_check_goto(aicp->mutx, fail);

	// init pool
	aicp->pool = tb_rpool_init(tb_align_pow2((maxn >> 3) + 1), sizeof(tb_aico_t), 0);
	tb_assert_and_check_goto(aicp->pool, fail);

	// init reactor
	if (s_init[type]) aicp->rtor = s_init[type](aicp);
	tb_assert_and_check_goto(aicp->rtor, fail);

	// ok
	return aicp;

fail:
	if (aicp) tb_aicp_exit(aicp);
	return TB_NULL;
}
tb_void_t tb_aicp_exit(tb_aicp_t* aicp)
{
	if (aicp)
	{
		// enter 
		if (aicp->mutx) tb_mutex_enter(aicp->mutx);

		// exit reactor
		if (aicp->rtor)
		{
			tb_assert(aicp->rtor && aicp->rtor->exit);
			aicp->rtor->exit(aicp->rtor);
			aicp->rtor = TB_NULL;
		}

		// exit pool
		if (aicp->pool) tb_rpool_exit(aicp->pool);
		aicp->pool = TB_NULL;

		// leave 
		if (aicp->mutx) tb_mutex_leave(aicp->mutx);

		// exit mutex
		if (aicp->mutx) tb_mutex_exit(aicp->mutx);
		aicp->mutx = TB_NULL;

		// free aicp
		tb_free(aicp);
	}
}
tb_size_t tb_aicp_maxn(tb_aicp_t* aicp)
{
	tb_assert_and_check_return_val(aicp, 0);
	
	// enter 
	if (aicp->mutx) tb_mutex_enter(aicp->mutx);

	// maxn
	tb_size_t maxn = aicp->maxn;

	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);

	// ok
	return maxn;
}
tb_size_t tb_aicp_size(tb_aicp_t* aicp)
{
	tb_assert_and_check_return_val(aicp, 0);

	// enter 
	if (aicp->mutx) tb_mutex_enter(aicp->mutx);

	// size
	tb_size_t size = aicp->pool? tb_rpool_size(aicp->pool) : 0;

	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);

	// ok
	return size;
}
tb_aico_t const* tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb, tb_pointer_t odata)
{
	tb_assert_and_check_return_val(aicp && handle && aicb, TB_NULL);

	// init aico
	tb_aico_t* aico = TB_NULL;

	// enter 
	if (aicp->mutx) tb_mutex_enter(aicp->mutx);

	// check
	tb_assert_and_check_goto(aicp->rtor && aicp->rtor->addo, end);
	tb_assert_and_check_goto(aicp->pool && tb_rpool_size(aicp->pool) < aicp->maxn, end);

	// add aico to pool
	aico = (tb_aico_t*)tb_rpool_malloc0(aicp->pool);
	tb_assert_and_check_goto(aico, end);

	// init aico
	aico->aioo.handle = handle;
	aico->aioo.otype = aicp->type;
	aico->aioo.etype = TB_AIOO_ETYPE_NULL;
	aico->aioo.odata = odata;
	aico->aicb = aicb;

	// add aico to native
	if (aicp->rtor->addo(aicp->rtor, aico)) goto end;
	
	// del aico from pool
	tb_rpool_free(aicp->pool, aico);
	aico = TB_NULL;

end:

	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);

	// ok?
	return aico;
}
tb_void_t tb_aicp_delo(tb_aicp_t* aicp, tb_aico_t const* aico)
{
	tb_assert_and_check_return_val(aicp && aico, TB_NULL);

	// enter 
	if (aicp->mutx) tb_mutex_enter(aicp->mutx);

	// check
	tb_assert_and_check_goto(aicp->rtor && aicp->rtor->delo, end);
	tb_assert_and_check_goto(aicp->pool && tb_rpool_size(aicp->pool), end);

	// del aico from native
	if (!aicp->rtor->delo(aicp->rtor, aico)) goto end;

	// del aico from pool
	tb_rpool_free(aicp->pool, aico);

end:
	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);
}
tb_bool_t tb_aicp_post(tb_aicp_t* aicp, tb_aice_t const* aice)
{
	tb_assert_and_check_return_val(aicp && aice, TB_FALSE);

	// init
	tb_bool_t ok = TB_FALSE;

	// enter 
	if (aicp->mutx) tb_mutex_enter(aicp->mutx);

	// check
	tb_assert_and_check_goto(aicp->rtor && aicp->rtor->post, end);

	// add aice to native
	ok = aicp->rtor->post(aicp->rtor, aice);

end:
	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);
	
	// ok?
	return ok;
}
tb_bool_t tb_aicp_conn(tb_aicp_t* aicp, tb_aico_t const* aico, tb_char_t const* host, tb_size_t port)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && host && port, TB_FALSE);

	// post
	tb_aice_t aice = {0};
	aice.code = TB_AICE_CODE_CONN;
	aice.aico = aico;
	aice.u.conn.port = port;
	if (tb_ipv4_set(&aice.u.conn.host, host))
		return tb_aicp_post(aicp, &aice);
	return TB_FALSE;
}
tb_bool_t tb_aicp_read(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && data && size, TB_FALSE);

	// post
	tb_aice_t aice = {0};
	aice.code = TB_AICE_CODE_READ;
	aice.aico = aico;
	aice.u.read.data = data;
	aice.u.read.size = size;
	return tb_aicp_post(aicp, &aice);
}
tb_bool_t tb_aicp_writ(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && data && size, TB_FALSE);

	// post
	tb_aice_t aice = {0};
	aice.code = TB_AICE_CODE_WRIT;
	aice.aico = aico;
	aice.u.read.data = data;
	aice.u.read.size = size;
	return tb_aicp_post(aicp, &aice);
}
tb_bool_t tb_aicp_sync(tb_aicp_t* aicp, tb_aico_t const* aico)
{
	// check
	tb_assert_and_check_return_val(aicp && aico, TB_FALSE);

	// post
	tb_aice_t aice = {0};
	aice.code = TB_AICE_CODE_SYNC;
	aice.aico = aico;
	return tb_aicp_post(aicp, &aice);
}
tb_long_t tb_aicp_wait(tb_aicp_t* aicp, tb_long_t timeout)
{
	tb_assert_and_check_return_val(aicp, -1);
	
	// init
	tb_long_t (*wait)(tb_aicp_reactor_t* , tb_long_t ) = TB_NULL;

	// enter 
	if (aicp->mutx) tb_mutex_enter(aicp->mutx);

	// check
	if (aicp->rtor) wait = aicp->rtor->wait;

	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);

	// wait
	return wait? wait(aicp->rtor, timeout) : -1;
}
tb_long_t tb_aicp_spak(tb_aicp_t* aicp)
{
	return 0;
}
