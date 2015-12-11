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
 * @file        allocator_buffer.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "allocator_buffer"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../static_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the buffer allocator type
typedef struct __tb_allocator_buffer_t
{
    // the base
    tb_allocator_t          base;

    // the pool
    tb_static_pool_ref_t    pool;

}tb_allocator_buffer_t, *tb_allocator_buffer_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_pointer_t tb_allocator_buffer_malloc(tb_allocator_ref_t self, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_allocator_buffer_ref_t allocator = (tb_allocator_buffer_ref_t)self;
    tb_check_return_val(allocator && allocator->pool, tb_null);

    // trace
    tb_trace_d("malloc(%lu) at %s(): %lu, %s", size, func_, line_, file_);

    // malloc it
    return tb_allocator_malloc_(allocator->pool, size __tb_debug_args__);
}
static tb_pointer_t tb_allocator_buffer_ralloc(tb_allocator_ref_t self, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_allocator_buffer_ref_t allocator = (tb_allocator_buffer_ref_t)self;
    tb_check_return_val(allocator && allocator->pool, tb_null);

    // trace
    tb_trace_d("ralloc(%p, %lu) at %s(): %lu, %s", data, size, func_, line_, file_);

    // ralloc it
    return tb_allocator_ralloc_(allocator->pool, data, size __tb_debug_args__);
}
static tb_bool_t tb_allocator_buffer_free(tb_allocator_ref_t self, tb_pointer_t data __tb_debug_decl__)
{ 
    // check
    tb_allocator_buffer_ref_t allocator = (tb_allocator_buffer_ref_t)self;
    tb_check_return_val(allocator && allocator->pool, tb_false);

    // trace    
    tb_trace_d("free(%p) at %s(): %lu, %s", data, func_, line_, file_);

    // free it
    return tb_allocator_free_(allocator->pool, data __tb_debug_args__);
}
#ifdef __tb_debug__
static tb_void_t tb_allocator_buffer_dump(tb_allocator_ref_t self)
{
    // check
    tb_allocator_buffer_ref_t allocator = (tb_allocator_buffer_ref_t)self;
    tb_check_return(allocator && allocator->pool);

    // dump it
    tb_allocator_dump(allocator->pool);
}
#endif
static tb_handle_t tb_allocator_buffer_instance_init(tb_cpointer_t* ppriv)
{
    // check
    tb_check_return_val(ppriv, tb_null);

    // the data and size
    tb_value_ref_t  tuple = (tb_value_ref_t)*ppriv;
    tb_byte_t*      data = (tb_byte_t*)tuple[0].ptr;
    tb_size_t       size = tuple[1].ul;

    // clear the private data first
    *ppriv = tb_null;

    // done
    tb_bool_t                   ok = tb_false;
    tb_static_pool_ref_t        pool = tb_null;
    tb_allocator_buffer_ref_t   allocator = tb_null;
    do
    {
        /* init the page first
         *
         * because this allocator may be called before tb_init()
         */
        if (!tb_page_init()) break ;

        /* init the native memory first
         *
         * because this allocator may be called before tb_init()
         */
        if (!tb_native_memory_init()) break ;

        // init pool
        pool = tb_static_pool_init(data, size);
        tb_assert_and_check_break(pool);

        // make allocator
        allocator = (tb_allocator_buffer_ref_t)tb_allocator_malloc0(pool, sizeof(tb_allocator_buffer_t));
        tb_assert_and_check_break(allocator); 

        // init allocator
        allocator->base.type    = TB_ALLOCATOR_BUFFER;
        allocator->base.malloc  = tb_allocator_buffer_malloc;
        allocator->base.ralloc  = tb_allocator_buffer_ralloc;
        allocator->base.free    = tb_allocator_buffer_free;
#ifdef __tb_debug__
        allocator->base.dump    = tb_allocator_buffer_dump;
#endif

        // save pool
        allocator->pool = pool;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit allocator
        if (allocator && pool) tb_allocator_free(pool, allocator);
        allocator = tb_null;

        // exit pool
        if (pool) tb_static_pool_exit(pool);
        pool= tb_null;
    }

    // ok?
    return (tb_handle_t)allocator;
}
static tb_void_t tb_allocator_buffer_instance_exit(tb_handle_t self, tb_cpointer_t priv)
{
    // check
    tb_allocator_buffer_ref_t allocator = (tb_allocator_buffer_ref_t)self;
    tb_assert_and_check_return(allocator);

    // get pool
    tb_static_pool_ref_t pool = allocator->pool;
    tb_assert_and_check_return(pool);

    // exit allocator
    tb_allocator_free(pool, allocator);
    allocator = tb_null;

    // dump pool
#ifdef __tb_debug__
    if (pool) tb_allocator_dump(pool);
#endif

    // exit pool
    if (pool) tb_static_pool_exit(pool);
    pool= tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_allocator_ref_t tb_allocator_buffer(tb_byte_t* data, tb_size_t size)
{
    /* init singleton first
     *
     * because this allocator may be called before tb_init()
     */
    if (!tb_singleton_init()) return tb_null;

    // init tuple
    tb_value_t tuple[2];
    tuple[0].ptr    = (tb_pointer_t)data;
    tuple[1].ul     = size;

    // get it
    return (tb_allocator_ref_t)tb_singleton_instance(TB_SINGLETON_TYPE_ALLOCATOR_BUFFER, tb_allocator_buffer_instance_init, tb_allocator_buffer_instance_exit, tb_null, tuple);
}
