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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		pool.c
 * @ingroup 	object
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../utils/utils.h"
#include "../platform/platform.h"
 
/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef __tb_small__
# 	define TB_OBJECT_POOL_GROW 		(256 * 1024)
#else
# 	define TB_OBJECT_POOL_GROW 		(512 * 1024)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the object pool type
typedef struct __tb_object_pool_t
{
	// the pool
	tb_handle_t 		pool;

	// the lock
	tb_spinlock_t 		lock;

}tb_object_pool_t;

/* ///////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_void_t tb_object_pool_instance_exit(tb_handle_t handle)
{
	if (handle) 
	{
		// dump it
#ifdef __tb_debug__
		tb_object_pool_dump(handle);
#endif

		// exit it
		tb_object_pool_exit(handle);
	}
}

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_object_pool_init()
{
	// done
	tb_object_pool_t* 	pool = tb_null;
	tb_bool_t 			ok = tb_false;
	do
	{
		// make pool
		pool = (tb_object_pool_t*)tb_malloc0(sizeof(tb_object_pool_t));
		tb_assert_and_check_break(pool);

		// init lock
		if (!tb_spinlock_init(&pool->lock)) break;

		// init pool
		pool->pool = tb_spool_init(TB_OBJECT_POOL_GROW, 0);
		tb_assert_and_check_break(pool->pool);

		// ok
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		// exit it
		if (pool) tb_object_pool_exit(pool);
		pool = tb_null;
	}

	// ok?
	return pool;
}
tb_void_t tb_object_pool_exit(tb_handle_t handle)
{
	// the pool
	tb_object_pool_t* pool = (tb_object_pool_t*)handle;
	tb_assert_and_check_return(pool);

	// enter
	tb_spinlock_enter(&pool->lock);

	// exit pool
	if (pool->pool) tb_spool_exit(pool->pool);
	pool->pool = tb_null;

	// leave
	tb_spinlock_leave(&pool->lock);

	// exit lock
	tb_spinlock_exit(&pool->lock);

	// exit it
	tb_free(pool);
}
tb_void_t tb_object_pool_clear(tb_handle_t handle)
{
	// the pool
	tb_object_pool_t* pool = (tb_object_pool_t*)handle;
	tb_assert_and_check_return(pool);

	// enter
	tb_spinlock_enter(&pool->lock);

	// clear pool
	if (pool->pool) tb_spool_clear(pool->pool);

	// leave
	tb_spinlock_leave(&pool->lock);
}
#ifdef __tb_debug__
tb_void_t tb_object_pool_dump(tb_handle_t handle)
{
	// the pool
	tb_object_pool_t* pool = (tb_object_pool_t*)handle;
	tb_assert_and_check_return(pool);

	// enter
	tb_spinlock_enter(&pool->lock);

	// dump
	if (pool->pool) tb_spool_dump(pool->pool, "object_pool");

	// leave
	tb_spinlock_leave(&pool->lock);
}
#endif
tb_object_t* tb_object_pool_get_impl(tb_handle_t handle, tb_size_t size, tb_size_t flag, tb_size_t type __tb_debug_decl__)
{
	// check
	tb_object_pool_t* pool = (tb_object_pool_t*)handle;
	tb_assert_and_check_return_val(pool && size && type, tb_null);

	// enter
	tb_spinlock_enter(&pool->lock);

	// make object
	tb_object_t* object = pool->pool? tb_spool_malloc0_impl(pool->pool, size __tb_debug_args__) : tb_null;

	// init object
	if (object) 
	{
		if (!tb_object_init(object, flag, type)) 
		{
			tb_spool_free_impl(pool->pool, object __tb_debug_args__);
			object = tb_null;
		}
	}

	// leave
	tb_spinlock_leave(&pool->lock);

	// ok?
	return object;
}
tb_void_t tb_object_pool_del_impl(tb_handle_t handle, tb_object_t* object __tb_debug_decl__)
{
	// check
	tb_object_pool_t* pool = (tb_object_pool_t*)handle;
	tb_assert_and_check_return(pool && object);

	// enter
	tb_spinlock_enter(&pool->lock);

	// exit object
	if (pool->pool) tb_spool_free_impl(pool->pool, object __tb_debug_args__);

	// leave
	tb_spinlock_leave(&pool->lock);
}
tb_handle_t tb_object_pool_instance()
{
	return tb_singleton_instance(TB_SINGLETON_TYPE_OBJECT_POOL, tb_object_pool_init, tb_object_pool_instance_exit, tb_null);
}

