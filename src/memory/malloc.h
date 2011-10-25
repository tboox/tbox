/*!The Tiny Box Library
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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		malloc.h
 *
 */
#ifndef TB_MEMORY_MALLOC_H
#define TB_MEMORY_MALLOC_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "mpool.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_bool_t 		tb_memory_init(tb_void_t* data, tb_size_t size);
tb_void_t 		tb_memory_exit();

#ifndef TB_DEBUG
tb_void_t* 		tb_memory_allocate(tb_size_t size);
tb_void_t*  	tb_memory_callocate(tb_size_t item, tb_size_t size);
tb_void_t* 		tb_memory_reallocate(tb_void_t* data, tb_size_t size);
tb_void_t 		tb_memory_deallocate(tb_void_t* data);
#else
tb_void_t* 		tb_memory_allocate(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
tb_void_t*  	tb_memory_callocate(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
tb_void_t* 		tb_memory_reallocate(tb_void_t* data, tb_size_t size,tb_char_t const* func,  tb_size_t line, tb_char_t const* file);
tb_void_t 		tb_memory_deallocate(tb_void_t* data, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

tb_void_t 		tb_memory_dump();
tb_bool_t 		tb_memory_check();


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

