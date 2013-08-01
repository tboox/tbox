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
 * @tstore		tstore.c
 * @ingroup 	platform
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
//#define TB_TRACE_IMPL_TAG 			"tstore"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tstore.h"
#include "mutex.h"
#include "atomic.h"
#include "thread.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the store
static tb_handle_t 	g_store = tb_null;

// the mutex
static tb_handle_t 	g_mutex = tb_null;

/* ///////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_void_t tb_tstore_free(tb_item_func_t* func, tb_pointer_t item)
{
	// data item
	tb_tstore_data_t* data = item? *((tb_tstore_data_t**)item) : tb_null;

	// free data
	if (data && data->free) data->free(data); 

	// clear item
	if (item) *((tb_tstore_data_t**)item) = tb_null;
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_tstore_init()
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

	// enter mutex
	if (g_mutex) tb_mutex_enter(g_mutex);

	// no store?
	if (!g_store) 
	{
		// init item func
		tb_item_func_t func = tb_item_func_ptr(); 
		func.free = tb_tstore_free;

		// init store
		g_store = tb_hash_init(8, tb_item_func_ptr(), func);
	}

	// leave mutex
	if (g_mutex) tb_mutex_leave(g_mutex);

	// ok?
	return g_store? tb_true : tb_false;
}
tb_void_t tb_tstore_exit()
{	
	// enter mutex
	if (g_mutex) tb_mutex_enter(g_mutex);

	// exit store
	if (g_store) tb_hash_exit(g_store);
	g_store = tb_null;

	// leave mutex
	if (g_mutex) tb_mutex_leave(g_mutex);

	// exit mutex
	tb_handle_t mutex = g_mutex;
	if (tb_atomic_fetch_and_set0((tb_atomic_t*)&g_mutex))
		tb_mutex_exit(mutex);
}
tb_void_t tb_tstore_setp(tb_tstore_data_t const* data)
{
	// check
	tb_check_return(tb_atomic_get((tb_atomic_t*)&g_mutex));

	// enter mutex
	if (g_mutex) tb_mutex_enter(g_mutex);

	// get data
	if (g_store) tb_hash_set(g_store, tb_thread_self(), data);

	// leave mutex
	if (g_mutex) tb_mutex_leave(g_mutex);
}
tb_tstore_data_t* tb_tstore_getp()
{
	// check
	tb_check_return_val(tb_atomic_get((tb_atomic_t*)&g_mutex), tb_null);

	// init data
	tb_pointer_t data = tb_null;

	// enter mutex
	if (g_mutex) tb_mutex_enter(g_mutex);

	// get data
	if (g_store) data = tb_hash_get(g_store, tb_thread_self());

	// leave mutex
	if (g_mutex) tb_mutex_leave(g_mutex);

	// ok?
	return (tb_tstore_data_t*)data;
}

