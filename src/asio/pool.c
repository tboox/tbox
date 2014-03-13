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
 * @file		pool.c
 * @ingroup 	asio
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pool.h"
#include "aico.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_pointer_t tb_aico_pool_malloc_impl(tb_aico_t* aico, tb_size_t size __tb_debug_decl__)
{
	// check 
	tb_assert_and_check_return_val(aico, tb_null);

	// init pool
	if (!aico->pool) aico->pool = tb_spool_init(TB_SPOOL_GROW_MICRO, 0);
	tb_assert_and_check_return_val(aico->pool, tb_null);

	// malloc
	return tb_spool_malloc_impl(aico->pool, size __tb_debug_args__);
}
tb_pointer_t tb_aico_pool_malloc0_impl(tb_aico_t* aico, tb_size_t size __tb_debug_decl__)
{
	// check 
	tb_assert_and_check_return_val(aico, tb_null);

	// init pool
	if (!aico->pool) aico->pool = tb_spool_init(TB_SPOOL_GROW_MICRO, 0);
	tb_assert_and_check_return_val(aico->pool, tb_null);

	// malloc0
	return tb_spool_malloc0_impl(aico->pool, size __tb_debug_args__);
}
tb_pointer_t tb_aico_pool_nalloc_impl(tb_aico_t* aico, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
	// check 
	tb_assert_and_check_return_val(aico, tb_null);

	// init pool
	if (!aico->pool) aico->pool = tb_spool_init(TB_SPOOL_GROW_MICRO, 0);
	tb_assert_and_check_return_val(aico->pool, tb_null);

	// nalloc
	return tb_spool_nalloc_impl(aico->pool, item, size __tb_debug_args__);
}
tb_pointer_t tb_aico_pool_nalloc0_impl(tb_aico_t* aico, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
	// check 
	tb_assert_and_check_return_val(aico, tb_null);

	// init pool
	if (!aico->pool) aico->pool = tb_spool_init(TB_SPOOL_GROW_MICRO, 0);
	tb_assert_and_check_return_val(aico->pool, tb_null);

	// nalloc0
	return tb_spool_nalloc0_impl(aico->pool, item, size __tb_debug_args__);
}
tb_pointer_t tb_aico_pool_ralloc_impl(tb_aico_t* aico, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
	// check 
	tb_assert_and_check_return_val(aico, tb_null);

	// init pool
	if (!aico->pool) aico->pool = tb_spool_init(TB_SPOOL_GROW_MICRO, 0);
	tb_assert_and_check_return_val(aico->pool, tb_null);

	// ralloc
	return tb_spool_ralloc_impl(aico->pool, data, size __tb_debug_args__);
}
tb_bool_t tb_aico_pool_free_impl(tb_aico_t* aico, tb_pointer_t data __tb_debug_decl__)
{
	// check 
	tb_assert_and_check_return_val(aico && aico->pool, tb_false);

	// free
	return tb_spool_free_impl(aico->pool, data __tb_debug_args__);
}

