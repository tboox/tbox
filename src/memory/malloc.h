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
 * @file		malloc.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_MALLOC_H
#define TB_MEMORY_MALLOC_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define tb_malloc_malloc(size) 				tb_malloc_malloc_impl(size __tb_debug_vals__)
#define tb_malloc_malloc0(size) 			tb_malloc_malloc0_impl(size __tb_debug_vals__)

#define tb_malloc_nalloc(item, size) 		tb_malloc_nalloc_impl(item, size __tb_debug_vals__)
#define tb_malloc_nalloc0(item, size) 		tb_malloc_nalloc0_impl(item, size __tb_debug_vals__)

#define tb_malloc_ralloc(data, size) 		tb_malloc_ralloc_impl(data, size __tb_debug_vals__)
#define tb_malloc_free(data) 				tb_malloc_free_impl(data __tb_debug_vals__)

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init
tb_bool_t 		tb_malloc_init(tb_byte_t* data, tb_size_t size, tb_size_t align);

// exit
tb_void_t 		tb_malloc_exit(tb_noarg_t);

// dump
tb_void_t 		tb_malloc_dump(tb_noarg_t);

// data size
tb_size_t 		tb_malloc_data_size(tb_cpointer_t data);

// data dump
tb_void_t 		tb_malloc_data_dump(tb_cpointer_t data, tb_char_t const* prefix);

// malloc
tb_pointer_t 	tb_malloc_malloc_impl(tb_size_t size __tb_debug_decl__);

// malloc0
tb_pointer_t 	tb_malloc_malloc0_impl(tb_size_t size __tb_debug_decl__);

// nalloc
tb_pointer_t  	tb_malloc_nalloc_impl(tb_size_t item, tb_size_t size __tb_debug_decl__);

// nalloc0
tb_pointer_t  	tb_malloc_nalloc0_impl(tb_size_t item, tb_size_t size __tb_debug_decl__);

// ralloc
tb_pointer_t 	tb_malloc_ralloc_impl(tb_pointer_t data, tb_size_t size __tb_debug_decl__);

// free
tb_bool_t 		tb_malloc_free_impl(tb_pointer_t data __tb_debug_decl__);

#endif

