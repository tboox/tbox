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
 * includes
 */
#include "memory.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef __tb_small__
#   define TB_POOL_CHUNK_GROW             (8)
#else
#   define TB_POOL_CHUNK_GROW             (16)
#endif

#ifdef __tb_small__
#   define TB_POOL_GROW_DEFAULT           TB_POOL_GROW_SMALL
#else
#   define TB_POOL_GROW_DEFAULT           TB_POOL_GROW_LARGE
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the pool chunk type
typedef struct __tb_pool_chunk_t
{
    // the static pool
    tb_static_pool_ref_t    pool;

    // the chunk data
    tb_byte_t*              data;

    // the chunk size
    tb_size_t               size;

}tb_pool_chunk_t;

// the pool info type
#ifdef __tb_debug__
typedef struct __tb_pool_info_t
{
    // the pred count
    tb_size_t               pred;

    // the aloc count
    tb_size_t               aloc;

}tb_pool_info_t;
#endif

// the pool impl type
typedef struct __tb_pool_impl_t
{
    // the pools align
    tb_size_t               align;

    // the chunk pools
    tb_pool_chunk_t*        pools;

    // the chunk pool count
    tb_size_t               pooln;

    // the chunk pool maxn
    tb_size_t               poolm;

    // the chunk grow
    tb_size_t               grow;

    // the chunk pred
    tb_size_t               pred;

    // the info
#ifdef __tb_debug__
    tb_pool_info_t          info;
#endif

}tb_pool_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
__tb_extern_c__ tb_pointer_t tb_static_pool_ralloc_fast(tb_static_pool_ref_t pool, tb_pointer_t data, tb_size_t size, tb_size_t* osize);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_pointer_t tb_pool_ralloc_fast(tb_pool_impl_t* impl, tb_pointer_t data, tb_size_t size, tb_size_t* osize)
{
    // check 
    tb_assert_and_check_return_val(impl && impl->pools, tb_null);

    // no size?
    tb_check_return_val(size, tb_null);

    // check osize
    tb_assert_and_check_return_val(osize && !*osize, tb_null);
    
    // reallocate it from the predicted pool first
    if (impl->pred)
    {
        // check
        tb_assert_and_check_return_val(impl->pred <= impl->pooln, tb_null);

        // the predicted pool
        tb_static_pool_ref_t bpool = impl->pools[impl->pred - 1].pool;
        if (bpool) 
        {
            // try reallocating it
            tb_pointer_t p = tb_static_pool_ralloc_fast(bpool, data, size, osize);

            // ok?
            tb_check_return_val(!p, p);

            // no space?
            tb_check_return_val(!*osize, tb_null);
        }
    }

    // reallocate it from the existing pool
    if (impl->pooln)
    {
        // allocate it from the last pool
        tb_size_t n = impl->pooln;
        while (n--)
        {
            tb_static_pool_ref_t bpool = impl->pools[n].pool;
            if (bpool) 
            {
                // try reallocating it
                tb_pointer_t p = tb_static_pool_ralloc_fast(bpool, data, size, osize);

                // ok?
                tb_check_return_val(!p, p);

                // no space?
                tb_check_return_val(!*osize, tb_null);
            }
        }
    }

    return tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_pool_ref_t tb_pool_init(tb_size_t grow, tb_size_t align)
{
    // done
    tb_bool_t       ok = tb_false;
    tb_pool_impl_t* impl = tb_null;
    do
    {
        // using the default grow
        if (!grow) grow = TB_POOL_GROW_DEFAULT;

        // init pool
        impl = tb_malloc0_type(tb_pool_impl_t);
        tb_assert_and_check_break(impl);

        // init pools align
        impl->align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
        impl->align = tb_max(impl->align, TB_CPU_BITBYTE);

        // init chunk grow
        impl->grow = grow;

        // init chunk pools
        impl->pooln = 0;
        impl->poolm = TB_POOL_CHUNK_GROW;
        impl->pools = (tb_pool_chunk_t*)tb_nalloc0(TB_POOL_CHUNK_GROW, sizeof(tb_pool_chunk_t));
        tb_assert_and_check_break(impl->pools);

        // init chunk pred
        impl->pred = 0;

        // init info
#ifdef __tb_debug__
        impl->info.pred = 0;
        impl->info.aloc = 0;
#endif
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
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
    tb_assert_and_check_return(impl);

    // clear
    tb_pool_clear(pool);

    // free pools
    if (impl->pools) 
    {   
        tb_size_t i = 0;
        tb_size_t n = impl->pooln;
        for (i = 0; i < n; i++)
        {
            if (impl->pools[i].pool) 
                tb_static_pool_exit(impl->pools[i].pool);

            if (impl->pools[i].data) 
                tb_free(impl->pools[i].data);
        }

        tb_free(impl->pools);
    }

    // free pool
    tb_free(impl);
}
tb_void_t tb_pool_clear(tb_pool_ref_t pool)
{
    // check 
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->pools);

    // clear pools
    tb_size_t i = 0;
    tb_size_t n = impl->pooln;
    for (i = 0; i < n; i++)
    {
        if (impl->pools[i].pool) 
            tb_static_pool_clear(impl->pools[i].pool);
    }
    
    // reinit grow
    impl->grow = 0;

    // reinit pred
    impl->pred = 0;

    // reinit info
#ifdef __tb_debug__
    impl->info.pred = 0;
    impl->info.aloc = 0;
#endif

}
tb_pointer_t tb_pool_malloc_(tb_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check 
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->pools, tb_null);

    // no size?
    tb_check_return_val(size, tb_null);
    
    // aloc++
#ifdef __tb_debug__
    impl->info.aloc++;
#endif

    // allocate it from the predicted pool first
    if (impl->pred)
    {
        // check
        tb_assert_and_check_return_val(impl->pred <= impl->pooln, tb_null);

        // the predicted pool
        tb_static_pool_ref_t bpool = impl->pools[impl->pred - 1].pool;
        if (bpool) 
        {
            // try allocating it
            tb_pointer_t p = tb_static_pool_malloc_(bpool, size __tb_debug_args__);

            // ok
            if (p) 
            {
                // pred++
#ifdef __tb_debug__
                impl->info.pred++;
#endif
                return p;
            }
        }
    }

    // allocate it from the existing pool
    if (impl->pooln)
    {
        // allocate it from the last pool
        tb_size_t n = impl->pooln;
        while (n--)
        {
            tb_static_pool_ref_t bpool = impl->pools[n].pool;
            if (bpool) 
            {
                // try allocating it
                tb_pointer_t p = tb_static_pool_malloc_(bpool, size __tb_debug_args__);

                // ok
                if (p) 
                {
                    impl->pred = n + 1;
                    return p;
                }
            }
        }
    }

    // grow pools if not enough
    if (impl->pooln >= impl->poolm)
    {
        // grow
        impl->poolm += TB_POOL_CHUNK_GROW;
        impl->pools = (tb_pool_chunk_t*)tb_ralloc(impl->pools, impl->poolm * sizeof(tb_pool_chunk_t));
        tb_assert_and_check_return_val(impl->pools, tb_null);
    }
    
    // append a new pool for allocation
    tb_pool_chunk_t* chunk = &impl->pools[impl->pooln];
    do
    {
        // clear the chunk
        tb_memset(chunk, 0, sizeof(tb_pool_chunk_t));

        // alloc chunk data
        chunk->size = impl->grow;
        chunk->data = tb_malloc_bytes(chunk->size);

        // no space
        tb_check_break(chunk->data);

        // init chunk pool
        chunk->pool = tb_static_pool_init(chunk->data, chunk->size, impl->align);
        tb_assert_and_check_break(chunk->pool);

        // try allocating it
        tb_pointer_t p = tb_static_pool_malloc_(chunk->pool, size __tb_debug_args__);

        // ok
        if (p) 
        {
            impl->pred = ++impl->pooln;
            return p;
        }
        
        tb_assertf(0, "the chunk size may be too small: %lu < %lu", chunk->size, size);

    } while (0);

    // clean chunk
    if (chunk->pool) tb_static_pool_exit(chunk->pool);
    if (chunk->data) tb_free(chunk->data);
    tb_memset(chunk, 0, sizeof(tb_pool_chunk_t));

    // fail
    return tb_null;
}
tb_pointer_t tb_pool_malloc0_(tb_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // malloc
    tb_pointer_t p = tb_pool_malloc_(pool, size __tb_debug_args__);

    // clear
    if (p && size) tb_memset(p, 0, size);

    // ok?
    return p;
}
tb_pointer_t tb_pool_nalloc_(tb_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(item, tb_null);

    // malloc
    return tb_pool_malloc_(pool, item * size __tb_debug_args__);
}

tb_pointer_t tb_pool_nalloc0_(tb_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(item, tb_null);

    // malloc
    return tb_pool_malloc0_(pool, item * size __tb_debug_args__);
}

tb_pointer_t tb_pool_ralloc_(tb_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl, tb_null);

    // free it if no size
    if (!size)
    {
        tb_pool_free_(pool, data __tb_debug_args__);
        return tb_null;
    }

    // alloc it if no data?
    if (!data) return tb_pool_malloc_(pool, size __tb_debug_args__);
    
    // ralloc it with fast mode if enough
    tb_size_t       osize = 0;
    tb_pointer_t    pdata = tb_pool_ralloc_fast(impl, data, size, &osize);
    tb_check_return_val(!pdata, pdata);
    tb_assert_and_check_return_val(osize < size, tb_null);

    // malloc it
    pdata = tb_pool_malloc_(pool, size __tb_debug_args__);
    tb_check_return_val(pdata, tb_null);
    tb_assert_and_check_return_val(pdata != data, pdata);

    // copy data
    tb_memcpy(pdata, data, osize);
    
    // free it
    tb_pool_free_(pool, data __tb_debug_args__);

    // ok
    return pdata;
}
tb_char_t* tb_pool_strdup_(tb_pool_ref_t pool, tb_char_t const* data __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && data, tb_null);

    // done
    tb_size_t   n = tb_strlen(data);
    tb_char_t*  p = (tb_char_t*)tb_pool_malloc_(pool, n + 1 __tb_debug_args__);
    if (p)
    {
        tb_memcpy(p, data, n);
        p[n] = '\0';
    }

    return p;
}

tb_char_t* tb_pool_strndup_(tb_pool_ref_t pool, tb_char_t const* data, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && data, tb_null);

    // done
    size = tb_strnlen(data, size);
    tb_char_t*  p = (tb_char_t*)tb_pool_malloc_(pool, size + 1 __tb_debug_args__);
    if (p)
    {
        tb_memcpy(p, data, size);
        p[size] = '\0';
    }

    return p;
}
tb_bool_t tb_pool_free_(tb_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{
    // check 
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->pools, tb_false);

    // no data?
    tb_check_return_val(data, tb_true);
    
    // free it from the predicted pool first
    if (impl->pred)
    {
        // check
        tb_assert_and_check_return_val(impl->pred <= impl->pooln, tb_false);

        // the predicted pool
        tb_static_pool_ref_t bpool = impl->pools[impl->pred - 1].pool;
        if (bpool) 
        {
            // try allocating it
            tb_bool_t r = tb_static_pool_free_(bpool, data __tb_debug_args__);

            // ok
            if (r) return r;
        }
    }

    // free it from the existing pool
    tb_size_t n = impl->pooln;
    while (n--)
    {
        tb_static_pool_ref_t bpool = impl->pools[n].pool;
        if (bpool) 
        {
            // try free it
            tb_bool_t r = tb_static_pool_free_(bpool, data __tb_debug_args__);

            // ok
            if (r) 
            {
                impl->pred = n + 1;
                return r;
            }
        }
    }

    // fail
    tb_assertf(0, "invalid free data address: %p", data);
    return tb_false;
}

#ifdef __tb_debug__
tb_void_t tb_pool_dump(tb_pool_ref_t pool, tb_char_t const* prefix)
{
    // check 
    tb_pool_impl_t* impl = (tb_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->pools);

    // prefix
    if (!prefix) prefix = "pool";

    // trace
#if 0
    tb_trace_i("======================================================================");
    tb_trace_i("%s: align: %lu",    prefix, impl->align);
    tb_trace_i("%s: pooln: %lu",    prefix, impl->pooln);
    tb_trace_i("%s: poolm: %lu",    prefix, impl->poolm);
    tb_trace_i("%s: grow: %lu",     prefix, impl->grow);
    tb_trace_i("%s: pred: %lu%%",   prefix, impl->info.aloc? ((impl->info.pred * 100) / impl->info.aloc) : 0);
#endif

    // dump
    tb_size_t i = 0;
    tb_size_t n = impl->pooln;
    for (i = 0; i < n; i++)
    {
        tb_static_pool_ref_t bpool = impl->pools[i].pool;
        if (bpool) tb_static_pool_dump(bpool, prefix);
    }
}
#endif
