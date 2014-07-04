/*The Treasure Box Library
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
 * @file        native_page_pool.h
 *
 */
#ifndef TB_MEMORY_IMPL_NATIVE_PAGE_POOL_H
#define TB_MEMORY_IMPL_NATIVE_PAGE_POOL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init the native page pool and the allocated data will be aligned by the page size
 * 
 * @return              the pool 
 */
tb_page_pool_ref_t      tb_native_page_pool_init(tb_noarg_t);

/* exit the pool
 *
 * @param pool          the pool 
 */
tb_void_t               tb_native_page_pool_exit(tb_page_pool_ref_t pool);

/* clear the pool
 *
 * @param pool          the pool 
 */
tb_void_t               tb_native_page_pool_clear(tb_page_pool_ref_t pool);

/* malloc data
 *
 * @param pool          the pool 
 * @param size          the size and must be aligned by the page size
 *
 * @return              the data address
 */
tb_pointer_t            tb_native_page_pool_malloc(tb_page_pool_ref_t pool, tb_size_t size __tb_debug_decl__);

/* realloc data
 *
 * @param pool          the pool 
 * @param data          the data address
 * @param size          the size and must be aligned by the page size
 *
 * @return              the new data 
 */
tb_pointer_t            tb_native_page_pool_ralloc(tb_page_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__);

/* free data
 *
 * @param pool          the pool 
 * @param data          the data address
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_native_page_pool_free(tb_page_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__);

#ifdef __tb_debug__
/* the data size
 *
 * @param handle        the pool
 * @param data          the data address
 *
 * @return              the data size
 */
tb_size_t               tb_native_page_pool_data_size(tb_page_pool_ref_t pool, tb_cpointer_t data);

/* dump the data 
 *
 * @param handle        the pool
 * @param data          the data address
 * @param prefix        the trace prefix
 */
tb_void_t               tb_native_page_pool_data_dump(tb_page_pool_ref_t pool, tb_cpointer_t data, tb_char_t const* prefix);

/* dump the pool
 *
 * @param handle        the pool
 * @param prefix        the trace prefix
 */
tb_void_t               tb_native_page_pool_dump(tb_page_pool_ref_t pool);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
