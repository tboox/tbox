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
 * @file		heap.c
 * @ingroup 	container
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "heap.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_heap_t* tb_heap_init(tb_size_t mode, tb_size_t grow, tb_item_func_t func)
{
	return tb_null;
}
tb_void_t tb_heap_exit(tb_heap_t* heap)
{
}
tb_void_t tb_heap_clear(tb_heap_t* heap)
{
}
tb_size_t tb_heap_size(tb_heap_t const* heap)
{
	return 0;
}
tb_size_t tb_heap_grow(tb_heap_t const* heap)
{
	return 0;
}
tb_size_t tb_heap_maxn(tb_heap_t const* heap)
{
	return 0;
}
tb_pointer_t tb_heap_top(tb_heap_t* heap)
{
	return tb_null;
}
tb_void_t tb_heap_put(tb_heap_t* heap, tb_cpointer_t data)
{
}
tb_void_t tb_heap_pop(tb_heap_t* heap)
{
}
tb_void_t tb_heap_walk(tb_heap_t* heap, tb_bool_t (*func)(tb_heap_t* heap, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data)
{
}
