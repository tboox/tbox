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
	if (s_init[type]) aiop->reactor = s_init[type](aiop);
	tb_assert_and_check_goto(aiop->reactor, fail);

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
		tb_assert(aiop->reactor && aiop->reactor->exit);
		aiop->reactor->exit(aiop->reactor);

		// free objects 
		if (aiop->objs) tb_free(aiop->objs);

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
	tb_assert_and_check_return_val(aiop && aiop->reactor && aiop->reactor->addo && aiop->size < aiop->maxn, 0);
	tb_assert_and_check_return_val(handle && etype, 0);

	// addo
	if (!aiop->reactor->addo(aiop->reactor, handle, etype)) return 0;

	// ok
	return ++aiop->size;
}
tb_size_t tb_aiop_seto(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->reactor && aiop->reactor->seto, 0);
	tb_assert_and_check_return_val(handle && etype, 0);

	// seto
	if (!aiop->reactor->seto(aiop->reactor, handle, etype)) return 0;

	// ok
	return aiop->size;
}
tb_size_t tb_aiop_delo(tb_aiop_t* aiop, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->reactor && aiop->reactor->delo && aiop->size, 0);
	tb_assert_and_check_return_val(handle, 0);

	// delo
	if (!aiop->reactor->delo(aiop->reactor, handle)) return 0;

	// ok
	return --aiop->size;
}
tb_long_t tb_aiop_wait(tb_aiop_t* aiop, tb_long_t timeout)
{	
	// check
	tb_assert_and_check_return_val(aiop && aiop->reactor && aiop->reactor->wait && aiop->reactor->sync, -1);

	// wait 
	tb_long_t evtn = aiop->reactor->wait(aiop->reactor, timeout);
	tb_assert_and_check_return_val(evtn >= 0, -1);
	
	// timeout?
	tb_check_return_val(evtn, 0);

	// init grow
	tb_size_t grow = tb_align8((aiop->maxn >> 3) + 1);

	// init objs
	if (!aiop->objs)
	{
		aiop->objn = evtn + grow;
		aiop->objs = tb_calloc(aiop->objn, sizeof(tb_aioo_t));
		tb_assert_and_check_return_val(aiop->objs, -1);
	}
	// grow objs if not enough
	else if (evtn > aiop->objn)
	{
		// grow size
		aiop->objn = evtn + grow;
		if (aiop->objn > aiop->maxn) aiop->objn = aiop->maxn;

		// grow data
		aiop->objs = tb_realloc(aiop->objs, aiop->objn * sizeof(tb_aioo_t));
		tb_assert_and_check_return_val(aiop->objs, -1);
	}
	tb_assert(evtn <= aiop->objn);

	// sync events to objects 
	aiop->reactor->sync(aiop->reactor, evtn);
	
	// ok
	return evtn;
}

tb_aioo_t* tb_aiop_objs(tb_aiop_t* aiop)
{
	tb_assert_and_check_return_val(aiop, TB_NULL);
	return aiop->objs;
}
