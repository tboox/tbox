/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
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

// dump & check
#ifdef TB_DEBUG
# 	define TB_MPOOL_DUMP() 							tb_mpool_dump()
# 	define TB_MPOOL_CHECK() 						tb_mpool_check()
#else
# 	define TB_MPOOL_DUMP() 					
# 	define TB_MPOOL_CHECK() 						(TB_TRUE)
#endif

/* /////////////////////////////////////////////////////////
 * the interfaces
 */
tb_bool_t 	tb_mpool_init(void* data, tb_size_t size);
void 		tb_mpool_exit();

#ifndef TB_DEBUG
void* 		tb_mpool_allocate(tb_size_t size);
void*  		tb_mpool_callocate(tb_size_t item, tb_size_t size);
void* 		tb_mpool_reallocate(void* data, tb_size_t size);
void 		tb_mpool_deallocate(void* data);
#else
void* 		tb_mpool_allocate(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
void*  		tb_mpool_callocate(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
void* 		tb_mpool_reallocate(void* data, tb_size_t size,tb_char_t const* func,  tb_size_t line, tb_char_t const* file);
void 		tb_mpool_deallocate(void* data, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
#endif

// for debug
void 		tb_mpool_dump();
tb_bool_t 	tb_mpool_check();

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
