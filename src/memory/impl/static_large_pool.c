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
 * @file        static_large_pool.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "static_large_pool"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_large_pool.h"

/*! the static page pool impl type
 *
 * <pre>
 *
 * .e.g pagesize == 4KB
 *
 *        --------------------------------------------------------------------------
 *       |                                     data                                 |
 *        --------------------------------------------------------------------------
 *                                              |
 *        --------------------------------------------------------------------------
 * pool: | head | 4KB | 16KB | 8KB | 128KB | ... | 32KB |       ...       |  4KB*N  |
 *        --------------------------------------------------------------------------
 *                       |                       |               |
 *                       |                       `---------------`
 *                       |                        merge free space when alloc or free
 *                       |
 *        ----------------------------
 *       | tb_pool_data_head_t | data |
 *        ----------------------------
 *                                                
 *        -----------------------
 * pred: | <=4KB :      4KB      |
 *       |-----------------------|
 *       | <=8KB :      8KB      |
 *       |-----------------------|
 *       | <=16KB :   12-16KB    |
 *       |-----------------------|
 *       | <=32KB :   20-32KB    |
 *       |-----------------------|
 *       | <=64KB :   36-64KB    |
 *       |-----------------------|
 *       | <=128KB :  68-128KB   |
 *       |-----------------------|
 *       | <=256KB :  132-256KB  |
 *       |-----------------------|
 *       | <=512KB :  260-512KB  |
 *       |-----------------------|
 *       | >512KB :   516-...KB  |
 *        -----------------------
 *
 * </pre>
 */
typedef __tb_aligned__(TB_POOL_DATA_ALIGN) struct __tb_static_large_pool_impl_t
{
    // the data size
    tb_size_t                       data_size;

    // the data head
    tb_pool_data_head_t*            data_head;

    // the data tail
    tb_pool_data_head_t*            data_tail;

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

}__tb_aligned__(TB_POOL_DATA_ALIGN) tb_static_large_pool_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * malloc implementation
 */
static tb_pool_data_head_t* tb_static_large_pool_malloc_find(tb_static_large_pool_impl_t* impl, tb_pool_data_head_t* data_head, tb_size_t walk_size, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(impl && data_head && size, tb_null);

    // the data tail
    tb_pool_data_head_t* data_tail = impl->data_tail;
    tb_check_return_val(data_head < data_tail, tb_null);

    // find the free data 
    while ((data_head + 1) <= data_tail && walk_size)
    {
        // the data size
        tb_size_t data_size = data_head->size;
            
        // allocate if the data is free
        if (data_head->free)
        {
            // is enough?           
            if (data_size >= size)
            {
                // split it if the free data is too large
                if (data_size > sizeof(tb_pool_data_head_t) + size)
                {
                    // split data_head
                    tb_pool_data_head_t* next_head = (tb_pool_data_head_t*)((tb_byte_t*)(data_head + 1) + size);
                    next_head->size = data_size - size - sizeof(tb_pool_data_head_t);
                    next_head->free = 1;
                    data_head->size = size;
                }
                // use the whole data
                else data_head->size = data_size;

                // alloc the data 
                data_head->free = 0;

                // return the data head
                return data_head;
            }
            else // attempt to merge next free data if the free data_head is too small
            {
                // the next data head
                tb_pool_data_head_t* next_head = (tb_pool_data_head_t*)((tb_byte_t*)(data_head + 1) + data_size);
            
                // break if doesn't exist next data
                if (next_head + 1 >= data_tail) break;

                // the next data is free?
                if (next_head->free)
                {
                    // merge next data
                    data_head->size += sizeof(tb_pool_data_head_t) + next_head->size;

                    // continue handle this data 
                    continue ;
                }
            }
        }

        // walk_size--
        walk_size--;
    
        // skip it if the data is non-free or too small
        data_head = (tb_pool_data_head_t*)((tb_byte_t*)(data_head + 1) + data_size);
    }

    // fail
    return tb_null;
}
static tb_pointer_t tb_static_large_pool_malloc_done(tb_static_large_pool_impl_t* impl, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(impl && impl->data_head, tb_null);

    // done
    tb_byte_t*              data_real = tb_null;
    tb_pool_data_head_t*    data_head = tb_null;
    do
    {
#ifdef __tb_debug__
        // the need size for patching 0xcc
        tb_size_t need = size + 1;
#else
        tb_size_t need = size;
#endif

        // TODO: pred
        // ...

        // find the free data from the first data head 
        data_head = tb_static_large_pool_malloc_find(impl, impl->data_head, -1, need);
        tb_check_break(data_head);

        // the real data
        data_real = (tb_byte_t*)&(data_head[1]);

#ifdef __tb_debug__
        // init the debug info
        data_head->debug.magic     = TB_POOL_DATA_MAGIC;
        data_head->debug.file      = file_;
        data_head->debug.func      = func_;
        data_head->debug.line      = line_;

        // save backtrace
        tb_pool_data_save_backtrace(data_head, 3);

        // make the dirty data and patch 0xcc for checking underflow
        tb_memset(data_real, TB_POOL_DATA_PATCH, need);

        // update the occupied size
        tb_size_t occupied_size = sizeof(tb_pool_data_head_t) - sizeof(tb_pool_data_debug_head_t) + size;
        impl->occupied_size += occupied_size;

        // update total size
        impl->total_size    += size;

        // update peak size
        if (occupied_size > impl->peak_size) impl->peak_size = occupied_size;

        // update malloc count
        impl->malloc_count++;
#endif

    } while (0);

    // ok?
    return data_real;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_large_pool_ref_t tb_static_large_pool_init(tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(data && size, tb_null);
    tb_assert_static(!(sizeof(tb_pool_data_head_t) & (TB_POOL_DATA_ALIGN - 1)));
    tb_assert_static(!(sizeof(tb_static_large_pool_impl_t) & (TB_POOL_DATA_ALIGN - 1)));

    // align data and size
    tb_size_t diff = tb_align((tb_size_t)data, TB_POOL_DATA_ALIGN) - (tb_size_t)data;
    tb_assert_and_check_return_val(size > diff + sizeof(tb_static_large_pool_impl_t) + sizeof(tb_pool_data_head_t), tb_null);
    size -= diff;
    data += diff;

    // init pool
    tb_static_large_pool_impl_t* impl = (tb_static_large_pool_impl_t*)data;
    tb_memset(impl, 0, sizeof(tb_static_large_pool_impl_t));

    // init pagesize
    impl->pagesize = tb_page_size();
    tb_assert_and_check_return_val(impl->pagesize, tb_null);

    // init data size
    impl->data_size = size - sizeof(tb_static_large_pool_impl_t) - sizeof(tb_pool_data_head_t);
    tb_assert_and_check_return_val(impl->data_size > impl->pagesize, tb_null);

    // init data head 
    impl->data_head = (tb_pool_data_head_t*)&impl[1];
    impl->data_head->free = 1;
    impl->data_head->cstr = 0;
    impl->data_head->size = impl->data_size;
    tb_assert_and_check_return_val(!((tb_size_t)impl->data_head & (TB_POOL_DATA_ALIGN - 1)), tb_null);

    // init data tail
    impl->data_tail = (tb_pool_data_head_t*)((tb_byte_t*)&impl->data_head[1] + impl->data_head->size);

    // ok
    return (tb_large_pool_ref_t)impl;
}
tb_void_t tb_static_large_pool_exit(tb_large_pool_ref_t pool)
{
    // check
    tb_static_large_pool_impl_t* impl = (tb_static_large_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // clear it
    tb_static_large_pool_clear(pool);
}
tb_void_t tb_static_large_pool_clear(tb_large_pool_ref_t pool)
{
    // check
    tb_static_large_pool_impl_t* impl = (tb_static_large_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->data_head && impl->data_size > impl->pagesize);

    // clear it
    impl->data_head->free = 1;
    impl->data_head->cstr = 0;
    impl->data_head->size = impl->data_size;
}
tb_pointer_t tb_static_large_pool_malloc(tb_large_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_static_large_pool_impl_t* impl = (tb_static_large_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl, tb_null);

    // done
    return tb_static_large_pool_malloc_done(impl, size __tb_debug_args__);
}
tb_pointer_t tb_static_large_pool_ralloc(tb_large_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    return tb_null;
}
tb_bool_t tb_static_large_pool_free(tb_large_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{
    return tb_false;
}
#ifdef __tb_debug__
tb_void_t tb_static_large_pool_dump(tb_large_pool_ref_t pool)
{
    // check
    tb_static_large_pool_impl_t* impl = (tb_static_large_pool_impl_t*)pool;
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
