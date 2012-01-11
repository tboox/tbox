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
 * \file		eiop.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "eiop.h"
#include "eio.h"

/* /////////////////////////////////////////////////////////
 * declaration
 */
tb_eiop_reactor_t* tb_eiop_reactor_file_init(tb_eiop_t*);
tb_eiop_reactor_t* tb_eiop_reactor_sock_init(tb_eiop_t*);

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_eiop_t* tb_eiop_init(tb_size_t type, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, TB_NULL);

	// alloc eiop
	tb_eiop_t* eiop = tb_calloc(1, sizeof(tb_eiop_t));
	tb_assert_and_check_return_val(eiop, TB_NULL);

	// init eiop
	eiop->type = type;
	eiop->maxn = maxn;

	// reactors
	static tb_eiop_reactor_t* (*s_init[])(tb_eiop_t*) = 
	{
		TB_NULL
	, 	TB_NULL 	//!< for qbuffer
	, 	tb_eiop_reactor_file_init
	,	tb_eiop_reactor_sock_init
	,	TB_NULL 	//!< for http
	,	TB_NULL 	//!< for gstream
	
	};
	tb_assert_and_check_goto(type < tb_arrayn(s_init), fail);

	// init reactor
	if (s_init[type]) eiop->reactor = s_init[type](eiop);
	tb_assert_and_check_goto(eiop->reactor, fail);

	// ok
	return eiop;

fail:
	if (eiop) tb_eiop_exit(eiop);
	return TB_NULL;
}

tb_void_t tb_eiop_exit(tb_eiop_t* eiop)
{
	if (eiop)
	{
		// exit reactor
		tb_assert(eiop->reactor && eiop->reactor->exit);
		eiop->reactor->exit(eiop->reactor);

		// free objects 
		if (eiop->objs) tb_free(eiop->objs);

		// free eiop
		tb_free(eiop);
	}
}
tb_size_t tb_eiop_maxn(tb_eiop_t* eiop)
{
	tb_assert_and_check_return_val(eiop, 0);
	return eiop->maxn;
}
tb_size_t tb_eiop_size(tb_eiop_t* eiop)
{
	tb_assert_and_check_return_val(eiop, 0);
	return eiop->size;
}
tb_size_t tb_eiop_addo(tb_eiop_t* eiop, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return_val(eiop && eiop->reactor && eiop->reactor->addo && eiop->size < eiop->maxn, 0);
	tb_assert_and_check_return_val(handle && etype, 0);

	// addo
	if (!eiop->reactor->addo(eiop->reactor, handle, etype)) return 0;

	// ok
	return ++eiop->size;
}
tb_size_t tb_eiop_seto(tb_eiop_t* eiop, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return_val(eiop && eiop->reactor && eiop->reactor->seto, 0);
	tb_assert_and_check_return_val(handle && etype, 0);

	// seto
	if (!eiop->reactor->seto(eiop->reactor, handle, etype)) return 0;

	// ok
	return eiop->size;
}
tb_size_t tb_eiop_delo(tb_eiop_t* eiop, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(eiop && eiop->reactor && eiop->reactor->delo && eiop->size, 0);
	tb_assert_and_check_return_val(handle, 0);

	// delo
	if (!eiop->reactor->delo(eiop->reactor, handle)) return 0;

	// ok
	return --eiop->size;
}
tb_long_t tb_eiop_wait(tb_eiop_t* eiop, tb_long_t timeout)
{	
	// check
	tb_assert_and_check_return_val(eiop && eiop->reactor && eiop->reactor->wait && eiop->reactor->sync, -1);

	// wait 
	tb_long_t evtn = eiop->reactor->wait(eiop->reactor, timeout);
	tb_assert_and_check_return_val(evtn >= 0, -1);
	
	// timeout?
	tb_check_return_val(evtn, 0);

	// init grow
	tb_size_t grow = tb_align8((eiop->maxn >> 3) + 1);

	// init objs
	if (!eiop->objs)
	{
		eiop->objn = evtn + grow;
		eiop->objs = tb_calloc(eiop->objn, sizeof(tb_eio_t));
		tb_assert_and_check_return_val(eiop->objs, -1);
	}
	// grow objs if not enough
	else if (evtn > eiop->objn)
	{
		// grow size
		eiop->objn = evtn + grow;
		if (eiop->objn > eiop->maxn) eiop->objn = eiop->maxn;

		// grow data
		eiop->objs = tb_realloc(eiop->objs, eiop->objn * sizeof(tb_eio_t));
		tb_assert_and_check_return_val(eiop->objs, -1);
	}
	tb_assert(evtn <= eiop->objn);

	// sync events to objects 
	eiop->reactor->sync(eiop->reactor, evtn);
	
	// ok
	return evtn;
}

tb_eio_t* tb_eiop_objs(tb_eiop_t* eiop)
{
	tb_assert_and_check_return_val(eiop, TB_NULL);
	return eiop->objs;
}
