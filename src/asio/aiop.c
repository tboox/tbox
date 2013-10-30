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
 * @file		aiop.c
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aiop.h"
#include "aioo.h"
#include "../math/math.h"

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_aiop_reactor_t* tb_aiop_reactor_init(tb_aiop_t* aiop);

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_aiop_t* tb_aiop_init(tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, tb_null);

	// alloc aiop
	tb_aiop_t* aiop = tb_malloc0(sizeof(tb_aiop_t));
	tb_assert_and_check_return_val(aiop, tb_null);

	// init aiop
	aiop->maxn = maxn;

	// init hash
	aiop->hash = tb_hash_init(tb_align8(tb_isqrti(maxn) + 1), tb_item_func_ptr(tb_null, tb_null), tb_item_func_ifm(sizeof(tb_aioo_t), tb_null, tb_null));
	tb_assert_and_check_goto(aiop->hash, fail);

	// init reactor
	aiop->rtor = tb_aiop_reactor_init(aiop);
	tb_assert_and_check_goto(aiop->rtor, fail);

	// ok
	return aiop;

fail:
	if (aiop) tb_aiop_exit(aiop);
	return tb_null;
}

tb_void_t tb_aiop_exit(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return(aiop);

	// exit reactor
	if (aiop->rtor && aiop->rtor->exit)
		aiop->rtor->exit(aiop->rtor);

	// exit hash
	if (aiop->hash) tb_hash_exit(aiop->hash);

	// free aiop
	tb_free(aiop);
}
tb_void_t tb_aiop_cler(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return(aiop);

	// clear reactor
	if (aiop->rtor && aiop->rtor->cler)
		aiop->rtor->cler(aiop->rtor);

	// clear hash
	if (aiop->hash) tb_hash_clear(aiop->hash);
}
tb_bool_t tb_aiop_addo(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t aioe, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->addo, tb_false);
	tb_assert_and_check_return_val(aiop->hash && tb_hash_size(aiop->hash) < aiop->maxn, tb_false);
	tb_assert_and_check_return_val(handle && aioe, tb_false);

	// add object to native
	if (!aiop->rtor->addo(aiop->rtor, handle, aioe)) return tb_false;

	// add object to hash
	tb_aioo_t aioo;
	tb_aioo_seto(&aioo, handle, aioe, data);
	tb_hash_set(aiop->hash, handle, &aioo);

	// ok
	return tb_true;
}
tb_bool_t tb_aiop_delo(tb_aiop_t* aiop, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->delo, tb_false);
	tb_assert_and_check_return_val(aiop->hash && tb_hash_size(aiop->hash), tb_false);
	tb_assert_and_check_return_val(handle, tb_false);

	// del object from native
	if (!aiop->rtor->delo(aiop->rtor, handle)) return tb_false;

	// del object from hash
	tb_hash_del(aiop->hash, handle);
	
	// ok
	return tb_true;
}
tb_size_t tb_aiop_gete(tb_aiop_t* aiop, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->seto, 0);
	tb_assert_and_check_return_val(aiop->hash && tb_hash_size(aiop->hash), 0);
	tb_assert_and_check_return_val(handle, 0);

	// get object from hash
	tb_aioo_t* aioo = tb_hash_get(aiop->hash, handle);
	tb_assert_and_check_return_val(aioo, 0);

	// get the event type
	return aioo->aioe;
}
tb_void_t tb_aiop_sete(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t aioe)
{
	// check
	tb_assert_and_check_return(aiop && aiop->rtor && aiop->rtor->seto);
	tb_assert_and_check_return(aiop->hash && tb_hash_size(aiop->hash));
	tb_assert_and_check_return(handle && aioe);

	// get object from hash
	tb_aioo_t* aioo = tb_hash_get(aiop->hash, handle);
	tb_assert_and_check_return(aioo);

	// no change?
	tb_check_return(aioe != aioo->aioe);

	// set object at native
	if (!aiop->rtor->seto(aiop->rtor, handle, aioe, aioo)) return ;

	// update the event type
	aioo->aioe = aioe;
}
tb_void_t tb_aiop_adde(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t aioe)
{
	// check
	tb_assert_and_check_return(aiop && aiop->rtor && aiop->rtor->seto);
	tb_assert_and_check_return(aiop->hash && tb_hash_size(aiop->hash));
	tb_assert_and_check_return(handle && aioe);

	// get object from hash
	tb_aioo_t* aioo = tb_hash_get(aiop->hash, handle);
	tb_assert_and_check_return(aioo);

	// no change?
	aioe |= aioo->aioe;

	// no change?
	tb_check_return(aioe != aioo->aioe);

	// set object at native
	if (!aiop->rtor->seto(aiop->rtor, handle, aioe, aioo)) return ;

	// update the event type
	aioo->aioe = aioe;
}
tb_void_t tb_aiop_dele(tb_aiop_t* aiop, tb_handle_t handle, tb_size_t aioe)
{
	// check
	tb_assert_and_check_return(aiop && aiop->rtor && aiop->rtor->seto);
	tb_assert_and_check_return(aiop->hash && tb_hash_size(aiop->hash));
	tb_assert_and_check_return(handle && aioe);

	// get object from hash
	tb_aioo_t* aioo = tb_hash_get(aiop->hash, handle);
	tb_assert_and_check_return(aioo);

	// no change?
	aioe = aioo->aioe & ~aioe;

	// no change?
	tb_check_return(aioe != aioo->aioe);

	// set object at native
	if (!aiop->rtor->seto(aiop->rtor, handle, aioe, aioo)) return ;

	// update the event type
	aioo->aioe = aioe;
}
tb_void_t tb_aiop_setp(tb_aiop_t* aiop, tb_handle_t handle, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(aiop && aiop->rtor && aiop->rtor->seto);
	tb_assert_and_check_return(aiop->hash && tb_hash_size(aiop->hash));
	tb_assert_and_check_return(handle);

	// get object from hash
	tb_aioo_t* aioo = tb_hash_get(aiop->hash, handle);
	tb_assert_and_check_return(aioo);

	// update the object data
	aioo->data = data;
}
tb_pointer_t tb_aiop_getp(tb_aiop_t* aiop, tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->hash && tb_hash_size(aiop->hash) && handle, tb_null);

	// get object from hash
	tb_aioo_t* aioo = tb_hash_get(aiop->hash, handle);
	
	// the object data
	return aioo? aioo->data : tb_null;
}
tb_long_t tb_aiop_wait(tb_aiop_t* aiop, tb_aioo_t* aioo, tb_size_t maxn, tb_long_t timeout)
{	
	// check
	tb_assert_and_check_return_val(aiop && aiop->rtor && aiop->rtor->wait, -1);

	// wait 
	return aiop->rtor->wait(aiop->rtor, aioo, maxn, timeout);
}

