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
 * trace
 */
#define TB_TRACE_MODULE_NAME                "string_pool"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pool.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"
#include "../container/container.h"
#include "../algorithm/algorithm.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the string pool impl type
typedef struct __tb_string_pool_impl_t
{
    // the cache
    tb_hash_ref_t           cache;

    // the lock
    tb_spinlock_t           lock;

}tb_string_pool_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_handle_t tb_string_pool_instance_init(tb_cpointer_t* ppriv)
{
    return tb_string_pool_init(tb_true);
}
static tb_void_t tb_string_pool_instance_exit(tb_handle_t handle, tb_cpointer_t priv)
{
    if (handle)
    {
#ifdef __tb_debug__
        // dump it
        tb_string_pool_dump((tb_string_pool_ref_t)handle);
#endif

        // exit it
        tb_string_pool_exit((tb_string_pool_ref_t)handle);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_string_pool_ref_t tb_string_pool()
{
    return (tb_string_pool_ref_t)tb_singleton_instance(TB_SINGLETON_TYPE_STRING_POOL, tb_string_pool_instance_init, tb_string_pool_instance_exit, tb_null);
}
tb_string_pool_ref_t tb_string_pool_init(tb_bool_t bcase)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_string_pool_impl_t*  impl = tb_null;
    do
    {
        // make pool
        impl = tb_malloc0_type(tb_string_pool_impl_t);
        tb_assert_and_check_break(impl);

        // init lock
        if (!tb_spinlock_init(&impl->lock)) break;

        // init hash
        impl->cache = tb_hash_init(0, tb_item_func_str(bcase, tb_null), tb_item_func_size());
        tb_assert_and_check_break(impl->cache);

        // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
        tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&impl->lock, TB_TRACE_MODULE_NAME);
#endif

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_string_pool_exit((tb_string_pool_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_string_pool_ref_t)impl;
}
tb_void_t tb_string_pool_exit(tb_string_pool_ref_t pool)
{
    // check
    tb_string_pool_impl_t* impl = (tb_string_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // enter
    tb_spinlock_enter(&impl->lock);

    // exit cache
    if (impl->cache) tb_hash_exit(impl->cache);
    impl->cache = tb_null;

    // leave
    tb_spinlock_leave(&impl->lock);

    // exit lock
    tb_spinlock_exit(&impl->lock);

    // exit it
    tb_free(impl);
}
tb_void_t tb_string_pool_clear(tb_string_pool_ref_t pool)
{
    // check
    tb_string_pool_impl_t* impl = (tb_string_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // enter
    tb_spinlock_enter(&impl->lock);

    // clear cache
    if (impl->cache) tb_hash_clear(impl->cache);

    // leave
    tb_spinlock_leave(&impl->lock);
}
tb_char_t const* tb_string_pool_put(tb_string_pool_ref_t pool, tb_char_t const* data)
{
    // check
    tb_string_pool_impl_t* impl = (tb_string_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && data, tb_null);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done
    tb_char_t const* cstr = tb_null;
    if (impl->cache)
    {
        // exists?
        tb_size_t               itor;
        tb_hash_item_t const*   item = tb_null;
        if (((itor = tb_hash_itor(impl->cache, data)) != tb_iterator_tail(impl->cache)) && (item = (tb_hash_item_t const*)tb_iterator_item(impl->cache, itor)))
        {
            // refn
            tb_size_t refn = (tb_size_t)item->data;

            // refn++
            if (refn) tb_iterator_copy(impl->cache, itor, (tb_pointer_t)(refn + 1));
            // no refn? remove it
            else
            {
                // assert
                tb_assert(0);

                // del it
                tb_iterator_delt(impl->cache, itor);
                item = tb_null;
            }
        }
        
        // no item? add it
        if (!item)
        {
            // add it
            if ((itor = tb_hash_set(impl->cache, data, (tb_pointer_t)1)) != tb_iterator_tail(impl->cache))
                item = (tb_hash_item_t const*)tb_iterator_item(impl->cache, itor);
        }

        // save the cstr
        if (item) cstr = (tb_char_t const*)item->name;
    }

    // leave
    tb_spinlock_leave(&impl->lock);

    // ok?
    return cstr;
}
tb_void_t tb_string_pool_del(tb_string_pool_ref_t pool, tb_char_t const* data)
{
    // check
    tb_string_pool_impl_t* impl = (tb_string_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && data);

    // enter
    tb_spinlock_enter(&impl->lock);

    // done
    tb_hash_item_t const* item = tb_null;
    if (impl->cache)
    {
        // exists?
        tb_size_t itor;
        if (((itor = tb_hash_itor(impl->cache, data)) != tb_iterator_tail(impl->cache)) && (item = (tb_hash_item_t const*)tb_iterator_item(impl->cache, itor)))
        {
            // refn
            tb_size_t refn = (tb_size_t)item->data;

            // refn--
            if (refn > 1) tb_iterator_copy(impl->cache, itor, (tb_pointer_t)(refn - 1));
            // del it
            else tb_iterator_delt(impl->cache, itor);
        }
    }

    // leave
    tb_spinlock_leave(&impl->lock);
}
#ifdef __tb_debug__
tb_void_t tb_string_pool_dump(tb_string_pool_ref_t pool)
{
    // check
    tb_string_pool_impl_t* impl = (tb_string_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // enter
    tb_spinlock_enter(&impl->lock);

    // dump cache
    if (impl->cache && tb_hash_size(impl->cache))
    {
        // trace
        tb_trace_i("======================================================================");
        tb_trace_i("size: %lu", tb_hash_size(impl->cache));

        // walk
        tb_for_all (tb_hash_item_t*, item, impl->cache)
        {
            if (item) tb_trace_i("item: refn: %lu, cstr: %s", (tb_size_t)item->data, item->name);
        }
        tb_trace_i("======================================================================");
    }

    // leave
    tb_spinlock_leave(&impl->lock);
}
#endif
