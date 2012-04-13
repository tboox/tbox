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
 * \author		ruki
 * \file		aipp.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aipp.h"
#include "aioo.h"

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */

tb_aipp_reactor_t* tb_aipp_reactor_file_init(tb_aipp_t* aipp);
tb_aipp_reactor_t* tb_aipp_reactor_sock_init(tb_aipp_t* aipp);

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_aipp_t* tb_aipp_init(tb_size_t type, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, TB_NULL);

	// alloc aipp
	tb_aipp_t* aipp = tb_malloc0(sizeof(tb_aipp_t));
	tb_assert_and_check_return_val(aipp, TB_NULL);

	// init aipp
	aipp->type = type;
	aipp->maxn = maxn;

	// reactors
	static tb_aipp_reactor_t* (*s_init[])(tb_aipp_t*) = 
	{
		TB_NULL
	, 	tb_aipp_reactor_file_init
	,	tb_aipp_reactor_sock_init
	
	};
	tb_assert_and_check_goto(type < tb_arrayn(s_init), fail);

	// init hash
	aipp->hash = tb_hash_init(tb_align8(tb_int32_sqrt(maxn) + 1), tb_item_func_ptr(), tb_item_func_ifm(sizeof(tb_aioo_t), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(aipp->hash, fail);

	// init reactor
	if (s_init[type]) aipp->rtor = s_init[type](aipp);
	tb_assert_and_check_goto(aipp->rtor, fail);

	// ok
	return aipp;

fail:
	if (aipp) tb_aipp_exit(aipp);
	return TB_NULL;
}

tb_void_t tb_aipp_exit(tb_aipp_t* aipp)
{
	if (aipp)
	{
		// exit reactor
		tb_assert(aipp->rtor && aipp->rtor->exit);
		aipp->rtor->exit(aipp->rtor);

		// exit hash
		if (aipp->hash) tb_hash_exit(aipp->hash);

		// free aipp
		tb_free(aipp);
	}
}
tb_size_t tb_aipp_maxn(tb_aipp_t* aipp)
{
	tb_assert_and_check_return_val(aipp, 0);
	return aipp->maxn;
}
tb_size_t tb_aipp_size(tb_aipp_t* aipp)
{
	tb_assert_and_check_return_val(aipp && aipp->hash, 0);
	return tb_hash_size(aipp->hash);
}
tb_bool_t tb_aipp_addo(tb_aipp_t* aipp, tb_handle_t handle, tb_size_t etype, tb_pointer_t odata)
{
	// check
	tb_assert_and_check_return_val(aipp && aipp->rtor && aipp->rtor->addo, TB_FALSE);
	tb_assert_and_check_return_val(aipp->hash && tb_hash_size(aipp->hash) < aipp->maxn, TB_FALSE);
	tb_assert_and_check_return_val(handle && etype, TB_FALSE);

	// add object to native
	if (!aipp->rtor->addo(aipp->rtor, handle, etype)) return TB_FALSE;

	// add object to hash
	tb_aioo_t o;
	tb_aioo_seto(&o, handle, aipp->type, etype, odata);
	tb_hash_set(aipp->hash, handle, &o);
	
	// ok
	return TB_TRUE;
}
tb_bool_t tb_aipp_delo(tb_aipp_t* aipp, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(aipp && aipp->rtor && aipp->rtor->delo, TB_FALSE);
	tb_assert_and_check_return_val(aipp->hash && tb_hash_size(aipp->hash), TB_FALSE);
	tb_assert_and_check_return_val(handle, TB_FALSE);

	// del object from native
	if (!aipp->rtor->delo(aipp->rtor, handle)) return TB_FALSE;

	// del object from hash
	tb_hash_del(aipp->hash, handle);
	
	// ok
	return TB_TRUE;
}
tb_size_t tb_aipp_gete(tb_aipp_t* aipp, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(aipp && aipp->rtor && aipp->rtor->seto, 0);
	tb_assert_and_check_return_val(aipp->hash && tb_hash_size(aipp->hash), 0);
	tb_assert_and_check_return_val(handle, 0);

	// get object from hash
	tb_aioo_t* o = tb_hash_get(aipp->hash, handle);
	tb_assert_and_check_return_val(o, 0);

	// get the event type
	return o->etype;
}
tb_void_t tb_aipp_sete(tb_aipp_t* aipp, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return(aipp && aipp->rtor && aipp->rtor->seto);
	tb_assert_and_check_return(aipp->hash && tb_hash_size(aipp->hash));
	tb_assert_and_check_return(handle && etype);

	// get object from hash
	tb_aioo_t* o = tb_hash_get(aipp->hash, handle);
	tb_assert_and_check_return(o);

	// no change?
	tb_check_return(etype != o->etype);

	// set object at native
	if (!aipp->rtor->seto(aipp->rtor, handle, etype, o)) return ;

	// update the event type
	o->etype = etype;
}
tb_void_t tb_aipp_adde(tb_aipp_t* aipp, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return(aipp && aipp->rtor && aipp->rtor->seto);
	tb_assert_and_check_return(aipp->hash && tb_hash_size(aipp->hash));
	tb_assert_and_check_return(handle && etype);

	// get object from hash
	tb_aioo_t* o = tb_hash_get(aipp->hash, handle);
	tb_assert_and_check_return(o);

	// no change?
	etype |= o->etype;

	// no change?
	tb_check_return(etype != o->etype);

	// set object at native
	if (!aipp->rtor->seto(aipp->rtor, handle, etype, o)) return ;

	// update the event type
	o->etype = etype;
}
tb_void_t tb_aipp_dele(tb_aipp_t* aipp, tb_handle_t handle, tb_size_t etype)
{
	// check
	tb_assert_and_check_return(aipp && aipp->rtor && aipp->rtor->seto);
	tb_assert_and_check_return(aipp->hash && tb_hash_size(aipp->hash));
	tb_assert_and_check_return(handle && etype);

	// get object from hash
	tb_aioo_t* o = tb_hash_get(aipp->hash, handle);
	tb_assert_and_check_return(o);

	// no change?
	etype = o->etype & ~etype;

	// no change?
	tb_check_return(etype != o->etype);

	// set object at native
	if (!aipp->rtor->seto(aipp->rtor, handle, etype, o)) return ;

	// update the event type
	o->etype = etype;
}
tb_void_t tb_aipp_setp(tb_aipp_t* aipp, tb_handle_t handle, tb_pointer_t odata)
{
	// check
	tb_assert_and_check_return(aipp && aipp->rtor && aipp->rtor->seto);
	tb_assert_and_check_return(aipp->hash && tb_hash_size(aipp->hash));
	tb_assert_and_check_return(handle);

	// get object from hash
	tb_aioo_t* o = tb_hash_get(aipp->hash, handle);
	tb_assert_and_check_return(o);

	// update the object data
	o->odata = odata;
}
tb_long_t tb_aipp_wait(tb_aipp_t* aipp, tb_aioo_t* aioo, tb_size_t maxn, tb_long_t timeout)
{	
	// check
	tb_assert_and_check_return_val(aipp && aipp->rtor && aipp->rtor->wait, -1);

	// wait 
	return aipp->rtor->wait(aipp->rtor, aioo, maxn, timeout);
}

