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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		aiop.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "aiop.h"
#include "aioo.h"

/* /////////////////////////////////////////////////////////
 * declaration
 */
tb_aiop_reactor_t* tb_aiop_reactor_file_init(tb_aiop_t*);
tb_aiop_reactor_t* tb_aiop_reactor_sock_init(tb_aiop_t*);

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_aiop_t* tb_aiop_init(tb_size_t type, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, TB_NULL);

	// alloc aiop
	tb_aiop_t* aiop = tb_calloc(1, sizeof(tb_aiop_t));
	tb_assert_and_check_return_val(aiop, TB_NULL);

	// init aiop
	aiop->type = type;
	aiop->maxn = maxn;

	// reactors
	static tb_aiop_reactor_t* (*s_init[])(tb_aiop_t*) = 
	{
		TB_NULL
	, 	TB_NULL 	//!< for qbuffer
	, 	tb_aiop_reactor_file_init
	,	tb_aiop_reactor_sock_init
	,	TB_NULL 	//!< for http
	,	TB_NULL 	//!< for gstream
	
	};
	tb_assert_and_check_goto(type < tb_arrayn(s_init), fail);

	// init reactor
	if (s_init[type]) aiop->rtor = s_init[type](aiop);
	tb_assert_and_check_goto(aiop->rtor, fail);

	// ok
	return aiop;

fail:
	if (aiop) tb_aiop_exit(aiop);
	return TB_NULL;
}

tb_void_t tb_aiop_exit(tb_aiop_t* aiop)
{
	if (aiop)
	{
		// exit reactor
		tb_assert(aiop->rtor && aiop->rtor->exit);
		aiop->rtor->exit(aiop->rtor);

		// free aiop
		tb_free(aiop);
	}
}
tb_size_t tb_aiop_maxn(tb_aiop_t* aiop)
{
	tb_assert_and_check_return_val(aiop, 0);
	return aiop->maxn;
}
tb_size_t tb_aiop_size(tb_aiop_t* aiop)
{
	tb_assert_and_check_return_val(aiop, 0);
	return aiop->size;
}
tb_size_t tb_aiop_addo(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->addo && aiop->size < aiop->maxn, 0);
	tb_assert_and_check_return_val(handle && etype, 0);

	// addo
	if (!aiop->rtor->addo(aiop->rtor, handle, etype)) return 0;

	// ok
	return ++aiop->size;
}
tb_size_t tb_aiop_seto(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->seto, 0);
	tb_assert_and_check_return_val(handle && etype, 0);

	// seto
	if (!aiop->rtor->seto(aiop->rtor, handle, etype)) return 0;

	// ok
	return aiop->size;
}
tb_size_t tb_aiop_delo(tb_aiop_t* aiop, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->delo && aiop->size, 0);
	tb_assert_and_check_return_val(handle, 0);

	// delo
	if (!aiop->rtor->delo(aiop->rtor, handle)) return 0;

	// ok
	return --aiop->size;
}
tb_long_t tb_aiop_wait(tb_aiop_t* aiop, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout)
{	
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->wait, -1);

	// wait 
	return aiop->rtor->wait(aiop->rtor, objs, objm, timeout);
}

