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
 * @file        static_page_pool.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "static_page_pool"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_page_pool.h"

/*! the static page pool impl type
 *
 */
typedef __tb_aligned__(TB_POOL_DATA_ALIGN) struct __tb_static_page_pool_impl_t
{
    // the data
    tb_byte_t*                      data;

    // the size
    tb_size_t                       size;

    // the page size
    tb_size_t                       pagesize;

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

}__tb_aligned__(TB_POOL_DATA_ALIGN) tb_static_page_pool_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_page_pool_ref_t tb_static_page_pool_init(tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(data && size, tb_null);
    tb_assert_static(!(sizeof(tb_static_page_pool_impl_t) & (TB_POOL_DATA_ALIGN - 1)));

    // align data and size
    tb_size_t diff = tb_align((tb_size_t)data, TB_POOL_DATA_ALIGN) - (tb_size_t)data;
    tb_assert_and_check_return_val(size > diff + sizeof(tb_static_page_pool_impl_t), tb_null);
    size -= diff;
    data += diff;

    // init pool
    tb_static_page_pool_impl_t* impl = (tb_static_page_pool_impl_t*)data;
    tb_memset(impl, 0, sizeof(tb_static_page_pool_impl_t));

    // init pagesize
    impl->pagesize = tb_page_size();
    tb_assert_and_check_return_val(impl->pagesize, tb_null);

    // init data 
    impl->data = (tb_byte_t*)&impl[1];
    tb_assert_and_check_return_val(!((tb_size_t)impl->data & (TB_POOL_DATA_ALIGN - 1)), tb_null);

    // init size
    impl->size = size - sizeof(tb_static_page_pool_impl_t);
    tb_assert_and_check_return_val(impl->size > impl->pagesize, tb_null);

    // ok
    return (tb_page_pool_ref_t)impl;
}
tb_void_t tb_static_page_pool_exit(tb_page_pool_ref_t pool)
{
    // check
    tb_static_page_pool_impl_t* impl = (tb_static_page_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // clear it
    tb_static_page_pool_clear(pool);
}
tb_void_t tb_static_page_pool_clear(tb_page_pool_ref_t pool)
{
}
tb_pointer_t tb_static_page_pool_malloc(tb_page_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    return tb_null;
}
tb_pointer_t tb_static_page_pool_ralloc(tb_page_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    return tb_null;
}
tb_bool_t tb_static_page_pool_free(tb_page_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{
    return tb_false;
}
#ifdef __tb_debug__
tb_void_t tb_static_page_pool_dump(tb_page_pool_ref_t pool)
{
    // check
    tb_static_page_pool_impl_t* impl = (tb_static_page_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // trace
    tb_trace_i("======================================================================");

    // trace debug info
    tb_trace_i("peak_size: %lu",            impl->peak_size);
    tb_trace_i("wast_rate: %llu/10000",     impl->occupied_size? (((tb_hize_t)impl->occupied_size - impl->total_size) * 10000) / (tb_hize_t)impl->occupied_size : 0);
    tb_trace_i("free_count: %lu",           impl->free_count);
    tb_trace_i("malloc_count: %lu",         impl->malloc_count);
    tb_trace_i("ralloc_count: %lu",         impl->ralloc_count);

    // trace
    tb_trace_i("======================================================================");
}
#endif
