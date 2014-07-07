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

// the native page pool data type
typedef struct __tb_native_page_pool_data_t
{
    // the data base
    tb_pool_data_t                  base;

    // the entry
    tb_list_entry_t                 entry;

}tb_native_page_pool_data_t;

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
        // make pool
        impl = (tb_native_page_pool_impl_t*)tb_native_memory_malloc0(sizeof(tb_native_page_pool_impl_t));
        tb_assert_and_check_break(impl);

        // init pages
        tb_list_entry_init(&impl->pages, tb_native_page_pool_data_t, entry, tb_null);

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

    // clear all pages
    tb_native_page_pool_clear(pool);

    // exit it
    tb_native_memory_free(impl);
}
tb_void_t tb_native_page_pool_clear(tb_page_pool_ref_t pool)
{
}
tb_pointer_t tb_native_page_pool_malloc(tb_page_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_native_page_pool_impl_t* impl = tb_native_page_pool_impl(pool);
    tb_assert_and_check_return_val(impl && impl->pagesize, tb_null);

    // done 
    tb_bool_t                   ok = tb_false;
    tb_size_t                   need = size + impl->pagesize;
    tb_byte_t*                  data = tb_null;
    tb_byte_t*                  data_aligned = tb_null;
    tb_native_page_pool_data_t* pool_data = tb_null;
    tb_bool_t                   pool_data_at_head = tb_false;
    do
    {
        // need++ for patching 0xcc
#ifdef __tb_debug__
        need++;
#endif

        // make data
        data = (tb_byte_t*)tb_native_memory_malloc(need);
        tb_assert_and_check_break(data);

        // the aligned data
        data_aligned = (tb_byte_t*)tb_align((tb_size_t)data, impl->pagesize);
        tb_assert_and_check_break(data_aligned >= data);

        // make the pool data
        if (data_aligned - data >= sizeof(tb_native_page_pool_data_t))
        {
            // make data
            pool_data = &(((tb_native_page_pool_data_t*)data_aligned)[-1]);

#ifdef __tb_debug__
            // save the real size
            pool_data->base.real = need;
#endif

            // the pool data is at the head?
            pool_data_at_head = tb_true;
        }
        else 
        {
            // make data
            pool_data = (tb_native_page_pool_data_t*)tb_native_memory_malloc0(sizeof(tb_native_page_pool_data_t));

#ifdef __tb_debug__
            // save the real size
            pool_data->base.real = need + sizeof(tb_native_page_pool_data_t);
#endif
        }
        tb_assert_and_check_break(pool_data);

        // init the pool data
        pool_data->base.data    = (tb_pointer_t)data;
        pool_data->base.size    = (tb_uint32_t)size;
        pool_data->base.type    = TB_POOL_DATA_TYPE_PAGE;
#ifdef __tb_debug__
        pool_data->base.magic   = TB_POOL_DATA_MAGIC;
        pool_data->base.file    = file_;
        pool_data->base.func    = func_;
        pool_data->base.line    = line_;

        // save backtrace
        tb_pool_data_save_backtrace(pool_data, 2);

        // patch 0xcc for checking memory underflow
        data_aligned[size] = TB_POOL_DATA_PATCH;
#endif

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit the pool data
        if (pool_data && !pool_data_at_head)
            tb_native_memory_free(pool_data);
        pool_data = tb_null;

        // exit the aligned data
        data_aligned = tb_null;

        // exit the data
        if (data) tb_native_memory_free(data);
        data = tb_null;
    }

    // ok?
    return (tb_pointer_t)data_aligned;
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
//    tb_native_page_pool_data_t* pool_data = tb_null;
    do
    {
        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}
#ifdef __tb_debug__
tb_size_t tb_native_page_pool_data_size(tb_page_pool_ref_t pool, tb_cpointer_t data)
{
    return 0;
}
tb_void_t tb_native_page_pool_data_dump(tb_page_pool_ref_t pool, tb_cpointer_t data, tb_char_t const* prefix)
{

}
tb_void_t tb_native_page_pool_dump(tb_page_pool_ref_t pool)
{
}
#endif
