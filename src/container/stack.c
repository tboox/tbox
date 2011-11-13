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
 * \file		stack.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "stack.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_stack_t* tb_stack_init(tb_size_t step, tb_size_t grow, tb_stack_item_func_t const* func)
{
	return tb_vector_init(step, grow, func);
}

tb_void_t tb_stack_exit(tb_stack_t* stack)
{
	tb_vector_exit(stack);
}
tb_void_t tb_stack_clear(tb_stack_t* stack)
{
	tb_vector_clear(stack);
}
tb_void_t tb_stack_put(tb_stack_t* stack, tb_cpointer_t item)
{
	tb_vector_insert_tail(stack, item);
}
tb_void_t tb_stack_pop(tb_stack_t* stack, tb_pointer_t item)
{
	tb_assert_and_check_return(stack);
	if (item) 
	{
		tb_cpointer_t last = tb_vector_const_at_last(stack);
		if (last) tb_memcpy(item, last, stack->step);
	}
	tb_vector_remove_last(stack);
}
tb_pointer_t tb_stack_itor_at(tb_stack_t* stack, tb_size_t itor)
{
	return tb_vector_itor_at(stack, itor);
}
tb_pointer_t tb_stack_at_head(tb_stack_t* stack)
{
	return tb_vector_at_head(stack);
}
tb_pointer_t tb_stack_at_last(tb_stack_t* stack)
{
	return tb_vector_at_last(stack);
}
tb_cpointer_t tb_stack_itor_const_at(tb_stack_t const* stack, tb_size_t itor)
{
	return tb_vector_itor_const_at(stack, itor);
}
tb_cpointer_t tb_stack_const_at_head(tb_stack_t const* stack)
{
	return tb_vector_const_at_head(stack);
}
tb_cpointer_t tb_stack_const_at_last(tb_stack_t const* stack)
{
	return tb_vector_const_at_last(stack);
}
tb_size_t tb_stack_itor_head(tb_stack_t const* stack)
{
	return tb_vector_itor_head(stack);
}
tb_size_t tb_stack_itor_last(tb_stack_t const* stack)
{
	return tb_vector_itor_last(stack);
}
tb_size_t tb_stack_itor_tail(tb_stack_t const* stack)
{
	return tb_vector_itor_tail(stack);
}
tb_size_t tb_stack_itor_next(tb_stack_t const* stack, tb_size_t itor)
{
	return tb_vector_itor_next(stack, itor);
}
tb_size_t tb_stack_itor_prev(tb_stack_t const* stack, tb_size_t itor)
{
	return tb_vector_itor_prev(stack, itor);
}
tb_size_t tb_stack_size(tb_stack_t const* stack)
{
	return tb_vector_size(stack);
}
tb_size_t tb_stack_maxn(tb_stack_t const* stack)
{
	return tb_vector_maxn(stack);

}

