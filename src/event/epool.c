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
 * \file		epool.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "epool.h"
#include "eobject.h"

/* /////////////////////////////////////////////////////////
 * declaration
 */
tb_epool_reactor_t* tb_epool_reactor_file_init(tb_epool_t*);
tb_epool_reactor_t* tb_epool_reactor_sock_init(tb_epool_t*);
tb_epool_reactor_t* tb_epool_reactor_evet_init(tb_epool_t*);

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_epool_t* tb_epool_init(tb_size_t type, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, TB_NULL);

	// alloc epool
	tb_epool_t* epool = tb_calloc(1, sizeof(tb_epool_t));
	tb_assert_and_check_return_val(epool, TB_NULL);

	// init epool
	epool->type = type;
	epool->maxn = maxn;

	// reactors
	static tb_epool_reactor_t* (*s_init[])(tb_epool_t*) = 
	{
		TB_NULL
	, 	TB_NULL 	//!< for qbuffer
	, 	tb_epool_reactor_file_init
	,	tb_epool_reactor_sock_init
	,	TB_NULL 	//!< for http
	,	TB_NULL 	//!< for gstream
	,	tb_epool_reactor_evet_init
	
	};
	tb_assert_and_check_goto(type < tb_arrayn(s_init), fail);

	// init reactor
	if (s_init[type]) epool->reactor = s_init[type](epool);
	tb_assert_and_check_goto(epool->reactor, fail);

	// ok
	return epool;

fail:
	if (epool) tb_epool_exit(epool);
	return TB_NULL;
}

tb_void_t tb_epool_exit(tb_epool_t* epool)
{
	if (epool)
	{
		// exit reactor
		tb_assert(epool->reactor && epool->reactor->exit);
		epool->reactor->exit(epool->reactor);

		// free objects 
		if (epool->objs) tb_free(epool->objs);

		// free epool
		tb_free(epool);
	}
}
tb_size_t tb_epool_maxn(tb_epool_t* epool)
{
	tb_assert_and_check_return_val(epool, 0);
	return epool->maxn;
}
tb_size_t tb_epool_size(tb_epool_t* epool)
{
	tb_assert_and_check_return_val(epool, 0);
	return epool->size;
}
tb_size_t tb_epool_addo(tb_epool_t* epool, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return_val(epool && epool->reactor && epool->reactor->addo && epool->size < epool->maxn, 0);
	tb_assert_and_check_return_val(handle && etype, 0);

	// addo
	if (!epool->reactor->addo(epool->reactor, handle, etype)) return 0;

	// ok
	return ++epool->size;
}
tb_size_t tb_epool_seto(tb_epool_t* epool, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return_val(epool && epool->reactor && epool->reactor->seto, 0);
	tb_assert_and_check_return_val(handle && etype, 0);

	// seto
	if (!epool->reactor->seto(epool->reactor, handle, etype)) return 0;

	// ok
	return epool->size;
}
tb_size_t tb_epool_delo(tb_epool_t* epool, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(epool && epool->reactor && epool->reactor->delo && epool->size, 0);
	tb_assert_and_check_return_val(handle, 0);

	// delo
	if (!epool->reactor->delo(epool->reactor, handle)) return 0;

	// ok
	return --epool->size;
}
tb_long_t tb_epool_wait(tb_epool_t* epool, tb_long_t timeout)
{	
	// check
	tb_assert_and_check_return_val(epool && epool->reactor && epool->reactor->wait && epool->reactor->sync, -1);

	// wait 
	tb_long_t evtn = epool->reactor->wait(epool->reactor, timeout);
	tb_assert_and_check_return_val(evtn >= 0, -1);
	
	// timeout?
	tb_check_return_val(evtn, 0);

	// init grow
	tb_size_t grow = tb_align8((epool->maxn >> 3) + 1);

	// init objs
	if (!epool->objs)
	{
		epool->objn = evtn + grow;
		epool->objs = tb_calloc(epool->objn, sizeof(tb_eobject_t));
		tb_assert_and_check_return_val(epool->objs, -1);
	}
	// grow objs if not enough
	else if (evtn > epool->objn)
	{
		// grow size
		epool->objn = evtn + grow;
		if (epool->objn > epool->maxn) epool->objn = epool->maxn;

		// grow data
		epool->objs = tb_realloc(epool->objs, epool->objn * sizeof(tb_eobject_t));
		tb_assert_and_check_return_val(epool->objs, -1);
	}
	tb_assert(evtn <= epool->objn);

	// sync events to objects 
	epool->reactor->sync(epool->reactor, evtn);
	
	// ok
	return evtn;
}

tb_eobject_t* tb_epool_objs(tb_epool_t* epool)
{
	tb_assert_and_check_return_val(epool, TB_NULL);
	return epool->objs;
}
