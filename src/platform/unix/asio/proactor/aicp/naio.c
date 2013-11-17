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
 * @naio		naio.c
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#undef TB_TRACE_IMPL_TAG
//#define TB_TRACE_IMPL_TAG 				"naio"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the naio proactor type
typedef struct __tb_aicp_proactor_naio_t
{
	// the proactor base
	tb_aicp_proactor_t 			base;

	// the unix proactor
	tb_aicp_proactor_t* 		uptr;

	// the proactor indx
	tb_size_t 					indx;

	// the aice spak
	tb_queue_t* 				spak;
	
	// the spak mutx
	tb_handle_t 				mutx;

}tb_aicp_proactor_naio_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aicp_proactor_naio_addo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_naio_t* ptor = (tb_aicp_proactor_naio_t*)proactor;
	tb_assert_and_check_return_val(ptor && aico && aico->handle && aico->type == TB_AICO_TYPE_FILE, tb_false);
	
	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_proactor_naio_delo(tb_aicp_proactor_t* proactor, tb_aico_t* aico)
{
	// check
	tb_aicp_proactor_naio_t* ptor = (tb_aicp_proactor_naio_t*)proactor;
	tb_assert_and_check_return_val(ptor && aico && aico->handle && aico->type == TB_AICO_TYPE_FILE, tb_false);
	
	// ok
	return tb_true;
}
static tb_bool_t tb_aicp_proactor_naio_post(tb_aicp_proactor_t* proactor, tb_aice_t const* list, tb_size_t size)
{
	// check

	tb_aicp_proactor_naio_t* ptor = (tb_aicp_proactor_naio_t*)proactor;
	tb_assert_and_check_return_val(ptor && list && size, tb_false);

	// ok?
	return 1;
}
static tb_long_t tb_aicp_proactor_naio_spak(tb_aicp_proactor_t* proactor, tb_aice_t* resp, tb_long_t timeout)
{
	// check
	tb_aicp_proactor_naio_t* ptor = (tb_aicp_proactor_naio_t*)proactor;
	tb_assert_and_check_return_val(ptor && resp, -1);

	// ok?
	return 1;
}
static tb_void_t tb_aicp_proactor_naio_exit(tb_aicp_proactor_t* proactor)
{
	tb_aicp_proactor_naio_t* ptor = (tb_aicp_proactor_naio_t*)proactor;
	if (ptor)
	{
		// trace
		tb_trace_impl("exit");

		// exit spak
		if (ptor->mutx) tb_mutex_enter(ptor->mutx);
		if (ptor->spak) tb_queue_exit(ptor->spak);
		ptor->spak = tb_null;
		if (ptor->mutx) tb_mutex_leave(ptor->mutx);

		// exit mutx
		if (ptor->mutx) tb_mutex_exit(ptor->mutx);
		ptor->mutx = tb_null;

		// exit it
		tb_free(ptor);
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
static tb_void_t tb_aicp_proactor_naio_init(tb_aicp_proactor_unix_t* uptr)
{
	// check
	tb_assert_and_check_return(uptr && uptr->base.aicp && uptr->base.aicp->maxn);

	// need this proactor?
	tb_check_return( 	!uptr->ptor_post[TB_AICO_TYPE_FILE][TB_AICE_CODE_READ]
					|| 	!uptr->ptor_post[TB_AICO_TYPE_FILE][TB_AICE_CODE_WRIT]
					);

	// make proactor
	tb_aicp_proactor_naio_t* ptor = tb_malloc0(sizeof(tb_aicp_proactor_naio_t));
	tb_assert_and_check_return(ptor);

	// init base
	ptor->uptr = uptr;
	ptor->base.aicp = uptr->base.aicp;
	ptor->base.addo = tb_aicp_proactor_naio_addo;
	ptor->base.delo = tb_aicp_proactor_naio_delo;
	ptor->base.exit = tb_aicp_proactor_naio_exit;
	ptor->base.post = tb_aicp_proactor_naio_post;
	ptor->base.spak = tb_aicp_proactor_naio_spak;

	// init mutx
	ptor->mutx = tb_mutex_init();
	tb_assert_and_check_goto(ptor->mutx, fail);

	// init spak
	ptor->spak = tb_queue_init((ptor->base.aicp->maxn << 2) + 16, tb_item_func_ifm(sizeof(tb_aice_t), tb_null, tb_null));
	tb_assert_and_check_goto(ptor->spak, fail);

	// add this proactor to the unix proactor list
	ptor->indx = uptr->ptor_size++;
	uptr->ptor_list[ptor->indx] = (tb_aicp_proactor_t*)ptor;

	// attach index to some aice post
	if (!uptr->ptor_post[TB_AICO_TYPE_FILE][TB_AICE_CODE_READ]) uptr->ptor_post[TB_AICO_TYPE_FILE][TB_AICE_CODE_READ] = ptor->indx + 1;
	if (!uptr->ptor_post[TB_AICO_TYPE_FILE][TB_AICE_CODE_WRIT]) uptr->ptor_post[TB_AICO_TYPE_FILE][TB_AICE_CODE_WRIT] = ptor->indx + 1;

	// ok
	return ;

fail:
	if (ptor) tb_aicp_proactor_naio_exit(ptor);
}

