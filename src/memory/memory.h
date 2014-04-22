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
 * @file		memory.h
 * @defgroup 	memory
 *
 */
#ifndef TB_MEMORY_H
#define TB_MEMORY_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "static_block_pool.h"
#include "static_fixed_pool.h"
#include "static_tiny_pool.h"
#include "global_pool.h"
#include "block_pool.h"
#include "fixed_pool.h"
#include "scoped_buffer.h"
#include "static_buffer.h"
#include "queue_buffer.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * description
 */

/*!architecture
 *
 * <pre>
 * block_pool --------------------- static_block_pool --
 *                |                     |              |
 *             global_pool -------------|              | 
 *                |                 static_tiny_pool --|- data: |-------------------------------|
 *                |                                    |
 * fixed_pool --------------------- static_fixed_pool --
 * </pre>
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init memory
 *
 * @param data 			the memory pool data
 * @param size 			the memory pool size
 * @param align 		the memory pool data align bytes
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_memory_init(tb_byte_t* data, tb_size_t size, tb_size_t align);

/// exit memory
tb_void_t 				tb_memory_exit(tb_noarg_t);

/*! malloc the memory
 *
 * @param size 			the size
 *
 * @return 				the data address
 */
tb_pointer_t 			tb_memory_malloc_(tb_size_t size __tb_debug_decl__);

/*! malloc the memory and fill zero 
 *
 * @param size 			the size
 *
 * @return 				the data address
 */
tb_pointer_t 			tb_memory_malloc0_(tb_size_t size __tb_debug_decl__);

/*! malloc the memory with the item count
 *
 * @param item 			the item count
 * @param size 			the item size
 *
 * @return 				the data address
 */
tb_pointer_t  			tb_memory_nalloc_(tb_size_t item, tb_size_t size __tb_debug_decl__);

/*! malloc the memory with the item count and fill zero
 *
 * @param item 			the item count
 * @param size 			the item size
 *
 * @return 				the data address
 */
tb_pointer_t  			tb_memory_nalloc0_(tb_size_t item, tb_size_t size __tb_debug_decl__);

/*! realloc the memory
 *
 * @param data 			the data address
 * @param size 			the size
 *
 * @return 				the new data address
 */
tb_pointer_t 			tb_memory_ralloc_(tb_pointer_t data, tb_size_t size __tb_debug_decl__);

/*! free the memory
 *
 * @param data 			the data address
 *
 * @return 				tb_true or tb_false
 */
tb_bool_t 				tb_memory_free_(tb_pointer_t data __tb_debug_decl__);

#ifdef __tb_debug__
/// dump memory
tb_void_t 				tb_memory_dump(tb_noarg_t);

/*! the data size in the memory only for the pool mode
 *
 * @param data 			the data address
 *
 * @return 				the data size, return zero if using the native memory
 */
tb_size_t 				tb_memory_data_size(tb_cpointer_t data);

/*! dump the memory data only for the pool mode
 *
 * @param data 			the data address
 * @param prefix 		the dump prefix info
 */
tb_void_t 				tb_memory_data_dump(tb_cpointer_t data, tb_char_t const* prefix);
#endif

#endif

