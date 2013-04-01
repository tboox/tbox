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
#include "../platform/platform.h"

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
	tb_assert_and_check_return_val(maxn, tb_null);

	// alloc aicp
	tb_aicp_t* aicp = tb_malloc0(sizeof(tb_aicp_t));
	tb_assert_and_check_return_val(aicp, tb_null);

	// init aicp
	aicp->type = type;
	aicp->maxn = maxn;
	aicp->kill = 0;

	// reactors
	static tb_aicp_reactor_t* (*s_init[])(tb_aicp_t*) = 
	{
		tb_null
	, 	tb_aicp_reactor_file_init
	,	tb_aicp_reactor_sock_init
	
	};
	tb_assert_and_check_goto(type < tb_arrayn(s_init), fail);

	// init mutx
	aicp->mutx.pool = tb_mutex_init(tb_null);
	aicp->mutx.post = tb_mutex_init(tb_null);
	aicp->mutx.resp = tb_mutex_init(tb_null);
	tb_assert_and_check_goto(aicp->mutx.pool && aicp->mutx.post && aicp->mutx.resp, fail);

	// init pool
	aicp->pool = tb_rpool_init(tb_align_pow2((maxn >> 3) + 1), sizeof(tb_aico_t), 0);
	tb_assert_and_check_goto(aicp->pool, fail);

	// init post
	aicp->post = tb_queue_init(TB_QUEUE_SIZE_DEFAULT, tb_item_func_ifm(sizeof(tb_aice_t), tb_null, tb_null));
	tb_assert_and_check_goto(aicp->post, fail);

	// init resp
	aicp->resp = tb_queue_init(TB_QUEUE_SIZE_DEFAULT, tb_item_func_ifm(sizeof(tb_aice_t), tb_null, tb_null));
	tb_assert_and_check_goto(aicp->resp, fail);

	// init reactor
	if (s_init[type]) aicp->rtor = s_init[type](aicp);
	tb_assert_and_check_goto(aicp->rtor, fail);

	// ok
	return aicp;

fail:
	if (aicp) tb_aicp_exit(aicp);
	return tb_null;
}
tb_void_t tb_aicp_exit(tb_aicp_t* aicp)
{
	if (aicp)
	{
		// is killed?
		if (!tb_atomic_get(&aicp->kill))
		{
			// kill
			tb_aicp_kill(aicp);

			// sleep some time
			tb_msleep(500);
		}

		// exit reactor
		if (aicp->rtor)
		{
			tb_assert(aicp->rtor && aicp->rtor->exit);
			aicp->rtor->exit(aicp->rtor);
			aicp->rtor = tb_null;
		}

		// exit resp
		if (aicp->mutx.resp) 
		{
			tb_mutex_enter(aicp->mutx.resp);

			if (aicp->resp) tb_queue_exit(aicp->resp);
			aicp->resp = tb_null;

			tb_mutex_leave(aicp->mutx.resp);
		}

		// exit post
		if (aicp->mutx.post) 
		{
			tb_mutex_enter(aicp->mutx.post);

			if (aicp->post) tb_queue_exit(aicp->post);
			aicp->post = tb_null;

			tb_mutex_leave(aicp->mutx.post);
		}

		// exit pool
		if (aicp->mutx.pool) 
		{
			tb_mutex_enter(aicp->mutx.pool);

			if (aicp->pool) tb_rpool_exit(aicp->pool);
			aicp->pool = tb_null;

			tb_mutex_leave(aicp->mutx.pool);
		}

		// exit mutx
		if (aicp->mutx.resp) tb_mutex_exit(aicp->mutx.resp);
		aicp->mutx.resp = tb_null;
		if (aicp->mutx.post) tb_mutex_exit(aicp->mutx.post);
		aicp->mutx.post = tb_null;
		if (aicp->mutx.pool) tb_mutex_exit(aicp->mutx.pool);
		aicp->mutx.pool = tb_null;

		// free aicp
		tb_free(aicp);
	}
}
tb_size_t tb_aicp_maxn(tb_aicp_t* aicp)
{
	tb_assert_and_check_return_val(aicp, 0);
	return aicp->maxn;
}
tb_size_t tb_aicp_size(tb_aicp_t* aicp)
{
	tb_assert_and_check_return_val(aicp, 0);

	// enter 
	if (aicp->mutx.pool) tb_mutex_enter(aicp->mutx.pool);

	// size
	tb_size_t size = aicp->pool? tb_rpool_size(aicp->pool) : 0;

	// leave 
	if (aicp->mutx.pool) tb_mutex_leave(aicp->mutx.pool);

	// ok
	return size;
}
tb_aico_t const* tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb, tb_pointer_t odata)
{
	tb_assert_and_check_return_val(aicp && handle && aicb, tb_null);

	// init aico
	tb_aico_t* aico = tb_null;

	// enter 
	if (aicp->mutx.pool) tb_mutex_enter(aicp->mutx.pool);

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
	aico = tb_null;

end:

	// leave 
	if (aicp->mutx.pool) tb_mutex_leave(aicp->mutx.pool);

	// ok?
	return aico;
}
tb_void_t tb_aicp_delo(tb_aicp_t* aicp, tb_aico_t const* aico)
{
	// check
	tb_assert_and_check_return(aicp && aico);

	// enter 
	if (aicp->mutx.pool) tb_mutex_enter(aicp->mutx.pool);

	// check
	tb_assert_and_check_goto(aicp->rtor && aicp->rtor->delo, end);
	tb_assert_and_check_goto(aicp->pool && tb_rpool_size(aicp->pool), end);

	// del aico from native
	if (!aicp->rtor->delo(aicp->rtor, aico)) goto end;

	// del aico from pool
	tb_rpool_free(aicp->pool, (tb_pointer_t)aico);

end:
	// leave 
	if (aicp->mutx.pool) tb_mutex_leave(aicp->mutx.pool);
}
tb_bool_t tb_aicp_post(tb_aicp_t* aicp, tb_aice_t const* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->post && aice, tb_false);

	// enter 
	if (aicp->mutx.post) tb_mutex_enter(aicp->mutx.post);

	// post aice
	tb_bool_t ok = tb_false;
	if (!tb_queue_full(aicp->post)) 
	{
		tb_queue_put(aicp->post, aice);
		ok = tb_true;
	}

	// leave 
	if (aicp->mutx.post) tb_mutex_leave(aicp->mutx.post);
	
	// ok?
	return ok;
}
tb_bool_t tb_aicp_sync(tb_aicp_t* aicp, tb_aico_t const* aico)
{
	// check
	tb_assert_and_check_return_val(aicp && aico, tb_false);

	// post
	tb_aice_t aice = {0};
	aice.code = TB_AICE_CODE_SYNC;
	aice.aico = (tb_pointer_t)aico;
	return tb_aicp_post(aicp, &aice);
}
tb_bool_t tb_aicp_acpt(tb_aicp_t* aicp, tb_aico_t const* aico)
{
	// check
	tb_assert_and_check_return_val(aicp && aico, tb_false);

	// post
	tb_aice_t aice = {0};
	aice.code = TB_AICE_CODE_ACPT;
	aice.aico = (tb_pointer_t)aico;
	return tb_aicp_post(aicp, &aice);
}
tb_bool_t tb_aicp_conn(tb_aicp_t* aicp, tb_aico_t const* aico, tb_char_t const* host, tb_size_t port)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && host && port, tb_false);

	// post
	tb_aice_t aice = {0};
	aice.code = TB_AICE_CODE_CONN;
	aice.u.conn.port = port;
	aice.aico = (tb_pointer_t)aico;
	if (tb_ipv4_set(&aice.u.conn.host, host))
		return tb_aicp_post(aicp, &aice);
	return tb_false;
}
tb_bool_t tb_aicp_read(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && data && size, tb_false);

	// post
	tb_aice_t aice = {0};
	aice.code = TB_AICE_CODE_READ;
	aice.u.read.data = data;
	aice.u.read.maxn = size;
	aice.aico = (tb_pointer_t)aico;
	return tb_aicp_post(aicp, &aice);
}
tb_bool_t tb_aicp_writ(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && data && size, tb_false);

	// post
	tb_aice_t aice = {0};
	aice.code = TB_AICE_CODE_WRIT;
	aice.u.read.data = data;
	aice.u.read.maxn = size;
	aice.aico = (tb_pointer_t)aico;
	return tb_aicp_post(aicp, &aice);
}
tb_bool_t tb_aicp_spak(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->post && aicp->resp && aicp->rtor && aicp->rtor->spak, tb_false);

	// enter 
	if (aicp->mutx.post) tb_mutex_enter(aicp->mutx.post);

	// get the posted aice
	tb_aice_t const* post = tb_null;
	if (!tb_queue_null(aicp->post)) 
	{
		post = tb_queue_get(aicp->post);
		tb_queue_pop(aicp->post);
	}

	// leave 
	if (aicp->mutx.post) tb_mutex_leave(aicp->mutx.post);

	// killed?
	tb_check_return_val(!tb_atomic_get(&aicp->kill), tb_false);

	// spak
	tb_aice_t const* 	resp = tb_null;
	tb_long_t 			resn = aicp->rtor->spak(aicp->rtor, post, &resp);

	// killed?
	tb_check_return_val(resn >= 0, tb_false);
	tb_check_return_val(!tb_atomic_get(&aicp->kill), tb_false);
	
	// no aice?
	tb_check_return_val(resn, tb_true);

	// enter 
	if (aicp->mutx.resp) tb_mutex_enter(aicp->mutx.resp);

	// push resp aices
	tb_size_t i = 0;
	for (i = 0; i < resn; i++) tb_queue_put(aicp->resp, &resp[i]);

	// leave 
	if (aicp->mutx.resp) tb_mutex_leave(aicp->mutx.resp);
		
	// ok?
	return tb_atomic_get(&aicp->kill)? tb_false : tb_true;
}

tb_bool_t tb_aicp_done(tb_aicp_t* aicp)
{
	return tb_false;
}

tb_void_t tb_aicp_kill(tb_aicp_t* aicp)
{
	tb_assert_and_check_return(aicp);

	// kill it
	tb_atomic_set(&aicp->kill, 1);
}

