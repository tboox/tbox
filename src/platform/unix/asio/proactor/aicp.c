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
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 			"proactor"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// the unix aicp proactor maxn
#define TB_AICP_PROACTOR_UNIX_MAXN 		(3)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the aio proactor type
typedef struct __tb_aicp_proactor_unix_t
{
	// the proactor base
	tb_aicp_proactor_t 				base;

	// the resp mutx
	tb_handle_t 					mutx;

	// the resp queue
	tb_queue_t* 					resp;

	// the resp wait
	tb_handle_t 					wait;

	// the proactor post index + 1 for the proactor list
	tb_size_t 						ptor_post[TB_AICO_TYPE_MAXN][TB_AICE_CODE_MAXN];

	// the proactor list
	tb_size_t 						ptor_size;
	tb_aicp_proactor_t* 			ptor_list[TB_AICP_PROACTOR_UNIX_MAXN];

}tb_aicp_proactor_unix_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_proactor_unix_need(tb_aicp_proactor_t* proactor, tb_size_t type, tb_size_t indx)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return_val(ptor && indx, tb_false);

	// need this type for this proactor index?
	switch (type)
	{
	case TB_AICO_TYPE_SOCK:
		{
			if (ptor->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_RECV] == indx) return tb_true;
			if (ptor->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_SEND] == indx) return tb_true;
			if (ptor->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_ACPT] == indx) return tb_true;
			if (ptor->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_CONN] == indx) return tb_true;
		}
		break;
	case TB_AICO_TYPE_FILE:
		{
			if (ptor->ptor_post[TB_AICO_TYPE_FILE][TB_AICE_CODE_READ] == indx) return tb_true;
			if (ptor->ptor_post[TB_AICO_TYPE_FILE][TB_AICE_CODE_WRIT] == indx) return tb_true;
		}
		break;
	default:
		break;
	}

	// no
	return tb_false;
}

static tb_bool_t tb_aicp_proactor_unix_addo(tb_aicp_proactor_t* proactor, tb_handle_t handle, tb_size_t type)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return_val(ptor && handle && type, tb_false);

	// walk proactors
	tb_size_t i = 0;
	tb_size_t n = ptor->ptor_size;
	tb_size_t addo = 0;
	for (; i < n; i++) 
	{
		// need this?
		if (tb_aicp_proactor_unix_need(proactor, type, i + 1))
		{
			// addo it
			tb_aicp_proactor_t* item = ptor->ptor_list[i];
			if (item && item->addo) 
			{
				if (!item->addo(item, handle, type)) break;
			}

			// addo++
			addo++;
		}
	}

	// ok?
	return addo? tb_true : tb_false;
}
static tb_void_t tb_aicp_proactor_unix_delo(tb_aicp_proactor_t* proactor, tb_handle_t handle)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return(ptor && handle);

	// walk proactors
	tb_size_t i = 0;
	tb_size_t n = ptor->ptor_size;
	for (; i < n; i++) 
	{
		tb_aicp_proactor_t* item = ptor->ptor_list[i];
		if (item && item->delo) item->delo(item, handle);
	}
}
static tb_bool_t tb_aicp_proactor_unix_post(tb_aicp_proactor_t* proactor, tb_aice_t const* list, tb_size_t size)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return_val(ptor && list && size && size <= TB_AICP_POST_MAXN, tb_false);

	// init aice list
	tb_size_t i = 0;
	tb_size_t aice_size[TB_AICP_PROACTOR_UNIX_MAXN] = {0};
	tb_aice_t aice_list[TB_AICP_PROACTOR_UNIX_MAXN][TB_AICP_POST_MAXN] = {0};
	for (i = 0; i < size; i++)
	{
		// the aice
		tb_aice_t const* aice = &list[i];
		tb_assert_and_check_break(aice->type < TB_AICO_TYPE_MAXN && aice->code < TB_AICE_CODE_MAXN);

		// the proactor index
		tb_size_t indx = ptor->ptor_post[aice->type][aice->code];
		tb_assert_and_check_break(indx && indx < TB_AICP_PROACTOR_UNIX_MAXN + 1);

		// append aice to the proactor
		indx--;
		aice_list[indx][aice_size[indx]++] = *aice;
		tb_assert_and_check_break(aice_size[indx] + 1 < TB_AICP_POST_MAXN);
	}

	// post aice list
	tb_size_t n = ptor->ptor_size;
	tb_size_t post = 0;
	for (i = 0; i < n; i++) 
	{
		tb_aicp_proactor_t* item = ptor->ptor_list[i];
		if (aice_size[i] && item->post(item, aice_list[i], aice_size[i])) post++;
	}

	// ok?
	return post? tb_true : tb_false;
}
static tb_long_t tb_aicp_proactor_unix_spak(tb_aicp_proactor_t* proactor, tb_aice_t* resp, tb_long_t timeout)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->wait && resp, -1);

	// trace
//	tb_trace_impl("spak[%u]: ..", (tb_uint16_t)tb_thread_self());

	// enter 
	if (ptor->mutx) tb_mutex_enter(ptor->mutx);

	// post aice
	tb_long_t ok = 0;
	if (!tb_queue_null(ptor->resp)) 
	{
		// get resp
		tb_aice_t const* aice = tb_queue_get(ptor->resp);
		if (aice) 
		{
			// save resp
			*resp = *aice;

			// trace
			tb_trace_impl("spak[%u]: code: %lu, size: %lu", (tb_uint16_t)tb_thread_self(), aice->code, tb_queue_size(ptor->resp));

			// pop it
			tb_queue_pop(ptor->resp);

			// ok
			ok = 1;
		}
	}

	// leave 
	if (ptor->mutx) tb_mutex_leave(ptor->mutx);

	// no aice? spak all proactors util get one
	if (!ok)
	{
		// spak proactors
		tb_size_t i = 0;
		tb_size_t n = ptor->ptor_size;
		for (; i < n; i++) 
		{
			tb_aicp_proactor_t* item = ptor->ptor_list[i];
			if (item && item->spak) 
			{
				// spak it
				ok = item->spak(item, resp, 0);

				// error?
				tb_assert_and_check_break(ok >= 0);

				// no aice? continue it
				tb_check_break(!ok);
			}
		}
	}

	// no aice? wait it
	tb_check_return_val(!ok, ok);

	// wait
	return tb_event_wait(ptor->wait, timeout);
}
static tb_void_t tb_aicp_proactor_unix_kill(tb_aicp_proactor_t* proactor)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return(ptor && ptor->base.aicp && ptor->wait);

	// the worker size
	tb_size_t work = tb_atomic_get(&ptor->base.aicp->work);

	// trace
	tb_trace_impl("kill: %lu", work);

	// kill all proactors
	tb_size_t i = 0;
	tb_size_t n = ptor->ptor_size;
	for (; i < n; i++) 
	{
		tb_aicp_proactor_t* item = ptor->ptor_list[i];
		if (item && item->kill) item->kill(item);
	}

	// post wait
	while (work--) tb_event_post(ptor->wait);
}
static tb_void_t tb_aicp_proactor_unix_exit(tb_aicp_proactor_t* proactor)
{
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	if (ptor)
	{
		// trace
		tb_trace_impl("exit");

		// exit all proactors
		tb_size_t i = 0;
		tb_size_t n = ptor->ptor_size;
		for (; i < n; i++) 
		{
			tb_aicp_proactor_t* item = ptor->ptor_list[i];
			if (item) item->exit(item);
		}

		// exit resp
		if (ptor->mutx) tb_mutex_enter(ptor->mutx);
		if (ptor->resp) tb_queue_exit(ptor->resp);
		ptor->resp = tb_null;
		if (ptor->mutx) tb_mutex_leave(ptor->mutx);

		// exit mutx
		if (ptor->mutx) tb_mutex_exit(ptor->mutx);
		ptor->mutx = tb_null;

		// exit wait
		if (ptor->wait) tb_event_exit(ptor->wait);
		ptor->wait = tb_null;

		// free it
		tb_free(ptor);
	}
}
static tb_void_t tb_aicp_proactor_unix_resp(tb_aicp_proactor_t* proactor, tb_aice_t const* aice)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return(ptor && ptor->resp && ptor->wait);

	// has aice?
	if (aice)
	{
		// enter 
		if (ptor->mutx) tb_mutex_enter(ptor->mutx);

		// post aice
		if (!tb_queue_full(ptor->resp)) 
		{
			// put
			tb_queue_put(ptor->resp, aice);

			// trace
			tb_trace_impl("resp: code: %lu, size: %lu", aice->code, tb_queue_size(ptor->resp));

			// post it
			tb_event_post(ptor->wait);
		}
		else
		{
			// assert
			tb_assert(0);
		}

		// leave 
		if (ptor->mutx) tb_mutex_leave(ptor->mutx);
	}
	// only post wait
	else tb_event_post(ptor->wait);
}

/* ///////////////////////////////////////////////////////////////////////
 * proactors
 */
#if 0//def TB_CONFIG_ASIO_HAVE_EPOLL
# 	include "aicp/epoll.c"
#endif

#ifdef TB_CONFIG_ASIO_HAVE_AIO
# 	include "aicp/aio.c"
#endif

#include "aicp/aiop.c"
#include "aicp/file.c"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp);
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->maxn, tb_null);

	// alloc proactor
	tb_aicp_proactor_unix_t* ptor = tb_malloc0(sizeof(tb_aicp_proactor_unix_t));
	tb_assert_and_check_return_val(ptor, tb_null);

	// init base
	ptor->base.aicp = aicp;
	ptor->base.kill = tb_aicp_proactor_unix_kill;
	ptor->base.exit = tb_aicp_proactor_unix_exit;
	ptor->base.addo = tb_aicp_proactor_unix_addo;
	ptor->base.delo = tb_aicp_proactor_unix_delo;
	ptor->base.post = tb_aicp_proactor_unix_post;
	ptor->base.spak = tb_aicp_proactor_unix_spak;
	
	// init mutx
	ptor->mutx = tb_mutex_init(tb_null);
	tb_assert_and_check_goto(ptor->mutx, fail);

	// init resp
	ptor->resp = tb_queue_init((aicp->maxn << 2) + 16, tb_item_func_ifm(sizeof(tb_aice_t), tb_null, tb_null));
	tb_assert_and_check_goto(ptor->resp, fail);

	// init wait
	ptor->wait = tb_event_init(tb_null);
	tb_assert_and_check_goto(ptor->wait, fail);

	// init epoll proactor
#if 0//def TB_CONFIG_ASIO_HAVE_EPOLL
	tb_aicp_proactor_epoll_init(ptor);
#endif

	// init kqueue proactor
#if 0//defined(TB_CONFIG_ASIO_HAVE_KQUEUE)
	tb_aicp_proactor_kqueue_init(ptor);
#endif

	// init aiop proactor
	tb_aicp_proactor_aiop_init(ptor);

	// init file proactor
	tb_aicp_proactor_file_init(ptor);

	// init aio proactor
#ifdef TB_CONFIG_ASIO_HAVE_AIO
	tb_aicp_proactor_aio_init(ptor);
#endif

	// check 
	tb_assert_and_check_goto(ptor->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_ACPT], fail);
	tb_assert_and_check_goto(ptor->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_CONN], fail);
	tb_assert_and_check_goto(ptor->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_RECV], fail);
	tb_assert_and_check_goto(ptor->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_SEND], fail);
	tb_assert_and_check_goto(ptor->ptor_post[TB_AICO_TYPE_FILE][TB_AICE_CODE_READ], fail);
	tb_assert_and_check_goto(ptor->ptor_post[TB_AICO_TYPE_FILE][TB_AICE_CODE_WRIT], fail);

	// ok
	return (tb_aicp_proactor_t*)ptor;

fail:
	if (ptor) tb_aicp_proactor_unix_exit(ptor);
	return tb_null;
}

