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
//#define TB_TRACE_IMPL_TAG 			"proactor"

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

// the unix proactor type
typedef struct __tb_aicp_proactor_unix_t
{
	// the proactor base
	tb_aicp_proactor_t 				base;

	// the spak wait
	tb_handle_t 					wait;

	// the proactor post index + 1 for the proactor list
	tb_size_t 						ptor_post[TB_AICO_TYPE_MAXN][TB_AICE_CODE_MAXN];

	// the proactor list
	tb_size_t 						ptor_size;
	tb_aicp_proactor_t* 			ptor_list[TB_AICP_PROACTOR_UNIX_MAXN];

	// the step list for the proactor aico
	tb_size_t						step_list[TB_AICP_PROACTOR_UNIX_MAXN];

}tb_aicp_proactor_unix_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_pointer_t tb_aicp_proactor_unix_getp(tb_aicp_proactor_t* proactor, tb_size_t indx, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return_val(ptor && aico && indx < ptor->ptor_size, tb_null);

	return (tb_pointer_t)(((tb_byte_t*)aico) + ptor->step_list[indx]);
}
static tb_bool_t tb_aicp_proactor_unix_need(tb_aicp_proactor_t* proactor, tb_size_t type, tb_size_t indx)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return_val(ptor && indx < ptor->ptor_size, tb_false);

	// indx++
	indx++;

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
static tb_bool_t tb_aicp_proactor_unix_addo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return_val(ptor && aico && aico->handle, tb_false);

	// walk proactors
	tb_size_t i = 0;
	tb_size_t n = ptor->ptor_size;
	tb_size_t addo = 0;
	for (; i < n; i++) 
	{
		// need this?
		if (tb_aicp_proactor_unix_need(proactor, aico->type, i))
		{
			// the item
			tb_aicp_proactor_t* item = ptor->ptor_list[i];
			tb_assert_and_check_break(item && item->addo);

			// addo aico
			if (item->addo(item, aico)) addo++;
		}
	}

	// ok?
	return addo? tb_true : tb_false;
}
static tb_bool_t tb_aicp_proactor_unix_delo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return_val(ptor && aico && aico->handle, tb_false);

	// walk proactors
	tb_size_t i = 0;
	tb_size_t n = ptor->ptor_size;
	tb_size_t delo = 0;
	for (; i < n; i++) 
	{
		// need this?
		if (tb_aicp_proactor_unix_need(proactor, aico->type, i))
		{
			// the item
			tb_aicp_proactor_t* item = ptor->ptor_list[i];
			tb_assert_and_check_break(item && item->delo);

			// delo it
			if (item->delo(item, aico)) delo++;
		}
	}

	// ok?
	return delo? tb_true : tb_false;
}
static tb_bool_t tb_aicp_proactor_unix_post(tb_aicp_proactor_t* proactor, tb_aice_t const* list, tb_size_t size)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return_val(ptor && list && size && size <= TB_AICP_POST_MAXN, tb_false);
	
	// only one aice?
	if (size == 1)
	{
		// the aico
		tb_aico_t const* aico = list->aico;
		tb_assert_and_check_return_val(aico->type < TB_AICO_TYPE_MAXN, tb_false);
		tb_assert_and_check_return_val(list->code < TB_AICE_CODE_MAXN, tb_false);

		// the proactor index
		tb_size_t indx = ptor->ptor_post[aico->type][list->code];
		tb_assert_and_check_return_val(indx && indx < ptor->ptor_size + 1, tb_false);

		// the proactor item
		tb_aicp_proactor_t* item = ptor->ptor_list[indx - 1];
		tb_assert_and_check_return_val(item, tb_false);

		// post it
		return item->post(item, list, 1);
	}
	else
	{
		// init aice list
		tb_size_t i = 0;
		tb_size_t aice_size[TB_AICP_PROACTOR_UNIX_MAXN] = {0};
		tb_aice_t aice_list[TB_AICP_PROACTOR_UNIX_MAXN][TB_AICP_POST_MAXN] = {0};
		for (i = 0; i < size; i++)
		{
			// the aice
			tb_aice_t const* aice = &list[i];
			tb_assert_and_check_break(aice->code < TB_AICE_CODE_MAXN);

			// the aico
			tb_aico_t const* aico = aice->aico;
			tb_assert_and_check_break(aico->type < TB_AICO_TYPE_MAXN);

			// the proactor index
			tb_size_t indx = ptor->ptor_post[aico->type][aice->code];
			tb_assert_and_check_break(indx && indx < ptor->ptor_size + 1);
			indx--;

			// append aice to the proactor
			aice_list[indx][aice_size[indx]++] = *aice;

			// full?
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
}
static tb_long_t tb_aicp_proactor_unix_spak(tb_aicp_proactor_t* proactor, tb_aice_t* resp, tb_long_t timeout)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->wait && resp, -1);

	// trace
//	tb_trace_impl("spak[%u]: ..", (tb_uint16_t)tb_thread_self());

	// spak proactors
	tb_size_t i = 0;
	tb_size_t n = ptor->ptor_size;
	tb_long_t ok = 0;
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

	// no aice? wait it
	tb_check_return_val(!ok, ok);

	// wait
	if (tb_semaphore_wait(ptor->wait, timeout) < 0) return -1;

	// continue 
	return 0;
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
	if (work) tb_semaphore_post(ptor->wait, work);
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

		// exit wait
		if (ptor->wait) tb_semaphore_exit(ptor->wait);
		ptor->wait = tb_null;

		// free it
		tb_free(ptor);
	}
}
static tb_void_t tb_aicp_proactor_unix_work(tb_aicp_proactor_t* proactor)
{
	// check
	tb_aicp_proactor_unix_t* ptor = (tb_aicp_proactor_unix_t*)proactor;
	tb_assert_and_check_return(ptor && ptor->wait && ptor->base.aicp);

	// the worker size
	tb_size_t work = tb_atomic_get(&ptor->base.aicp->work);

	// the semaphore value
	tb_long_t value = tb_semaphore_value(ptor->wait);

	// post wait
	if (value >= 0 && value < work) tb_semaphore_post(ptor->wait, work - value);
}


/* ///////////////////////////////////////////////////////////////////////
 * proactors
 */
#if 0//def TB_CONFIG_ASIO_POLL_HAVE_EPOLL
# 	include "aicp/epoll.c"
#endif

#ifdef TB_CONFIG_ASIO_POLL_HAVE_AIO
//# 	include "aicp/aio.c"
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

	// init wait
	ptor->wait = tb_semaphore_init(0);
	tb_assert_and_check_goto(ptor->wait, fail);

	// init epoll proactor
#if 0//def TB_CONFIG_ASIO_POLL_HAVE_EPOLL
	tb_aicp_proactor_epoll_init(ptor);
#endif

	// init aiop proactor
	tb_aicp_proactor_aiop_init(ptor);

	// init file proactor
	tb_aicp_proactor_file_init(ptor);

	// init aio proactor
#ifdef TB_CONFIG_ASIO_POLL_HAVE_AIO
//	tb_aicp_proactor_aio_init(ptor);
#endif

	// check 
	tb_assert_and_check_goto(ptor->ptor_size, fail);
	tb_assert_and_check_goto(ptor->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_ACPT], fail);
	tb_assert_and_check_goto(ptor->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_CONN], fail);
	tb_assert_and_check_goto(ptor->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_RECV], fail);
	tb_assert_and_check_goto(ptor->ptor_post[TB_AICO_TYPE_SOCK][TB_AICE_CODE_SEND], fail);
	tb_assert_and_check_goto(ptor->ptor_post[TB_AICO_TYPE_FILE][TB_AICE_CODE_READ], fail);
	tb_assert_and_check_goto(ptor->ptor_post[TB_AICO_TYPE_FILE][TB_AICE_CODE_WRIT], fail);

	// init step
	ptor->base.step = sizeof(tb_aico_t);
	{
		// walk list
		tb_size_t i = 0;
		tb_size_t n = ptor->ptor_size;
		for (i = 0; i < n; i++)
		{
			// the item
			tb_aicp_proactor_t* item = ptor->ptor_list[i];
			tb_assert_and_check_goto(item, fail);

			// save the proactor aico step
			ptor->step_list[i] = ptor->base.step;

			// save the proactor aico size
			ptor->base.step += item->step;
		}
	}

	// ok
	return (tb_aicp_proactor_t*)ptor;

fail:
	if (ptor) tb_aicp_proactor_unix_exit(ptor);
	return tb_null;
}

