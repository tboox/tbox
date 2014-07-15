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
 * @file        native_large_pool.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "native_large_pool"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "native_large_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the native large pool ref 
#define tb_native_large_pool_ref(pool)   ((tb_large_pool_ref_t)((tb_size_t)(pool) | 0x1))

// the native large pool impl 
#define tb_native_large_pool_impl(pool)  ((tb_native_large_pool_impl_t*)((tb_size_t)(pool) & ~0x1))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the native large data head type
typedef __tb_aligned__(TB_POOL_DATA_ALIGN) struct __tb_native_large_data_head_t
{
    // the pool reference
    tb_pointer_t                    pool;

    // the entry
    tb_list_entry_t                 entry;

    // the data head base
    tb_pool_data_head_t             base;

}__tb_aligned__(TB_POOL_DATA_ALIGN) tb_native_large_data_head_t;

/*! the native large pool impl type
 *
 * <pre>
 *        -----------       -----------               -----------
 * pool: |||  data   | <=> |||  data   | <=> ... <=> |||  data   | <=> |
 *        -----------       -----------               -----------      |
 *              |                                                      |
 *              `------------------------------------------------------`
 * </pre>
 */
typedef struct __tb_native_large_pool_impl_t
{
    // the lock
    tb_spinlock_t                   lock;

    // the data list
    tb_list_entry_head_t            data_list;

#ifdef __tb_debug__

    // the peak size
    tb_size_t                       peak_size;

    // the total size
    tb_size_t                       total_size;

    // the occupied size
    tb_size_t                       occupied_size;

    // the malloc count
    tb_size_t                       malloc_count;

    // the ralloc count
    tb_size_t                       ralloc_count;

    // the free count
    tb_size_t                       free_count;
#endif

}tb_native_large_pool_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
#ifdef __tb_debug__
static tb_void_t tb_native_large_pool_check_data(tb_native_large_pool_impl_t* impl, tb_native_large_data_head_t const* data_head)
{
    // check
    tb_assert_and_check_return(impl && data_head);

    // done
    tb_bool_t           ok = tb_false;
    tb_byte_t const*    data = (tb_byte_t const*)&(data_head[1]);
    do
    {
        // check
        tb_assertf_break(data_head->base.debug.magic != (tb_uint16_t)~TB_POOL_DATA_MAGIC, "data have been freed: %p", data);
        tb_assertf_break(data_head->base.debug.magic == TB_POOL_DATA_MAGIC, "the invalid data: %p", data);
        tb_assertf_break(((tb_byte_t*)data)[data_head->base.size] == TB_POOL_DATA_PATCH, "data underflow");

        // ok
        ok = tb_true;

    } while (0);

    // failed? dump it
#ifdef __tb_debug__
    if (!ok) 
    {
        // dump data
        tb_pool_data_dump(data, tb_true, "[native_large_pool]: [error]: ");

        // abort
        tb_abort();
    }
#endif
}
static tb_void_t tb_native_large_pool_check_last(tb_native_large_pool_impl_t* impl)
{
    // check
    tb_assert_and_check_return(impl);

    // non-empty?
    if (!tb_list_entry_is_null(&impl->data_list))
    {
        // the last entry
        tb_list_entry_ref_t data_last = tb_list_entry_last(&impl->data_list);
        tb_assert_and_check_return(data_last);

        // check it
        tb_native_large_pool_check_data(impl, (tb_native_large_data_head_t*)tb_list_entry(&impl->data_list, data_last));
    }
}
static tb_void_t tb_native_large_pool_check_prev(tb_native_large_pool_impl_t* impl, tb_native_large_data_head_t const* data_head)
{
    // check
    tb_assert_and_check_return(impl && data_head);

    // non-empty?
    if (!tb_list_entry_is_null(&impl->data_list))
    {
        // the prev entry
        tb_list_entry_ref_t data_prev = tb_list_entry_prev(&impl->data_list, (tb_list_entry_ref_t)&data_head->entry);
        tb_assert_and_check_return(data_prev);

        // not tail entry
        tb_check_return(data_prev != tb_list_entry_tail(&impl->data_list));

        // check it
        tb_native_large_pool_check_data(impl, (tb_native_large_data_head_t*)tb_list_entry(&impl->data_list, data_prev));
    }
}
static tb_void_t tb_native_large_pool_check_next(tb_native_large_pool_impl_t* impl, tb_native_large_data_head_t const* data_head)
{
    // check
    tb_assert_and_check_return(impl && data_head);

    // non-empty?
    if (!tb_list_entry_is_null(&impl->data_list))
    {
        // the next entry
        tb_list_entry_ref_t data_next = tb_list_entry_next(&impl->data_list, (tb_list_entry_ref_t)&data_head->entry);
        tb_assert_and_check_return(data_next);

        // not tail entry
        tb_check_return(data_next != tb_list_entry_tail(&impl->data_list));

        // check it
        tb_native_large_pool_check_data(impl, (tb_native_large_data_head_t*)tb_list_entry(&impl->data_list, data_next));
    }
}
#endif
static tb_bool_t tb_native_large_pool_free_done(tb_native_large_pool_impl_t* impl, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(impl && data, tb_false);

    // done
    tb_bool_t                       ok = tb_false;
    tb_native_large_data_head_t*    data_head = tb_null;
    do
    {
        // the data head
        data_head = &(((tb_native_large_data_head_t*)data)[-1]);
        tb_assertf_break(data_head->base.debug.magic != (tb_uint16_t)~TB_POOL_DATA_MAGIC, "double free data: %p", data);
        tb_assertf_break(data_head->base.debug.magic == TB_POOL_DATA_MAGIC, "free invalid data: %p", data);
        tb_assertf_and_check_break(data_head->pool == (tb_pointer_t)tb_native_large_pool_ref(impl), "the data: %p not belong to pool: %p", data, tb_native_large_pool_ref(impl));
        tb_assertf_break(((tb_byte_t*)data)[data_head->base.size] == TB_POOL_DATA_PATCH, "data underflow");

#ifdef __tb_debug__
        // check the last data
        tb_native_large_pool_check_last(impl);

        // check the prev data
        tb_native_large_pool_check_prev(impl, data_head);

        // check the next data
        tb_native_large_pool_check_next(impl, data_head);

        // for checking double-free
        data_head->base.debug.magic = (tb_uint16_t)~TB_POOL_DATA_MAGIC;

        // update the occupied size
        impl->occupied_size -= sizeof(tb_native_large_data_head_t) - TB_POOL_DATA_HEAD_DIFF_SIZE + data_head->base.size;
 
        // update the total size
        impl->total_size    -= data_head->base.size;
   
        // update the free count
        impl->free_count++;
#endif

        // remove the data from the data_list
        tb_list_entry_remove(&impl->data_list, &data_head->entry);

        // free it
        tb_native_memory_free(data_head);

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_large_pool_ref_t tb_native_large_pool_init()
{
    // done
    tb_bool_t                       ok = tb_false;
    tb_native_large_pool_impl_t*    impl = tb_null;
    do
    {
        // check
        tb_assert_static(!(sizeof(tb_native_large_data_head_t) & (TB_POOL_DATA_ALIGN - 1)));

        // make pool
        impl = (tb_native_large_pool_impl_t*)tb_native_memory_malloc0(sizeof(tb_native_large_pool_impl_t));
        tb_assert_and_check_break(impl);

        // init lock
        if (!tb_spinlock_init(&impl->lock)) break;

        // init data_list
        tb_list_entry_init(&impl->data_list, tb_native_large_data_head_t, entry, tb_null);

        // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
        tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&impl->lock, TB_TRACE_MODULE_NAME);
#endif

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_native_large_pool_exit(tb_native_large_pool_ref(impl));
        impl = tb_null;
    }

    // ok?
    return tb_native_large_pool_ref(impl);
}
tb_void_t tb_native_large_pool_exit(tb_large_pool_ref_t pool)
{
    // check
    tb_native_large_pool_impl_t* impl = tb_native_large_pool_impl(pool);
    tb_assert_and_check_return(impl);

    // clear it
    tb_native_large_pool_clear(pool);

    // exit lock
    tb_spinlock_exit(&impl->lock);

    // exit it
    tb_native_memory_free(impl);
}
tb_void_t tb_native_large_pool_clear(tb_large_pool_ref_t pool)
{
    // check
    tb_native_large_pool_impl_t* impl = tb_native_large_pool_impl(pool);
    tb_assert_and_check_return(impl);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done
    do
    {
        // the iterator
        tb_iterator_ref_t iterator = tb_list_entry_itor(&impl->data_list);
        tb_assert_and_check_break(iterator);

        // walk it
        tb_size_t itor = tb_iterator_head(iterator);
        while (itor != tb_iterator_tail(iterator))
        {
            // the data head
            tb_native_large_data_head_t* data_head = (tb_native_large_data_head_t*)tb_iterator_item(iterator, itor);
            tb_assert_and_check_break(data_head);

            // save next
            tb_size_t next = tb_iterator_next(iterator, itor);

            // exit data
            tb_native_large_pool_free_done(impl, (tb_pointer_t)&data_head[1] __tb_debug_vals__);

            // next
            itor = next;
        }

    } while (0);

    // leave
    tb_spinlock_leave(&impl->lock);
}
tb_pointer_t tb_native_large_pool_malloc(tb_large_pool_ref_t pool, tb_size_t size, tb_size_t* real __tb_debug_decl__)
{
    // check
    tb_native_large_pool_impl_t* impl = tb_native_large_pool_impl(pool);
    tb_assert_and_check_return_val(impl, tb_null);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done 
#ifdef __tb_debug__
    tb_size_t                       patch = 1; // patch 0xcc
#else
    tb_size_t                       patch = 0;
#endif
    tb_bool_t                       ok = tb_false;
    tb_size_t                       need = sizeof(tb_native_large_data_head_t) + size + patch;
    tb_byte_t*                      data = tb_null;
    tb_byte_t*                      data_real = tb_null;
    tb_native_large_data_head_t*    data_head = tb_null;
    do
    {
#ifdef __tb_debug__
        // check the last data
        tb_native_large_pool_check_last(impl);
#endif

        // make data
        data = (tb_byte_t*)tb_native_memory_malloc(need);
        tb_assert_and_check_break(data);
        tb_assert_and_check_break(!(((tb_size_t)data) & 0x1));

        // make the real data
        data_real = data + sizeof(tb_native_large_data_head_t);

        // init the data head
        data_head = (tb_native_large_data_head_t*)data;
        data_head->base.size            = (tb_uint32_t)size;
#ifdef __tb_debug__
        data_head->base.debug.magic     = TB_POOL_DATA_MAGIC;
        data_head->base.debug.file      = file_;
        data_head->base.debug.func      = func_;
        data_head->base.debug.line      = line_;

        // save backtrace
        tb_pool_data_save_backtrace(&data_head->base, 2);

        // make the dirty data and patch 0xcc for checking underflow
        tb_memset_(data_real, TB_POOL_DATA_PATCH, size + patch);
#endif

        // save pool reference for checking data range
        data_head->pool = (tb_pointer_t)pool;

        // save the data to the data_list
        tb_list_entry_insert_tail(&impl->data_list, &data_head->entry);

        // save the real size
        if (real) *real = size;

#ifdef __tb_debug__
        // update the occupied size
        impl->occupied_size += need - TB_POOL_DATA_HEAD_DIFF_SIZE - patch;

        // update the total size
        impl->total_size    += size;

        // update the peak size
        if (impl->total_size > impl->peak_size) impl->peak_size = impl->total_size;

        // update the malloc count
        impl->malloc_count++;
#endif

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit the data
        if (data) tb_native_memory_free(data);
        data = tb_null;
        data_real = tb_null;
    }

    // leave
    tb_spinlock_leave(&impl->lock);

    // ok?
    return (tb_pointer_t)data_real;
}
tb_pointer_t tb_native_large_pool_ralloc(tb_large_pool_ref_t pool, tb_pointer_t data, tb_size_t size, tb_size_t* real __tb_debug_decl__)
{
    // check
    tb_native_large_pool_impl_t* impl = tb_native_large_pool_impl(pool);
    tb_assert_and_check_return_val(impl, tb_null);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done 
#ifdef __tb_debug__
    tb_size_t                       patch = 1; // patch 0xcc
#else
    tb_size_t                       patch = 0;
#endif
    tb_bool_t                       ok = tb_false;
    tb_bool_t                       removed = tb_false;
    tb_size_t                       need = sizeof(tb_native_large_data_head_t) + size + patch;
    tb_byte_t*                      data_real = tb_null;
    tb_native_large_data_head_t*    data_head = tb_null;
    do
    {
        // the data head
        data_head = &(((tb_native_large_data_head_t*)data)[-1]);
        tb_assertf_break(data_head->base.debug.magic != (tb_uint16_t)~TB_POOL_DATA_MAGIC, "ralloc freed data: %p", data);
        tb_assertf_break(data_head->base.debug.magic == TB_POOL_DATA_MAGIC, "ralloc invalid data: %p", data);
        tb_assertf_and_check_break(data_head->pool == (tb_pointer_t)pool, "the data: %p not belong to pool: %p", data, pool);
        tb_assertf_break(((tb_byte_t*)data)[data_head->base.size] == TB_POOL_DATA_PATCH, "data underflow");

#ifdef __tb_debug__
        // check the last data
        tb_native_large_pool_check_last(impl);

        // check the prev data
        tb_native_large_pool_check_prev(impl, data_head);

        // check the next data
        tb_native_large_pool_check_next(impl, data_head);

        // update the occupied size
        impl->occupied_size -= data_head->base.size;
 
        // update the total size
        impl->total_size -= data_head->base.size;

        // the previous size
        tb_size_t prev_size = data_head->base.size;
#endif

        // remove the data from the data_list
        tb_list_entry_remove(&impl->data_list, &data_head->entry);
        removed = tb_true;

        // ralloc data
        data = (tb_byte_t*)tb_native_memory_ralloc(data_head, need);
        tb_assert_and_check_break(data);
        tb_assert_and_check_break(!(((tb_size_t)data) & 0x1));

        // update the real data
        data_real = data + sizeof(tb_native_large_data_head_t);

        // update the data head
        data_head = (tb_native_large_data_head_t*)data;
        data_head->base.size            = (tb_uint32_t)size;
#ifdef __tb_debug__
        data_head->base.debug.file      = file_;
        data_head->base.debug.func      = func_;
        data_head->base.debug.line      = line_;

        // check
        tb_assertf_break(data_head->base.debug.magic == TB_POOL_DATA_MAGIC, "ralloc data have been changed: %p", data);

        // update backtrace
        tb_pool_data_save_backtrace(&data_head->base, 2);

        // make the dirty data 
        if (size > prev_size) tb_memset_(data_real + prev_size, TB_POOL_DATA_PATCH, size - prev_size);

        // patch 0xcc for checking underflow
        data_real[size] = TB_POOL_DATA_PATCH;
#endif

        // save the data to the data_list
        tb_list_entry_insert_tail(&impl->data_list, &data_head->entry);
        removed = tb_false;

        // save the real size
        if (real) *real = size;

#ifdef __tb_debug__
        // update the occupied size
        impl->occupied_size += size;

        // update the total size
        impl->total_size    += size;

        // update the peak size
        if (impl->total_size > impl->peak_size) impl->peak_size = impl->total_size;

        // update the ralloc count
        impl->ralloc_count++;
#endif

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // restore data to data_list
        if (data_head && removed) tb_list_entry_insert_tail(&impl->data_list, &data_head->entry);

        // clear it
        data = tb_null;
        data_real = tb_null;
    }

    // leave
    tb_spinlock_leave(&impl->lock);

    // ok?
    return (tb_pointer_t)data_real;
}
tb_bool_t tb_native_large_pool_free(tb_large_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_native_large_pool_impl_t* impl = tb_native_large_pool_impl(pool);
    tb_assert_and_check_return_val(impl && data, tb_false);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done
    tb_bool_t ok = tb_native_large_pool_free_done(impl, data __tb_debug_args__);

    // leave
    tb_spinlock_leave(&impl->lock);

    // ok?
    return ok;
}
#ifdef __tb_debug__
tb_void_t tb_native_large_pool_dump(tb_large_pool_ref_t pool)
{
    // check
    tb_native_large_pool_impl_t* impl = tb_native_large_pool_impl(pool);
    tb_assert_and_check_return(impl);

    // enter
    tb_spinlock_enter(&impl->lock);

    // trace
    tb_trace_i("");

    // exit all data_list
    tb_for_all_if (tb_native_large_data_head_t*, data_head, tb_list_entry_itor(&impl->data_list), data_head)
    {
        // check it
        tb_native_large_pool_check_data(impl, data_head);

        // trace
        tb_trace_e("leak: %p", &data_head[1]);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)&data_head[1], tb_false, "[native_large_pool]: [error]: ");
    }

    // trace debug info
    tb_trace_i("peak_size: %lu",            impl->peak_size);
    tb_trace_i("wast_rate: %llu/10000",     impl->occupied_size? (((tb_hize_t)impl->occupied_size - impl->total_size) * 10000) / (tb_hize_t)impl->occupied_size : 0);
    tb_trace_i("free_count: %lu",           impl->free_count);
    tb_trace_i("malloc_count: %lu",         impl->malloc_count);
    tb_trace_i("ralloc_count: %lu",         impl->ralloc_count);

    // leave
    tb_spinlock_leave(&impl->lock);

}
#endif
