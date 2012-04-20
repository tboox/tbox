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
 * @file		gpool.h
 * @ingroup 	memory
 *
 */
#ifndef TB_MEMORY_GPOOL_H
#define TB_MEMORY_GPOOL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_DEBUG
#	define tb_gpool_malloc(handle, size) 				tb_gpool_malloc_impl(handle, size, __tb_func__, __tb_line__, __tb_file__)
#	define tb_gpool_malloc0(handle, size) 				tb_gpool_malloc0_impl(handle, size, __tb_func__, __tb_line__, __tb_file__)

#	define tb_gpool_nalloc(handle, item, size) 			tb_gpool_nalloc_impl(handle, item, size, __tb_func__, __tb_line__, __tb_file__)
#	define tb_gpool_nalloc0(handle, item, size) 		tb_gpool_nalloc0_impl(handle, item, size, __tb_func__, __tb_line__, __tb_file__)

#	define tb_gpool_ralloc(handle, data, size) 			tb_gpool_ralloc_impl(handle, data, size, __tb_func__, __tb_line__, __tb_file__)
# 	define tb_gpool_free(handle, data) 					tb_gpool_free_impl(handle, data, __tb_func__, __tb_line__, __tb_file__)
#else
#	define tb_gpool_malloc(handle, size) 				tb_gpool_malloc_impl(handle, size)
#	define tb_gpool_malloc0(handle, size) 				tb_gpool_malloc0_impl(handle, size)

#	define tb_gpool_nalloc(handle, item, size) 			tb_gpool_nalloc_impl(handle, item, size)
#	define tb_gpool_nalloc0(handle, item, size) 		tb_gpool_nalloc0_impl(handle, item, size)

#	define tb_gpool_ralloc(handle, data, size) 			tb_gpool_ralloc_impl(handle, data, size)
# 	define tb_gpool_free(handle, data) 					tb_gpool_free_impl(handle, data)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init
tb_handle_t 	tb_gpool_init(tb_byte_t* data, tb_size_t size, tb_size_t align);

// exit
tb_void_t 		tb_gpool_exit(tb_handle_t handle);

// clear
tb_void_t 		tb_gpool_clear(tb_handle_t handle);

// malloc
#ifndef TB_DEBUG
tb_pointer_t 	tb_gpool_malloc_impl(tb_handle_t handle, tb_size_t size);
#else
tb_pointer_t 	tb_gpool_malloc_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// malloc0
#ifndef TB_DEBUG
tb_pointer_t 	tb_gpool_malloc0_impl(tb_handle_t handle, tb_size_t size);
#else
tb_pointer_t 	tb_gpool_malloc0_impl(tb_handle_t handle, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// nalloc
#ifndef TB_DEBUG
tb_pointer_t  	tb_gpool_nalloc_impl(tb_handle_t handle, tb_size_t item, tb_size_t size);
#else
tb_pointer_t  	tb_gpool_nalloc_impl(tb_handle_t handle, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// nalloc0
#ifndef TB_DEBUG
tb_pointer_t  	tb_gpool_nalloc0_impl(tb_handle_t handle, tb_size_t item, tb_size_t size);
#else
tb_pointer_t  	tb_gpool_nalloc0_impl(tb_handle_t handle, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// ralloc
#ifndef TB_DEBUG
tb_pointer_t 	tb_gpool_ralloc_impl(tb_handle_t handle, tb_pointer_t data, tb_size_t size);
#else
tb_pointer_t 	tb_gpool_ralloc_impl(tb_handle_t handle, tb_pointer_t data, tb_size_t size, tb_char_t const* func,  tb_size_t line, tb_char_t const* file);
#endif

// free
#ifndef TB_DEBUG
tb_bool_t 		tb_gpool_free_impl(tb_handle_t handle, tb_pointer_t data);
#else
tb_bool_t 		tb_gpool_free_impl(tb_handle_t handle, tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// dump
tb_void_t 		tb_gpool_dump(tb_handle_t handle);

#endif
