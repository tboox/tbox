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

/* /////////////////////////////////////////////////////////
 * decls
 */

tb_epool_t* 	tb_epool_init_impl(tb_size_t maxn);
tb_void_t 		tb_epool_exit_impl(tb_epool_t* pool);
tb_size_t 		tb_epool_addo_impl(tb_epool_t* pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype);
tb_size_t 		tb_epool_delo_impl(tb_epool_t* pool, tb_handle_t handle);
tb_size_t 		tb_epool_sete_impl(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype);
tb_size_t 		tb_epool_adde_impl(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype);
tb_size_t 		tb_epool_dele_impl(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype);

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_epool_t* tb_epool_init(tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, TB_NULL);

	// alloc pool
	tb_epool_t* pool = tb_epool_init_impl(maxn);
	tb_assert_and_check_return_val(pool, TB_NULL);

	// init pool
	pool->maxn = maxn;

	// init the objects hash
	pool->objs = tb_hash_init(tb_align8(tb_int32_sqrt(maxn)), tb_item_func_ptr(), tb_item_func_ifm(sizeof(tb_eobject_t), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(pool->objs, fail);

	// ok
	return pool;

fail:
	if (pool) tb_epool_exit(pool);
	return TB_NULL;
}
tb_void_t tb_epool_exit(tb_epool_t* pool)
{
	if (pool)
	{
		if (pool->objs) tb_hash_exit(pool->objs);
		tb_epool_exit_impl(pool);
	}
}
tb_size_t tb_epool_addo(tb_epool_t* pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype)
{
	tb_assert_and_check_return_val(pool && handle && otype, 0);

	// init object
	tb_eobject_t o;
	if (!tb_eobject_init(&o, otype, etype, handle)) return 0;

	// set object
	tb_hash_set(pool->objs, handle, &o);

	//ok
	return tb_hash_size(pool->objs);
}
tb_size_t tb_epool_delo(tb_epool_t* pool, tb_handle_t handle)
{
	tb_assert_and_check_return_val(pool && handle, 0);

	// del object
	tb_hash_del(pool->objs, handle);

	//ok
	return tb_hash_size(pool->objs);
}
tb_size_t tb_epool_sete(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype)
{
	tb_assert_and_check_return_val(pool && handle, 0);

	// get object
	tb_eobject_t* o = (tb_eobject_t*)tb_hash_at(pool->objs, handle);
	tb_assert_and_check_return_val(o, 0);

	// set etype
	o->etype = etype;

	//ok
	return o->etype;
}
tb_size_t tb_epool_adde(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype)
{
	tb_assert_and_check_return_val(pool && handle, 0);

	// get object
	tb_eobject_t* o = (tb_eobject_t*)tb_hash_at(pool->objs, handle);
	tb_assert_and_check_return_val(o, 0);

	// set etype
	o->etype |= etype;

	//ok
	return o->etype;
}
tb_size_t tb_epool_dele(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype)
{
	tb_assert_and_check_return_val(pool && handle, 0);

	// get object
	tb_eobject_t* o = (tb_eobject_t*)tb_hash_at(pool->objs, handle);
	tb_assert_and_check_return_val(o, 0);

	// set etype
	o->etype &= ~etype;

	//ok
	return o->etype;
}
tb_size_t tb_epool_wait(tb_epool_t* pool, tb_eobject_t* objs, tb_size_t maxn, tb_long_t timeout)
{
	return 0;
}
