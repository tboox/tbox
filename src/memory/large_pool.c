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
 * @file        large_pool.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "large_pool"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "large_pool.h"
#include "impl/static_large_pool.h"
#include "impl/native_large_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// is native pool?
#define tb_large_pool_is_native(pool)        (((tb_size_t)(pool)) & 0x1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_large_pool_ref_t tb_large_pool_init(tb_byte_t* data, tb_size_t size)
{
    // init pool
    return (data && size)? tb_static_large_pool_init(data, size) : tb_native_large_pool_init();
}
tb_void_t tb_large_pool_exit(tb_large_pool_ref_t pool)
{
    // check
    tb_assert_and_check_return(pool);

    // exit pool
    if (tb_large_pool_is_native(pool)) tb_native_large_pool_exit(pool);
    else tb_static_large_pool_exit(pool);
}
tb_void_t tb_large_pool_clear(tb_large_pool_ref_t pool)
{
    // check
    tb_assert_and_check_return(pool);

    // clear pool
    if (tb_large_pool_is_native(pool)) tb_native_large_pool_clear(pool);
    else tb_static_large_pool_clear(pool);
}
tb_pointer_t tb_large_pool_malloc_(tb_large_pool_ref_t pool, tb_size_t size, tb_size_t* real __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // malloc data
    tb_pointer_t data = tb_large_pool_is_native(pool)? tb_native_large_pool_malloc(pool, size, real __tb_debug_args__) : tb_static_large_pool_malloc(pool, size, real __tb_debug_args__);
    tb_assertf_abort(data, "malloc(%lu) failed!", size);
    tb_assert_abort(!real || *real >= size);

    // ok
    return data;
}
tb_pointer_t tb_large_pool_malloc0_(tb_large_pool_ref_t pool, tb_size_t size, tb_size_t* real __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // malloc0 data
    tb_pointer_t data = tb_large_pool_is_native(pool)? tb_native_large_pool_malloc(pool, size, real __tb_debug_args__) : tb_static_large_pool_malloc(pool, size, real __tb_debug_args__);
    tb_assertf_abort(data, "malloc0(%lu) failed!", size);
    tb_assert_abort(!real || *real >= size);

    // clear it
    tb_memset(data, 0, real? *real : size);

    // ok
    return data;
}
tb_pointer_t tb_large_pool_nalloc_(tb_large_pool_ref_t pool, tb_size_t item, tb_size_t size, tb_size_t* real __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && size, tb_null);
    tb_assert_and_check_return_val((item * size) <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // nalloc data
    tb_pointer_t data = tb_large_pool_is_native(pool)? tb_native_large_pool_malloc(pool, item * size, real __tb_debug_args__) : tb_static_large_pool_malloc(pool, item * size, real __tb_debug_args__);
    tb_assertf_abort(data, "nalloc(%lu, %lu) failed!", item, size);
    tb_assert_abort(!real || *real >= item * size);

    // ok
    return data;
}
tb_pointer_t tb_large_pool_nalloc0_(tb_large_pool_ref_t pool, tb_size_t item, tb_size_t size, tb_size_t* real __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && size, tb_null);
    tb_assert_and_check_return_val((item * size) <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // nalloc0 data
    tb_pointer_t data = tb_large_pool_is_native(pool)? tb_native_large_pool_malloc(pool, item * size, real __tb_debug_args__) : tb_static_large_pool_malloc(pool, item * size, real __tb_debug_args__);
    tb_assertf_abort(data, "nalloc0(%lu, %lu) failed!", item, size);
    tb_assert_abort(!real || *real >= item * size);

    // clear it
    tb_memset(data, 0, real? *real : item * size);

    // ok
    return data;
}
tb_pointer_t tb_large_pool_ralloc_(tb_large_pool_ref_t pool, tb_pointer_t data, tb_size_t size, tb_size_t* real __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && data && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_POOL_DATA_SIZE_MAXN, tb_null);

    // ralloc data
    tb_pointer_t p = tb_large_pool_is_native(pool)? tb_native_large_pool_ralloc(pool, data, size, real __tb_debug_args__) : tb_static_large_pool_ralloc(pool, data, size, real __tb_debug_args__);

    // failed? dump it
#ifdef __tb_debug__
    if (!p) 
    {
        // trace
        tb_trace_e("ralloc(%p, %lu) failed! at %s(): %lu, %s", data, size, func_, line_, file_);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)data, tb_true, "[large_pool]: [error]: ");

        // abort
        tb_abort();
    }
#endif

    // check
    tb_assert_abort(!real || *real >= size);

    // ok
    return p;
}
tb_bool_t tb_large_pool_free_(tb_large_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && data, tb_false);

    // free data
    tb_bool_t ok = tb_large_pool_is_native(pool)? tb_native_large_pool_free(pool, data __tb_debug_args__) : tb_static_large_pool_free(pool, data __tb_debug_args__);

    // failed? dump it
#ifdef __tb_debug__
    if (!ok) 
    {
        // trace
        tb_trace_e("free(%p) failed! at %s(): %lu, %s", data, func_, line_, file_);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)data, tb_true, "[large_pool]: [error]: ");

        // abort
        tb_abort();
    }
#endif

    // ok
    return ok;
}
#ifdef __tb_debug__
tb_void_t tb_large_pool_dump(tb_large_pool_ref_t pool)
{
    // check
    tb_assert_and_check_return(pool);

    // dump the pool
    return tb_large_pool_is_native(pool)? tb_native_large_pool_dump(pool) : tb_static_large_pool_dump(pool);
}
#endif
