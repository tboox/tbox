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
 * @file		pool.h
 * @ingroup 	asio
 *
 */
#ifndef TB_ASIO_POOL_H
#define TB_ASIO_POOL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "aico.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define tb_aico_pool_malloc(aico, size) 			tb_aico_pool_malloc_impl(aico, size __tb_debug_vals__)
#define tb_aico_pool_malloc0(aico, size) 			tb_aico_pool_malloc0_impl(aico, size __tb_debug_vals__)

#define tb_aico_pool_nalloc(aico, item, size) 		tb_aico_pool_nalloc_impl(aico, item, size __tb_debug_vals__)
#define tb_aico_pool_nalloc0(aico, item, size) 		tb_aico_pool_nalloc0_impl(aico, item, size __tb_debug_vals__)

#define tb_aico_pool_strdup(aico, data) 			tb_aico_pool_strdup_impl(aico, data __tb_debug_vals__)
#define tb_aico_pool_strndup(aico, data, size) 		tb_aico_pool_strndup_impl(aico, data, size __tb_debug_vals__)

#define tb_aico_pool_ralloc(aico, data, size) 		tb_aico_pool_ralloc_impl(aico, data, size __tb_debug_vals__)
#define tb_aico_pool_free(aico, data) 				tb_aico_pool_free_impl(aico, data __tb_debug_vals__)


/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// malloc
tb_pointer_t 	tb_aico_pool_malloc_impl(tb_aico_t* aico, tb_size_t size __tb_debug_decl__);

// malloc0
tb_pointer_t 	tb_aico_pool_malloc0_impl(tb_aico_t* aico, tb_size_t size __tb_debug_decl__);

// nalloc
tb_pointer_t  	tb_aico_pool_nalloc_impl(tb_aico_t* aico, tb_size_t item, tb_size_t size __tb_debug_decl__);

// nalloc0
tb_pointer_t  	tb_aico_pool_nalloc0_impl(tb_aico_t* aico, tb_size_t item, tb_size_t size __tb_debug_decl__);

// ralloc
tb_pointer_t 	tb_aico_pool_ralloc_impl(tb_aico_t* aico, tb_pointer_t data, tb_size_t size __tb_debug_decl__);

// strdup
tb_char_t* 		tb_aico_pool_strdup_impl(tb_aico_t* aico, tb_char_t const* data __tb_debug_decl__);

// strndup
tb_char_t* 		tb_aico_pool_strndup_impl(tb_aico_t* aico, tb_char_t const* data, tb_size_t size __tb_debug_decl__);

// free
tb_bool_t 		tb_aico_pool_free_impl(tb_aico_t* aico, tb_pointer_t data __tb_debug_decl__);

#endif
