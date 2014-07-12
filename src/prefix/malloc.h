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
 * @author      ruki
 * @file        malloc.h
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
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define tb_malloc(size)                     tb_pool_malloc_(tb_pool(), size __tb_debug_vals__)
#define tb_malloc0(size)                    tb_pool_malloc0_(tb_pool(), size __tb_debug_vals__)
#define tb_nalloc(item, size)               tb_pool_nalloc_(tb_pool(), item, size __tb_debug_vals__)
#define tb_nalloc0(item, size)              tb_pool_nalloc0_(tb_pool(), item, size __tb_debug_vals__)
#define tb_ralloc(data, size)               tb_pool_ralloc_(tb_pool(), (tb_pointer_t)data, size __tb_debug_vals__)

#define tb_malloc_cstr(size)                (tb_char_t*)tb_pool_malloc_(tb_pool(), size __tb_debug_vals__)
#define tb_malloc0_cstr(size)               (tb_char_t*)tb_pool_malloc0_(tb_pool(), size __tb_debug_vals__)
#define tb_nalloc_cstr(item, size)          (tb_char_t*)tb_pool_nalloc_(tb_pool(), item, size __tb_debug_vals__)
#define tb_nalloc0_cstr(item, size)         (tb_char_t*)tb_pool_nalloc0_(tb_pool(), item, size __tb_debug_vals__)
#define tb_ralloc_cstr(data, size)          (tb_char_t*)tb_pool_ralloc_(tb_pool(), (tb_pointer_t)data, size __tb_debug_vals__)

#define tb_malloc_bytes(size)               (tb_byte_t*)tb_pool_malloc_(tb_pool(), size __tb_debug_vals__)
#define tb_malloc0_bytes(size)              (tb_byte_t*)tb_pool_malloc0_(tb_pool(), size __tb_debug_vals__)
#define tb_nalloc_bytes(item, size)         (tb_byte_t*)tb_pool_nalloc_(tb_pool(), item, size __tb_debug_vals__)
#define tb_nalloc0_bytes(item, size)        (tb_byte_t*)tb_pool_nalloc0_(tb_pool(), item, size __tb_debug_vals__)
#define tb_ralloc_bytes(data, size)         (tb_byte_t*)tb_pool_ralloc_(tb_pool(), (tb_pointer_t)data, size __tb_debug_vals__)

#define tb_malloc_type(type)                (type*)tb_pool_malloc_(tb_pool(), sizeof(type) __tb_debug_vals__)
#define tb_malloc0_type(type)               (type*)tb_pool_malloc0_(tb_pool(), sizeof(type) __tb_debug_vals__)
#define tb_nalloc_type(item, type)          (type*)tb_pool_nalloc_(tb_pool(), item, sizeof(type) __tb_debug_vals__)
#define tb_nalloc0_type(item, type)         (type*)tb_pool_nalloc0_(tb_pool(), item, sizeof(type) __tb_debug_vals__)
#define tb_ralloc_type(data, item, type)    (type*)tb_pool_ralloc_(tb_pool(), (tb_pointer_t)data, ((item) * (size)) __tb_debug_vals__)

#define tb_free(data)                       tb_pool_free_(tb_pool(), (tb_pointer_t)data __tb_debug_vals__)

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_pool_ref_t   tb_pool(tb_noarg_t);
tb_pointer_t    tb_pool_malloc_(tb_pool_ref_t pool, tb_size_t size __tb_debug_decl__);
tb_pointer_t    tb_pool_malloc0_(tb_pool_ref_t pool, tb_size_t size __tb_debug_decl__);
tb_pointer_t    tb_pool_nalloc_(tb_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__);
tb_pointer_t    tb_pool_nalloc0_(tb_pool_ref_t pool, tb_size_t item, tb_size_t size __tb_debug_decl__);
tb_pointer_t    tb_pool_ralloc_(tb_pool_ref_t pool, tb_pointer_t data, tb_size_t size __tb_debug_decl__);
tb_bool_t       tb_pool_free_(tb_pool_ref_t pool, tb_pointer_t data __tb_debug_decl__);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif


