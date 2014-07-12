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
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "fixed_pool"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fixed_pool.h"
#include "large_pool.h"
#include "impl/static_fixed_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the fixed pool impl type
typedef struct __tb_fixed_pool_impl_t
{
    // the allocated size
    tb_size_t                       size;

    // the slot size
    tb_size_t                       slot_size;

    // the item size
    tb_size_t                       item_size;

    // the init func
    tb_fixed_pool_item_init_func_t  func_init;

    // the exit func
    tb_fixed_pool_item_exit_func_t  func_exit;

    // the private data
    tb_cpointer_t                   func_priv;

}tb_fixed_pool_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_fixed_pool_ref_t tb_fixed_pool_init(tb_size_t slot_size, tb_size_t item_size, tb_fixed_pool_item_init_func_t item_init, tb_fixed_pool_item_exit_func_t item_exit, tb_cpointer_t priv)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_fixed_pool_impl_t*   impl = tb_null;
    do
    {
        // make pool
        impl = (tb_fixed_pool_impl_t*)tb_large_pool_malloc0(tb_large_pool(), sizeof(tb_fixed_pool_impl_t), tb_null);
        tb_assert_and_check_break(impl);

        // init pool
        impl->slot_size = slot_size;
        impl->item_size = item_size;
        impl->func_init = item_init;
        impl->func_exit = item_exit;
        impl->func_priv = priv;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_fixed_pool_exit((tb_fixed_pool_ref_t)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_fixed_pool_ref_t)impl;
}
tb_void_t tb_fixed_pool_exit(tb_fixed_pool_ref_t pool)
{
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

    // clear it
    tb_fixed_pool_clear(pool);

    // exit it
    tb_large_pool_free(tb_large_pool(), impl);
}
tb_size_t tb_fixed_pool_size(tb_fixed_pool_ref_t pool)
{
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)pool;
    tb_assert_and_check_return_val(impl, 0);

    // the size
    return impl->size;
}
tb_void_t tb_fixed_pool_clear(tb_fixed_pool_ref_t pool)
{
    // check
    tb_fixed_pool_impl_t* impl = (tb_fixed_pool_impl_t*)pool;
    tb_assert_and_check_return(impl);

}
tb_pointer_t tb_fixed_pool_malloc_(tb_fixed_pool_ref_t pool __tb_debug_decl__)
{
    return tb_null;
}
tb_pointer_t tb_fixed_pool_malloc0_(tb_fixed_pool_ref_t pool __tb_debug_decl__)
{
    return tb_null;
}
tb_bool_t tb_fixed_pool_free_(tb_fixed_pool_ref_t pool, tb_pointer_t item __tb_debug_decl__)
{
    return tb_false;
}
tb_void_t tb_fixed_pool_walk(tb_fixed_pool_ref_t pool, tb_fixed_pool_item_walk_func_t func, tb_cpointer_t priv)
{
}
#ifdef __tb_debug__
tb_void_t tb_fixed_pool_dump(tb_fixed_pool_ref_t pool)
{
}
#endif
