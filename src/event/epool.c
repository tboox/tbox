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
 * implemention
 */
tb_epool_t* tb_epool_init(tb_size_t maxc)
{
	// check
	tb_assert_and_check_return_val(maxc, TB_NULL);

	// alloc pool
	tb_epool_t* pool = tb_calloc(1, sizeof(tb_epool_t));
	tb_assert_and_check_return_val(pool, TB_NULL);

	// init pool
	pool->maxc = maxc;

	// init the objects hash
	pool->objs = tb_hash_init(TB_HASH_SIZE_DEFAULT, tb_item_func_ptr(), tb_item_func_ifm(sizeof(tb_eobject_t), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(pool->hash, fail);

	// ok
	return pool;

fail:
	if (pool) tb_epool_exit(pool);
	return TB_NULL;
}
tb_void_t tb_epool_exit(tb_epool_t* pool)
{
}
tb_size_t tb_epool_addo(tb_epool_t* pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype)
{
	return 0;
}
tb_size_t tb_epool_delo(tb_epool_t* pool, tb_handle_t handle)
{
	return 0;
}
tb_size_t tb_epool_sete(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype)
{
	return 0;
}
tb_size_t tb_epool_adde(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype)
{
	return 0;
}
tb_size_t tb_epool_dele(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype)
{
	return 0;
}
tb_size_t tb_epool_wait(tb_epool_t* pool, tb_eobject_t* objs, tb_size_t maxn, tb_long_t timeout)
{
	return 0;
}
