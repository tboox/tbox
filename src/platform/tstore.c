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
#include "atomic.h"
#include "thread.h"
#include "spinlock.h"
#include "../container/container.h"

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the store
static tb_handle_t 		g_store = tb_null;

// the lock
static tb_spinlock_t 	g_lock = TB_SPINLOCK_INIT;

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
	// enter lock
	tb_spinlock_enter(&g_lock);

	// no store?
	if (!g_store) 
	{
		// init store
		g_store = tb_hash_init(8, tb_item_func_ptr(tb_null, tb_null), tb_item_func_ptr(tb_tstore_free, tb_null));
	}

	// leave lock
	tb_spinlock_leave(&g_lock);

	// ok?
	return g_store? tb_true : tb_false;
}
tb_void_t tb_tstore_exit()
{	
	// enter lock
	tb_spinlock_enter(&g_lock);

	// exit store
	if (g_store) tb_hash_exit(g_store);
	g_store = tb_null;

	// leave lock
	tb_spinlock_leave(&g_lock);
}
tb_void_t tb_tstore_setp(tb_tstore_data_t const* data)
{
	// enter lock
	tb_spinlock_enter(&g_lock);

	// get data
	if (g_store) tb_hash_set(g_store, tb_thread_self(), data);

	// leave lock
	tb_spinlock_leave(&g_lock);
}
tb_tstore_data_t* tb_tstore_getp()
{
	// init data
	tb_pointer_t data = tb_null;

	// enter lock
	tb_spinlock_enter(&g_lock);

	// get data
	if (g_store) data = tb_hash_get(g_store, tb_thread_self());

	// leave lock
	tb_spinlock_leave(&g_lock);

	// ok?
	return (tb_tstore_data_t*)data;
}

