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
 * @file        fixed_pool.c
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
#   define TB_FIXED_POOL_CHUNK_GROW             (8)
#else
#   define TB_FIXED_POOL_CHUNK_GROW             (16)
#endif

#ifdef __tb_small__
#   define TB_FIXED_POOL_GROW_DEFAULT           TB_FIXED_POOL_GROW_SMALL
#else
#   define TB_FIXED_POOL_GROW_DEFAULT           TB_FIXED_POOL_GROW_LARGE
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the pool chunk type
typedef struct __tb_fixed_pool_chunk_t
{
    // the static fixed pool
    tb_handle_t             pool;

    // the chunk
    tb_byte_t*              data;
    tb_size_t               size;

}tb_fixed_pool_chunk_t;

#ifdef __tb_debug__
/// the pool info type
typedef struct __tb_fixed_pool_info_t
{
    // the pred count
    tb_size_t               pred;

    // the aloc count
    tb_size_t               aloc;

}tb_fixed_pool_info_t;
#endif

/// the small or string pool
typedef struct __tb_fixed_pool_t
{
    // the pools align
    tb_size_t               align;

    // the size
    tb_size_t               size;

    // the chunk pools
    tb_fixed_pool_chunk_t*  pools;

    // the chunk pool count
    tb_size_t               pooln;

    // the chunk pool maxn
    tb_size_t               poolm;

    // the chunk step
    tb_size_t               step;

    // the chunk grow
    tb_size_t               grow;

    // the chunk pred
    tb_size_t               pred;

    // the info
#ifdef __tb_debug__
    tb_fixed_pool_info_t    info;
#endif

}tb_fixed_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_fixed_pool_item_func(tb_pointer_t item, tb_cpointer_t priv)
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
tb_handle_t tb_fixed_pool_init(tb_size_t grow, tb_size_t step, tb_size_t align)
{
    // check
    tb_assert_and_check_return_val(step, tb_null);

    // init pool
    tb_fixed_pool_t* pool = (tb_fixed_pool_t*)tb_malloc0(sizeof(tb_fixed_pool_t));
    tb_assert_and_check_return_val(pool, tb_null);

    // using the default grow
    if (!grow) grow = TB_FIXED_POOL_GROW_DEFAULT;

    // init pools size
    pool->size = 0;

    // init pools align
    pool->align = align? tb_align_pow2(align) : TB_CPU_BITBYTE;
    pool->align = tb_max(pool->align, TB_CPU_BITBYTE);

    // init chunk step
    pool->step = step;

    // init chunk grow
    pool->grow = tb_align_pow2((sizeof(tb_size_t) << 3) + grow * step);

    // init chunk pools
    pool->pooln = 0;
    pool->poolm = TB_FIXED_POOL_CHUNK_GROW;
    pool->pools = (tb_fixed_pool_chunk_t*)tb_nalloc0(TB_FIXED_POOL_CHUNK_GROW, sizeof(tb_fixed_pool_chunk_t));
    tb_assert_and_check_goto(pool->pools, fail);

    // init chunk pred
    pool->pred = 0;

    // init info
#ifdef __tb_debug__
    pool->info.pred = 0;
    pool->info.aloc = 0;
#endif

    // ok
    return (tb_handle_t)pool;

fail:
    if (pool) tb_fixed_pool_exit(pool);
    return tb_null;
}

tb_void_t tb_fixed_pool_exit(tb_handle_t handle)
{
    // check 
    tb_fixed_pool_t* pool = (tb_fixed_pool_t*)handle;
    tb_assert_and_check_return(pool);

    // clear
    tb_fixed_pool_clear(handle);

    // free pools
    if (pool->pools) 
    {   
        tb_size_t i = 0;
        tb_size_t n = pool->pooln;
        for (i = 0; i < n; i++)
        {
            if (pool->pools[i].pool) 
                tb_static_fixed_pool_exit(pool->pools[i].pool);

            if (pool->pools[i].data) 
                tb_free(pool->pools[i].data);
        }

        tb_free(pool->pools);
    }

    // free pool
    tb_free(pool);
}
tb_void_t tb_fixed_pool_clear(tb_handle_t handle)
{
    // check 
    tb_fixed_pool_t* pool = (tb_fixed_pool_t*)handle;
    tb_assert_and_check_return(pool && pool->pools);

    // clear pools
    tb_size_t i = 0;
    tb_size_t n = pool->pooln;
    for (i = 0; i < n; i++)
    {
        if (pool->pools[i].pool) 
            tb_static_fixed_pool_clear(pool->pools[i].pool);
    }
    
    // reinit size
    pool->size = 0;

    // reinit grow
    pool->grow = 0;

    // reinit pred
    pool->pred = 0;

    // reinit info
#ifdef __tb_debug__
    pool->info.pred = 0;
    pool->info.aloc = 0;
#endif

}
tb_size_t tb_fixed_pool_size(tb_handle_t handle)
{
    // check 
    tb_fixed_pool_t* pool = (tb_fixed_pool_t*)handle;
    tb_assert_and_check_return_val(pool, 0);

    return pool->size;
}
tb_pointer_t tb_fixed_pool_malloc(tb_handle_t handle)
{
    // check 
    tb_fixed_pool_t* pool = (tb_fixed_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->pools, tb_null);

    // aloc++
#ifdef __tb_debug__
    pool->info.aloc++;
#endif

    // allocate it from the predicted pool first
    if (pool->pred)
    {
        // check
        tb_assert_and_check_return_val(pool->pred <= pool->pooln, tb_null);

        // the predicted pool
        tb_handle_t fpool = pool->pools[pool->pred - 1].pool;
        if (fpool) 
        {
            // try allocating it
            tb_pointer_t p = tb_static_fixed_pool_malloc(fpool);

            // ok
            if (p) 
            {
                // size++
                pool->size++;

                // pred++
#ifdef __tb_debug__
                pool->info.pred++;
#endif
                return p;
            }
        }
    }

    // allocate it from the existing pool
    if (pool->pooln)
    {
        // allocate it from the last pool
        tb_size_t n = pool->pooln;
        while (n--)
        {
            tb_handle_t fpool = pool->pools[n].pool;
            if (fpool) 
            {
                // try allocating it
                tb_pointer_t p = tb_static_fixed_pool_malloc(fpool);

                // ok
                if (p) 
                {
                    // size++
                    pool->size++;

                    // pred
                    pool->pred = n + 1;
                    return p;
                }
            }
        }
    }

    // grow pools if not enough
    if (pool->pooln >= pool->poolm)
    {
        // grow
        pool->poolm += TB_FIXED_POOL_CHUNK_GROW;
        pool->pools = (tb_fixed_pool_chunk_t*)tb_ralloc(pool->pools, pool->poolm * sizeof(tb_fixed_pool_chunk_t));
        tb_assert_and_check_return_val(pool->pools, tb_null);
    }
    
    // append a new pool for allocation
    tb_fixed_pool_chunk_t* chunk = &pool->pools[pool->pooln];
    do
    {
        // clear the chunk
        tb_memset(chunk, 0, sizeof(tb_fixed_pool_chunk_t));

        // alloc chunk data
        chunk->size = pool->grow;
        chunk->data = tb_malloc(chunk->size);

        // no space?
        tb_check_break(chunk->data);

        // init chunk pool
        chunk->pool = tb_static_fixed_pool_init(chunk->data, chunk->size, pool->step, pool->align);
        tb_assert_and_check_break(chunk->pool);

        // try allocating it
        tb_pointer_t p = tb_static_fixed_pool_malloc(chunk->pool);

        // ok
        if (p) 
        {
            // size++
            pool->size++;

            // pred
            pool->pred = ++pool->pooln;
            return p;
        }
        
        tb_assert_message(0, "the chunk size may be too small: %lu < %lu", chunk->size, pool->step);

    } while (0);

    // clean chunk
    if (chunk->pool) tb_static_fixed_pool_exit(chunk->pool);
    if (chunk->data) tb_free(chunk->data);
    tb_memset(chunk, 0, sizeof(tb_fixed_pool_chunk_t));

    // fail
    return tb_null;
}

tb_pointer_t tb_fixed_pool_malloc0(tb_handle_t handle)
{
    // check 
    tb_fixed_pool_t* pool = (tb_fixed_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->pools, tb_null);

    // malloc
    tb_byte_t* p = tb_fixed_pool_malloc(handle);

    // clear
    if (p) tb_memset(p, 0, pool->step);

    // ok?
    return p;
}

tb_bool_t tb_fixed_pool_free(tb_handle_t handle, tb_pointer_t data)
{
    // check 
    tb_fixed_pool_t* pool = (tb_fixed_pool_t*)handle;
    tb_assert_and_check_return_val(pool && pool->pools && pool->size, tb_false);

    // no data?
    tb_check_return_val(data, tb_true);
    
    // free it from the predicted pool first
    if (pool->pred)
    {
        // check
        tb_assert_and_check_return_val(pool->pred <= pool->pooln, tb_false);

        // the predicted pool
        tb_handle_t fpool = pool->pools[pool->pred - 1].pool;
        if (fpool) 
        {
            // try allocating it
            tb_bool_t r = tb_static_fixed_pool_free(fpool, data);

            // ok
            if (r) 
            {
                // size--
                pool->size--;
                return tb_true;
            }
        }
    }

    // free it from the existing pool
    tb_size_t n = pool->pooln;
    while (n--)
    {
        tb_handle_t fpool = pool->pools[n].pool;
        if (fpool) 
        {
            // try free it
            tb_bool_t r = tb_static_fixed_pool_free(fpool, data);

            // ok
            if (r) 
            {
                // size--
                pool->size--;
        
                // pred
                pool->pred = n + 1;
                return tb_true;
            }
        }
    }

    // fail
    tb_assert_message(0, "invalid free data address: %p", data);
    return tb_false;
}

tb_pointer_t tb_fixed_pool_memdup(tb_handle_t handle, tb_cpointer_t data)
{
    // check 
    tb_fixed_pool_t* pool = (tb_fixed_pool_t*)handle;
    tb_assert_and_check_return_val(pool && data, tb_null);

    // init
    tb_size_t   n = pool->step;
    tb_char_t*  p = tb_fixed_pool_malloc(handle);

    // copy
    if (p) tb_memcpy(p, data, n);

    // ok?
    return p;
}
tb_void_t tb_fixed_pool_walk(tb_handle_t handle, tb_bool_t (*func)(tb_pointer_t item, tb_cpointer_t priv), tb_cpointer_t priv)
{
    // check 
    tb_fixed_pool_t* pool = (tb_fixed_pool_t*)handle;
    tb_assert_and_check_return(pool && func);

    // walk pools
    tb_size_t i = 0;
    tb_size_t n = pool->pooln;
    for (i = 0; i < n; i++)
    {
        if (pool->pools[i].pool) 
        {
            // done walk
            tb_value_t tuple[3];
            tuple[0].ptr = (tb_pointer_t)func;
            tuple[1].cptr = priv;
            tuple[2].b = tb_true;
            tb_static_fixed_pool_walk(pool->pools[i].pool, tb_fixed_pool_item_func, tuple);

            // ok?
            if (!tuple[2].b) break;
        }
    }
}

#ifdef __tb_debug__
tb_void_t tb_fixed_pool_dump(tb_handle_t handle)
{
    // check 
    tb_fixed_pool_t* pool = (tb_fixed_pool_t*)handle;
    tb_assert_and_check_return(pool && pool->pools);

    // trace
    tb_trace_i("======================================================================");
    tb_trace_i("pool: align: %lu",  pool->align);
    tb_trace_i("pool: pooln: %lu",  pool->pooln);
    tb_trace_i("pool: poolm: %lu",  pool->poolm);
    tb_trace_i("pool: size: %lu",   pool->size);
    tb_trace_i("pool: grow: %lu",   pool->grow);
    tb_trace_i("pool: pred: %lu%%", pool->info.aloc? ((pool->info.pred * 100) / pool->info.aloc) : 0);

    // dump
    tb_size_t i = 0;
    tb_size_t n = pool->pooln;
    for (i = 0; i < n; i++)
    {
        tb_handle_t fpool = pool->pools[i].pool;
        if (fpool) tb_static_fixed_pool_dump(fpool);
    }
}
#endif
