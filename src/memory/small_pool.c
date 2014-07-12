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
 * @file        small_pool.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "small_pool"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "small_pool.h"
#include "large_pool.h"
#include "fixed_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_small_pool_ref_t tb_small_pool_init()
{
    return tb_null;
}
tb_void_t tb_small_pool_exit(tb_small_pool_ref_t pool)
{
    // check
    tb_assert_and_check_return(pool);

}
tb_void_t tb_small_pool_clear(tb_small_pool_ref_t pool)
{
    // check
    tb_assert_and_check_return(pool);

}
tb_pointer_t tb_small_pool_malloc_(tb_small_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_SMALL_POOL_DATA_SIZE_MAXN, tb_null);

    return tb_null;
}
tb_pointer_t tb_small_pool_malloc0_(tb_small_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_SMALL_POOL_DATA_SIZE_MAXN, tb_null);
    
    return tb_null;
}
tb_pointer_t tb_small_pool_nalloc_(tb_small_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && size, tb_null);
    tb_assert_and_check_return_val(item * size <= TB_SMALL_POOL_DATA_SIZE_MAXN, tb_null);

    return tb_null;
}
tb_pointer_t tb_small_pool_nalloc0_(tb_small_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && size, tb_null);
    tb_assert_and_check_return_val(item * size <= TB_SMALL_POOL_DATA_SIZE_MAXN, tb_null);

    return tb_null;
}
tb_pointer_t tb_small_pool_ralloc_(tb_small_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && data && size, tb_null);
    tb_assert_and_check_return_val(size <= TB_SMALL_POOL_DATA_SIZE_MAXN, tb_null);

    return tb_null;
}
tb_bool_t tb_small_pool_free_(tb_small_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{
    // check
    tb_assert_and_check_return_val(pool && data, tb_false);

    return tb_false;
}
#ifdef __tb_debug__
tb_void_t tb_small_pool_dump(tb_small_pool_ref_t pool)
{
    // check
    tb_assert_and_check_return(pool);
}
#endif
