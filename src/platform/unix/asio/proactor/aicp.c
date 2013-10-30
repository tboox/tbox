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
 *
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
 * types
 */

// the aiop proactor type
typedef struct __tb_aicp_proactor_aiop_t
{
	// the proactor base
	tb_aicp_proactor_t 		base;

	// the aiop
	tb_handle_t 			aiop;

}tb_aicp_proactor_aiop_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
#if 0
static tb_bool_t tb_aicp_proactor_aiop_addo(tb_aicp_proactor_t* proactor, tb_handle_t handle, tb_size_t type)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->aiop && proactor->aicp, tb_false);

	// addo
	return tb_aiop_addo(ptor->aiop, aico->aioo.handle, aico->aioo.type, aico);
}
static tb_bool_t tb_aicp_proactor_aiop_delo(tb_aicp_proactor_t* proactor, tb_handle_t handle)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->aiop && proactor->aicp, tb_false);

	// delo
	return tb_aiop_delo(ptor->aiop, aico->aioo.handle);
}
static tb_long_t tb_aicp_proactor_aiop_spak(tb_aicp_proactor_t* proactor, tb_aice_t const* post, tb_aice_t const** resp)
{
	// check
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	tb_assert_and_check_return_val(ptor && ptor->aiop && proactor->aicp, -1);


	// ok
	return 0;
}

static tb_void_t tb_aicp_proactor_aiop_exit(tb_aicp_proactor_t* proactor)
{
	tb_aicp_proactor_aiop_t* ptor = (tb_aicp_proactor_aiop_t*)proactor;
	if (ptor)
	{
		// exit aiop
		if (ptor->aiop) tb_aiop_exit(ptor->aiop);

		// free it
		tb_free(ptor);
	}
}
#endif

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp);
tb_aicp_proactor_t* tb_aicp_proactor_init(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->maxn, tb_null);

#if 0
	// alloc proactor
	tb_aicp_proactor_aiop_t* ptor = tb_malloc0(sizeof(tb_aicp_proactor_aiop_t));
	tb_assert_and_check_return_val(ptor, tb_null);

	// init base
	ptor->base.aicp = aicp;
//	ptor->base.exit = tb_aicp_proactor_aiop_exit;
//	ptor->base.addo = tb_aicp_proactor_aiop_addo;
//	ptor->base.delo = tb_aicp_proactor_aiop_delo;
//	ptor->base.spak = tb_aicp_proactor_aiop_spak;

	// init aiop
	ptor->aiop = tb_aiop_init(aicp->type, aicp->maxn);
	tb_assert_and_check_goto(ptor->aiop, fail);

	// ok
	return (tb_aicp_proactor_t*)ptor;

fail:
	if (ptor) tb_aicp_proactor_aiop_exit(ptor);
	return tb_null;
#else
	tb_trace_noimpl();
	return tb_null;
#endif
}

