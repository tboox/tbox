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

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aiop.h"
#include "aioo.h"
#include "reactor/aiop.h"

/* ///////////////////////////////////////////////////////////////////////
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
	, 	tb_aiop_reactor_file_init
	,	tb_aiop_reactor_sock_init
	,	tb_aiop_reactor_http_init
	,	tb_aiop_reactor_gstm_init
	
	};
	tb_assert_and_check_goto(type < tb_arrayn(s_init), fail);

	// init hash
	aiop->hash = tb_hash_init(tb_align8(tb_int32_sqrt(maxn) + 1), tb_item_func_ptr(), tb_item_func_ifm(sizeof(tb_aioo_t), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(aiop->hash, fail);

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

		// exit hash
		if (aiop->hash) tb_hash_exit(aiop->hash);

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
	tb_assert_and_check_return_val(aiop && aiop->hash, 0);
	return tb_hash_size(aiop->hash);
}
tb_size_t tb_aiop_addo(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t etype, tb_pointer_t odata)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->addo, 0);
	tb_assert_and_check_return_val(aiop->hash && tb_hash_size(aiop->hash) < aiop->maxn, 0);
	tb_assert_and_check_return_val(handle && etype, 0);

	// add object to native
	if (!aiop->rtor->addo(aiop->rtor, handle, etype)) return 0;

	// add object to hash
	tb_aioo_t o;
	tb_aioo_seto(&o, handle, aiop->type, etype, odata);
	tb_hash_set(aiop->hash, handle, &o);
	
	// ok
	return tb_hash_size(aiop->hash);
}
tb_size_t tb_aiop_delo(tb_aiop_t* aiop, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->delo, 0);
	tb_assert_and_check_return_val(aiop->hash && tb_hash_size(aiop->hash), 0);
	tb_assert_and_check_return_val(handle, 0);

	// del object from native
	if (!aiop->rtor->delo(aiop->rtor, handle)) return 0;

	// del object from hash
	tb_hash_del(aiop->hash, handle);
	
	// ok
	return tb_hash_size(aiop->hash);
}
tb_size_t tb_aiop_gete(tb_aiop_t* aiop, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->seto, 0);
	tb_assert_and_check_return_val(aiop->hash && tb_hash_size(aiop->hash), 0);
	tb_assert_and_check_return_val(handle, 0);

	// get object from hash
	tb_aioo_t* o = tb_hash_get(aiop->hash, handle);
	tb_assert_and_check_return_val(o, 0);

	// get the event type
	return o->etype;
}
tb_void_t tb_aiop_sete(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return(aiop && aiop->rtor && aiop->rtor->seto);
	tb_assert_and_check_return(aiop->hash && tb_hash_size(aiop->hash));
	tb_assert_and_check_return(handle && etype);

	// get object from hash
	tb_aioo_t* o = tb_hash_get(aiop->hash, handle);
	tb_assert_and_check_return(o);

	// no change?
	tb_check_return(etype != o->etype);

	// set object at native
	if (!aiop->rtor->seto(aiop->rtor, handle, etype, o)) return ;

	// update the event type
	o->etype = etype;
}
tb_void_t tb_aiop_adde(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return(aiop && aiop->rtor && aiop->rtor->seto);
	tb_assert_and_check_return(aiop->hash && tb_hash_size(aiop->hash));
	tb_assert_and_check_return(handle && etype);

	// get object from hash
	tb_aioo_t* o = tb_hash_get(aiop->hash, handle);
	tb_assert_and_check_return(o);

	// no change?
	etype |= o->etype;

	// no change?
	tb_check_return(etype != o->etype);

	// set object at native
	if (!aiop->rtor->seto(aiop->rtor, handle, etype, o)) return ;

	// update the event type
	o->etype = etype;
}
tb_void_t tb_aiop_dele(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return(aiop && aiop->rtor && aiop->rtor->seto);
	tb_assert_and_check_return(aiop->hash && tb_hash_size(aiop->hash));
	tb_assert_and_check_return(handle && etype);

	// get object from hash
	tb_aioo_t* o = tb_hash_get(aiop->hash, handle);
	tb_assert_and_check_return(o);

	// no change?
	etype = o->etype & ~etype;

	// no change?
	tb_check_return(etype != o->etype);

	// set object at native
	if (!aiop->rtor->seto(aiop->rtor, handle, etype, o)) return ;

	// update the event type
	o->etype = etype;
}
tb_void_t tb_aiop_setp(tb_aiop_t* aiop, tb_handle_t handle, tb_pointer_t odata)
{
	// check
	tb_assert_and_check_return(aiop && aiop->rtor && aiop->rtor->seto);
	tb_assert_and_check_return(aiop->hash && tb_hash_size(aiop->hash));
	tb_assert_and_check_return(handle);

	// get object from hash
	tb_aioo_t* o = tb_hash_get(aiop->hash, handle);
	tb_assert_and_check_return(o);

	// update the object data
	o->odata = odata;
}
tb_long_t tb_aiop_wait(tb_aiop_t* aiop, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout)
{	
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->wait, -1);

	// wait 
	return aiop->rtor->wait(aiop->rtor, objs, objm, timeout);
}

