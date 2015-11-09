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
 * @file        static_pool.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "static_pool"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_pool.h"
#include "impl/static_large_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the static pool type
typedef struct __tb_static_pool_t
{
    // the pool
    tb_large_pool_ref_t     pool;

    // the lock
    tb_spinlock_t           lock;

}tb_static_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_static_pool_ref_t tb_static_pool_init(tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(size > sizeof(tb_static_pool_t), tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_static_pool_t*   pool = tb_null;
    do
    {
        // calculate the pool base
        tb_byte_t* base = (tb_byte_t*)tb_align8((tb_size_t)data);
        tb_assert_and_check_break(base && size - (base - data) > sizeof(tb_static_pool_t));

        // calculate the pool size
        size -= base - data;

        // make pool
        pool = (tb_static_pool_t*)base;
        tb_assert_and_check_break(pool);

        // init pool
        pool->pool = tb_static_large_pool_init(base + sizeof(tb_static_pool_t), size - sizeof(tb_static_pool_t), 8);
        tb_assert_and_check_break(pool->pool);

        // init lock
        if (!tb_spinlock_init(&pool->lock)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (pool) tb_static_pool_exit((tb_static_pool_ref_t)pool);
        pool = tb_null;
    }

    // ok?
    return (tb_static_pool_ref_t)pool;
}
tb_void_t tb_static_pool_exit(tb_static_pool_ref_t self)
{
    // check
    tb_static_pool_t* pool = (tb_static_pool_t*)self;
    tb_assert_and_check_return(pool);

    // exit pool
    tb_spinlock_enter(&pool->lock);
    if (pool->pool) tb_static_large_pool_exit(pool->pool);
    pool->pool = tb_null;
    tb_spinlock_leave(&pool->lock);

    // exit lock
    tb_spinlock_exit(&pool->lock);
}
tb_void_t tb_static_pool_clear(tb_static_pool_ref_t self)
{
    // check
    tb_static_pool_t* pool = (tb_static_pool_t*)self;
    tb_assert_and_check_return(pool && pool->pool);

    // enter
    tb_spinlock_enter(&pool->lock);

    // clear pool
    tb_static_large_pool_clear(pool->pool);

    // leave
    tb_spinlock_leave(&pool->lock);
}
tb_pointer_t tb_static_pool_malloc_(tb_static_pool_ref_t self, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_static_pool_t* pool = (tb_static_pool_t*)self;
    tb_assert_and_check_return_val(pool && pool->pool && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // enter
    tb_spinlock_enter(&pool->lock);

    // malloc data
    tb_pointer_t data = tb_static_large_pool_malloc(pool->pool, size, tb_null __tb_debug_args__);
    tb_assertf_abort(data, "malloc(%lu) failed!", size);
    tb_assertf_abort(!(((tb_size_t)data) & (TB_POOL_DATA_ALIGN - 1)), "malloc(%lu): unaligned data: %p", size, data);

    // leave
    tb_spinlock_leave(&pool->lock);

    // ok
    return data;
}
tb_pointer_t tb_static_pool_malloc0_(tb_static_pool_ref_t self, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_static_pool_t* pool = (tb_static_pool_t*)self;
    tb_assert_and_check_return_val(pool && pool->pool && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // enter
    tb_spinlock_enter(&pool->lock);

    // malloc0 data
    tb_pointer_t data = tb_static_large_pool_malloc(pool->pool, size, tb_null __tb_debug_args__);
    tb_assertf_abort(data, "malloc0(%lu) failed!", size);
    tb_assertf_abort(!(((tb_size_t)data) & (TB_POOL_DATA_ALIGN - 1)), "malloc0(%lu): unaligned data: %p", size, data);

    // clear it
    tb_memset_(data, 0, size);

    // leave
    tb_spinlock_leave(&pool->lock);

    // ok
    return data;
}
tb_pointer_t tb_static_pool_nalloc_(tb_static_pool_ref_t self, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_static_pool_t* pool = (tb_static_pool_t*)self;
    tb_assert_and_check_return_val(pool && pool->pool && size, tb_null);
    tb_assert_and_check_return_val((item * size) <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // enter
    tb_spinlock_enter(&pool->lock);

    // nalloc data
    tb_pointer_t data = tb_static_large_pool_malloc(pool->pool, item * size, tb_null __tb_debug_args__);
    tb_assertf_abort(data, "nalloc(%lu, %lu) failed!", item, size);
    tb_assertf_abort(!(((tb_size_t)data) & (TB_POOL_DATA_ALIGN - 1)), "nalloc(%lu, %lu): unaligned data: %p", item, size, data);

    // leave
    tb_spinlock_leave(&pool->lock);

    // ok
    return data;
}
tb_pointer_t tb_static_pool_nalloc0_(tb_static_pool_ref_t self, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_static_pool_t* pool = (tb_static_pool_t*)self;
    tb_assert_and_check_return_val(pool && pool->pool && size, tb_null);
    tb_assert_and_check_return_val((item * size) <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // enter
    tb_spinlock_enter(&pool->lock);

    // nalloc0 data
    tb_pointer_t data = tb_static_large_pool_malloc(pool->pool, item * size, tb_null __tb_debug_args__);
    tb_assertf_abort(data, "nalloc0(%lu, %lu) failed!", item, size);
    tb_assertf_abort(!(((tb_size_t)data) & (TB_POOL_DATA_ALIGN - 1)), "nalloc0(%lu, %lu): unaligned data: %p", item, size, data);

    // clear it
    tb_memset_(data, 0, item * size);

    // leave
    tb_spinlock_leave(&pool->lock);

    // ok
    return data;
}
tb_pointer_t tb_static_pool_ralloc_(tb_static_pool_ref_t self, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_static_pool_t* pool = (tb_static_pool_t*)self;
    tb_assert_and_check_return_val(pool && pool->pool && data && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // enter
    tb_spinlock_enter(&pool->lock);

    // ralloc data
    tb_pointer_t data_new = tb_static_large_pool_ralloc(pool->pool, data, size, tb_null __tb_debug_args__);

    // failed? dump it
#ifdef __tb_debug__
    if (!data_new) 
    {
        // trace
        tb_trace_e("ralloc(%p, %lu) failed! at %s(): %lu, %s", data, size, func_, line_, file_);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)data, tb_true, "[static_pool]: [error]: ");

        // abort
        tb_abort();
    }
#endif

    // check
    tb_assertf_abort(!(((tb_size_t)data_new) & (TB_POOL_DATA_ALIGN - 1)), "ralloc(%lu): unaligned data: %p", size, data);

    // leave
    tb_spinlock_leave(&pool->lock);

    // ok
    return data_new;
}
tb_bool_t tb_static_pool_free_(tb_static_pool_ref_t self, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_static_pool_t* pool = (tb_static_pool_t*)self;
    tb_assert_and_check_return_val(pool && pool->pool && data, tb_false);

    // enter
    tb_spinlock_enter(&pool->lock);

    // free data
    tb_bool_t ok = tb_static_large_pool_free(pool->pool, data __tb_debug_args__);

    // failed? dump it
#ifdef __tb_debug__
    if (!ok) 
    {
        // trace
        tb_trace_e("free(%p) failed! at %s(): %lu, %s", data, func_, line_, file_);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)data, tb_true, "[static_pool]: [error]: ");

        // abort
        tb_abort();
    }
#endif

    // leave
    tb_spinlock_leave(&pool->lock);

    // ok
    return ok;
}
#ifdef __tb_debug__
tb_void_t tb_static_pool_dump(tb_static_pool_ref_t self)
{
    // check
    tb_static_pool_t* pool = (tb_static_pool_t*)self;
    tb_assert_and_check_return(pool && pool->pool);

    // enter
    tb_spinlock_enter(&pool->lock);

    // dump the pool
    tb_static_large_pool_dump(pool->pool);

    // leave
    tb_spinlock_leave(&pool->lock);
}
#endif
