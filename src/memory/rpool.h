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
 * @file		rpool.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_RPOOL_H
#define TB_MEMORY_RPOOL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_RPOOL_GROW_MICRO 				(128)
#define TB_RPOOL_GROW_SMALL 				(1024)
#define TB_RPOOL_GROW_LARGE 				(4096)

#ifdef __tb_small__
# 	define TB_RPOOL_GROW_DEFAULT 			TB_RPOOL_GROW_SMALL
#else
# 	define TB_RPOOL_GROW_DEFAULT 			TB_RPOOL_GROW_LARGE
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init regular pool
 *
 * @param grow 		the grow
 * @param step 		the step
 * @param align 	the align
 *
 * @return 			the pool handle
 */
tb_handle_t 		tb_rpool_init(tb_size_t grow, tb_size_t step, tb_size_t align);

/*! exit regular pool
 *
 * @param handle 	the pool handle
 */
tb_void_t 			tb_rpool_exit(tb_handle_t handle);

/*! the regular pool item count
 *
 * @param handle 	the pool handle
 *
 * @return 			the item count
 */
tb_size_t 			tb_rpool_size(tb_handle_t handle);

/*! clear regular pool
 *
 * @param handle 	the pool handle
 */
tb_void_t 			tb_rpool_clear(tb_handle_t handle);

/*! malloc regular pool data
 *
 * @param handle 	the pool handle
 * 
 * @return 			the data pointer
 */
tb_pointer_t 		tb_rpool_malloc(tb_handle_t handle);

/*! malloc regular pool data and clear it
 *
 * @param handle 	the pool handle
 *
 * @return 			the data pointer
 */
tb_pointer_t 		tb_rpool_malloc0(tb_handle_t handle);

/*! duplicate regular pool data 
 *
 * @param handle 	the pool handle
 * @param data		the data pointer
 *
 * @return 			the duplicated data pointer
 */
tb_pointer_t 		tb_rpool_memdup(tb_handle_t handle, tb_cpointer_t data);

/*! free regular pool data
 *
 * @param handle 	the pool handle
 * @param data 		the pool item data
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_rpool_free(tb_handle_t handle, tb_pointer_t data);

/*! walk regular pool item
 *
 * @code
 * tb_bool_t tb_rpool_item_func(tb_pointer_t item, tb_pointer_t data)
 * {
 * 		// is tail?
 * 		if (!item) ;
 *
 * 		// ok
 * 		return tb_true;
 *
 * fail:
 * 		// break
 * 		return tb_false;
 * }
 * @endcode
 *
 * @param handle 	the pool handle
 * @param func 		the walk func
 * @param data 		the walk data
 *
 */
tb_void_t 			tb_rpool_walk(tb_handle_t handle, tb_bool_t (*func)(tb_pointer_t item, tb_pointer_t data), tb_pointer_t data);

/*! dump regular pool
 *
 * @param handle 	the pool handle
 */
tb_void_t 			tb_rpool_dump(tb_handle_t handle);

#endif
