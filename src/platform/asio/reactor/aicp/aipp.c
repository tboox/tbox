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
 * @file		aipp.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the aipp reactor type
typedef struct __tb_aicp_reactor_aipp_t
{
	// the reactor base
	tb_aicp_reactor_t 		base;

	// the aipp
	tb_handle_t 			aipp;

}tb_aicp_reactor_aipp_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
#if 0
static tb_bool_t tb_aicp_reactor_aipp_addo(tb_aicp_reactor_t* reactor, tb_handle_t handle, tb_size_t otype)
{
	// check
	tb_aicp_reactor_aipp_t* rtor = (tb_aicp_reactor_aipp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->aipp && reactor->aicp, tb_false);

	// addo
	return tb_aipp_addo(rtor->aipp, aico->aioo.handle, aico->aioo.otype, aico);
}
static tb_bool_t tb_aicp_reactor_aipp_delo(tb_aicp_reactor_t* reactor, tb_handle_t handle)
{
	// check
	tb_aicp_reactor_aipp_t* rtor = (tb_aicp_reactor_aipp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->aipp && reactor->aicp, tb_false);

	// delo
	return tb_aipp_delo(rtor->aipp, aico->aioo.handle);
}
static tb_long_t tb_aicp_reactor_aipp_spak(tb_aicp_reactor_t* reactor, tb_aice_t const* post, tb_aice_t const** resp)
{
	// check
	tb_aicp_reactor_aipp_t* rtor = (tb_aicp_reactor_aipp_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->aipp && reactor->aicp, -1);


	// ok
	return 0;
}

static tb_void_t tb_aicp_reactor_aipp_exit(tb_aicp_reactor_t* reactor)
{
	tb_aicp_reactor_aipp_t* rtor = (tb_aicp_reactor_aipp_t*)reactor;
	if (rtor)
	{
		// exit aipp
		if (rtor->aipp) tb_aipp_exit(rtor->aipp);

		// free it
		tb_free(rtor);
	}
}
#endif
static tb_aicp_reactor_t* tb_aicp_reactor_aipp_init(tb_aicp_t* aicp)
{
	// check
	tb_assert_and_check_return_val(aicp && aicp->maxn, tb_null);

#if 0
	// alloc reactor
	tb_aicp_reactor_aipp_t* rtor = tb_malloc0(sizeof(tb_aicp_reactor_aipp_t));
	tb_assert_and_check_return_val(rtor, tb_null);

	// init base
	rtor->base.aicp = aicp;
//	rtor->base.exit = tb_aicp_reactor_aipp_exit;
//	rtor->base.addo = tb_aicp_reactor_aipp_addo;
//	rtor->base.delo = tb_aicp_reactor_aipp_delo;
//	rtor->base.spak = tb_aicp_reactor_aipp_spak;

	// init aipp
	rtor->aipp = tb_aipp_init(aicp->type, aicp->maxn);
	tb_assert_and_check_goto(rtor->aipp, fail);

	// ok
	return (tb_aicp_reactor_t*)rtor;

fail:
	if (rtor) tb_aicp_reactor_aipp_exit(rtor);
	return tb_null;
#else
	tb_trace_noimpl();
	return tb_null;
#endif
}

