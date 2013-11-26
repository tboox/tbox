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
 * aico
 */
static tb_aico_t* tb_aicp_aico_init(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(aicp && handle && type, tb_null);

	// enter 
	if (aicp->mutx) tb_mutex_enter(aicp->mutx);

	// make aico
	tb_aico_t* aico = aicp->pool? (tb_aico_t*)tb_rpool_malloc0(aicp->pool) : tb_null;

	// init aico
	if (aico)
	{
		aico->type 		= type;
		aico->handle 	= handle;
	}

	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);
	
	// ok?
	return aico;
}
static tb_void_t tb_aicp_aico_exit(tb_aicp_t* aicp, tb_aico_t const* aico)
{
	// check
	tb_assert_and_check_return(aicp);

	// enter 
	if (aicp->mutx) tb_mutex_enter(aicp->mutx);

	// exit it
	if (aico) tb_rpool_free(aicp->pool, aico);

	// leave 
	if (aicp->mutx) tb_mutex_leave(aicp->mutx);
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

	// init proactor
	aicp->ptor = tb_aicp_proactor_init(aicp);
	tb_assert_and_check_goto(aicp->ptor && aicp->ptor->step >= sizeof(tb_aico_t), fail);

	// init mutx
	aicp->mutx = tb_mutex_init();
	tb_assert_and_check_goto(aicp->mutx, fail);

	// init pool
	aicp->pool = tb_rpool_init((maxn >> 2) + 16, aicp->ptor->step, 0);
	tb_assert_and_check_goto(aicp->pool, fail);

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

		// exit pool
		if (aicp->mutx) tb_mutex_enter(aicp->mutx);
		if (aicp->pool) tb_rpool_exit(aicp->pool);
		aicp->pool = tb_null;
		if (aicp->mutx) tb_mutex_leave(aicp->mutx);

		// exit mutx
		if (aicp->mutx) tb_mutex_exit(aicp->mutx);
		aicp->mutx = tb_null;

		// free aicp
		tb_free(aicp);
	}
}
tb_aico_t const* tb_aicp_addo(tb_aicp_t* aicp, tb_handle_t handle, tb_size_t type)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->addo && handle && type, tb_null);

	// done
	tb_bool_t 	ok = tb_false;
	tb_aico_t* 	aico = tb_null;
	do
	{
		// init aico
		aico = tb_aicp_aico_init(aicp, handle, type);
		tb_assert_and_check_break(aico);

		// addo aico
		if (!aicp->ptor->addo(aicp->ptor, aico)) break;

		// ok
		ok = tb_true;

	} while (0);

	// failed? remove aico
	if (!ok && aico) 
	{
		tb_aicp_aico_exit(aicp, aico);
		aico = tb_null;
	}

	// ok?
	return aico;
}
tb_void_t tb_aicp_delo(tb_aicp_t* aicp, tb_aico_t const* aico)
{
	// check
	tb_assert_and_check_return(aicp && aicp->ptor && aicp->ptor->delo && aico);

	// delo
	if (aicp->ptor->delo(aicp->ptor, (tb_aico_t*)aico))
		tb_aicp_aico_exit(aicp, aico);
}
tb_bool_t tb_aicp_post(tb_aicp_t* aicp, tb_aice_t const* list, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->ptor && aicp->ptor->post, tb_false);
	tb_assert_and_check_return_val(list && size && size <= TB_AICP_POST_MAXN, tb_false);

	// post++
#ifdef __tb_debug__
	{
		tb_size_t i = 0;
		for (i = 0; i < size; i++)
		{
			// the aico
			tb_aico_t* aico = list[i].aico;
			tb_assert_and_check_return_val(aico, tb_false);

			// post++
			tb_assert_return_val(!tb_atomic_fetch_and_inc(&aico->post), tb_false);
		}
	}
#endif

	// post
	return aicp->ptor->post(aicp->ptor, list, size);
}
tb_bool_t tb_aicp_acpt(tb_aicp_t* aicp, tb_aico_t const* aico, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aico, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_ACPT;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_conn(tb_aicp_t* aicp, tb_aico_t const* aico, tb_char_t const* host, tb_size_t port, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && host && port, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_CONN;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.conn.host 		= host;
	aice.u.conn.port 		= port;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_recv(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_RECV;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.recv.data 		= data;
	aice.u.recv.size 		= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_send(tb_aicp_t* aicp, tb_aico_t const* aico, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_SEND;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.send.data 		= data;
	aice.u.send.size 		= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_recvv(tb_aicp_t* aicp, tb_aico_t const* aico, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_RECVV;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.recvv.list 		= list;
	aice.u.recvv.size 		= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_sendv(tb_aicp_t* aicp, tb_aico_t const* aico, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_SENDV;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.sendv.list 		= list;
	aice.u.sendv.size 		= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_sendfile(tb_aicp_t* aicp, tb_aico_t const* aico, tb_handle_t file, tb_hize_t seek, tb_hize_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && file, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_SENDFILE;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.sendfile.file 	= file;
	aice.u.sendfile.seek 	= seek;
	aice.u.sendfile.size 	= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_read(tb_aicp_t* aicp, tb_aico_t const* aico, tb_hize_t seek, tb_byte_t* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_READ;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.read.seek 		= seek;
	aice.u.read.data 		= data;
	aice.u.read.size 		= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_writ(tb_aicp_t* aicp, tb_aico_t const* aico, tb_hize_t seek, tb_byte_t const* data, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && data && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_WRIT;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.writ.seek 		= seek;
	aice.u.writ.data 		= data;
	aice.u.writ.size 		= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_readv(tb_aicp_t* aicp, tb_aico_t const* aico, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_READV;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.readv.seek 		= seek;
	aice.u.readv.list 		= list;
	aice.u.readv.size 		= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_writv(tb_aicp_t* aicp, tb_aico_t const* aico, tb_hize_t seek, tb_iovec_t const* list, tb_size_t size, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aico && list && size, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_WRITV;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;
	aice.u.writv.seek 		= seek;
	aice.u.writv.list 		= list;
	aice.u.writv.size 		= size;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_bool_t tb_aicp_fsync(tb_aicp_t* aicp, tb_aico_t const* aico, tb_aicb_t aicb_func, tb_cpointer_t aicb_data)
{
	// check
	tb_assert_and_check_return_val(aicp && aico, tb_false);

	// init
	tb_aice_t 				aice = {0};
	aice.code 				= TB_AICE_CODE_FSYNC;
	aice.aicb 				= aicb_func;
	aice.data 				= aicb_data;
	aice.aico 				= aico;

	// post
	return tb_aicp_post(aicp, &aice, 1);
}
tb_void_t tb_aicp_loop(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return(aicp && aicp->ptor);

	// the spak
	tb_long_t (*spak)(tb_aicp_proactor_t* , tb_aice_t* , tb_long_t ) = aicp->ptor->spak;
	tb_assert_and_check_return(spak);

	// worker++
	tb_atomic_fetch_and_inc(&aicp->work);

	// loop
	while (!tb_atomic_get(&aicp->kill))
	{
		// spak
		tb_aice_t 	resp = {0};
		tb_long_t	ok = spak(aicp->ptor, &resp, -1);

		// spak ctime
		tb_ctime_spak();

		// failed? exit all loops
		if (ok < 0) tb_aicp_kill(aicp);

		// killed? break it
		tb_check_break(!tb_atomic_get(&aicp->kill));
		
		// timeout?
		tb_check_continue(ok);

		// post--
		tb_assert(resp.aico? (tb_atomic_fetch_and_set0(&resp.aico->post) == 1) : 0);

		// done aicb
		if (resp.aicb && !resp.aicb(aicp, &resp)) 
		{
			// exit all loops
			tb_aicp_kill(aicp);
			break;
		}
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

