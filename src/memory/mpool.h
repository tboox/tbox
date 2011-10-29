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
 * \file		mpool.h
 *
 */
#ifndef TB_MEMORY_MPOOL_H
#define TB_MEMORY_MPOOL_H

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
#	define tb_mpool_malloc(hpool, size) 				tb_mpool_allocate(hpool, size, __tb_func__, __tb_line__, __tb_file__)
# 	define tb_mpool_calloc(hpool, item, size) 			tb_mpool_callocate(hpool, item, size, __tb_func__, __tb_line__, __tb_file__)
# 	define tb_mpool_realloc(hpool, data, size) 			tb_mpool_reallocate(hpool, data, size, __tb_func__, __tb_line__, __tb_file__)
# 	define tb_mpool_free(hpool, data) 					tb_mpool_deallocate(hpool, data, __tb_func__, __tb_line__, __tb_file__)
#else
# 	define tb_mpool_malloc(hpool, size) 				tb_mpool_allocate(hpool, size)
# 	define tb_mpool_calloc(hpool, item, size) 			tb_mpool_callocate(hpool, item, size)
# 	define tb_mpool_realloc(hpool, data, size) 			tb_mpool_reallocate(hpool, data, size)
# 	define tb_mpool_free(hpool, data) 					tb_mpool_deallocate(hpool, data)
#endif

#define TB_MPOOL_SIZE_MIN 								g_mpool_size_min

/* /////////////////////////////////////////////////////////
 * the globals
 */
extern tb_size_t const g_mpool_size_min;

/* /////////////////////////////////////////////////////////
 * the interfaces
 */
tb_handle_t 	tb_mpool_init(tb_void_t* data, tb_size_t size);
tb_void_t 		tb_mpool_exit(tb_handle_t hpool);

#ifndef TB_DEBUG
tb_void_t* 		tb_mpool_allocate(tb_handle_t hpool, tb_size_t size);
tb_void_t*  	tb_mpool_callocate(tb_handle_t hpool, tb_size_t item, tb_size_t size);
tb_void_t* 		tb_mpool_reallocate(tb_handle_t hpool, tb_void_t* data, tb_size_t size);
tb_bool_t 		tb_mpool_deallocate(tb_handle_t hpool, tb_void_t* data);
#else
tb_void_t* 		tb_mpool_allocate(tb_handle_t hpool, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
tb_void_t*  	tb_mpool_callocate(tb_handle_t hpool, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
tb_void_t* 		tb_mpool_reallocate(tb_handle_t hpool, tb_void_t* data, tb_size_t size,tb_char_t const* func,  tb_size_t line, tb_char_t const* file);
tb_bool_t 		tb_mpool_deallocate(tb_handle_t hpool, tb_void_t* data, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

tb_void_t 		tb_mpool_dump(tb_handle_t hpool);
tb_bool_t 		tb_mpool_check(tb_handle_t hpool);

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
