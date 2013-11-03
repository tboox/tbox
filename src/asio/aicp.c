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
 * @ingroup 	asio
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
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp);

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

	// init proactor
	aicp->ptor = tb_aicp_proactor_init(aicp);
	tb_assert_and_check_goto(aicp->ptor, fail);

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
		// kill all
		if (!tb_atomic_get(&aicp->kill)) tb_aicp_kill(aicp);
	
		// wait workers 
		tb_hong_t time = tb_mclock();
		while (tb_atomic_get(&aicp->work) && (tb_mclock() < time + 5000)) tb_msleep(500);

		// exit proactor
		if (aicp->ptor)
		{
			tb_assert(aicp->ptor && aicp->ptor->exit);
			aicp->ptor->exit(aicp->ptor);
			aicp->ptor = tb_null;
		}

		// free aicp
		tb_free(aicp);
	}
}
tb_bool_t tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->addo && handle && type, tb_false);

	// addo
	return aicp->ptor->addo(aicp->ptor, handle, type);
}
tb_void_t tb_aicp_delo(tb_aicp_t* aicp, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return(aicp && aicp->ptor && aicp->ptor->delo && handle);

	// delo
	aicp->ptor->delo(aicp->ptor, handle);
}
tb_bool_t tb_aicp_post(tb_aicp_t* aicp, tb_aice_t const* list, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post, tb_false);
	tb_assert_and_check_return_val(list && size && size <= TB_AICP_POST_MAXN, tb_false);

	// post
	return aicp->ptor->post(aicp->ptor, list, size);
}
tb_bool_t tb_aicp_acpt(tb_aicp_t* aicp, tb_handle_t handle, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post && handle, tb_false);

	// init
	tb_aice_t 			aice = {0};
	aice.code 			= TB_AICE_CODE_ACPT;
	aice.aicb 			= aicb_func;
	aice.data 			= aicb_data;
	aice.type 			= TB_AICO_TYPE_SOCK;
	aice.handle 		= handle;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_conn(tb_aicp_t* aicp, tb_handle_t handle, tb_char_t const* host, tb_size_t port, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post && handle && host && port, tb_false);

	// init
	tb_aice_t 			aice = {0};
	aice.code 			= TB_AICE_CODE_CONN;
	aice.aicb 			= aicb_func;
	aice.data 			= aicb_data;
	aice.type 			= TB_AICO_TYPE_SOCK;
	aice.handle 		= handle;
	aice.u.conn.host 	= host;
	aice.u.conn.port 	= port;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_read(tb_aicp_t* aicp, tb_handle_t handle, tb_hize_t seek, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post && handle && data && size, tb_false);

	// init
	tb_aice_t 			aice = {0};
	aice.code 			= TB_AICE_CODE_READ;
	aice.aicb 			= aicb_func;
	aice.data 			= aicb_data;
	aice.type 			= TB_AICO_TYPE_FILE;
	aice.handle 		= handle;
	aice.u.read.seek 	= seek;
	aice.u.read.data 	= data;
	aice.u.read.size 	= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_writ(tb_aicp_t* aicp, tb_handle_t handle, tb_hize_t seek, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post && handle && data && size, tb_false);

	// init
	tb_aice_t 			aice = {0};
	aice.code 			= TB_AICE_CODE_WRIT;
	aice.aicb 			= aicb_func;
	aice.data 			= aicb_data;
	aice.type 			= TB_AICO_TYPE_FILE;
	aice.handle 		= handle;
	aice.u.writ.seek 	= seek;
	aice.u.writ.data 	= data;
	aice.u.writ.size 	= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_recv(tb_aicp_t* aicp, tb_handle_t handle, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post && handle && data && size, tb_false);

	// init
	tb_aice_t 			aice = {0};
	aice.code 			= TB_AICE_CODE_RECV;
	aice.aicb 			= aicb_func;
	aice.data 			= aicb_data;
	aice.type 			= TB_AICO_TYPE_SOCK;
	aice.handle 		= handle;
	aice.u.recv.data 	= data;
	aice.u.recv.size 	= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_send(tb_aicp_t* aicp, tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post && handle && data && size, tb_false);

	// init
	tb_aice_t 			aice = {0};
	aice.code 			= TB_AICE_CODE_SEND;
	aice.aicb 			= aicb_func;
	aice.data 			= aicb_data;
	aice.type 			= TB_AICO_TYPE_SOCK;
	aice.handle 		= handle;
	aice.u.send.data 	= data;
	aice.u.send.size 	= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_void_t tb_aicp_loop(tb_aicp_t* aicp, tb_long_t timeout)
{
	// check
	tb_assert_and_check_return(aicp && aicp->ptor && aicp->ptor->spak);

	// worker++
	tb_atomic_fetch_and_inc(&aicp->work);

	// loop
	while (!tb_atomic_get(&aicp->kill))
	{
		// spak
		tb_aice_t 	resp = {0};
		tb_long_t	ok = aicp->ptor->spak(aicp->ptor, &resp, timeout);

		// failed? exit all loops
		if (ok < 0) tb_aicp_kill(aicp);

		// killed? break it
		tb_check_break(!tb_atomic_get(&aicp->kill));
		
		// timeout?
		tb_check_continue(ok);

		// done aicb
		if (resp.aicb && !resp.aicb(aicp, &resp)) break;
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
		// kill proactor
		if (aicp->ptor && aicp->ptor->kill) aicp->ptor->kill(aicp->ptor);
	}
}

