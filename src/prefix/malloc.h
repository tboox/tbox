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
 * @file		malloc.h
 *
 */
#ifndef TB_PREFIX_MALLOC_H
#define TB_PREFIX_MALLOC_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "../memory/malloc.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// malloc & free
#ifdef TB_CONFIG_MEMORY_POOL
#	define tb_malloc(size) 				tb_memory_malloc(size)
#	define tb_malloc0(size) 			tb_memory_malloc0(size)

#	define tb_nalloc(item, size) 		tb_memory_nalloc(item, size)
#	define tb_nalloc0(item, size) 		tb_memory_nalloc0(item, size)

#	define tb_ralloc(data, size) 		tb_memory_ralloc(data, size)
# 	define tb_free(data) 				tb_memory_free(data)
#else

tb_pointer_t 	tb_malloc(tb_size_t size);
tb_pointer_t 	tb_malloc0(tb_size_t size);

tb_pointer_t 	tb_nalloc(tb_size_t item, tb_size_t size);
tb_pointer_t 	tb_nalloc0(tb_size_t item, tb_size_t size);

tb_pointer_t 	tb_ralloc(tb_pointer_t data, tb_size_t size);
tb_void_t 		tb_free(tb_pointer_t data);

#endif


#endif


