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
 * \file		aicp.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aicp.h"
#include "aioo.h"
#include "../math/math.h"

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */

tb_aicp_reactor_t* tb_aicp_reactor_file_init(tb_aicp_t* aicp);
tb_aicp_reactor_t* tb_aicp_reactor_sock_init(tb_aicp_t* aicp);

/* ///////////////////////////////////////////////////////////////////////
 * implemention
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
	aicp->pool = tb_fpool_init(tb_align_pow2((maxn >> 3) + 1), tb_align_pow2((maxn >> 3) + 1), tb_item_func_ifm(sizeof(tb_aico_t), TB_NULL, TB_NULL));
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
		if (aicp->pool) tb_fpool_exit(aicp->pool);
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
	tb_size_t size = aicp->pool? tb_fpool_size(aicp->pool) : 0;

	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);

	// ok
	return size;
}
tb_handle_t tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb, tb_pointer_t odata)
{
	tb_assert_and_check_return_val(aicp && handle && aicb, TB_NULL);

	// init aico
	tb_aico_t 	aico;
	aico.self = TB_NULL;
	aico.aioo.handle = handle;
	aico.aioo.otype = aicp->type;
	aico.aioo.etype = TB_AIOO_ETYPE_NULL;
	aico.aioo.odata = odata;
	aico.aicb = aicb;

	// enter 
	if (aicp->mutx) tb_mutex_enter(aicp->mutx);

	// check
	tb_assert_and_check_goto(aicp->rtor && aicp->rtor->addo, end);
	tb_assert_and_check_goto(aicp->pool && tb_fpool_size(aicp->pool) < aicp->maxn, end);

	// add aico to pool
	aico.self = (tb_handle_t)tb_fpool_put(aicp->pool, &aico);
	tb_assert_and_check_goto(aico.self, end);

	// add aico to native
	if (aicp->rtor->addo(aicp->rtor, &aico)) goto end;
	
	// del aico from pool
	tb_fpool_del(aicp->pool, aico.self);
	aico.self = TB_NULL;

end:

	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);

	// ok?
	return aico.self;
}
tb_void_t tb_aicp_delo(tb_aicp_t* aicp, tb_handle_t aico)
{
	tb_assert_and_check_return_val(aicp && aico, TB_NULL);

	// enter 
	if (aicp->mutx) tb_mutex_enter(aicp->mutx);

	// check
	tb_assert_and_check_goto(aicp->rtor && aicp->rtor->delo, end);
	tb_assert_and_check_goto(aicp->pool && tb_fpool_size(aicp->pool), end);

	// get the aico from pool
	tb_aico_t const* o = (tb_aico_t const*)tb_fpool_get(aicp->pool, aico);
	tb_assert_and_check_goto(o, end);
	
	// del aico from native
	if (!aicp->rtor->delo(aicp->rtor, o)) goto end;

	// del aico from pool
	tb_fpool_del(aicp->pool, aico);

end:
	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);
}
tb_void_t tb_aicp_setp(tb_aicp_t* aicp, tb_handle_t aico, tb_pointer_t odata)
{
	tb_assert_and_check_return_val(aicp && aico, TB_NULL);

	// enter 
	if (aicp->mutx) tb_mutex_enter(aicp->mutx);

	// check
	tb_assert_and_check_goto(aicp->pool && tb_fpool_size(aicp->pool), end);

	// get the aico from pool
	tb_aico_t* o = (tb_aico_t*)tb_fpool_get(aicp->pool, aico);
	tb_assert_and_check_goto(o, end);
	
	// set odata
	o->aioo.odata = odata;

end:
	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);
}
tb_void_t tb_aicp_post(tb_aicp_t* aicp, tb_handle_t aico, tb_aice_t const* aice)
{
	tb_assert_and_check_return_val(aicp && aico && aice, TB_NULL);

	// enter 
	if (aicp->mutx) tb_mutex_enter(aicp->mutx);

	// check
	tb_assert_and_check_goto(aicp->rtor && aicp->rtor->post, end);
	tb_assert_and_check_goto(aicp->pool && tb_fpool_size(aicp->pool), end);

	// get the aico from pool
	tb_aico_t const* o = (tb_aico_t const*)tb_fpool_get(aicp->pool, aico);
	tb_assert_and_check_goto(o, end);
	
	// add aice to native
	aicp->rtor->post(aicp->rtor, o, aice);

end:
	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);
}
tb_void_t tb_aicp_resv(tb_aicp_t* aicp, tb_handle_t aico, tb_char_t const* name)
{
	// check
	tb_assert_and_check_return(aicp && aico && name);

	// post
	tb_aice_t aice;
	aice.code = TB_AICE_CODE_RESV;
	aice.u.resv.name = name;
	tb_aicp_post(aicp, aico, &aice);
}
tb_void_t tb_aicp_conn(tb_aicp_t* aicp, tb_handle_t aico, tb_char_t const* host, tb_size_t port)
{
	// check
	tb_assert_and_check_return(aicp && aico && host && port);

	// post
	tb_aice_t aice;
	aice.code = TB_AICE_CODE_CONN;
	aice.u.conn.port = port;
	if (tb_ipv4_set(&aice.u.conn.host, host))
		tb_aicp_post(aicp, aico, &aice);
}
tb_void_t tb_aicp_read(tb_aicp_t* aicp, tb_handle_t aico, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(aicp && aico && data && size);

	// post
	tb_aice_t aice;
	aice.code = TB_AICE_CODE_READ;
	aice.u.read.data = data;
	aice.u.read.size = size;
	tb_aicp_post(aicp, aico, &aice);
}
tb_void_t tb_aicp_writ(tb_aicp_t* aicp, tb_handle_t aico, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(aicp && aico && data && size);

	// post
	tb_aice_t aice;
	aice.code = TB_AICE_CODE_WRIT;
	aice.u.read.data = data;
	aice.u.read.size = size;
	tb_aicp_post(aicp, aico, &aice);
}
tb_void_t tb_aicp_sync(tb_aicp_t* aicp, tb_handle_t aico)
{
	// check
	tb_assert_and_check_return(aicp && aico);

	// post
	tb_aice_t aice;
	aice.code = TB_AICE_CODE_SYNC;
	tb_aicp_post(aicp, aico, &aice);
}
tb_void_t tb_aicp_seek(tb_aicp_t* aicp, tb_handle_t aico, tb_hize_t offset)
{
	// check
	tb_assert_and_check_return(aicp && aico);

	// post
	tb_aice_t aice;
	aice.code = TB_AICE_CODE_SEEK;
	aice.u.seek.offset = offset;
	tb_aicp_post(aicp, aico, &aice);
}
tb_void_t tb_aicp_skip(tb_aicp_t* aicp, tb_handle_t aico, tb_hize_t size)
{
	// check
	tb_assert_and_check_return(aicp && aico && size);

	// post
	tb_aice_t aice;
	aice.code = TB_AICE_CODE_SKIP;
	aice.u.skip.size = size;
	tb_aicp_post(aicp, aico, &aice);
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
