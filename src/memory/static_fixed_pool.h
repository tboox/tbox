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
 * @file		static_fixed_pool.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_STATIC_FIXED_POOL_H
#define TB_MEMORY_STATIC_FIXED_POOL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init pool
 *
 * @param data 		the data
 * @param size 		the size
 * @param step 		the step
 * @param align 	the align
 *
 * @return 			the pool handle
 */
tb_handle_t 		tb_static_fixed_pool_init(tb_byte_t* data, tb_size_t size, tb_size_t step, tb_size_t align);

/*! exit pool
 *
 * @param handle 	the pool handle
 */
tb_void_t 			tb_static_fixed_pool_exit(tb_handle_t handle);

/*! the pool item count
 *
 * @param handle 	the pool handle
 *
 * @return 			the item count
 */
tb_size_t 			tb_static_fixed_pool_size(tb_handle_t handle);

/*! clear pool
 *
 * @param handle 	the pool handle
 */
tb_void_t 			tb_static_fixed_pool_clear(tb_handle_t handle);

/*! malloc pool data
 *
 * @param handle 	the pool handle
 * 
 * @return 			the data pointer
 */
tb_pointer_t 		tb_static_fixed_pool_malloc(tb_handle_t handle);

/*! malloc pool data and clear it
 *
 * @param handle 	the pool handle
 *
 * @return 			the data pointer
 */
tb_pointer_t 		tb_static_fixed_pool_malloc0(tb_handle_t handle);

/*! duplicate pool data 
 *
 * @param handle 	the pool handle
 * @param data		the data pointer
 *
 * @return 			the duplicated data pointer
 */
tb_pointer_t 		tb_static_fixed_pool_memdup(tb_handle_t handle, tb_cpointer_t data);

/*! free pool data
 *
 * @param handle 	the pool handle
 * @param data 		the pool item data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_static_fixed_pool_free(tb_handle_t handle, tb_pointer_t data);

/*! walk pool item
 *
 * @code
 * tb_bool_t tb_static_fixed_pool_item_func(tb_pointer_t item, tb_pointer_t data)
 * {
 * 		// ok or break
 * 		return tb_true;
 * }
 * @endcode
 *
 * @param handle 	the pool handle
 * @param func 		the walk func
 * @param data 		the walk data
 *
 */
tb_void_t 			tb_static_fixed_pool_walk(tb_handle_t handle, tb_bool_t (*func)(tb_pointer_t item, tb_pointer_t data), tb_pointer_t data);

#ifdef __tb_debug__
/*! dump pool
 *
 * @param handle 	the pool handle
 */
tb_void_t 			tb_static_fixed_pool_dump(tb_handle_t handle);
#endif


#endif
