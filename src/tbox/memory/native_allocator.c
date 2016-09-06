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
 * @file        native_allocator.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "native_allocator"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "native_allocator.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_pointer_t tb_native_allocator_malloc(tb_allocator_ref_t allocator, tb_size_t size __tb_debug_decl__)
{
    // trace
    tb_trace_d("malloc(%lu) at %s(): %lu, %s", size, func_, line_, file_);

    // malloc it
    return tb_native_memory_malloc(size);
}
static tb_pointer_t tb_native_allocator_ralloc(tb_allocator_ref_t allocator, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // trace
    tb_trace_d("ralloc(%p, %lu) at %s(): %lu, %s", data, size, func_, line_, file_);

    // ralloc it
    return tb_native_memory_ralloc(data, size);
}
static tb_bool_t tb_native_allocator_free(tb_allocator_ref_t allocator, tb_pointer_t data __tb_debug_decl__)
{
    // trace    
    tb_trace_d("free(%p) at %s(): %lu, %s", data, func_, line_, file_);

    // free it
    return tb_native_memory_free(data);
}
static tb_bool_t tb_native_allocator_instance_init(tb_handle_t instance, tb_cpointer_t priv)
{
    // check
    tb_allocator_ref_t allocator = (tb_allocator_ref_t)instance;
    tb_check_return_val(allocator, tb_false);

    /* init the native memory first
     *
     * because this allocator may be called before tb_init()
     */
    if (!tb_native_memory_init()) return tb_false;

    // init allocator
    allocator->type         = TB_ALLOCATOR_NATIVE;
    allocator->malloc       = tb_native_allocator_malloc;
    allocator->ralloc       = tb_native_allocator_ralloc;
    allocator->free         = tb_native_allocator_free;
#ifdef __tb_debug__
    allocator->dump         = tb_null;
    allocator->have         = tb_null;
#endif

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_allocator_ref_t tb_native_allocator()
{
    // init
    static tb_atomic_t      s_inited = 0;
    static tb_allocator_t   s_allocator = {0};

    // init the static instance
    tb_singleton_static_init(&s_inited, &s_allocator, tb_native_allocator_instance_init, tb_null);

    // ok
    return &s_allocator;
}

