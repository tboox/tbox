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
static tb_bool_t tb_aicp_post(tb_aicp_t* aicp, tb_aice_t const* aice)
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
	tb_assert_and_check_goto(aicp->mutx.pool && aicp->mutx.post, fail);

	// init pool
	aicp->pool = tb_rpool_init(tb_align_pow2((maxn >> 3) + 1), sizeof(tb_aico_t), 0);
	tb_assert_and_check_goto(aicp->pool, fail);

	// init post
	aicp->post = tb_queue_init(TB_QUEUE_SIZE_DEFAULT, tb_item_func_ifm(sizeof(tb_aice_t), tb_null, tb_null));
	tb_assert_and_check_goto(aicp->post, fail);

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
	// check
	tb_assert_and_check_return_val(aicp && aicp->pool, 0);

	// enter 
	if (aicp->mutx.pool) tb_mutex_enter(aicp->mutx.pool);

	// size
	tb_size_t size = tb_rpool_size(aicp->pool);

	// leave 
	if (aicp->mutx.pool) tb_mutex_leave(aicp->mutx.pool);

	// ok
	return size;
}
tb_aico_t const* tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb, tb_pointer_t odata)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->rtor && aicp->rtor->addo && aicp->pool && handle, tb_null);

	// enter 
	if (aicp->mutx.pool) tb_mutex_enter(aicp->mutx.pool);

	// done
	tb_bool_t 	ok = tb_false;
	tb_aico_t* 	aico = tb_null;
	do
	{
		// check
		tb_assert_and_check_break(tb_rpool_size(aicp->pool) < aicp->maxn);

		// add aico to pool
		aico = (tb_aico_t*)tb_rpool_malloc0(aicp->pool);
		tb_assert_and_check_break(aico);

		// init aico
		aico->aioo.handle = handle;
		aico->aioo.otype = aicp->type;
		aico->aioo.etype = TB_AIOO_ETYPE_NONE;
		aico->aioo.odata = odata;
		aico->aicb = aicb;

		// add aico to native
		if (!aicp->rtor->addo(aicp->rtor, aico)) break;

		// ok
		ok = tb_true;
		
	} while (0);

	// failed?
	if (!ok)
	{
		// del aico from pool
		tb_rpool_free(aicp->pool, aico);
		aico = tb_null;
	}

	// leave 
	if (aicp->mutx.pool) tb_mutex_leave(aicp->mutx.pool);

	// ok?
	return aico;
}
tb_void_t tb_aicp_delo(tb_aicp_t* aicp, tb_aico_t const* aico)
{
	// check
	tb_assert_and_check_return(aicp && aicp->rtor && aicp->rtor->delo && aicp->pool && aico);

	// enter 
	if (aicp->mutx.pool) tb_mutex_enter(aicp->mutx.pool);

	// done
	do
	{
		// check
		tb_assert_and_check_break(tb_rpool_size(aicp->pool));

		// del aico from native
		if (!aicp->rtor->delo(aicp->rtor, aico)) break;

		// del aico from pool
		tb_rpool_free(aicp->pool, (tb_pointer_t)aico);

	} while (0);

	// leave 
	if (aicp->mutx.pool) tb_mutex_leave(aicp->mutx.pool);
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
tb_bool_t tb_aicp_clos(tb_aicp_t* aicp, tb_aico_t const* aico)
{
	// check
	tb_assert_and_check_return_val(aicp && aico, tb_false);

	// post
	tb_aice_t aice = {0};
	aice.code = TB_AICE_CODE_CLOS;
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
tb_bool_t tb_aicp_writ(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && data && size, tb_false);

	// post
	tb_aice_t aice = {0};
	aice.code = TB_AICE_CODE_WRIT;
	aice.u.writ.data = data;
	aice.u.writ.maxn = size;
	aice.aico = (tb_pointer_t)aico;
	return tb_aicp_post(aicp, &aice);
}
tb_long_t tb_aicp_spak(tb_aicp_t* aicp, tb_long_t timeout)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->post && aicp->rtor && aicp->rtor->spak, -1);

	// spak
	tb_aice_t 	resp = {0};
	tb_long_t	ok = aicp->rtor->spak(aicp->rtor, &resp, timeout);
	tb_assert_and_check_return_val(ok >= 0, -1);

	// killed?
	tb_check_return_val(!tb_atomic_get(&aicp->kill), -1);
	
	// timeout?
	tb_check_return_val(ok, 0);

	// aico
	tb_aico_t const* aico = (tb_aico_t const*)resp.aico;
	tb_assert_and_check_return_val(aico, -1);

	// done aicb
	if (aico->aicb) if (!aico->aicb(aicp, aico, &resp)) return -1;

	// killed?
	tb_check_return_val(!tb_atomic_get(&aicp->kill), -1);

	// ok?
	return ok;
}
tb_void_t tb_aicp_kill(tb_aicp_t* aicp)
{
	tb_assert_and_check_return(aicp);

	// kill it
	tb_atomic_set(&aicp->kill, 1);
}

