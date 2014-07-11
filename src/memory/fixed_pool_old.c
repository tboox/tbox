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
 * @file        fixed_pool_old.c
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
#   define TB_FIXED_POOL_OLD_CHUNK_GROW             (8)
#else
#   define TB_FIXED_POOL_OLD_CHUNK_GROW             (16)
#endif

#ifdef __tb_small__
#   define TB_FIXED_POOL_OLD_GROW_DEFAULT           TB_FIXED_POOL_OLD_GROW_SMALL
#else
#   define TB_FIXED_POOL_OLD_GROW_DEFAULT           TB_FIXED_POOL_OLD_GROW_LARGE
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the fixed pool chunk type
typedef struct __tb_fixed_pool_old_chunk_t
{
    // the static fixed pool
    tb_static_fixed_pool_old_ref_t  pool;

    // the chunk data
    tb_byte_t*                  data;

    // the chunk size
    tb_size_t                   size;

}tb_fixed_pool_old_chunk_t;

#ifdef __tb_debug__
// the fixed pool info type
typedef struct __tb_fixed_pool_old_info_t
{
    // the pred count
    tb_size_t                   pred;

    // the aloc count
    tb_size_t                   aloc;

}tb_fixed_pool_old_info_t;
#endif

// the fixed pool impl type
typedef struct __tb_fixed_pool_old_impl_t
{
    // the pools align
    tb_size_t                   align;

    // the size
    tb_size_t                   size;

    // the chunk pools
    tb_fixed_pool_old_chunk_t*      pools;

    // the chunk pool count
    tb_size_t                   pooln;

    // the chunk pool maxn
    tb_size_t                   poolm;

    // the chunk step
    tb_size_t                   step;

    // the chunk grow
    tb_size_t                   grow;

    // the chunk pred
    tb_size_t                   pred;

    // the info
#ifdef __tb_debug__
    tb_fixed_pool_old_info_t        info;
#endif

}tb_fixed_pool_old_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_fixed_pool_old_item_func(tb_pointer_t item, tb_cpointer_t priv)
{
    // check
    tb_value_t* tuple = (tb_value_t*)priv;
    tb_assert_and_check_return_val(tuple, tb_false);

    // the func
    typedef tb_bool_t (*func_t)(tb_pointer_t , tb_cpointer_t );
    func_t func = (func_t)tuple[0].ptr;

    // the data
    priv = tuple[1].cptr;

    // done func
    tb_bool_t ok = func(item, priv);

    // save it
    tuple[2].b = ok;

    // ok?
    return ok;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_fixed_pool_old_ref_t tb_fixed_pool_old_init(tb_size_t grow, tb_size_t step, tb_size_t align)
{
    // check
    tb_assert_and_check_return_val(step, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    tb_fixed_pool_old_impl_t*   impl = tb_null;
    do
    {
        // make pool
        impl = tb_malloc0_type(tb_fixed_pool_old_impl_t);
        tb_assert_and_check_break(impl);

        // using the default grow
        if (!grow) grow = TB_FIXED_POOL_OLD_GROW_DEFAULT;

        // init pools size
        impl->size = 0;

        // init pools align
        impl->align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
        impl->align = tb_max(impl->align, TB_CPU_BITBYTE);

        // init chunk step
        impl->step = step;

        // init chunk grow
        impl->grow = tb_align_pow2((sizeof(tb_size_t) << 3) + grow * step);

        // init chunk pools
        impl->pooln = 0;
        impl->poolm = TB_FIXED_POOL_OLD_CHUNK_GROW;
        impl->pools = (tb_fixed_pool_old_chunk_t*)tb_nalloc0(TB_FIXED_POOL_OLD_CHUNK_GROW, sizeof(tb_fixed_pool_old_chunk_t));
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
        if (impl) tb_fixed_pool_old_exit((tb_fixed_pool_old_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_fixed_pool_old_ref_t)impl;
}
tb_void_t tb_fixed_pool_old_exit(tb_fixed_pool_old_ref_t pool)
{
    // check 
    tb_fixed_pool_old_impl_t* impl = (tb_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // clear
    tb_fixed_pool_old_clear(pool);

    // free pools
    if (impl->pools) 
    {   
        tb_size_t i = 0;
        tb_size_t n = impl->pooln;
        for (i = 0; i < n; i++)
        {
            if (impl->pools[i].pool) 
                tb_static_fixed_pool_old_exit(impl->pools[i].pool);

            if (impl->pools[i].data) 
                tb_free(impl->pools[i].data);
        }

        tb_free(impl->pools);
    }

    // free pool
    tb_free(impl);
}
tb_void_t tb_fixed_pool_old_clear(tb_fixed_pool_old_ref_t pool)
{
    // check 
    tb_fixed_pool_old_impl_t* impl = (tb_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->pools);

    // clear pools
    tb_size_t i = 0;
    tb_size_t n = impl->pooln;
    for (i = 0; i < n; i++)
    {
        if (impl->pools[i].pool) 
            tb_static_fixed_pool_old_clear(impl->pools[i].pool);
    }
    
    // reinit size
    impl->size = 0;

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
tb_size_t tb_fixed_pool_old_size(tb_fixed_pool_old_ref_t pool)
{
    // check 
    tb_fixed_pool_old_impl_t* impl = (tb_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return_val(impl, 0);

    return impl->size;
}
tb_pointer_t tb_fixed_pool_old_malloc(tb_fixed_pool_old_ref_t pool)
{
    // check 
    tb_fixed_pool_old_impl_t* impl = (tb_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->pools, tb_null);

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
        tb_static_fixed_pool_old_ref_t fpool = impl->pools[impl->pred - 1].pool;
        if (fpool) 
        {
            // try allocating it
            tb_pointer_t p = tb_static_fixed_pool_old_malloc(fpool);

            // ok
            if (p) 
            {
                // size++
                impl->size++;

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
            tb_static_fixed_pool_old_ref_t fpool = impl->pools[n].pool;
            if (fpool) 
            {
                // try allocating it
                tb_pointer_t p = tb_static_fixed_pool_old_malloc(fpool);

                // ok
                if (p) 
                {
                    // size++
                    impl->size++;

                    // pred
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
        impl->poolm += TB_FIXED_POOL_OLD_CHUNK_GROW;
        impl->pools = (tb_fixed_pool_old_chunk_t*)tb_ralloc(impl->pools, impl->poolm * sizeof(tb_fixed_pool_old_chunk_t));
        tb_assert_and_check_return_val(impl->pools, tb_null);
    }
    
    // append a new pool for allocation
    tb_fixed_pool_old_chunk_t* chunk = &impl->pools[impl->pooln];
    do
    {
        // clear the chunk
        tb_memset(chunk, 0, sizeof(tb_fixed_pool_old_chunk_t));

        // alloc chunk data
        chunk->size = impl->grow;
        chunk->data = tb_malloc_bytes(chunk->size);

        // no space?
        tb_check_break(chunk->data);

        // init chunk pool
        chunk->pool = tb_static_fixed_pool_old_init(chunk->data, chunk->size, impl->step, impl->align);
        tb_assert_and_check_break(chunk->pool);

        // try allocating it
        tb_pointer_t p = tb_static_fixed_pool_old_malloc(chunk->pool);

        // ok
        if (p) 
        {
            // size++
            impl->size++;

            // pred
            impl->pred = ++impl->pooln;
            return p;
        }
        
        tb_assertf(0, "the chunk size may be too small: %lu < %lu", chunk->size, impl->step);

    } while (0);

    // clean chunk
    if (chunk->pool) tb_static_fixed_pool_old_exit(chunk->pool);
    if (chunk->data) tb_free(chunk->data);
    tb_memset(chunk, 0, sizeof(tb_fixed_pool_old_chunk_t));

    // fail
    return tb_null;
}

tb_pointer_t tb_fixed_pool_old_malloc0(tb_fixed_pool_old_ref_t pool)
{
    // check 
    tb_fixed_pool_old_impl_t* impl = (tb_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->pools, tb_null);

    // malloc
    tb_pointer_t p = tb_fixed_pool_old_malloc(pool);

    // clear
    if (p) tb_memset(p, 0, impl->step);

    // ok?
    return p;
}

tb_bool_t tb_fixed_pool_old_free(tb_fixed_pool_old_ref_t pool, tb_pointer_t data)
{
    // check 
    tb_fixed_pool_old_impl_t* impl = (tb_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return_val(impl && impl->pools && impl->size, tb_false);

    // no data?
    tb_check_return_val(data, tb_true);
    
    // free it from the predicted pool first
    if (impl->pred)
    {
        // check
        tb_assert_and_check_return_val(impl->pred <= impl->pooln, tb_false);

        // the predicted pool
        tb_static_fixed_pool_old_ref_t fpool = impl->pools[impl->pred - 1].pool;
        if (fpool) 
        {
            // try allocating it
            tb_bool_t r = tb_static_fixed_pool_old_free(fpool, data);

            // ok
            if (r) 
            {
                // size--
                impl->size--;
                return tb_true;
            }
        }
    }

    // free it from the existing pool
    tb_size_t n = impl->pooln;
    while (n--)
    {
        tb_static_fixed_pool_old_ref_t fpool = impl->pools[n].pool;
        if (fpool) 
        {
            // try free it
            tb_bool_t r = tb_static_fixed_pool_old_free(fpool, data);

            // ok
            if (r) 
            {
                // size--
                impl->size--;
        
                // pred
                impl->pred = n + 1;
                return tb_true;
            }
        }
    }

    // fail
    tb_assertf(0, "invalid free data address: %p", data);
    return tb_false;
}

tb_pointer_t tb_fixed_pool_old_memdup(tb_fixed_pool_old_ref_t pool, tb_cpointer_t data)
{
    // check 
    tb_fixed_pool_old_impl_t* impl = (tb_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return_val(impl && data, tb_null);

    // init
    tb_size_t       n = impl->step;
    tb_pointer_t    p = tb_fixed_pool_old_malloc(pool);

    // copy
    if (p) tb_memcpy(p, data, n);

    // ok?
    return p;
}
tb_void_t tb_fixed_pool_old_walk(tb_fixed_pool_old_ref_t pool, tb_bool_t (*func)(tb_pointer_t item, tb_cpointer_t priv), tb_cpointer_t priv)
{
    // check 
    tb_fixed_pool_old_impl_t* impl = (tb_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return(impl && func);

    // walk pools
    tb_size_t i = 0;
    tb_size_t n = impl->pooln;
    for (i = 0; i < n; i++)
    {
        if (impl->pools[i].pool) 
        {
            // done walk
            tb_value_t tuple[3];
            tuple[0].ptr = (tb_pointer_t)func;
            tuple[1].cptr = priv;
            tuple[2].b = tb_true;
            tb_static_fixed_pool_old_walk(impl->pools[i].pool, tb_fixed_pool_old_item_func, tuple);

            // ok?
            if (!tuple[2].b) break;
        }
    }
}

#ifdef __tb_debug__
tb_void_t tb_fixed_pool_old_dump(tb_fixed_pool_old_ref_t pool)
{
    // check 
    tb_fixed_pool_old_impl_t* impl = (tb_fixed_pool_old_impl_t*)pool;
    tb_assert_and_check_return(impl && impl->pools);

    // trace
    tb_trace_i("======================================================================");
    tb_trace_i("pool: align: %lu",  impl->align);
    tb_trace_i("pool: pooln: %lu",  impl->pooln);
    tb_trace_i("pool: poolm: %lu",  impl->poolm);
    tb_trace_i("pool: size: %lu",   impl->size);
    tb_trace_i("pool: grow: %lu",   impl->grow);
    tb_trace_i("pool: pred: %lu%%", impl->info.aloc? ((impl->info.pred * 100) / impl->info.aloc) : 0);

    // dump
    tb_size_t i = 0;
    tb_size_t n = impl->pooln;
    for (i = 0; i < n; i++)
    {
        tb_static_fixed_pool_old_ref_t fpool = impl->pools[i].pool;
        if (fpool) tb_static_fixed_pool_old_dump(fpool);
    }
}
#endif
