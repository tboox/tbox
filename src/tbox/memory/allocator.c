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
 * @file        allocator.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "allocator"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "allocator.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_pointer_t tb_allocator_native_malloc(tb_allocator_ref_t allocator, tb_size_t size __tb_debug_decl__)
{
    // trace
    tb_trace_d("malloc(%lu) at %s(): %lu, %s", size, func_, line_, file_);

    // malloc it
    return tb_native_memory_malloc(size);
}
static tb_pointer_t tb_allocator_native_ralloc(tb_allocator_ref_t allocator, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // trace
    tb_trace_d("ralloc(%p, %lu) at %s(): %lu, %s", data, size, func_, line_, file_);

    // ralloc it
    return tb_native_memory_ralloc(data, size);
}
static tb_bool_t tb_allocator_native_free(tb_allocator_ref_t allocator, tb_pointer_t data __tb_debug_decl__)
{
    // trace    
    tb_trace_d("free(%p) at %s(): %lu, %s", data, func_, line_, file_);

    // free it
    return tb_native_memory_free(data);
}
static tb_bool_t tb_allocator_native_instance_init(tb_handle_t instance)
{
    // check
    tb_allocator_ref_t allocator = (tb_allocator_ref_t)instance;
    tb_check_return_val(allocator, tb_false);

    // init allocator
    allocator->malloc   = tb_allocator_native_malloc;
    allocator->ralloc   = tb_allocator_native_ralloc;
    allocator->free     = tb_allocator_native_free;
#ifdef __tb_debug__
    allocator->dump     = tb_null;
#endif

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_allocator_ref_t tb_allocator_native()
{
    // init
    static tb_atomic_t      s_is_inited = 0;
    static tb_allocator_t   s_allocator = {0};

    // init the static instance
    tb_singleton_static_init(&s_is_inited, &s_allocator, tb_allocator_native_instance_init);

    // ok
    return &s_allocator;
}
tb_pointer_t tb_allocator_malloc_(tb_allocator_ref_t allocator, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator && allocator->malloc, tb_null);

    // malloc it
    return allocator->malloc(allocator, size __tb_debug_args__);
}
tb_pointer_t tb_allocator_malloc0_(tb_allocator_ref_t allocator, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator, tb_null);

    // malloc it
    tb_pointer_t data = tb_allocator_malloc_(allocator, size __tb_debug_args__);

    // clear it
    if (data && size) tb_memset_(data, 0, size);

    // ok
    return data;
}
tb_pointer_t tb_allocator_nalloc_(tb_allocator_ref_t allocator, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator, tb_null);

    // nalloc it
    return tb_allocator_malloc_(allocator, item * size __tb_debug_args__);
}
tb_pointer_t tb_allocator_nalloc0_(tb_allocator_ref_t allocator, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator, tb_null);

    // nalloc0 it
    return tb_allocator_malloc0_(allocator, item * size __tb_debug_args__);
}
tb_pointer_t tb_allocator_ralloc_(tb_allocator_ref_t allocator, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator && allocator->ralloc, tb_null);

    // ralloc it
    return allocator->ralloc(allocator, data, size __tb_debug_args__);
}
tb_bool_t tb_allocator_free_(tb_allocator_ref_t allocator, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(allocator && allocator->free, tb_false);

    // free it
    return allocator->free(allocator, data __tb_debug_args__);
}
#ifdef __tb_debug__
tb_void_t tb_allocator_dump(tb_allocator_ref_t allocator)
{
    // check
    tb_assert_and_check_return(allocator);

    // dump it
    if (allocator->dump) allocator->dump(allocator);
}
#endif
