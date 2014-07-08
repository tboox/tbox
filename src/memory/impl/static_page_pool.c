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
 * @file        static_page_pool.c
 * @ingroup     memory
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "static_page_pool"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "static_page_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * the implementation
 */
tb_page_pool_ref_t tb_static_page_pool_init(tb_byte_t* data, tb_size_t size)
{
    return tb_null;
}
tb_void_t tb_static_page_pool_exit(tb_page_pool_ref_t pool)
{
}
tb_void_t tb_static_page_pool_clear(tb_page_pool_ref_t pool)
{
}
tb_pointer_t tb_static_page_pool_malloc(tb_page_pool_ref_t pool, tb_size_t size __tb_debug_decl__)
{
    return tb_null;
}
tb_pointer_t tb_static_page_pool_ralloc(tb_page_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__)
{
    return tb_null;
}
tb_bool_t tb_static_page_pool_free(tb_page_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__)
{
    return tb_false;
}
#ifdef __tb_debug__
tb_void_t tb_static_page_pool_dump(tb_page_pool_ref_t pool)
{
}
#endif
