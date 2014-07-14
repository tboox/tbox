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
 * @file        pool.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "pool"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pool.h"
#include "large_pool.h"
#include "small_pool.h"
#include "impl/prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the pool impl type
typedef struct __tb_pool_impl_t
{
    // the large pool
    tb_large_pool_ref_t     large_pool;

    // the small pool
    tb_small_pool_ref_t     small_pool;

}tb_pool_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_handle_t tb_pool_instance_init(tb_cpointer_t* ppriv)
{
    // init it
    return tb_pool_init(tb_null);
}
static tb_void_t tb_pool_instance_exit(tb_handle_t pool, tb_cpointer_t priv)
{
#ifdef __tb_debug__
    // dump it
    tb_pool_dump((tb_pool_ref_t)pool);
#endif

    // exit it
    tb_pool_exit((tb_pool_ref_t)pool);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_pool_ref_t tb_pool()
{
    return (tb_pool_ref_t)tb_singleton_instance(TB_SINGLETON_TYPE_POOL, tb_pool_instance_init, tb_pool_instance_exit, tb_null);
}
tb_pool_ref_t tb_pool_init(tb_large_pool_ref_t large_pool)
{
    // done
    tb_bool_t       ok = tb_false;
    tb_pool_impl_t* impl = tb_null;
    do
    {
        // using the default large pool 
        if (!large_pool) large_pool = tb_large_pool();
        tb_assert_and_check_break(large_pool);

        // make pool
        impl = (tb_pool_impl_t*)tb_large_pool_malloc0(large_pool, sizeof(tb_pool_impl_t), tb_null);
        tb_assert_and_check_break(impl);

        // init pool
        impl->large_pool = large_pool;
        impl->small_pool = tb_small_pool_init(large_pool);
        tb_assert_and_check_break(impl->small_pool);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        if (impl) tb_pool_exit((tb_pool_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_pool_ref_t)impl;
}
tb_void_t tb_pool_exit(tb_pool_ref_t pool)
{
    // check
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->large_pool);

    // exit small pool
    if (impl->small_pool) tb_small_pool_exit(impl->small_pool);
    impl->small_pool = tb_null;

    // exit pool
    tb_large_pool_free(impl->large_pool, impl);
}
tb_pointer_t tb_pool_malloc_(tb_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->large_pool && impl->small_pool && size, tb_null);

    // done
    return size <= TB_SMALL_POOL_DATA_SIZE_MAXN? tb_small_pool_malloc_(impl->small_pool, size __tb_debug_args__) : tb_large_pool_malloc_(impl->large_pool, size, tb_null __tb_debug_args__);
}
tb_pointer_t tb_pool_malloc0_(tb_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->large_pool && impl->small_pool && size, tb_null);
    
    // done
    return size <= TB_SMALL_POOL_DATA_SIZE_MAXN? tb_small_pool_malloc0_(impl->small_pool, size __tb_debug_args__) : tb_large_pool_malloc0_(impl->large_pool, size, tb_null __tb_debug_args__);
}
tb_pointer_t tb_pool_nalloc_(tb_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->large_pool && impl->small_pool && size, tb_null);

    // done
    return (item * size) <= TB_SMALL_POOL_DATA_SIZE_MAXN? tb_small_pool_nalloc_(impl->small_pool, item, size __tb_debug_args__) : tb_large_pool_nalloc_(impl->large_pool, item, size, tb_null __tb_debug_args__);
}
tb_pointer_t tb_pool_nalloc0_(tb_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->large_pool && impl->small_pool && size, tb_null);

    // done
    return (item * size) <= TB_SMALL_POOL_DATA_SIZE_MAXN? tb_small_pool_nalloc0_(impl->small_pool, item, size __tb_debug_args__) : tb_large_pool_nalloc0_(impl->large_pool, item, size, tb_null __tb_debug_args__);
}
tb_pointer_t tb_pool_ralloc_(tb_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->large_pool && impl->small_pool && data && size, tb_null);

    // done
    tb_pointer_t data_new = tb_null;
    do
    {
        // the data head
        tb_pool_data_head_t* data_head = &(((tb_pool_data_head_t*)data)[-1]);
        tb_assertf_break(data_head->debug.magic == TB_POOL_DATA_MAGIC, "ralloc invalid data: %p", data);

        // small => small
        if (data_head->size <= TB_SMALL_POOL_DATA_SIZE_MAXN && size <= TB_SMALL_POOL_DATA_SIZE_MAXN)
            data_new = tb_small_pool_ralloc_(impl->small_pool, data, size __tb_debug_args__);
        // small => large
        else if (data_head->size <= TB_SMALL_POOL_DATA_SIZE_MAXN)
        {
            // make the new data
            data_new = tb_large_pool_malloc_(impl->large_pool, size, tb_null __tb_debug_args__);
            tb_assert_and_check_break(data_new);

            // copy the old data
            tb_memcpy(data_new, data, tb_min(data_head->size, size));

            // free the old data
            tb_small_pool_free_(impl->small_pool, data __tb_debug_args__);
        }
        // large => small
        else if (size <= TB_SMALL_POOL_DATA_SIZE_MAXN)
        {
            // make the new data
            data_new = tb_small_pool_malloc_(impl->small_pool, size __tb_debug_args__);
            tb_assert_and_check_break(data_new);

            // copy the old data
            tb_memcpy(data_new, data, tb_min(data_head->size, size));

            // free the old data
            tb_large_pool_free_(impl->large_pool, data __tb_debug_args__);
        }
        // large => large
        else data_new = tb_large_pool_ralloc_(impl->large_pool, data, size, tb_null __tb_debug_args__);

    } while (0);

    // failed? dump it
#ifdef __tb_debug__
    if (!data_new) 
    {
        // trace
        tb_trace_e("ralloc(%p, %lu) failed! at %s(): %lu, %s", data, size, func_, line_, file_);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)data, tb_true, "[pool]: [error]: ");

        // abort
        tb_abort();
    }
#endif

    // ok?
    return data_new;
}
tb_bool_t tb_pool_free_(tb_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->large_pool && impl->small_pool && data, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // the data head
        tb_pool_data_head_t* data_head = &(((tb_pool_data_head_t*)data)[-1]);
        tb_assertf_break(data_head->debug.magic == TB_POOL_DATA_MAGIC, "free invalid data: %p", data);

        // free it
        ok = (data_head->size <= TB_SMALL_POOL_DATA_SIZE_MAXN)? tb_small_pool_free_(impl->small_pool, data __tb_debug_args__) : tb_large_pool_free_(impl->large_pool, data __tb_debug_args__);

    } while (0);

    // failed? dump it
#ifdef __tb_debug__
    if (!ok) 
    {
        // trace
        tb_trace_e("free(%p) failed! at %s(): %lu, %s", data, func_, line_, file_);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)data, tb_true, "[pool]: [error]: ");

        // abort
        tb_abort();
    }
#endif

    // ok?
    return ok;
}
#ifdef __tb_debug__
tb_void_t tb_pool_dump(tb_pool_ref_t pool)
{
    // check
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->large_pool && impl->small_pool);

    // dump pool
    tb_small_pool_dump(impl->small_pool);
    tb_large_pool_dump(impl->large_pool);

}
#endif
