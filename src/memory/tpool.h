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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		tpool.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_TPOOL_H
#define TB_MEMORY_TPOOL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init
tb_handle_t 	tb_tpool_init(tb_byte_t* data, tb_size_t size, tb_size_t align);

// exit
tb_void_t 		tb_tpool_exit(tb_handle_t handle);

// limit
tb_size_t 		tb_tpool_limit(tb_handle_t handle);

// clear
tb_void_t 		tb_tpool_clear(tb_handle_t handle);

// malloc
tb_pointer_t 	tb_tpool_malloc(tb_handle_t handle, tb_size_t size);

// malloc0
tb_pointer_t 	tb_tpool_malloc0(tb_handle_t handle, tb_size_t size);

// nalloc
tb_pointer_t  	tb_tpool_nalloc(tb_handle_t handle, tb_size_t item, tb_size_t size);

// nalloc0
tb_pointer_t  	tb_tpool_nalloc0(tb_handle_t handle, tb_size_t item, tb_size_t size);

// ralloc
tb_pointer_t 	tb_tpool_ralloc(tb_handle_t handle, tb_pointer_t data, tb_size_t size);

// free
tb_bool_t 		tb_tpool_free(tb_handle_t handle, tb_pointer_t data);

// dump
tb_void_t 		tb_tpool_dump(tb_handle_t handle);

#endif
