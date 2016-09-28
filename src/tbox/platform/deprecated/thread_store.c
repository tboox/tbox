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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @tstore      thread_store.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "thread_store"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "thread_store.h"
#include "../atomic.h"
#include "../thread.h"
#include "../spinlock.h"
#include "../../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the store
static tb_hash_map_ref_t        g_store = tb_null;

// the lock
static tb_spinlock_t            g_lock = TB_SPINLOCK_INIT;

/* //////////////////////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_void_t tb_thread_store_free(tb_element_ref_t element, tb_pointer_t buff)
{
    // data item
    tb_thread_store_data_ref_t data = buff? *((tb_thread_store_data_ref_t*)buff) : tb_null;

    // free data
    if (data && data->free) data->free(data); 

    // clear it
    if (buff) *((tb_thread_store_data_ref_t*)buff) = tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_thread_store_init_env()
{
    // enter lock
    tb_spinlock_enter(&g_lock);

    // no store?
    if (!g_store) 
    {
        // init store
        g_store = tb_hash_map_init(8, tb_element_size(), tb_element_ptr(tb_thread_store_free, tb_null));
    }

    // leave lock
    tb_spinlock_leave(&g_lock);

    // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
    tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&g_lock, TB_TRACE_MODULE_NAME);
#endif

    // ok?
    return g_store? tb_true : tb_false;
}
tb_void_t tb_thread_store_exit_env()
{   
    // enter lock
    tb_spinlock_enter(&g_lock);

    // exit store
    if (g_store) tb_hash_map_exit(g_store);
    g_store = tb_null;

    // leave lock
    tb_spinlock_leave(&g_lock);
}
tb_void_t tb_thread_store_setp(tb_thread_store_data_t const* data)
{
    // enter lock
    tb_spinlock_enter(&g_lock);

    // get data
    if (g_store) tb_hash_map_insert(g_store, (tb_pointer_t)tb_thread_self(), data);

    // leave lock
    tb_spinlock_leave(&g_lock);
}
tb_thread_store_data_ref_t tb_thread_store_getp()
{
    // init data
    tb_pointer_t data = tb_null;

    // enter lock
    tb_spinlock_enter(&g_lock);

    // get data
    if (g_store) data = tb_hash_map_get(g_store, (tb_pointer_t)tb_thread_self());

    // leave lock
    tb_spinlock_leave(&g_lock);

    // ok?
    return (tb_thread_store_data_ref_t)data;
}

