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
 * @file        object_pool.c
 * @ingroup     object
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "object_pool"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"
#include "../utils/utils.h"
#include "../platform/platform.h"
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef __tb_small__
#   define TB_OBJECT_POOL_GROW      (256 * 1024)
#else
#   define TB_OBJECT_POOL_GROW      (512 * 1024)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the object pool impl type
typedef struct __tb_object_pool_impl_t
{
    // the pool
    tb_pool_ref_t       pool;

    // the lock
    tb_spinlock_t       lock;

}tb_object_pool_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_handle_t tb_object_pool_instance_init(tb_cpointer_t* ppriv)
{
    return tb_object_pool_init();
}
static tb_void_t tb_object_pool_instance_exit(tb_handle_t pool, tb_cpointer_t priv)
{
    if (pool) 
    {
        // dump it
#ifdef __tb_debug__
        tb_object_pool_dump((tb_object_pool_ref_t)pool);
#endif

        // exit it
        tb_object_pool_exit((tb_object_pool_ref_t)pool);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_pool_ref_t tb_object_pool()
{
    return (tb_object_pool_ref_t)tb_singleton_instance(TB_SINGLETON_TYPE_OBJECT_POOL, tb_object_pool_instance_init, tb_object_pool_instance_exit, tb_null);
}
tb_object_pool_ref_t tb_object_pool_init()
{
    // done
    tb_bool_t               ok = tb_false;
    tb_object_pool_impl_t*  impl = tb_null;
    do
    {
        // make pool
        impl = tb_malloc0_type(tb_object_pool_impl_t);
        tb_assert_and_check_break(impl);

        // init lock
        if (!tb_spinlock_init(&impl->lock)) break;

        // init pool
        impl->pool = tb_pool_init(TB_OBJECT_POOL_GROW, 0);
        tb_assert_and_check_break(impl->pool);

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
        if (impl) tb_object_pool_exit((tb_object_pool_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_object_pool_ref_t)impl;
}
tb_void_t tb_object_pool_exit(tb_object_pool_ref_t pool)
{
    // the pool
    tb_object_pool_impl_t* impl = (tb_object_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // enter
    tb_spinlock_enter(&impl->lock);

    // exit pool
    if (impl->pool) tb_pool_exit(impl->pool);
    impl->pool = tb_null;

    // leave
    tb_spinlock_leave(&impl->lock);

    // exit lock
    tb_spinlock_exit(&impl->lock);

    // exit it
    tb_free(impl);
}
tb_void_t tb_object_pool_clear(tb_object_pool_ref_t pool)
{
    // the pool
    tb_object_pool_impl_t* impl = (tb_object_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // enter
    tb_spinlock_enter(&impl->lock);

    // clear pool
    if (impl->pool) tb_pool_clear(impl->pool);

    // leave
    tb_spinlock_leave(&impl->lock);
}
#ifdef __tb_debug__
tb_void_t tb_object_pool_dump(tb_object_pool_ref_t pool)
{
    // the pool
    tb_object_pool_impl_t* impl = (tb_object_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // enter
    tb_spinlock_enter(&impl->lock);

    // dump
    if (impl->pool) tb_pool_dump(impl->pool, "[object_pool]");

    // leave
    tb_spinlock_leave(&impl->lock);
}
#endif
tb_object_ref_t tb_object_pool_get_(tb_object_pool_ref_t pool, tb_size_t size, tb_size_t flag, tb_size_t type __tb_debug_decl__)
{
    // check
    tb_object_pool_impl_t* impl = (tb_object_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl && size && type, tb_null);

    // enter
    tb_spinlock_enter(&impl->lock);

    // make object
    tb_object_ref_t object = impl->pool? (tb_object_ref_t)tb_pool_malloc0_(impl->pool, size __tb_debug_args__) : tb_null;

    // init object
    if (object) 
    {
        if (!tb_object_init(object, flag, type)) 
        {
            tb_pool_free_(impl->pool, object __tb_debug_args__);
            object = tb_null;
        }
    }

    // leave
    tb_spinlock_leave(&impl->lock);

    // ok?
    return object;
}
tb_void_t tb_object_pool_del_(tb_object_pool_ref_t pool, tb_object_ref_t object __tb_debug_decl__)
{
    // check
    tb_object_pool_impl_t* impl = (tb_object_pool_impl_t*)pool;
    tb_assert_and_check_return(impl && object);

    // enter
    tb_spinlock_enter(&impl->lock);

    // exit object
    if (impl->pool) tb_pool_free_(impl->pool, object __tb_debug_args__);

    // leave
    tb_spinlock_leave(&impl->lock);
}

