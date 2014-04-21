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
 *
 */
#ifndef TB_PREFIX_MALLOC_H
#define TB_PREFIX_MALLOC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "keyword.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define tb_malloc(size) 			tb_memory_malloc_(size __tb_debug_vals__)
#define tb_malloc0(size) 			tb_memory_malloc0_(size __tb_debug_vals__)
#define tb_nalloc(item, size) 		tb_memory_nalloc_(item, size __tb_debug_vals__)
#define tb_nalloc0(item, size) 		tb_memory_nalloc0_(item, size __tb_debug_vals__)
#define tb_ralloc(data, size) 		tb_memory_ralloc_((tb_pointer_t)data, size __tb_debug_vals__)
#define tb_free(data) 				tb_memory_free_((tb_pointer_t)data __tb_debug_vals__)

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_pointer_t 	tb_memory_malloc_(tb_size_t size __tb_debug_decl__);
tb_pointer_t 	tb_memory_malloc0_(tb_size_t size __tb_debug_decl__);
tb_pointer_t  	tb_memory_nalloc_(tb_size_t item, tb_size_t size __tb_debug_decl__);
tb_pointer_t  	tb_memory_nalloc0_(tb_size_t item, tb_size_t size __tb_debug_decl__);
tb_pointer_t 	tb_memory_ralloc_(tb_pointer_t data, tb_size_t size __tb_debug_decl__);
tb_bool_t 		tb_memory_free_(tb_pointer_t data __tb_debug_decl__);


#endif


