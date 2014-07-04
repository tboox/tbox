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
 * @file        page_pool.h
 * @ingroup     memory
 *
 */
#ifndef TB_MEMORY_PAGE_POOL_H
#define TB_MEMORY_PAGE_POOL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define tb_page_pool_malloc(handle, size)             tb_page_pool_malloc_(handle, size __tb_debug_vals__)
#define tb_page_pool_malloc0(handle, size)            tb_page_pool_malloc0_(handle, size __tb_debug_vals__)

#define tb_page_pool_nalloc(handle, item, size)       tb_page_pool_nalloc_(handle, item, size __tb_debug_vals__)
#define tb_page_pool_nalloc0(handle, item, size)      tb_page_pool_nalloc0_(handle, item, size __tb_debug_vals__)

#define tb_page_pool_ralloc(handle, data, size)       tb_page_pool_ralloc_(handle, data, size __tb_debug_vals__)
#define tb_page_pool_free(handle, data)               tb_page_pool_free_(handle, data __tb_debug_vals__)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the page pool ref type
 *
 *  -------------------------      ----------------------
 * |       native memory     |    |         data         |
 *  -------------------------      ---------------------- 
 *              |                             |
 *  -------------------------      ----------------------
 * |       native pool       |    |      static pool     |
 *  -------------------------      ---------------------- 
 *              |                             |
 *  -----------------------------------------------------
 * |  if (pool address & 1)     |           else         |
 * |-----------------------------------------------------|
 * |                       page pool                     |
 *  ----------------------------------------------------- 
 *
 */
typedef struct{}*       tb_page_pool_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the page pool and the allocated data will be aligned by the page size
 * 
 * @param data          the pool data, using the native memory if be null
 * @param size          the pool size
 *
 * @return              the pool 
 */
tb_page_pool_ref_t      tb_page_pool_init(tb_byte_t* data, tb_size_t size);

/*! exit the pool
 *
 * @param pool          the pool 
 */
tb_void_t               tb_page_pool_exit(tb_page_pool_ref_t pool);

/*! clear the pool
 *
 * @param pool          the pool 
 */
tb_void_t               tb_page_pool_clear(tb_page_pool_ref_t pool);

/*! malloc data
 *
 * @param pool          the pool 
 * @param size          the size
 *
 * @return              the data address
 */
tb_pointer_t            tb_page_pool_malloc_(tb_page_pool_ref_t pool, tb_size_t size __tb_debug_decl__);

/*! malloc data and fill zero 
 *
 * @param pool          the pool 
 * @param size          the size and must be aligned by the page size
 *
 * @return              the data address
 */
tb_pointer_t            tb_page_pool_malloc0_(tb_page_pool_ref_t pool, tb_size_t size __tb_debug_decl__);

/*! malloc data with the item count
 *
 * @param pool          the pool 
 * @param item          the item count
 * @param size          the item size and must be aligned by the page size
 *
 * @return              the data address
 */
tb_pointer_t            tb_page_pool_nalloc_(tb_page_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__);

/*! malloc data with the item count and fill zero
 *
 * @param pool          the pool 
 * @param item          the item count
 * @param size          the item size and must be aligned by the page size
 *
 * @return              the data address
 */
tb_pointer_t            tb_page_pool_nalloc0_(tb_page_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__);

/*! realloc data
 *
 * @param pool          the pool 
 * @param data          the data address
 * @param size          the data size and must be aligned by the page size
 *
 * @return              the new data address
 */
tb_pointer_t            tb_page_pool_ralloc_(tb_page_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__);

/*! free data
 *
 * @param pool          the pool 
 * @param data          the data address
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_page_pool_free_(tb_page_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__);

#ifdef __tb_debug__
/*! the data size
 *
 * @param handle        the pool
 * @param data          the data address
 *
 * @return              the data size
 */
tb_size_t               tb_page_pool_data_size(tb_page_pool_ref_t pool, tb_cpointer_t data);

/*! dump the data 
 *
 * @param handle        the pool
 * @param data          the data address
 * @param prefix        the trace prefix
 */
tb_void_t               tb_page_pool_data_dump(tb_page_pool_ref_t pool, tb_cpointer_t data, tb_char_t const* prefix);

/*! dump the pool
 *
 * @param handle        the pool
 * @param prefix        the trace prefix
 */
tb_void_t               tb_page_pool_dump(tb_page_pool_ref_t pool);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
