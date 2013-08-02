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
 * @file		memory.c
 * @defgroup 	memory
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "memory.h"

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
#ifndef TB_CONFIG_MEMORY_POOL
tb_bool_t tb_malloc_init();
tb_void_t tb_malloc_exit();
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_memory_init(tb_byte_t* data, tb_size_t size, tb_size_t align)
{
	// init malloc pool
#ifdef TB_CONFIG_MEMORY_POOL
	if (!tb_malloc_init(data, size, TB_CPU_BITBYTE)) return tb_false;
#else
	if (!tb_malloc_init()) return tb_false;
#endif

	// init scache
	if (!tb_scache_init(align)) return tb_false;

	// ok
	return tb_true;
}
tb_void_t tb_memory_exit()
{
	// exit scache
	tb_scache_exit();

	// dump malloc
#ifdef TB_CONFIG_MEMORY_POOL
	tb_malloc_dump();
#endif

	// exit malloc
	tb_malloc_exit();
}

