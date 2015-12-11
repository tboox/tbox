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
 * @file        large_pool.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "large_pool"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "large_pool.h"
#include "impl/impl.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// is native pool?
#define tb_large_pool_is_native(pool)        (((tb_size_t)(pool)) & 0x1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_large_pool_ref_t tb_large_pool_init(tb_byte_t* data, tb_size_t size)
{
    // init pool
    return (data && size)? tb_static_large_pool_init(data, size, tb_page_size()) : tb_native_large_pool_init();
}
tb_void_t tb_large_pool_exit(tb_large_pool_ref_t pool)
{
    // check
    tb_assert_and_check_return(pool);

    // exit pool
    if (tb_large_pool_is_native(pool)) tb_native_large_pool_exit(pool);
    else tb_static_large_pool_exit(pool);
}
tb_void_t tb_large_pool_clear(tb_large_pool_ref_t pool)
{
    // check
    tb_assert_and_check_return(pool);

    // clear pool
    if (tb_large_pool_is_native(pool)) tb_native_large_pool_clear(pool);
    else tb_static_large_pool_clear(pool);
}

