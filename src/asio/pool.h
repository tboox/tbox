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
 * @file		pool.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_POOL_H
#define TB_ASIO_POOL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aicp.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef __tb_debug__
#	define tb_aicp_pool_malloc(aicp, size) 				tb_aicp_pool_malloc_impl(aicp, size, __tb_func__, __tb_line__, __tb_file__)
#	define tb_aicp_pool_malloc0(aicp, size) 			tb_aicp_pool_malloc0_impl(aicp, size, __tb_func__, __tb_line__, __tb_file__)

#	define tb_aicp_pool_nalloc(aicp, item, size) 		tb_aicp_pool_nalloc_impl(aicp, item, size, __tb_func__, __tb_line__, __tb_file__)
#	define tb_aicp_pool_nalloc0(aicp, item, size) 		tb_aicp_pool_nalloc0_impl(aicp, item, size, __tb_func__, __tb_line__, __tb_file__)

#	define tb_aicp_pool_strdup(aicp, data) 				tb_aicp_pool_strdup_impl(aicp, data, __tb_func__, __tb_line__, __tb_file__)
#	define tb_aicp_pool_strndup(aicp, data, size) 		tb_aicp_pool_strndup_impl(aicp, data, size, __tb_func__, __tb_line__, __tb_file__)

#	define tb_aicp_pool_ralloc(aicp, data, size) 		tb_aicp_pool_ralloc_impl(aicp, data, size, __tb_func__, __tb_line__, __tb_file__)
# 	define tb_aicp_pool_free(aicp, data) 				tb_aicp_pool_free_impl(aicp, data, __tb_func__, __tb_line__, __tb_file__)
#else
#	define tb_aicp_pool_malloc(aicp, size) 				tb_aicp_pool_malloc_impl(aicp, size)
#	define tb_aicp_pool_malloc0(aicp, size) 			tb_aicp_pool_malloc0_impl(aicp, size)

#	define tb_aicp_pool_nalloc(aicp, item, size) 		tb_aicp_pool_nalloc_impl(aicp, item, size)
#	define tb_aicp_pool_nalloc0(aicp, item, size) 		tb_aicp_pool_nalloc0_impl(aicp, item, size)

#	define tb_aicp_pool_strdup(aicp, data) 				tb_aicp_pool_strdup_impl(aicp, data)
#	define tb_aicp_pool_strndup(aicp, data, size) 		tb_aicp_pool_strndup_impl(aicp, data, size)

#	define tb_aicp_pool_ralloc(aicp, data, size) 		tb_aicp_pool_ralloc_impl(aicp, data, size)
# 	define tb_aicp_pool_free(aicp, data) 				tb_aicp_pool_free_impl(aicp, data)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// malloc
#ifndef __tb_debug__
tb_pointer_t 	tb_aicp_pool_malloc_impl(tb_aicp_t* aicp, tb_size_t size);
#else
tb_pointer_t 	tb_aicp_pool_malloc_impl(tb_aicp_t* aicp, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// malloc0
#ifndef __tb_debug__
tb_pointer_t 	tb_aicp_pool_malloc0_impl(tb_aicp_t* aicp, tb_size_t size);
#else
tb_pointer_t 	tb_aicp_pool_malloc0_impl(tb_aicp_t* aicp, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// nalloc
#ifndef __tb_debug__
tb_pointer_t  	tb_aicp_pool_nalloc_impl(tb_aicp_t* aicp, tb_size_t item, tb_size_t size);
#else
tb_pointer_t  	tb_aicp_pool_nalloc_impl(tb_aicp_t* aicp, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// nalloc0
#ifndef __tb_debug__
tb_pointer_t  	tb_aicp_pool_nalloc0_impl(tb_aicp_t* aicp, tb_size_t item, tb_size_t size);
#else
tb_pointer_t  	tb_aicp_pool_nalloc0_impl(tb_aicp_t* aicp, tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// ralloc
#ifndef __tb_debug__
tb_pointer_t 	tb_aicp_pool_ralloc_impl(tb_aicp_t* aicp, tb_pointer_t data, tb_size_t size);
#else
tb_pointer_t 	tb_aicp_pool_ralloc_impl(tb_aicp_t* aicp, tb_pointer_t data, tb_size_t size, tb_char_t const* func,  tb_size_t line, tb_char_t const* file);
#endif

// strdup
#ifndef __tb_debug__
tb_char_t* 		tb_aicp_pool_strdup_impl(tb_aicp_t* aicp, tb_char_t const* data);
#else
tb_char_t* 		tb_aicp_pool_strdup_impl(tb_aicp_t* aicp, tb_char_t const* data, tb_char_t const* func,  tb_size_t line, tb_char_t const* file);
#endif

// strndup
#ifndef __tb_debug__
tb_char_t* 		tb_aicp_pool_strndup_impl(tb_aicp_t* aicp, tb_char_t const* data, tb_size_t size);
#else
tb_char_t* 		tb_aicp_pool_strndup_impl(tb_aicp_t* aicp, tb_char_t const* data, tb_size_t size, tb_char_t const* func,  tb_size_t line, tb_char_t const* file);
#endif

// free
#ifndef __tb_debug__
tb_bool_t 		tb_aicp_pool_free_impl(tb_aicp_t* aicp, tb_pointer_t data);
#else
tb_bool_t 		tb_aicp_pool_free_impl(tb_aicp_t* aicp, tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

#endif
