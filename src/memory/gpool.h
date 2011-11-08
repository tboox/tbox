/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		gpool.h
 *
 */
#ifndef TB_MEMORY_GPOOL_H
#define TB_MEMORY_GPOOL_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_DEBUG
#	define tb_gpool_malloc(hpool, size) 				tb_gpool_allocate(hpool, size, __tb_func__, __tb_line__, __tb_file__)
# 	define tb_gpool_calloc(hpool, item, size) 			tb_gpool_callocate(hpool, item, size, __tb_func__, __tb_line__, __tb_file__)
# 	define tb_gpool_realloc(hpool, data, size) 			tb_gpool_reallocate(hpool, data, size, __tb_func__, __tb_line__, __tb_file__)
# 	define tb_gpool_free(hpool, data) 					tb_gpool_deallocate(hpool, data, __tb_func__, __tb_line__, __tb_file__)
#else
# 	define tb_gpool_malloc(hpool, size) 				tb_gpool_allocate(hpool, size)
# 	define tb_gpool_calloc(hpool, item, size) 			tb_gpool_callocate(hpool, item, size)
# 	define tb_gpool_realloc(hpool, data, size) 			tb_gpool_reallocate(hpool, data, size)
# 	define tb_gpool_free(hpool, data) 					tb_gpool_deallocate(hpool, data)
#endif

#define TB_GPOOL_SIZE_MIN 								g_gpool_size_min

/* /////////////////////////////////////////////////////////
 * the globals
 */
extern tb_size_t const g_gpool_size_min;

/* /////////////////////////////////////////////////////////
 * the interfaces
 */
tb_handle_t 	tb_gpool_init(tb_void_t* data, tb_size_t size);
tb_void_t 		tb_gpool_exit(tb_handle_t hpool);

#ifndef TB_DEBUG
tb_void_t* 		tb_gpool_allocate(tb_handle_t hpool, tb_size_t size);
tb_void_t*  	tb_gpool_callocate(tb_handle_t hpool, tb_size_t item, tb_size_t size);
tb_void_t* 		tb_gpool_reallocate(tb_handle_t hpool, tb_void_t* data, tb_size_t size);
tb_bool_t 		tb_gpool_deallocate(tb_handle_t hpool, tb_void_t* data);
#else
tb_void_t* 		tb_gpool_allocate(tb_handle_t hpool, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
tb_void_t*  	tb_gpool_callocate(tb_handle_t hpool, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
tb_void_t* 		tb_gpool_reallocate(tb_handle_t hpool, tb_void_t* data, tb_size_t size,tb_char_t const* func,  tb_size_t line, tb_char_t const* file);
tb_bool_t 		tb_gpool_deallocate(tb_handle_t hpool, tb_void_t* data, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

tb_void_t 		tb_gpool_dump(tb_handle_t hpool);
tb_bool_t 		tb_gpool_check(tb_handle_t hpool);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
