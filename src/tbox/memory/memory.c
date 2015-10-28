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
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the allocator 
__tb_extern_c__ extern tb_allocator_ref_t   g_allocator;

// the large pool data
__tb_extern_c__ extern tb_byte_t*           g_large_pool_data;

// the large pool size
__tb_extern_c__ extern tb_size_t            g_large_pool_size;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_memory_init(tb_allocator_ref_t allocator, tb_byte_t* data, tb_size_t size)
{
    // done
    tb_bool_t ok = tb_false;
    do
    {   
        // init page
        if (!tb_page_init()) break;

        // init the native memory
        if (!tb_native_memory_init()) break;

        // init the allocator
        g_allocator = allocator;

        // init the large pool data
        g_large_pool_data = data;
        g_large_pool_size = size;

        // init the pool
        tb_assert_and_check_break(tb_pool());

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
    // exit the native memory
    tb_native_memory_exit();

    // exit page
    tb_page_exit();
}
