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
tb_aicp_reactor_t* tb_aicp_reactor_init(tb_aicp_t* aicp);

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_pointer_t tb_aicp_pool_malloc(tb_aicp_t* aicp, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->pool && size, tb_null);

	// enter 
	if (aicp->mutx.pool) tb_mutex_enter(aicp->mutx.pool);

	// make data
	tb_pointer_t data = tb_spool_malloc(aicp->pool, size);

	// leave 
	if (aicp->mutx.pool) tb_mutex_leave(aicp->mutx.pool);
	
	// ok?
	return data;
}
static tb_pointer_t tb_aicp_pool_memdup(tb_aicp_t* aicp, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->pool && data && size, tb_null);

	// enter 
	if (aicp->mutx.pool) tb_mutex_enter(aicp->mutx.pool);

	// make copy
	tb_pointer_t copy = tb_spool_malloc(aicp->pool, size);
	if (copy) tb_memcpy(copy, data, size);

	// leave 
	if (aicp->mutx.pool) tb_mutex_leave(aicp->mutx.pool);
	
	// ok?
	return copy;
}
static tb_char_t* tb_aicp_pool_strdup(tb_aicp_t* aicp, tb_char_t const* data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->pool && data, tb_null);

	// enter 
	if (aicp->mutx.pool) tb_mutex_enter(aicp->mutx.pool);

	// make copy
	tb_char_t* copy = tb_spool_strdup(aicp->pool, data);

	// leave 
	if (aicp->mutx.pool) tb_mutex_leave(aicp->mutx.pool);
	
	// ok?
	return copy;
}
static tb_void_t tb_aicp_pool_free(tb_aicp_t* aicp, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(aicp && aicp->pool);

	// enter 
	if (aicp->mutx.pool) tb_mutex_enter(aicp->mutx.pool);

	// free data
	if (data) tb_spool_free(aicp->pool, data);

	// leave 
	if (aicp->mutx.pool) tb_mutex_leave(aicp->mutx.pool);
}
static tb_void_t tb_aicp_aice_exit(tb_aicp_t* aicp, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return(aicp && aice);

	// null?
	tb_check_return(aice->code);

	// done
	switch (aice->code)
	{
	case TB_AICE_CODE_RECV:
		if (aice->u.recv.data) tb_aicp_pool_free(aicp, aice->u.recv.data);
		break;
	case TB_AICE_CODE_SEND:
		if (aice->u.send.data) tb_aicp_pool_free(aicp, aice->u.send.data);
		break;
	case TB_AICE_CODE_READ:
		if (aice->u.read.data) tb_aicp_pool_free(aicp, aice->u.read.data);
		break;
	case TB_AICE_CODE_WRIT:
		if (aice->u.writ.data) tb_aicp_pool_free(aicp, aice->u.writ.data);
		break;
	case TB_AICE_CODE_CONN:
		if (aice->u.conn.host) tb_aicp_pool_free(aicp, aice->u.conn.host);
		break;
	default:
		break;
	}
}
static tb_bool_t tb_aicp_aice_post(tb_aicp_t* aicp, tb_aice_t* aice)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->rtor && aicp->rtor->post && aice, tb_false);

	// post
	tb_long_t ok = aicp->rtor->post(aicp->rtor, aice);

	// done it now?
	if (ok > 0)
	{
		// done aicb
		if (aice->aicb && !aice->aicb(aicp, aice))
		{
			// kill all loops
			tb_aicp_kill(aicp);
		}
	}

	// exit aice
	if (ok > 0 || ok < 0) tb_aicp_aice_exit(aicp, aice);

	// ok?
	return (ok >= 0)? tb_true : tb_false;
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_aicp_t* tb_aicp_init(tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, tb_null);

	// alloc aicp
	tb_aicp_t* aicp = tb_malloc0(sizeof(tb_aicp_t));
	tb_assert_and_check_return_val(aicp, tb_null);

	// init aicp
	aicp->maxn = maxn;
	aicp->kill = 0;

	// init mutx
	aicp->mutx.pool = tb_mutex_init(tb_null);
	tb_assert_and_check_goto(aicp->mutx.pool, fail);

	// init pool
	aicp->pool = tb_spool_init(TB_SPOOL_GROW_DEFAULT, 0);
	tb_assert_and_check_goto(aicp->pool, fail);

	// init reactor
	aicp->rtor = tb_aicp_reactor_init(aicp);
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

		// exit pool
		if (aicp->mutx.pool) 
		{
			tb_mutex_enter(aicp->mutx.pool);
			if (aicp->pool) tb_spool_exit(aicp->pool); aicp->pool = tb_null;
			tb_mutex_leave(aicp->mutx.pool);
		}

		// exit mutx
		if (aicp->mutx.pool) tb_mutex_exit(aicp->mutx.pool);
		aicp->mutx.pool = tb_null;

		// free aicp
		tb_free(aicp);
	}
}
tb_bool_t tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t otype)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->rtor && aicp->rtor->addo && handle && otype, tb_false);

	// addo
	return aicp->rtor->addo(aicp->rtor, handle, otype);
}
tb_void_t tb_aicp_delo(tb_aicp_t* aicp, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return(aicp && aicp->rtor && aicp->rtor->delo && handle);

	// delo
	aicp->rtor->delo(aicp->rtor, handle);
}
tb_bool_t tb_aicp_acpt(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->rtor && aicp->rtor->post && handle, tb_false);

	// init
	tb_aice_t 			aice = {0};
	aice.code 			= TB_AICE_CODE_ACPT;
	aice.aicb 			= aicb_func;
	aice.data 			= aicb_data;
	aice.otype 			= TB_AIOO_OTYPE_SOCK;
	aice.handle 		= handle;

	// post
	return tb_aicp_aice_post(aicp, &aice);
}
tb_bool_t tb_aicp_conn(tb_aicp_t* aicp, tb_handle_t handle, tb_char_t const* host, tb_size_t port, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->rtor && aicp->rtor->post && handle && host && port, tb_false);

	// init
	tb_aice_t 			aice = {0};
	aice.code 			= TB_AICE_CODE_CONN;
	aice.aicb 			= aicb_func;
	aice.data 			= aicb_data;
	aice.otype 			= TB_AIOO_OTYPE_SOCK;
	aice.handle 		= handle;
	aice.u.conn.host 	= tb_aicp_pool_strdup(aicp, host);
	aice.u.conn.port 	= port;
	tb_assert_and_check_return_val(aice.u.conn.host, tb_false);

	// post
	return tb_aicp_aice_post(aicp, &aice);
}
tb_bool_t tb_aicp_read(tb_aicp_t* aicp, tb_handle_t handle, tb_hize_t seek, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->rtor && aicp->rtor->post && handle && size, tb_false);

	// init
	tb_aice_t 			aice = {0};
	aice.code 			= TB_AICE_CODE_READ;
	aice.aicb 			= aicb_func;
	aice.data 			= aicb_data;
	aice.otype 			= TB_AIOO_OTYPE_FILE;
	aice.handle 		= handle;
	aice.u.read.seek 	= seek;
	aice.u.read.data 	= tb_aicp_pool_malloc(aicp, size);
	aice.u.read.size 	= size;
	tb_assert_and_check_return_val(aice.u.read.data, tb_false);

	// post
	return tb_aicp_aice_post(aicp, &aice);
}
tb_bool_t tb_aicp_writ(tb_aicp_t* aicp, tb_handle_t handle, tb_hize_t seek, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->rtor && aicp->rtor->post && handle && data && size, tb_false);

	// init
	tb_aice_t 			aice = {0};
	aice.code 			= TB_AICE_CODE_WRIT;
	aice.aicb 			= aicb_func;
	aice.data 			= aicb_data;
	aice.otype 			= TB_AIOO_OTYPE_FILE;
	aice.handle 		= handle;
	aice.u.writ.seek 	= seek;
	aice.u.writ.data 	= tb_aicp_pool_memdup(aicp, data, size);
	aice.u.writ.size 	= size;
	tb_assert_and_check_return_val(aice.u.writ.data, tb_false);

	// post
	return tb_aicp_aice_post(aicp, &aice);
}
tb_bool_t tb_aicp_recv(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->rtor && aicp->rtor->post && handle && size, tb_false);

	// init
	tb_aice_t 			aice = {0};
	aice.code 			= TB_AICE_CODE_RECV;
	aice.aicb 			= aicb_func;
	aice.data 			= aicb_data;
	aice.otype 			= TB_AIOO_OTYPE_SOCK;
	aice.handle 		= handle;
	aice.u.recv.data 	= tb_aicp_pool_malloc(aicp, size);
	aice.u.recv.size 	= size;
	tb_assert_and_check_return_val(aice.u.recv.data, tb_false);

	// post
	return tb_aicp_aice_post(aicp, &aice);
}
tb_bool_t tb_aicp_send(tb_aicp_t* aicp, tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->rtor && aicp->rtor->post && handle && data && size, tb_false);

	// init
	tb_aice_t 			aice = {0};
	aice.code 			= TB_AICE_CODE_SEND;
	aice.aicb 			= aicb_func;
	aice.data 			= aicb_data;
	aice.otype 			= TB_AIOO_OTYPE_SOCK;
	aice.handle 		= handle;
	aice.u.send.data 	= tb_aicp_pool_memdup(aicp, data, size);
	aice.u.send.size 	= size;
	tb_assert_and_check_return_val(aice.u.send.data, tb_false);

	// post
	return tb_aicp_aice_post(aicp, &aice);
}
tb_void_t tb_aicp_loop(tb_aicp_t* aicp, tb_long_t timeout)
{
	// check
	tb_assert_and_check_return(aicp && aicp->rtor && aicp->rtor->spak);

	// worker++
	tb_atomic_fetch_and_inc(&aicp->work);

	// loop
	while (!tb_atomic_get(&aicp->kill))
	{
		// spak
		tb_aice_t 	resp = {0};
		tb_long_t	ok = aicp->rtor->spak(aicp->rtor, &resp, timeout);

		// failed? exit all loops
		if (ok < 0) tb_aicp_kill(aicp);

		// killed? break it
		if (tb_atomic_get(&aicp->kill))
		{
			tb_aicp_aice_exit(aicp, &resp);
			break;
		}
		
		// timeout?
		tb_check_continue(ok);

		// done aicb
		if (resp.aicb && !resp.aicb(aicp, &resp)) break;

		// exit resp
		tb_aicp_aice_exit(aicp, &resp);
	}

	// worker--
	tb_atomic_fetch_and_dec(&aicp->work);
}
tb_void_t tb_aicp_kill(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return(aicp);

	// kill it
	if (!tb_atomic_fetch_and_set(&aicp->kill, 1))
	{
		// kill reactor
		if (aicp->rtor && aicp->rtor->kill) aicp->rtor->kill(aicp->rtor);
	}
}

