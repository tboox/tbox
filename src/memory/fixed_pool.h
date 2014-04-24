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
 * @file		fixed_pool.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_FIXED_POOL_H
#define TB_MEMORY_FIXED_POOL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_FIXED_POOL_GROW_MICRO 				(128)
#define TB_FIXED_POOL_GROW_SMALL 				(1024)
#define TB_FIXED_POOL_GROW_LARGE 				(4096)
#ifdef __tb_small__
# 	define TB_FIXED_POOL_GROW_DEFAULT 			TB_FIXED_POOL_GROW_SMALL
#else
# 	define TB_FIXED_POOL_GROW_DEFAULT 			TB_FIXED_POOL_GROW_LARGE
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init fixed pool
 *
 * @param grow 		the grow
 * @param step 		the step
 * @param align 	the align
 *
 * @return 			the pool handle
 */
tb_handle_t 		tb_fixed_pool_init(tb_size_t grow, tb_size_t step, tb_size_t align);

/*! exit fixed pool
 *
 * @param handle 	the pool handle
 */
tb_void_t 			tb_fixed_pool_exit(tb_handle_t handle);

/*! the fixed pool item count
 *
 * @param handle 	the pool handle
 *
 * @return 			the item count
 */
tb_size_t 			tb_fixed_pool_size(tb_handle_t handle);

/*! clear fixed pool
 *
 * @param handle 	the pool handle
 */
tb_void_t 			tb_fixed_pool_clear(tb_handle_t handle);

/*! malloc fixed pool data
 *
 * @param handle 	the pool handle
 * 
 * @return 			the data pointer
 */
tb_pointer_t 		tb_fixed_pool_malloc(tb_handle_t handle);

/*! malloc fixed pool data and clear it
 *
 * @param handle 	the pool handle
 *
 * @return 			the data pointer
 */
tb_pointer_t 		tb_fixed_pool_malloc0(tb_handle_t handle);

/*! duplicate fixed pool data 
 *
 * @param handle 	the pool handle
 * @param data		the data pointer
 *
 * @return 			the duplicated data pointer
 */
tb_pointer_t 		tb_fixed_pool_memdup(tb_handle_t handle, tb_cpointer_t data);

/*! free fixed pool data
 *
 * @param handle 	the pool handle
 * @param data 		the pool item data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_fixed_pool_free(tb_handle_t handle, tb_pointer_t data);

/*! walk fixed pool item
 *
 * @code
 * tb_bool_t tb_fixed_pool_item_func(tb_pointer_t item, tb_pointer_t data)
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
tb_void_t 			tb_fixed_pool_walk(tb_handle_t handle, tb_bool_t (*func)(tb_pointer_t item, tb_pointer_t data), tb_pointer_t data);

#ifdef __tb_debug__
/*! dump fixed pool
 *
 * @param handle 	the pool handle
 */
tb_void_t 			tb_fixed_pool_dump(tb_handle_t handle);
#endif

#endif
