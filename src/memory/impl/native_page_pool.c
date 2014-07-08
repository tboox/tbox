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
 * @file        native_page_pool.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "native_page_pool"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "native_page_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the native page pool ref 
#define tb_native_page_pool_ref(pool)   ((tb_page_pool_ref_t)((tb_size_t)(pool) | 0x1))

// the native page pool impl 
#define tb_native_page_pool_impl(pool)  ((tb_native_page_pool_impl_t*)((tb_size_t)(pool) & ~0x1))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the native page data head type
typedef __tb_aligned__(TB_POOL_DATA_ALIGN) struct __tb_native_page_data_head_t
{
    // the pool reference
    tb_pointer_t                    pool;

    // the entry
    tb_list_entry_t                 entry;

    // the data head base
    tb_pool_data_head_t             base;

}__tb_aligned__(TB_POOL_DATA_ALIGN) tb_native_page_data_head_t;

// the native page pool impl type
typedef struct __tb_native_page_pool_impl_t
{
    // the pages
    tb_list_entry_head_t            pages;

    // the page size
    tb_size_t                       pagesize;

}tb_native_page_pool_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_page_pool_ref_t tb_native_page_pool_init()
{
    // done
    tb_bool_t                   ok = tb_false;
    tb_native_page_pool_impl_t* impl = tb_null;
    do
    {
        // check
        tb_assert_static(!(sizeof(tb_native_page_data_head_t) & (TB_POOL_DATA_ALIGN - 1)));

        // make pool
        impl = (tb_native_page_pool_impl_t*)tb_native_memory_malloc0(sizeof(tb_native_page_pool_impl_t));
        tb_assert_and_check_break(impl);

        // init pages
        tb_list_entry_init(&impl->pages, tb_native_page_data_head_t, entry, tb_null);

        // init pagesize
        impl->pagesize = tb_page_size();
        tb_assert_and_check_break(impl->pagesize);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_native_page_pool_exit(tb_native_page_pool_ref(impl));
        impl = tb_null;
    }

    // ok?
    return tb_native_page_pool_ref(impl);
}
tb_void_t tb_native_page_pool_exit(tb_page_pool_ref_t pool)
{
    // check
    tb_native_page_pool_impl_t* impl = tb_native_page_pool_impl(pool);
    tb_assert_and_check_return(impl);

    // clear it
    tb_native_page_pool_clear(pool);

    // exit it
    tb_native_memory_free(impl);
}
tb_void_t tb_native_page_pool_clear(tb_page_pool_ref_t pool)
{
    // check
    tb_native_page_pool_impl_t* impl = tb_native_page_pool_impl(pool);
    tb_assert_and_check_return(impl);

    // walk it
    tb_for_all_if (tb_native_page_data_head_t*, data_head, tb_list_entry_itor(&impl->pages), data_head)
    {
        // exit data
        tb_native_page_pool_free(pool, (tb_pointer_t)&data_head[1] __tb_debug_vals__);
    }
}
tb_pointer_t tb_native_page_pool_malloc(tb_page_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_native_page_pool_impl_t* impl = tb_native_page_pool_impl(pool);
    tb_assert_and_check_return_val(impl && impl->pagesize, tb_null);

    // done 
#ifdef __tb_debug__
    tb_size_t                   patch = 1; // patch 0xcc
#else
    tb_size_t                   patch = 0;
#endif
    tb_bool_t                   ok = tb_false;
    tb_size_t                   need = sizeof(tb_native_page_data_head_t) + size + patch;
    tb_byte_t*                  data = tb_null;
    tb_byte_t*                  data_real = tb_null;
    tb_native_page_data_head_t* data_head = tb_null;
    do
    {
        // make data
        data = (tb_byte_t*)tb_native_memory_malloc(need);
        tb_assert_and_check_break(data);
        tb_assert_and_check_break(!(((tb_size_t)data) & 0x1));

        // make the real data
        data_real = data + sizeof(tb_native_page_data_head_t);

        // make the data head
        data_head = (tb_native_page_data_head_t*)data;

        // init the data head
        data_head->base.size            = (tb_uint32_t)size;
        data_head->base.cstr            = 0;
        data_head->base.free            = 0;
#ifdef __tb_debug__
        data_head->base.debug.magic     = TB_POOL_DATA_MAGIC;
        data_head->base.debug.file      = file_;
        data_head->base.debug.func      = func_;
        data_head->base.debug.line      = line_;
        data_head->base.debug.real      = need - sizeof(tb_pool_data_debug_head_t);

        // save backtrace
        tb_pool_data_save_backtrace(&data_head->base, 2);

        // make the dirty data and patch 0xcc for checking underflow
        tb_memset(data_real, TB_POOL_DATA_PATCH, size + 1);
#endif

        // save pool reference for checking data range
        data_head->pool = (tb_pointer_t)pool;

        // save the data to the pages
        tb_list_entry_insert_tail(&impl->pages, &data_head->entry);

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

    // ok?
    return (tb_pointer_t)data_real;
}
tb_pointer_t tb_native_page_pool_ralloc(tb_page_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    return tb_null;
}
tb_bool_t tb_native_page_pool_free(tb_page_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_native_page_pool_impl_t* impl = tb_native_page_pool_impl(pool);
    tb_assert_and_check_return_val(impl && impl->pagesize, tb_false);

    // done
    tb_bool_t                   ok = tb_false;
    tb_native_page_data_head_t* data_head = tb_null;
    do
    {
        // no data?
        tb_assert_and_check_break(data);

        // the data head
        data_head = &(((tb_native_page_data_head_t*)data)[-1]);
        tb_assertf_break(data_head->base.debug.magic != (tb_uint16_t)~TB_POOL_DATA_MAGIC, "double free data: %p", data);
        tb_assertf_break(data_head->base.debug.magic == TB_POOL_DATA_MAGIC, "free invalid data: %p", data);
        tb_assertf_and_check_break(data_head->pool == (tb_pointer_t)pool, "the data: %p not belong to pool: %p", data, pool);
        tb_assertf_break(((tb_byte_t*)data)[data_head->base.size] == TB_POOL_DATA_PATCH, "data underflow");

        // remove the data from the pages
        tb_list_entry_remove(&impl->pages, &data_head->entry);

#ifdef __tb_debug__
        // clear magic for checking double-free
        data_head->base.debug.magic = (tb_uint16_t)~TB_POOL_DATA_MAGIC;
#endif

        // free it
        tb_native_memory_free(data_head);

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}
#ifdef __tb_debug__
tb_void_t tb_native_page_pool_dump(tb_page_pool_ref_t pool)
{
    // check
    tb_native_page_pool_impl_t* impl = tb_native_page_pool_impl(pool);
    tb_assert_and_check_return(impl);

    // trace
    tb_trace_i("======================================================================");

    // exit all pages
    tb_for_all_if (tb_native_page_data_head_t*, data_head, tb_list_entry_itor(&impl->pages), data_head)
    {
        // trace
        tb_trace_e("leak: %p", &data_head[1]);

        // dump data
        tb_pool_data_dump((tb_byte_t const*)&data_head[1], tb_false, "[native_page_pool]: [error]: ");
    }

    // TODO: dump wast, peak, used, frag
    tb_trace_i("peak: %lu", 0);
    tb_trace_i("used: %lu", 0);
    tb_trace_i("frag: %lu", 0);
    tb_trace_i("wait: %lu%%", 0);

    // trace
    tb_trace_i("======================================================================");
}
#endif
