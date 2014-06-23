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
 * @author      ruki
 * @file        memory.c
 * @defgroup    memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "memory"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "memory.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the pool
static tb_handle_t      g_pool = tb_null;

// the lock
static tb_spinlock_t    g_lock = TB_SPINLOCK_INIT; 

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_memory_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
    // done
    tb_bool_t ok = tb_false;
    do
    {   
        // init the native memory
        if (!tb_native_memory_init()) break;

        // using pool?
        if (data && size)
        {
            // enter
            tb_spinlock_enter(&g_lock);

            // check
            tb_assert(!g_pool);

            // init pool
            tb_handle_t pool = g_pool = tb_global_pool_init(data, size, align);

            // leave
            tb_spinlock_leave(&g_lock);

            // check
            tb_assert_and_check_break(pool);

            // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
            tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&g_lock, TB_TRACE_MODULE_NAME);
#endif
        }
        
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
    // enter
    tb_spinlock_enter(&g_lock);

    // the pool
    tb_handle_t pool = g_pool;
    
    // exit pool
    if (pool) 
    {
#ifdef __tb_debug__
        // dump it
        tb_global_pool_dump(pool);
#endif

        // exit it
        tb_global_pool_exit(pool);
    }
    g_pool = tb_null;

    // leave
    tb_spinlock_leave(&g_lock);

    // exit lock
    tb_spinlock_exit(&g_lock);

    // exit the native memory
    tb_native_memory_exit();
}
tb_pointer_t tb_memory_malloc_(tb_size_t size __tb_debug_decl__)
{
    // enter
    tb_spinlock_enter(&g_lock);

    // the pool
    tb_handle_t pool = g_pool;

    // malloc
    tb_pointer_t data = pool? tb_global_pool_malloc_(pool, size __tb_debug_args__) : tb_null;

    // leave
    tb_spinlock_leave(&g_lock);

    // malloc it from the native memory
    if (!pool) data = tb_native_memory_malloc(size);

    // ok?
    return data;
}
tb_pointer_t tb_memory_malloc0_(tb_size_t size __tb_debug_decl__)
{
    // enter
    tb_spinlock_enter(&g_lock);

    // the pool
    tb_handle_t pool = g_pool;

    // malloc0
    tb_pointer_t data = pool? tb_global_pool_malloc0_(pool, size __tb_debug_args__) : tb_null;

    // leave
    tb_spinlock_leave(&g_lock);

    // malloc0 it from the native memory
    if (!pool) data = tb_native_memory_malloc0(size);

    // ok?
    return data;
}
tb_pointer_t tb_memory_nalloc_(tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // enter
    tb_spinlock_enter(&g_lock);

    // the pool
    tb_handle_t pool = g_pool;

    // nalloc
    tb_pointer_t data = pool? tb_global_pool_nalloc_(pool, item, size __tb_debug_args__) : tb_null;

    // leave
    tb_spinlock_leave(&g_lock);

    // nalloc it from the native memory
    if (!pool) data = tb_native_memory_nalloc(item, size);

    // ok?
    return data;
}
tb_pointer_t tb_memory_nalloc0_(tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // enter
    tb_spinlock_enter(&g_lock);

    // the pool
    tb_handle_t pool = g_pool;

    // nalloc0
    tb_pointer_t data = pool? tb_global_pool_nalloc0_(pool, item, size __tb_debug_args__) : tb_null;

    // leave
    tb_spinlock_leave(&g_lock);
    
    // nalloc0 it from the native memory
    if (!pool) data = tb_native_memory_nalloc0(item, size);

    // ok?
    return data;
}
tb_pointer_t tb_memory_ralloc_(tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // enter
    tb_spinlock_enter(&g_lock);

    // the pool
    tb_handle_t pool = g_pool;

    // ralloc
    if (pool) data = tb_global_pool_ralloc_(pool, data, size __tb_debug_args__);

    // leave
    tb_spinlock_leave(&g_lock);
    
    // ralloc it from the native memory
    if (!pool) data = tb_native_memory_ralloc(data, size);

    // ok?
    return data;
}
tb_bool_t tb_memory_free_(tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_check_return_val(data, tb_true);

    // enter
    tb_spinlock_enter(&g_lock);

    // the pool
    tb_handle_t pool = g_pool;

    // free it
    tb_bool_t ok = pool? tb_global_pool_free_(pool, data __tb_debug_args__) : tb_false;

    // leave
    tb_spinlock_leave(&g_lock);
    
    // free it from the native memory
    if (!pool) ok = tb_native_memory_free(data);

    // ok?
    return ok;
}
#ifdef __tb_debug__
tb_size_t tb_memory_data_size(tb_cpointer_t data)
{
    // try to enter, ensure outside the pool
    tb_size_t size = 0;
    if (tb_spinlock_enter_try_without_profiler(&g_lock))
    {
        // size
        size = g_pool? tb_global_pool_data_size(g_pool, data) : 0;

        // leave
        tb_spinlock_leave(&g_lock);
    }

    // ok?
    return size;
}
tb_void_t tb_memory_data_dump(tb_cpointer_t data, tb_char_t const* prefix)
{
    // try to enter, ensure outside the pool
    if (tb_spinlock_enter_try_without_profiler(&g_lock))
    {
        // dump
        if (g_pool) tb_global_pool_data_dump(g_pool, data, prefix);

        // leave
        tb_spinlock_leave(&g_lock);
    }
}
tb_void_t tb_memory_dump()
{
    // enter
    tb_spinlock_enter(&g_lock);
    
    // dump
    if (g_pool) tb_global_pool_dump(g_pool);

    // leave
    tb_spinlock_leave(&g_lock);
}
#endif
