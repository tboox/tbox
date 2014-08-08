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
 * @file        static_large_pool.h
 */
#ifndef TB_MEMORY_IMPL_STATIC_LARGE_POOL_H
#define TB_MEMORY_IMPL_STATIC_LARGE_POOL_H

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

/* init the large pool and the allocated data will be aligned by the page size
 * 
 * @param data          the pool data
 * @param size          the pool size
 *
 * @return              the pool 
 */
tb_large_pool_ref_t     tb_static_large_pool_init(tb_byte_t* data, tb_size_t size);

/* exit the pool
 *
 * @param pool          the pool 
 */
tb_void_t               tb_static_large_pool_exit(tb_large_pool_ref_t pool);

/* clear the pool
 *
 * @param pool          the pool 
 */
tb_void_t               tb_static_large_pool_clear(tb_large_pool_ref_t pool);

/* malloc data
 *
 * @param pool          the pool 
 * @param size          the size 
 * @param real          the real allocated size >= size, optional
 *
 * @return              the data 
 */
tb_pointer_t            tb_static_large_pool_malloc(tb_large_pool_ref_t pool, tb_size_t size, tb_size_t* real __tb_debug_decl__);

/* realloc data
 *
 * @param pool          the pool 
 * @param data          the data address
 * @param size          the data size 
 * @param real          the real allocated size >= size, optional
 *
 * @return              the new data address 
 */
tb_pointer_t            tb_static_large_pool_ralloc(tb_large_pool_ref_t pool, tb_pointer_t data, tb_size_t size, tb_size_t* real __tb_debug_decl__);

/* free data
 *
 * @param pool          the pool 
 * @param data          the data address
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_static_large_pool_free(tb_large_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__);

#ifdef __tb_debug__
/* dump the pool
 *
 * @param handle        the pool
 * @param prefix        the trace prefix
 */
tb_void_t               tb_static_large_pool_dump(tb_large_pool_ref_t pool);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
