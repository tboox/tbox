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
 * @file        static_fixed_pool.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "static_fixed_pool"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_fixed_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_static_fixed_pool_ref_t tb_static_fixed_pool_init(tb_pointer_t data, tb_size_t size, tb_size_t item_size)
{
    return tb_null;
}
tb_void_t tb_static_fixed_pool_exit(tb_static_fixed_pool_ref_t pool)
{
}
tb_size_t tb_static_fixed_pool_size(tb_static_fixed_pool_ref_t pool)
{
    return 0;
}
tb_size_t tb_static_fixed_pool_maxn(tb_static_fixed_pool_ref_t pool)
{
    return 0;
}
tb_bool_t tb_static_fixed_pool_full(tb_static_fixed_pool_ref_t pool)
{
    return tb_false;
}
tb_bool_t tb_static_fixed_pool_null(tb_static_fixed_pool_ref_t pool)
{
    return tb_false;
}
tb_void_t tb_static_fixed_pool_clear(tb_static_fixed_pool_ref_t pool)
{
}
tb_pointer_t tb_static_fixed_pool_malloc(tb_static_fixed_pool_ref_t pool __tb_debug_decl__)
{
    return tb_null;
}
tb_bool_t tb_static_fixed_pool_free(tb_static_fixed_pool_ref_t pool, tb_pointer_t item __tb_debug_decl__)
{
    return tb_false;
}
tb_void_t tb_static_fixed_pool_walk(tb_static_fixed_pool_ref_t pool, tb_fixed_pool_item_walk_func_t func, tb_cpointer_t priv)
{
}
#ifdef __tb_debug__
tb_void_t tb_static_fixed_pool_dump(tb_static_fixed_pool_ref_t pool)
{
}
#endif
