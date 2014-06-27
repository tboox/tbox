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
 * @file        global_pool.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "global_pool.h"
#include "static_pool.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the magic number
#define TB_GLOBAL_POOL_MAGIC                        (0xdead)

// the align maxn
#define TB_GLOBAL_POOL_ALIGN_MAXN                   (128)

// the tpool minn
#define TB_GLOBAL_POOL_TPOOL_MINN                   (512 * 1024)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the global pool impl type
typedef struct __tb_global_pool_impl_t
{
    // the magic 
    tb_size_t               magic   : 16;

    // the align
    tb_size_t               align   : 8;

    // the data
    tb_byte_t*              data;

    // the size
    tb_size_t               size;

    // the pool
    tb_static_pool_ref_t    bpool;

}tb_global_pool_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * the implementation
 */
tb_global_pool_ref_t tb_global_pool_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
    // check
    tb_assert_and_check_return_val(data && size, tb_null);

    // align
    align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
    align = tb_max(align, TB_CPU_BITBYTE);
    tb_assert_and_check_return_val(align <= TB_GLOBAL_POOL_ALIGN_MAXN, tb_null);

    // align data
    tb_size_t byte = (tb_size_t)tb_align((tb_size_t)data, align) - (tb_size_t)data;
    tb_assert_and_check_return_val(size >= byte, tb_null);
    size -= byte;
    data += byte;

    // init pool
    tb_global_pool_impl_t* impl = (tb_global_pool_impl_t*)data;
    tb_memset(impl, 0, sizeof(tb_global_pool_impl_t));

    // init magic
    impl->magic = TB_GLOBAL_POOL_MAGIC;

    // init align
    impl->align = align;

    // init data
    impl->data = (tb_byte_t*)tb_align((tb_size_t)&impl[1], impl->align);
    tb_assert_and_check_return_val(data + size > impl->data, tb_null);

    // init size
    impl->size = (tb_byte_t*)data + size - impl->data;
    tb_assert_and_check_return_val(impl->size, tb_null);

    // init bpool
    impl->bpool = tb_static_pool_init(impl->data, impl->size, impl->align);
    tb_assert_and_check_return_val(impl->bpool, tb_null);

    // TODO: using tiny pool
    // ..
   
    // ok
    return (tb_global_pool_ref_t)impl;
}
tb_void_t tb_global_pool_exit(tb_global_pool_ref_t pool)
{
    // check 
    tb_global_pool_impl_t* impl = (tb_global_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->magic == TB_GLOBAL_POOL_MAGIC);

    // clear body
    tb_global_pool_clear(pool);

    // exit bpool
    if (impl->bpool) tb_static_pool_exit(impl->bpool);

    // clear pool
    tb_memset(impl, 0, sizeof(tb_global_pool_impl_t));   
}
tb_void_t tb_global_pool_clear(tb_global_pool_ref_t pool)
{
    // check 
    tb_global_pool_impl_t* impl = (tb_global_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->magic == TB_GLOBAL_POOL_MAGIC);

    // clear bpool
    if (impl->bpool) tb_static_pool_clear(impl->bpool);
}
tb_pointer_t tb_global_pool_malloc_(tb_global_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check 
    tb_global_pool_impl_t* impl = (tb_global_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->magic == TB_GLOBAL_POOL_MAGIC && impl->bpool, tb_null);

    // malloc it from bpool
    return tb_static_pool_malloc_(impl->bpool, size __tb_debug_args__);
}
tb_pointer_t tb_global_pool_malloc0_(tb_global_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check 
    tb_global_pool_impl_t* impl = (tb_global_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->magic == TB_GLOBAL_POOL_MAGIC && impl->bpool, tb_null);

    // malloc it from bpool
    return tb_static_pool_malloc0_(impl->bpool, size __tb_debug_args__);
}
tb_pointer_t tb_global_pool_nalloc_(tb_global_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check 
    tb_global_pool_impl_t* impl = (tb_global_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->magic == TB_GLOBAL_POOL_MAGIC && impl->bpool, tb_null);

    // malloc it from bpool
    return tb_static_pool_nalloc_(impl->bpool, item, size __tb_debug_args__);
}
tb_pointer_t tb_global_pool_nalloc0_(tb_global_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check 
    tb_global_pool_impl_t* impl = (tb_global_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->magic == TB_GLOBAL_POOL_MAGIC && impl->bpool, tb_null);

    // malloc it from bpool
    return tb_static_pool_nalloc0_(impl->bpool, item, size __tb_debug_args__);
}
tb_pointer_t tb_global_pool_ralloc_(tb_global_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // check 
    tb_global_pool_impl_t* impl = (tb_global_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->magic == TB_GLOBAL_POOL_MAGIC && impl->bpool, tb_null);

    // ralloc it from bpool
    return tb_static_pool_ralloc_(impl->bpool, data, size __tb_debug_args__);
}
tb_bool_t tb_global_pool_free_(tb_global_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{
    // check 
    tb_global_pool_impl_t* impl = (tb_global_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->magic == TB_GLOBAL_POOL_MAGIC && impl->bpool, tb_false);

    // free it to bpool
    return tb_static_pool_free_(impl->bpool, data __tb_debug_args__);
}
#ifdef __tb_debug__
tb_size_t tb_global_pool_data_size(tb_global_pool_ref_t pool, tb_cpointer_t data)
{
    // check 
    tb_global_pool_impl_t* impl = (tb_global_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->magic == TB_GLOBAL_POOL_MAGIC && impl->bpool, 0);

    // the data size
    return tb_static_pool_data_size(impl->bpool, data);
}
tb_void_t tb_global_pool_data_dump(tb_global_pool_ref_t pool, tb_cpointer_t data, tb_char_t const* prefix)
{
    // check 
    tb_global_pool_impl_t* impl = (tb_global_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->magic == TB_GLOBAL_POOL_MAGIC && impl->bpool);

    // the data dump
    tb_static_pool_data_dump(impl->bpool, data, prefix);
}
tb_void_t tb_global_pool_dump(tb_global_pool_ref_t pool)
{
    // check 
    tb_global_pool_impl_t* impl = (tb_global_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->magic == TB_GLOBAL_POOL_MAGIC && impl->bpool);

    // dump bpool
    tb_static_pool_dump(impl->bpool, "pool");
}
#endif
