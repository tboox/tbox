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
 * @file        small_pool.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "small_pool"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "small_pool.h"
#include "large_pool.h"
#include "fixed_pool.h"
#include "impl/prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the small pool impl type
typedef struct __tb_small_pool_impl_t
{
    // the large pool
    tb_large_pool_ref_t     large_pool;

    // the fixed pool
    tb_fixed_pool_ref_t     fixed_pool[12];

}tb_small_pool_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_fixed_pool_ref_t tb_small_pool_find_fixed(tb_small_pool_impl_t* impl, tb_size_t size)
{
    // check
    tb_assert_return_val(impl && size && size <= TB_SMALL_POOL_DATA_SIZE_MAXN, tb_null);

    // done
    tb_fixed_pool_ref_t fixed_pool = tb_null;
    do
    {
        // the fixed pool index
        tb_size_t index = 0;
        tb_size_t space = 0;
        if (size > 64 && size < 193)
        {
            if (size < 97)
            {
                index = 3;
                space = 96;
            }
            else if (size > 128)
            {
                index = 5;
                space = 192;
            }
            else 
            {
                index = 4;
                space = 128;
            }
        }
        else if (size > 192 && size < 513)
        {
            if (size < 257)
            {
                index = 6;
                space = 256;
            }
            else if (size > 384)
            {
                index = 8;
                space = 512;
            }
            else 
            {
                index = 7;
                space = 384;
            }
        }
        else if (size < 65)
        {
            if (size < 17)
            {
                index = 0;
                space = 16;
            }
            else if (size > 32)
            {
                index = 2;
                space = 64;
            }
            else 
            {
                index = 1;
                space = 32;
            }
        }
        else 
        {
            if (size < 1025)
            {
                index = 9;
                space = 1024;
            }
            else if (size > 2048)
            {
                index = 11;
                space = 3072;
            }
            else 
            {
                index = 10;
                space = 2048;
            }
        }

        // trace
        tb_trace_d("find: size: %lu => index: %lu, space: %lu", size, index, space);

        // make fixed pool if not exists
        if (!impl->fixed_pool[index]) impl->fixed_pool[index] = tb_fixed_pool_init(impl->large_pool, 0, space, tb_null, tb_null, tb_null);
        tb_assert_and_check_break(impl->fixed_pool[index]);

        // ok
        fixed_pool = impl->fixed_pool[index];

    } while (0);

    // ok?
    return fixed_pool;
}
#ifdef __tb_debug__
static tb_bool_t tb_small_pool_item_check(tb_pointer_t data, tb_cpointer_t priv)
{
    // check
    tb_fixed_pool_ref_t fixed_pool = (tb_fixed_pool_ref_t)priv;
    tb_assert_return_val(fixed_pool && data, tb_false);

    // done 
    tb_bool_t ok = tb_false;
    do
    {
        // the data head
        tb_pool_data_head_t* data_head = &(((tb_pool_data_head_t*)data)[-1]);
        tb_assertf_break(data_head->debug.magic == TB_POOL_DATA_MAGIC, "invalid data: %p", data);

        // the data space
        tb_size_t space = tb_fixed_pool_item_size(fixed_pool);
        tb_assert_and_check_break(space >= data_head->size);

        // check underflow
        tb_assertf_break(space == data_head->size || ((tb_byte_t*)data)[data_head->size] == TB_POOL_DATA_PATCH, "data underflow");

        // ok
        ok = tb_true;

    } while (0);

    // continue?
    return ok;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_small_pool_ref_t tb_small_pool_init(tb_large_pool_ref_t large_pool)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_small_pool_impl_t*   impl = tb_null;
    do
    {
        // using the default large pool 
        if (!large_pool) large_pool = tb_large_pool();
        tb_assert_and_check_break(large_pool);

        // make pool
        impl = (tb_small_pool_impl_t*)tb_large_pool_malloc0(large_pool, sizeof(tb_small_pool_impl_t), tb_null);
        tb_assert_and_check_break(impl);

        // init pool
        impl->large_pool = large_pool;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        if (impl) tb_small_pool_exit((tb_small_pool_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_small_pool_ref_t)impl;
}
tb_void_t tb_small_pool_exit(tb_small_pool_ref_t pool)
{
    // check
    tb_small_pool_impl_t* impl = (tb_small_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->large_pool);

    // exit fixed pool
    tb_size_t i = 0;
    tb_size_t n = tb_arrayn(impl->fixed_pool);
    for (i = 0; i < n; i++)
    {
        // exit it
        if (impl->fixed_pool[i]) tb_fixed_pool_exit(impl->fixed_pool[i]);
        impl->fixed_pool[i] = tb_null;
    }

    // exit pool
    tb_large_pool_free(impl->large_pool, impl);
}
tb_void_t tb_small_pool_clear(tb_small_pool_ref_t pool)
{
    // check
    tb_small_pool_impl_t* impl = (tb_small_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->large_pool);

    // clear fixed pool
    tb_size_t i = 0;
    tb_size_t n = tb_arrayn(impl->fixed_pool);
    for (i = 0; i < n; i++)
    {
        // clear it
        if (impl->fixed_pool[i]) tb_fixed_pool_clear(impl->fixed_pool[i]);
    }
}
tb_pointer_t tb_small_pool_malloc_(tb_small_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_small_pool_impl_t* impl = (tb_small_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->large_pool && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_SMALL_POOL_DATA_SIZE_MAXN, tb_null);

    // done
    tb_pointer_t data = tb_null;
    do
    {
        // the fixed pool
        tb_fixed_pool_ref_t fixed_pool = tb_small_pool_find_fixed(impl, size);
        tb_assert_and_check_break(fixed_pool);

        // done
        data = tb_fixed_pool_malloc_(fixed_pool __tb_debug_args__);
        tb_assert_and_check_break(data);

        // the data head
        tb_pool_data_head_t* data_head = &(((tb_pool_data_head_t*)data)[-1]);
        tb_assert_abort(data_head->debug.magic == TB_POOL_DATA_MAGIC);

#ifdef __tb_debug__
        // fill the patch bytes
        if (data_head->size > size) tb_memset_((tb_byte_t*)data + size, TB_POOL_DATA_PATCH, data_head->size - size);
#endif

        // update size
        data_head->size = size;

    } while (0);

    // check
    tb_assertf_abort(data, "malloc(%lu) failed!", size);

    // ok?
    return data;
}
tb_pointer_t tb_small_pool_malloc0_(tb_small_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_small_pool_impl_t* impl = (tb_small_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->large_pool && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_SMALL_POOL_DATA_SIZE_MAXN, tb_null);
   
    // done
    tb_pointer_t data = tb_null;
    do
    {
        // the fixed pool
        tb_fixed_pool_ref_t fixed_pool = tb_small_pool_find_fixed(impl, size);
        tb_assert_and_check_break(fixed_pool);

        // done
        data = tb_fixed_pool_malloc0_(fixed_pool __tb_debug_args__);
        tb_assert_and_check_break(data);

        // the data head
        tb_pool_data_head_t* data_head = &(((tb_pool_data_head_t*)data)[-1]);
        tb_assert_abort(data_head->debug.magic == TB_POOL_DATA_MAGIC);

#ifdef __tb_debug__
        // fill the patch bytes
        if (data_head->size > size) tb_memset_((tb_byte_t*)data + size, TB_POOL_DATA_PATCH, data_head->size - size);
#endif

        // update size
        data_head->size = size;

    } while (0);

    // check
    tb_assertf_abort(data, "malloc0(%lu) failed!", size);

    // ok?
    return data;
}
tb_pointer_t tb_small_pool_nalloc_(tb_small_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_small_pool_impl_t* impl = (tb_small_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->large_pool && size, tb_null);
    tb_assert_and_check_return_val(item * size <= TB_SMALL_POOL_DATA_SIZE_MAXN, tb_null);

    // done
    tb_pointer_t data = tb_null;
    do
    {
        // the fixed pool
        tb_fixed_pool_ref_t fixed_pool = tb_small_pool_find_fixed(impl, item * size);
        tb_assert_and_check_break(fixed_pool);

        // done
        data = tb_fixed_pool_malloc_(fixed_pool __tb_debug_args__);
        tb_assert_and_check_break(data);

        // the data head
        tb_pool_data_head_t* data_head = &(((tb_pool_data_head_t*)data)[-1]);
        tb_assert_abort(data_head->debug.magic == TB_POOL_DATA_MAGIC);

#ifdef __tb_debug__
        // fill the patch bytes
        if (data_head->size > (item * size)) tb_memset_((tb_byte_t*)data + (item * size), TB_POOL_DATA_PATCH, data_head->size - (item * size));
#endif

        // update size
        data_head->size = item * size;
   
    } while (0);

    // check
    tb_assertf_abort(data, "nalloc(%lu, %lu) failed!", item, size);

    // ok?
    return data;
}
tb_pointer_t tb_small_pool_nalloc0_(tb_small_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_small_pool_impl_t* impl = (tb_small_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->large_pool && size, tb_null);
    tb_assert_and_check_return_val(item * size <= TB_SMALL_POOL_DATA_SIZE_MAXN, tb_null);

    // done
    tb_pointer_t data = tb_null;
    do
    {
        // the fixed pool
        tb_fixed_pool_ref_t fixed_pool = tb_small_pool_find_fixed(impl, item * size);
        tb_assert_and_check_break(fixed_pool);

        // done
        data = tb_fixed_pool_malloc0_(fixed_pool __tb_debug_args__);
        tb_assert_and_check_break(data);

        // the data head
        tb_pool_data_head_t* data_head = &(((tb_pool_data_head_t*)data)[-1]);
        tb_assert_abort(data_head->debug.magic == TB_POOL_DATA_MAGIC);

#ifdef __tb_debug__
        // fill the patch bytes
        if (data_head->size > (item * size)) tb_memset_((tb_byte_t*)data + (item * size), TB_POOL_DATA_PATCH, data_head->size - (item * size));
#endif

        // update size
        data_head->size = item * size;
 
    } while (0);

    // check
    tb_assertf_abort(data, "nalloc(%lu, %lu) failed!", item, size);

    // ok?
    return data;
}
tb_pointer_t tb_small_pool_ralloc_(tb_small_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_small_pool_impl_t* impl = (tb_small_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->large_pool && data && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_SMALL_POOL_DATA_SIZE_MAXN, tb_null);

    // done
    tb_pointer_t data_new = tb_null;
    do
    {
        // the old data head
        tb_pool_data_head_t* data_head_old = &(((tb_pool_data_head_t*)data)[-1]);
        tb_assertf_break(data_head_old->debug.magic == TB_POOL_DATA_MAGIC, "ralloc invalid data: %p", data);

        // the old fixed pool
        tb_fixed_pool_ref_t fixed_pool_old = tb_small_pool_find_fixed(impl, data_head_old->size);
        tb_assert_and_check_break(fixed_pool_old);

        // the old data space
        tb_size_t space_old = tb_fixed_pool_item_size(fixed_pool_old);
        tb_assert_and_check_break(space_old >= data_head_old->size);

        // check underflow
        tb_assertf_break(space_old == data_head_old->size || ((tb_byte_t*)data)[data_head_old->size] == TB_POOL_DATA_PATCH, "data underflow");

        // the new fixed pool
        tb_fixed_pool_ref_t fixed_pool_new = tb_small_pool_find_fixed(impl, size);
        tb_assert_and_check_break(fixed_pool_new);

        // small space?
        if (fixed_pool_old == fixed_pool_new) 
        {
#ifdef __tb_debug__
            // fill the patch bytes
            if (data_head_old->size > size) tb_memset_((tb_byte_t*)data + size, TB_POOL_DATA_PATCH, data_head_old->size - size);
#endif
            // only update size
            data_head_old->size = size;

            // ok
            data_new = data;
            break;
        }

        // make the new data
        data_new = tb_fixed_pool_malloc_(fixed_pool_new __tb_debug_args__);
        tb_assert_and_check_break(data_new);

        // the new data head
        tb_pool_data_head_t* data_head_new = &(((tb_pool_data_head_t*)data_new)[-1]);
        tb_assert_abort(data_head_new->debug.magic == TB_POOL_DATA_MAGIC);

#ifdef __tb_debug__
        // fill the patch bytes
        if (data_head_new->size > size) tb_memset_((tb_byte_t*)data_new + size, TB_POOL_DATA_PATCH, data_head_new->size - size);
#endif

        // update size
        data_head_new->size = size;

        // copy the old data
        tb_memcpy_(data_new, data, tb_min(data_head_old->size, size));

        // free the old data
        tb_fixed_pool_free_(fixed_pool_old, data __tb_debug_args__);

    } while (0);

    // failed? dump it
#ifdef __tb_debug__
    if (!data_new) 
    {
        // trace
        tb_trace_e("ralloc(%p, %lu) failed! at %s(): %lu, %s", data, size, func_, line_, file_);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)data, tb_true, "[small_pool]: [error]: ");

        // abort
        tb_abort();
    }
#endif

    // ok
    return data_new;
}
tb_bool_t tb_small_pool_free_(tb_small_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_small_pool_impl_t* impl = (tb_small_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->large_pool && data, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // the data head
        tb_pool_data_head_t* data_head = &(((tb_pool_data_head_t*)data)[-1]);
        tb_assertf_break(data_head->debug.magic == TB_POOL_DATA_MAGIC, "free invalid data: %p", data);

        // the fixed pool
        tb_fixed_pool_ref_t fixed_pool = tb_small_pool_find_fixed(impl, data_head->size);
        tb_assert_and_check_break(fixed_pool);

        // the data space
        tb_size_t space = tb_fixed_pool_item_size(fixed_pool);
        tb_assert_and_check_break(space >= data_head->size);

        // check underflow
        tb_assertf_break(space == data_head->size || ((tb_byte_t*)data)[data_head->size] == TB_POOL_DATA_PATCH, "data underflow");

        // done
        ok = tb_fixed_pool_free_(fixed_pool, data __tb_debug_args__);

    } while (0);

    // failed? dump it
#ifdef __tb_debug__
    if (!ok) 
    {
        // trace
        tb_trace_e("free(%p) failed! at %s(): %lu, %s", data, func_, line_, file_);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)data, tb_true, "[small_pool]: [error]: ");

        // abort
        tb_abort();
    }
#endif

    // ok?
    return ok;
}
#ifdef __tb_debug__
tb_void_t tb_small_pool_dump(tb_small_pool_ref_t pool)
{
    // check
    tb_small_pool_impl_t* impl = (tb_small_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->large_pool);

    // trace
    tb_trace_i("");

    // dump fixed pool
    tb_size_t i = 0;
    tb_size_t n = tb_arrayn(impl->fixed_pool);
    for (i = 0; i < n; i++)
    {
        // exists?
        if (impl->fixed_pool[i]) 
        {
            // check it
            tb_fixed_pool_walk(impl->fixed_pool[i], tb_small_pool_item_check, (tb_cpointer_t)impl->fixed_pool[i]);

            // dump it
            tb_fixed_pool_dump(impl->fixed_pool[i]);
        }
    }
}
#endif
