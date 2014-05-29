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
 * @file        string_pool.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "string_pool"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string_pool.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"
#include "../container/container.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the string pool type
typedef struct __tb_string_pool_t
{
    // the block pool
    tb_handle_t             pool;

    // the hash cache
    tb_hash_t*              cache;

    // the lock
    tb_spinlock_t           lock;

}tb_string_pool_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_handle_t tb_string_pool_instance_init(tb_cpointer_t* ppriv)
{
    return tb_string_pool_init(tb_true, 0);
}
static tb_void_t tb_string_pool_instance_exit(tb_handle_t handle, tb_cpointer_t priv)
{
    if (handle)
    {
#ifdef __tb_debug__
        // dump it
        tb_string_pool_dump(handle);
#endif

        // exit it
        tb_string_pool_exit(handle);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_string_pool()
{
    return tb_singleton_instance(TB_SINGLETON_TYPE_STRING_POOL, tb_string_pool_instance_init, tb_string_pool_instance_exit, tb_null);
}
tb_handle_t tb_string_pool_init(tb_bool_t bcase, tb_size_t align)
{
    // done
    tb_bool_t           ok = tb_false;
    tb_string_pool_t*   pool = tb_null;
    do
    {
        // make pool
        pool = (tb_string_pool_t*)tb_malloc0(sizeof(tb_string_pool_t));
        tb_assert_and_check_break(pool);

        // init lock
        if (!tb_spinlock_init(&pool->lock)) break;

        // init pool
        pool->pool = tb_block_pool_init(0, align);
        tb_assert_and_check_break(pool->pool);

        // init hash
        pool->cache = tb_hash_init(0, tb_item_func_str(bcase, pool->pool), tb_item_func_size());
        tb_assert_and_check_break(pool->cache);

        // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
        tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&pool->lock, TB_TRACE_MODULE_NAME);
#endif

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (pool) tb_string_pool_exit((tb_handle_t)pool);
        pool = tb_null;
    }

    // ok?
    return (tb_handle_t)pool;
}
tb_void_t tb_string_pool_exit(tb_handle_t handle)
{
    // check
    tb_string_pool_t* pool = (tb_string_pool_t*)handle;
    tb_assert_and_check_return(pool);

    // enter
    tb_spinlock_enter(&pool->lock);

    // exit cache
    if (pool->cache) tb_hash_exit(pool->cache);
    pool->cache = tb_null;

    // exit pool
    if (pool->pool) tb_block_pool_exit(pool->pool);
    pool->pool = tb_null;

    // leave
    tb_spinlock_leave(&pool->lock);

    // exit lock
    tb_spinlock_exit(&pool->lock);

    // exit it
    tb_free(pool);
}
tb_void_t tb_string_pool_clear(tb_handle_t handle)
{
    // check
    tb_string_pool_t* pool = (tb_string_pool_t*)handle;
    tb_assert_and_check_return(pool);

    // enter
    tb_spinlock_enter(&pool->lock);

    // clear cache
    if (pool->cache) tb_hash_clear(pool->cache);

    // clear pool
    if (pool->pool) tb_block_pool_clear(pool->pool);

    // leave
    tb_spinlock_leave(&pool->lock);
}
tb_char_t const* tb_string_pool_put(tb_handle_t handle, tb_char_t const* data)
{
    // check
    tb_string_pool_t* pool = (tb_string_pool_t*)handle;
    tb_assert_and_check_return_val(pool && data, tb_null);

    // enter
    tb_spinlock_enter(&pool->lock);

    // done
    tb_char_t const* cstr = tb_null;
    if (pool->cache)
    {
        // exists?
        tb_size_t               itor;
        tb_hash_item_t const*   item = tb_null;
        if (((itor = tb_hash_itor(pool->cache, data)) != tb_iterator_tail(pool->cache)) && (item = tb_iterator_item(pool->cache, itor)))
        {
            // refn
            tb_size_t refn = (tb_size_t)item->data;

            // refn++
            if (refn) tb_iterator_copy(pool->cache, itor, (tb_pointer_t)(refn + 1));
            // no refn? remove it
            else
            {
                // assert
                tb_assert(0);

                // del it
                tb_iterator_delt(pool->cache, itor);
                item = tb_null;
            }
        }
        
        // no item? add it
        if (!item)
        {
            // add it
            if ((itor = tb_hash_set(pool->cache, data, (tb_pointer_t)1)) != tb_iterator_tail(pool->cache))
                item = tb_iterator_item(pool->cache, itor);
        }

        // save the cstr
        if (item) cstr = item->name;
    }

    // leave
    tb_spinlock_leave(&pool->lock);

    // ok?
    return cstr;
}
tb_void_t tb_string_pool_del(tb_handle_t handle, tb_char_t const* data)
{
    // check
    tb_string_pool_t* pool = (tb_string_pool_t*)handle;
    tb_assert_and_check_return(pool && data);

    // enter
    tb_spinlock_enter(&pool->lock);

    // done
    tb_hash_item_t const* item = tb_null;
    if (pool->cache)
    {
        // exists?
        tb_size_t itor;
        if (((itor = tb_hash_itor(pool->cache, data)) != tb_iterator_tail(pool->cache)) && (item = tb_iterator_item(pool->cache, itor)))
        {
            // refn
            tb_size_t refn = (tb_size_t)item->data;

            // refn--
            if (refn > 1) tb_iterator_copy(pool->cache, itor, (tb_pointer_t)(refn - 1));
            // del it
            else tb_iterator_delt(pool->cache, itor);
        }
    }

    // leave
    tb_spinlock_leave(&pool->lock);
}
#ifdef __tb_debug__
tb_void_t tb_string_pool_dump(tb_handle_t handle)
{
    // check
    tb_string_pool_t* pool = (tb_string_pool_t*)handle;
    tb_assert_and_check_return(pool);

    // enter
    tb_spinlock_enter(&pool->lock);

    // dump cache
    if (pool->cache && tb_hash_size(pool->cache))
    {
        // trace
        tb_trace_i("======================================================================");
        tb_trace_i("size: %lu", tb_hash_size(pool->cache));

        // walk
        tb_for_all (tb_hash_item_t*, item, pool->cache)
        {
            if (item) tb_trace_i("item: refn: %lu, cstr: %s", (tb_size_t)item->data, item->name);
        }
        tb_trace_i("======================================================================");
    }

    // leave
    tb_spinlock_leave(&pool->lock);
}
#endif
