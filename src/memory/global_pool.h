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
 * @file		global_pool.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_GLOBAL_POOL_H
#define TB_MEMORY_GLOBAL_POOL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define tb_global_pool_malloc(handle, size) 			tb_global_pool_malloc_(handle, size __tb_debug_vals__)
#define tb_global_pool_malloc0(handle, size) 			tb_global_pool_malloc0_(handle, size __tb_debug_vals__)

#define tb_global_pool_nalloc(handle, item, size) 		tb_global_pool_nalloc_(handle, item, size __tb_debug_vals__)
#define tb_global_pool_nalloc0(handle, item, size) 		tb_global_pool_nalloc0_(handle, item, size __tb_debug_vals__)

#define tb_global_pool_ralloc(handle, data, size) 		tb_global_pool_ralloc_(handle, data, size __tb_debug_vals__)
#define tb_global_pool_free(handle, data) 				tb_global_pool_free_(handle, data __tb_debug_vals__)

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init
tb_handle_t 	tb_global_pool_init(tb_byte_t* data, tb_size_t size, tb_size_t align);

// exit
tb_void_t 		tb_global_pool_exit(tb_handle_t handle);

// clear
tb_void_t 		tb_global_pool_clear(tb_handle_t handle);

// malloc
tb_pointer_t 	tb_global_pool_malloc_(tb_handle_t handle, tb_size_t size __tb_debug_decl__);

// malloc0
tb_pointer_t 	tb_global_pool_malloc0_(tb_handle_t handle, tb_size_t size __tb_debug_decl__);

// nalloc
tb_pointer_t  	tb_global_pool_nalloc_(tb_handle_t handle, tb_size_t item, tb_size_t size __tb_debug_decl__);

// nalloc0
tb_pointer_t  	tb_global_pool_nalloc0_(tb_handle_t handle, tb_size_t item, tb_size_t size __tb_debug_decl__);

// ralloc
tb_pointer_t 	tb_global_pool_ralloc_(tb_handle_t handle, tb_pointer_t data, tb_size_t size __tb_debug_decl__);

// free
tb_bool_t 		tb_global_pool_free_(tb_handle_t handle, tb_pointer_t data __tb_debug_decl__);

// data size
tb_size_t 		tb_global_pool_data_size(tb_handle_t handle, tb_cpointer_t data);

// data dump
tb_void_t 		tb_global_pool_data_dump(tb_handle_t handle, tb_cpointer_t data, tb_char_t const* prefix);

// dump
tb_void_t 		tb_global_pool_dump(tb_handle_t handle);

#endif
