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
#include "static_tiny_pool.h"
#include "static_block_pool.h"
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

/// the generic pool type
typedef struct __tb_global_pool_t
{
    /// the magic 
    tb_size_t       magic   : 16;

    /// the align
    tb_size_t       align   : 8;

    /// the data
    tb_byte_t*      data;

    /// the size
    tb_size_t       size;

    /// the tdata
    tb_byte_t*      tdata;

    /// the tsize
    tb_size_t       tsize;

    /// the tpool
    tb_handle_t     tpool;

    /// the bpool
    tb_handle_t     bpool;

}tb_global_pool_t;


/* //////////////////////////////////////////////////////////////////////////////////////
 * the implementation
 */
tb_handle_t tb_global_pool_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
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
    tb_global_pool_t* pool = (tb_global_pool_t*)data;
    tb_memset(pool, 0, sizeof(tb_global_pool_t));

    // init magic
    pool->magic = TB_GLOBAL_POOL_MAGIC;

    // init align
    pool->align = align;

    // init data
    pool->data = (tb_byte_t*)tb_align((tb_size_t)&pool[1], pool->align);
    tb_assert_and_check_return_val(data + size > pool->data, tb_null);

    // init size
    pool->size = (tb_byte_t*)data + size - pool->data;
    tb_assert_and_check_return_val(pool->size, tb_null);

    // init bpool
    pool->bpool = tb_static_block_pool_init(pool->data, pool->size, pool->align);
    tb_assert_and_check_return_val(pool->bpool, tb_null);
    
    // FIXME: alloc will be too slower now if space is small
#if 0
    // init tpool
    pool->tsize = pool->size >> 3;
    if (pool->tsize >= TB_GLOBAL_POOL_TPOOL_MINN)
    {
        pool->tdata = tb_static_block_pool_malloc(pool->bpool, pool->tsize);
        if (pool->tdata)
        {
            pool->tpool = tb_tiny_pool_init(pool->tdata, pool->tsize, pool->align);
            tb_assert_and_check_return_val(pool->tpool, tb_null);
        }
    }
#endif

    // ok
    return ((tb_handle_t)pool);
}
tb_void_t tb_global_pool_exit(tb_handle_t handle)
{
    // check 
    tb_global_pool_t* pool = (tb_global_pool_t*)handle;
    tb_assert_and_check_return(pool && pool->magic == TB_GLOBAL_POOL_MAGIC);

    // clear body
    tb_global_pool_clear(handle);

    // exit bpool
    if (pool->bpool) tb_static_block_pool_exit(pool->bpool);

    // clear pool
    tb_memset(pool, 0, sizeof(tb_global_pool_t));   
}
tb_void_t tb_global_pool_clear(tb_handle_t handle)
{
    // check 
    tb_global_pool_t* pool = (tb_global_pool_t*)handle;
    tb_assert_and_check_return(pool && pool->magic == TB_GLOBAL_POOL_MAGIC);

    // clear bpool
    if (pool->bpool) tb_static_block_pool_clear(pool->bpool);

    // reinit tpool
    pool->tdata = tb_null;
    pool->tpool = tb_null;
    if (pool->tsize >= TB_GLOBAL_POOL_TPOOL_MINN)
    {
        pool->tdata = tb_static_block_pool_malloc(pool->bpool, pool->tsize);
        if (pool->tdata) pool->tpool = tb_tiny_pool_init(pool->tdata, pool->tsize, pool->align);
    }
}

tb_pointer_t tb_global_pool_malloc_(tb_handle_t handle, tb_size_t size __tb_debug_decl__)
{
    // check 
    tb_global_pool_t* pool = (tb_global_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->magic == TB_GLOBAL_POOL_MAGIC && pool->bpool, tb_null);

    // try malloc it from tpool
    if (pool->tpool && size <= tb_tiny_pool_limit(pool->tpool))
    {
        tb_pointer_t data = tb_tiny_pool_malloc(pool->tpool, size);
        if (data) return data;
    }

    // malloc it from bpool
    return tb_static_block_pool_malloc_(pool->bpool, size __tb_debug_args__);
}

tb_pointer_t tb_global_pool_malloc0_(tb_handle_t handle, tb_size_t size __tb_debug_decl__)
{
    // check 
    tb_global_pool_t* pool = (tb_global_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->magic == TB_GLOBAL_POOL_MAGIC && pool->bpool, tb_null);

    // try malloc it from tpool
    if (pool->tpool && size <= tb_tiny_pool_limit(pool->tpool))
    {
        tb_pointer_t data = tb_tiny_pool_malloc0(pool->tpool, size);
        if (data) return data;
    }

    // malloc it from bpool
    return tb_static_block_pool_malloc0_(pool->bpool, size __tb_debug_args__);
}

tb_pointer_t tb_global_pool_nalloc_(tb_handle_t handle, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check 
    tb_global_pool_t* pool = (tb_global_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->magic == TB_GLOBAL_POOL_MAGIC && pool->bpool, tb_null);

    // try malloc it from tpool
    if (pool->tpool && item * size <= tb_tiny_pool_limit(pool->tpool))
    {
        tb_pointer_t data = tb_tiny_pool_nalloc(pool->tpool, item, size);
        if (data) return data;
    }

    // malloc it from bpool
    return tb_static_block_pool_nalloc_(pool->bpool, item, size __tb_debug_args__);
}

tb_pointer_t tb_global_pool_nalloc0_(tb_handle_t handle, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check 
    tb_global_pool_t* pool = (tb_global_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->magic == TB_GLOBAL_POOL_MAGIC && pool->bpool, tb_null);

    // try malloc it from tpool
    if (pool->tpool && item * size <= tb_tiny_pool_limit(pool->tpool))
    {
        tb_pointer_t data = tb_tiny_pool_nalloc0(pool->tpool, item, size);
        if (data) return data;
    }

    // malloc it from bpool
    return tb_static_block_pool_nalloc0_(pool->bpool, item, size __tb_debug_args__);
}
tb_pointer_t tb_global_pool_ralloc_(tb_handle_t handle, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // check 
    tb_global_pool_t* pool = (tb_global_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->magic == TB_GLOBAL_POOL_MAGIC && pool->bpool, tb_null);

    // try ralloc it from tpool
    if (pool->tpool && (tb_byte_t*)data > pool->tdata && (tb_byte_t*)data < pool->tdata + pool->tsize)
    {
        // ralloc it
        tb_pointer_t pdata = tb_null;
        if (size <= tb_tiny_pool_limit(pool->tpool) && (pdata = tb_tiny_pool_ralloc(pool->tpool, data, size))) return pdata;
        else
        {
            // malloc it
            pdata = tb_static_block_pool_malloc_(pool->bpool, size __tb_debug_args__);
            tb_check_return_val(pdata, tb_null);
            tb_assert_and_check_return_val(pdata != data, pdata);

            // copy data
            tb_size_t osize = tb_min(tb_tiny_pool_limit(pool->tpool), size);
            tb_memcpy(pdata, data, osize);
            
            // free it
            tb_tiny_pool_free(pool->tpool, data);

            // ok
            return pdata;
        }
    }

    // ralloc it from bpool
    return tb_static_block_pool_ralloc_(pool->bpool, data, size __tb_debug_args__);
}

tb_bool_t tb_global_pool_free_(tb_handle_t handle, tb_pointer_t data __tb_debug_decl__)
{
    // check 
    tb_global_pool_t* pool = (tb_global_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->magic == TB_GLOBAL_POOL_MAGIC && pool->bpool, tb_false);

    // free it to tpool
    if (pool->tpool && (tb_byte_t*)data > pool->tdata && (tb_byte_t*)data < pool->tdata + pool->tsize)
        return tb_tiny_pool_free(pool->tpool, data);

    // free it to bpool
    return tb_static_block_pool_free_(pool->bpool, data __tb_debug_args__);
}

#ifdef __tb_debug__
tb_size_t tb_global_pool_data_size(tb_handle_t handle, tb_cpointer_t data)
{
    // check 
    tb_global_pool_t* pool = (tb_global_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->magic == TB_GLOBAL_POOL_MAGIC && pool->bpool, 0);

    // the data size
    return tb_static_block_pool_data_size(pool->bpool, data);
}
tb_void_t tb_global_pool_data_dump(tb_handle_t handle, tb_cpointer_t data, tb_char_t const* prefix)
{
    // check 
    tb_global_pool_t* pool = (tb_global_pool_t*)handle;
    tb_assert_and_check_return(pool && pool->magic == TB_GLOBAL_POOL_MAGIC && pool->bpool);

    // the data dump
    tb_static_block_pool_data_dump(pool->bpool, data, prefix);
}
tb_void_t tb_global_pool_dump(tb_handle_t handle)
{
    // check 
    tb_global_pool_t* pool = (tb_global_pool_t*)handle;
    tb_assert_and_check_return(pool && pool->magic == TB_GLOBAL_POOL_MAGIC && pool->bpool);

    // dump tpool
    if (pool->tpool) tb_tiny_pool_dump(pool->tpool);

    // dump bpool
    tb_static_block_pool_dump(pool->bpool, "pool");
}
#endif
