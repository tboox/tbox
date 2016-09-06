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
#include "allocator.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the allocator 
__tb_extern_c__ extern tb_allocator_ref_t   g_allocator;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_memory_init(tb_allocator_ref_t allocator)
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
#if defined(__tb_small__) && !defined(__tb_debug__)
        g_allocator = allocator? allocator : tb_native_allocator();
#else
        g_allocator = allocator? allocator : tb_default_allocator(tb_null, 0);
#endif
        tb_assert_and_check_break(g_allocator);

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
