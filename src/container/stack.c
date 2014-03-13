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
 * @file		stack.c
 * @ingroup 	container
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stack.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_stack_t* tb_stack_init(tb_size_t grow, tb_item_func_t func)
{
	return tb_vector_init(grow, func);
}
tb_void_t tb_stack_exit(tb_stack_t* stack)
{
	tb_vector_exit(stack);
}
tb_void_t tb_stack_clear(tb_stack_t* stack)
{
	tb_vector_clear(stack);
}
tb_void_t tb_stack_copy(tb_stack_t* stack, tb_stack_t* copy)
{
	tb_vector_copy(stack, copy);
}
tb_void_t tb_stack_put(tb_stack_t* stack, tb_cpointer_t data)
{
	tb_vector_insert_tail(stack, data);
}
tb_void_t tb_stack_pop(tb_stack_t* stack)
{
	tb_vector_remove_last(stack);
}
tb_pointer_t tb_stack_top(tb_stack_t const* stack)
{
	return tb_vector_last(stack);
}
tb_pointer_t tb_stack_head(tb_stack_t const* stack)
{
	return tb_vector_head(stack);
}
tb_pointer_t tb_stack_last(tb_stack_t const* stack)
{
	return tb_vector_last(stack);
}
tb_size_t tb_stack_size(tb_stack_t const* stack)
{
	return tb_vector_size(stack);
}
tb_size_t tb_stack_maxn(tb_stack_t const* stack)
{
	return tb_vector_maxn(stack);
}
tb_void_t tb_stack_walk(tb_stack_t* stack, tb_bool_t (*func)(tb_stack_t* stack, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data)
{
	tb_vector_walk(stack, func, data);
}
