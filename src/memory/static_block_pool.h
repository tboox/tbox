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
 * @author		ruki
 * @file		static_block_pool.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_STATIC_BLOCK_POOL_H
#define TB_MEMORY_STATIC_BLOCK_POOL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define tb_static_block_pool_malloc(handle, size) 				tb_static_block_pool_malloc_(handle, size __tb_debug_vals__)
#define tb_static_block_pool_malloc0(handle, size) 				tb_static_block_pool_malloc0_(handle, size __tb_debug_vals__)

#define tb_static_block_pool_nalloc(handle, item, size) 		tb_static_block_pool_nalloc_(handle, item, size __tb_debug_vals__)
#define tb_static_block_pool_nalloc0(handle, item, size) 		tb_static_block_pool_nalloc0_(handle, item, size __tb_debug_vals__)

#define tb_static_block_pool_ralloc(handle, data, size) 		tb_static_block_pool_ralloc_(handle, data, size __tb_debug_vals__)
#define tb_static_block_pool_free(handle, data) 				tb_static_block_pool_free_(handle, data __tb_debug_vals__)


/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the static block pool
 * 
 * @param data 		the pool data
 * @param size 		the pool size
 * @param align 	the pool bytes align, using the cpu-aligned if be zero
 *
 * @return 			the pool handle
 */
tb_handle_t 		tb_static_block_pool_init(tb_byte_t* data, tb_size_t size, tb_size_t align);

/*! exit the static block pool
 *
 * @param handle 	the pool handle
 */
tb_void_t 			tb_static_block_pool_exit(tb_handle_t handle);

/*! clear the static block pool
 *
 * @param handle 	the pool handle
 */
tb_void_t 			tb_static_block_pool_clear(tb_handle_t handle);

/*! malloc the memory
 *
 * @param handle 	the pool handle
 * @param size 		the size
 *
 * @return 			the data address
 */
tb_pointer_t 		tb_static_block_pool_malloc_(tb_handle_t handle, tb_size_t size __tb_debug_decl__);

/*! malloc the memory and fill zero 
 *
 * @param handle 	the pool handle
 * @param size 		the size
 *
 * @return 			the data address
 */
tb_pointer_t 		tb_static_block_pool_malloc0_(tb_handle_t handle, tb_size_t size __tb_debug_decl__);

/*! malloc the memory with the item count
 *
 * @param handle 	the pool handle
 * @param item 		the item count
 * @param size 		the item size
 *
 * @return 			the data address
 */
tb_pointer_t  		tb_static_block_pool_nalloc_(tb_handle_t handle, tb_size_t item, tb_size_t size __tb_debug_decl__);

/*! malloc the memory with the item count and fill zero
 *
 * @param handle 	the pool handle
 * @param item 		the item count
 * @param size 		the item size
 *
 * @return 			the data address
 */
tb_pointer_t  		tb_static_block_pool_nalloc0_(tb_handle_t handle, tb_size_t item, tb_size_t size __tb_debug_decl__);

/*! realloc the memory
 *
 * @param handle 	the pool handle
 * @param data 		the data address
 * @param size 		the data size
 *
 * @return 			the new data address
 */
tb_pointer_t 		tb_static_block_pool_ralloc_(tb_handle_t handle, tb_pointer_t data, tb_size_t size __tb_debug_decl__);

/*! free the memory
 *
 * @param handle 	the pool handle
 * @param data 		the data address
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_static_block_pool_free_(tb_handle_t handle, tb_pointer_t data __tb_debug_decl__);

#ifdef __tb_debug__
/*! the memory data size
 *
 * @param handle 	the pool handle
 * @param data 		the data address
 *
 * @return 			the data size
 */
tb_size_t 			tb_static_block_pool_data_size(tb_handle_t handle, tb_cpointer_t data);

/*! dump the memory data 
 *
 * @param handle 	the pool handle
 * @param data 		the data address
 * @param prefix 	the trace prefix
 */
tb_void_t 			tb_static_block_pool_data_dump(tb_handle_t handle, tb_cpointer_t data, tb_char_t const* prefix);

/*! dump the static block pool
 *
 * @param handle 	the pool handle
 * @param prefix 	the trace prefix
 */
tb_void_t 			tb_static_block_pool_dump(tb_handle_t handle, tb_char_t const* prefix);
#endif

#endif
