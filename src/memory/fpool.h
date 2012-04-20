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
 * @file		fpool.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_FPOOL_H
#define TB_MEMORY_FPOOL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init
tb_handle_t 	tb_fpool_init(tb_byte_t* data, tb_size_t size, tb_size_t step, tb_size_t align);

// exit
tb_void_t 		tb_fpool_exit(tb_handle_t handle);

// clear
tb_void_t 		tb_fpool_clear(tb_handle_t handle);

// malloc
tb_pointer_t 	tb_fpool_malloc(tb_handle_t handle);

// malloc0
tb_pointer_t 	tb_fpool_malloc0(tb_handle_t handle);

// free
tb_bool_t 		tb_fpool_free(tb_handle_t handle, tb_pointer_t data);

// dump
tb_void_t 		tb_fpool_dump(tb_handle_t handle);

#endif
