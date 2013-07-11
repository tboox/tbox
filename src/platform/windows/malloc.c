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
 * @file		malloc.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../mutex.h"
#include "../atomic.h"

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the heap
static tb_handle_t g_heap = tb_null;

// the mutex
static tb_handle_t g_mutex = tb_null;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_malloc_init()
{	
	// init mutex?
	if (!tb_atomic_get((tb_atomic_t*)&g_mutex))
	{
		// init mutex
		tb_handle_t mutex = tb_mutex_init(tb_null);
		tb_assert_and_check_return_val(mutex, tb_null);
		
		// exit mutex if the mutex have been inited already
		if (tb_atomic_fetch_and_set((tb_atomic_t*)&g_mutex, mutex))
			tb_mutex_exit(mutex);
	}

	// init data
	tb_pointer_t data = tb_null;

	// enter mutex
	if (g_mutex) tb_mutex_enter(g_mutex);

	// init heap
	if (!g_heap) g_heap = (tb_handle_t)HeapCreate(0, 0, 0);

	// leave mutex
	if (g_mutex) tb_mutex_leave(g_mutex);

	// ok?
	return g_heap? tb_true : tb_false;
}
tb_void_t tb_malloc_exit()
{	
	// enter mutex
	if (g_mutex) tb_mutex_enter(g_mutex);

	// exit heap
	if (g_heap) HeapDestroy(g_heap);
	g_heap = tb_null;

	// leave mutex
	if (g_mutex) tb_mutex_leave(g_mutex);

	// exit mutex
	tb_handle_t mutex = g_mutex;
	if (tb_atomic_fetch_and_set0((tb_atomic_t*)&g_mutex))
		tb_mutex_exit(mutex);
}
tb_pointer_t tb_malloc(tb_size_t size)
{
	// check
	tb_check_return_val(size, tb_null);

	// init mutex?
	if (!tb_malloc_init()) return tb_null;

	// init data
	tb_pointer_t data = tb_null;

	// enter mutex
	if (g_mutex) tb_mutex_enter(g_mutex);

	// alloc data
	if (g_heap) data = HeapAlloc((HANDLE)g_heap, 0, (SIZE_T)size);

	// leave mutex
	if (g_mutex) tb_mutex_leave(g_mutex);

	// ok?
	return data;
}
tb_pointer_t tb_malloc0(tb_size_t size)
{
	// check
	tb_check_return_val(size, tb_null);

	// init mutex?
	if (!tb_malloc_init()) return tb_null;

	// init data
	tb_pointer_t data = tb_null;

	// enter mutex
	if (g_mutex) tb_mutex_enter(g_mutex);

	// alloc data
	if (g_heap) data = HeapAlloc((HANDLE)g_heap, HEAP_ZERO_MEMORY, (SIZE_T)size);

	// leave mutex
	if (g_mutex) tb_mutex_leave(g_mutex);

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
		// enter mutex
		if (g_mutex) tb_mutex_enter(g_mutex);

		// realloc
		if (g_heap) data = (tb_pointer_t)HeapReAlloc((HANDLE)g_heap, 0, data, (SIZE_T)size);

		// leave mutex
		if (g_mutex) tb_mutex_leave(g_mutex);

		// ok?
		return data;
	}
}
tb_void_t tb_free(tb_pointer_t data)
{
	// enter mutex
	if (g_mutex) tb_mutex_enter(g_mutex);

	// free data
	if (g_heap && data) HeapFree((HANDLE)g_heap, 0, data);

	// leave mutex
	if (g_mutex) tb_mutex_leave(g_mutex);
}

