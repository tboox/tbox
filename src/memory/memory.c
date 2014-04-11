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
 * @file		memory.c
 * @defgroup 	memory
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "memory.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the pool
static tb_handle_t 		g_pool = tb_null;

// the lock
static tb_spinlock_t 	g_lock = TB_SPINLOCK_INIT; 

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_memory_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
	// done
	tb_bool_t ok = tb_false;
	do
	{
		// using pool?
		if (data && size)
		{
			// enter
			tb_spinlock_enter(&g_lock);

			// check
			tb_assert(!g_pool);

			// init pool
			tb_handle_t pool = g_pool = tb_gpool_init(data, size, align);

			// leave
			tb_spinlock_leave(&g_lock);

			// check
			tb_assert_and_check_break(pool);
		}
		else
		{
			// init the native memory
			if (!tb_native_memory_init()) break;
		}

		// init scache
		if (!tb_scache_init(align)) break;

		// ok
		ok = tb_true;

	} while (0);

	// failed? exit it
	if (!ok) tb_memory_exit();
	
	// ok?
	return ok;
}
tb_void_t tb_memory_exit()
{
	// exit scache
	tb_scache_exit();

	// enter
	tb_spinlock_enter(&g_lock);

	// the pool
	tb_handle_t pool = g_pool;
	
	// exit pool
	if (pool) 
	{
#ifdef __tb_debug__
		// dump it
		tb_gpool_dump(pool);
#endif

		// exit it
		tb_gpool_exit(pool);
	}
	g_pool = tb_null;

	// leave
	tb_spinlock_leave(&g_lock);

	// exit the native memory
	if (!pool) tb_native_memory_exit();

	// exit lock
	tb_spinlock_exit(&g_lock);
}
tb_pointer_t tb_memory_malloc_impl(tb_size_t size __tb_debug_decl__)
{
	// enter
	tb_spinlock_enter(&g_lock);

	// the pool
	tb_handle_t pool = g_pool;

	// malloc
	tb_pointer_t data = pool? tb_gpool_malloc_impl(pool, size __tb_debug_args__) : tb_null;

	// leave
	tb_spinlock_leave(&g_lock);

	// malloc it from the native memory
	if (!pool) data = tb_native_malloc(size);

	// ok?
	return data;
}
tb_pointer_t tb_memory_malloc0_impl(tb_size_t size __tb_debug_decl__)
{
	// enter
	tb_spinlock_enter(&g_lock);

	// the pool
	tb_handle_t pool = g_pool;

	// malloc0
	tb_pointer_t data = pool? tb_gpool_malloc0_impl(pool, size __tb_debug_args__) : tb_null;

	// leave
	tb_spinlock_leave(&g_lock);

	// malloc0 it from the native memory
	if (!pool) data = tb_native_malloc0(size);

	// ok?
	return data;
}
tb_pointer_t tb_memory_nalloc_impl(tb_size_t item, tb_size_t size __tb_debug_decl__)
{
	// enter
	tb_spinlock_enter(&g_lock);

	// the pool
	tb_handle_t pool = g_pool;

	// nalloc
	tb_pointer_t data = pool? tb_gpool_nalloc_impl(pool, item, size __tb_debug_args__) : tb_null;

	// leave
	tb_spinlock_leave(&g_lock);

	// nalloc it from the native memory
	if (!pool) data = tb_native_nalloc(item, size);

	// ok?
	return data;
}
tb_pointer_t tb_memory_nalloc0_impl(tb_size_t item, tb_size_t size __tb_debug_decl__)
{
	// enter
	tb_spinlock_enter(&g_lock);

	// the pool
	tb_handle_t pool = g_pool;

	// nalloc0
	tb_pointer_t data = pool? tb_gpool_nalloc0_impl(pool, item, size __tb_debug_args__) : tb_null;

	// leave
	tb_spinlock_leave(&g_lock);
	
	// nalloc0 it from the native memory
	if (!pool) data = tb_native_nalloc0(item, size);

	// ok?
	return data;
}
tb_pointer_t tb_memory_ralloc_impl(tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
	// enter
	tb_spinlock_enter(&g_lock);

	// the pool
	tb_handle_t pool = g_pool;

	// ralloc
	data = pool? tb_gpool_ralloc_impl(pool, data, size __tb_debug_args__) : tb_null;

	// leave
	tb_spinlock_leave(&g_lock);
	
	// ralloc it from the native memory
	if (!pool) data = tb_native_ralloc(data, size);

	// ok?
	return data;
}
tb_bool_t tb_memory_free_impl(tb_pointer_t data __tb_debug_decl__)
{
	// check
	tb_check_return_val(data, tb_true);

	// enter
	tb_spinlock_enter(&g_lock);

	// the pool
	tb_handle_t pool = g_pool;

	// free it
	tb_bool_t ok = pool? tb_gpool_free_impl(pool, data __tb_debug_args__) : tb_false;

	// leave
	tb_spinlock_leave(&g_lock);
	
	// free it from the native memory
	if (!pool) ok = tb_native_free(data);

	// ok?
	return ok;
}
#ifdef __tb_debug__
tb_size_t tb_memory_data_size(tb_cpointer_t data)
{
	// try to enter, ensure outside the pool
	tb_size_t size = 0;
	if (tb_spinlock_enter_try(&g_lock))
	{
		// size
		size = g_pool? tb_gpool_data_size(g_pool, data) : 0;

		// leave
		tb_spinlock_leave(&g_lock);
	}

	// ok?
	return size;
}
tb_void_t tb_memory_data_dump(tb_cpointer_t data, tb_char_t const* prefix)
{
	// try to enter, ensure outside the pool
	if (tb_spinlock_enter_try(&g_lock))
	{
		// dump
		if (g_pool) tb_gpool_data_dump(g_pool, data, prefix);

		// leave
		tb_spinlock_leave(&g_lock);
	}
}
tb_void_t tb_memory_dump()
{
	// enter
	tb_spinlock_enter(&g_lock);
	
	// dump
	if (g_pool) tb_gpool_dump(g_pool);

	// leave
	tb_spinlock_leave(&g_lock);
}
#endif
