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
 * @file		malloc.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../sched.h"
#include "../atomic.h"
#include "../spinlock.h"

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the heap
static tb_handle_t 		g_heap = tb_null;

// the lock
static tb_spinlock_t 	g_lock = TB_SPINLOCK_INIT; 

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
#ifndef TB_CONFIG_MEMORY_POOL
tb_bool_t tb_malloc_init(tb_noarg_t);
tb_void_t tb_malloc_exit(tb_noarg_t);
#endif

/* ///////////////////////////////////////////////////////////////////////
 * locker
 */
static __tb_inline_force__ tb_void_t tb_malloc_lock_enter(tb_noarg_t)
{
	// init tryn
	__tb_volatile__ tb_size_t tryn = 5;

	// lock it
	while (tb_atomic_fetch_and_pset(&g_lock, 0, 1))
	{
		if (!tryn--)
		{
			// yield the processor
			tb_sched_yield();
//			tb_usleep(1);

			// reset tryn
			tryn = 5;
		}
	}
}
static __tb_inline_force__ tb_size_t tb_malloc_lock_enter_try(tb_noarg_t)
{
	// try lock it
	return !tb_atomic_fetch_and_pset(&g_lock, 0, 1);
}
static __tb_inline_force__ tb_void_t tb_malloc_lock_leave(tb_noarg_t)
{
    g_lock = 0;
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_malloc_init()
{	
	// init heap
	tb_bool_t ok = tb_false;
	tb_spinlock_enter(&g_lock);
	if (!g_heap) g_heap = (tb_handle_t)HeapCreate(0, 0, 0);
	if (g_heap) ok = tb_true;
	tb_spinlock_leave(&g_lock);

	// ok?
	return ok;
}
tb_void_t tb_malloc_exit()
{	
	// enter 
	tb_spinlock_enter(&g_lock);

	// exit heap
	if (g_heap) HeapDestroy(g_heap);
	g_heap = tb_null;

	// leave
	tb_spinlock_leave(&g_lock);

	// exit lock
	g_lock = 0;
}
tb_pointer_t tb_malloc(tb_size_t size)
{
	// check
	tb_check_return_val(size, tb_null);

	// init data
	tb_pointer_t data = tb_null;

	// enter 
	tb_spinlock_enter(&g_lock);

	// alloc data
	if (g_heap) data = HeapAlloc((HANDLE)g_heap, 0, (SIZE_T)size);

	// leave
	tb_spinlock_leave(&g_lock);

	// ok?
	return data;
}
tb_pointer_t tb_malloc0(tb_size_t size)
{
	// check
	tb_check_return_val(size, tb_null);

	// init data
	tb_pointer_t data = tb_null;

	// enter 
	tb_spinlock_enter(&g_lock);

	// alloc data
	if (g_heap) data = HeapAlloc((HANDLE)g_heap, HEAP_ZERO_MEMORY, (SIZE_T)size);

	// leave
	tb_spinlock_leave(&g_lock);

	// ok?
	return data;
}
tb_pointer_t tb_nalloc(tb_size_t item, tb_size_t size)
{
	tb_check_return_val(item && size, tb_null);	
	return tb_malloc(item * size);
}
tb_pointer_t tb_nalloc0(tb_size_t item, tb_size_t size)
{
	tb_check_return_val(item && size, tb_null);		
	return tb_malloc0(item * size);
}
tb_pointer_t tb_ralloc(tb_pointer_t data, tb_size_t size)
{
	if (!size) 
	{
		tb_free(data);
		return tb_null;
	}
	else if (!data) return tb_malloc(size);
	else 
	{
		// enter 
		tb_spinlock_enter(&g_lock);

		// realloc
		if (g_heap) data = (tb_pointer_t)HeapReAlloc((HANDLE)g_heap, 0, data, (SIZE_T)size);

		// leave
		tb_spinlock_leave(&g_lock);

		// ok?
		return data;
	}
}
tb_void_t tb_free(tb_pointer_t data)
{
	// enter 
	tb_spinlock_enter(&g_lock);

	// free data
	if (g_heap && data) HeapFree((HANDLE)g_heap, 0, data);

	// leave
	tb_spinlock_leave(&g_lock);
}

