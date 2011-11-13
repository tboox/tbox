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
 * \file		stack.h
 *
 */
#ifndef TB_CONTAINER_STACK_H
#define TB_CONTAINER_STACK_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "vector.h"

/* /////////////////////////////////////////////////////////
 * types
 */

/* the stack type
 *
 * stack: |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||------|
 *       head                                                           last    tail
 *
 * stack: |||||||||||||||||||||||||||||||||||||||||------|
 *       head                                   last    tail
 *
 * head: => the first item
 * last: => the last item
 * tail: => behind the last item, no item

 * performance: 
 *
 * push: 	fast
 * pop: 	fast
 *
 * iterator:
 * next: 	fast
 * prev: 	fast
 *
 * \note the index of the same item is fixed
 *
 */
typedef tb_vector_t 			tb_stack_t;
typedef tb_vector_item_func_t 	tb_stack_item_func_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_stack_t* 		tb_stack_init(tb_size_t step, tb_size_t grow, tb_stack_item_func_t const* func);
tb_void_t 			tb_stack_exit(tb_stack_t* stack);

// accessors
tb_pointer_t 			tb_stack_at_head(tb_stack_t* stack);
tb_pointer_t 			tb_stack_at_last(tb_stack_t* stack);

tb_cpointer_t 	tb_stack_const_at_head(tb_stack_t const* stack);
tb_cpointer_t 	tb_stack_const_at_last(tb_stack_t const* stack);

// modifiors
tb_void_t 			tb_stack_clear(tb_stack_t* stack);

tb_void_t 	 		tb_stack_put(tb_stack_t* stack, tb_cpointer_t item);
tb_void_t 	 		tb_stack_pop(tb_stack_t* stack, tb_pointer_t item);

/* iterator
 * 
 * tb_size_t itor = tb_stack_itor_head(stack);
 * tb_size_t tail = tb_stack_itor_tail(stack);
 * for (; itor != tail; itor = tb_stack_itor_next(stack, itor))
 * {
 * 		tb_cpointer_t item = tb_stack_itor_const_at(stack, itor);
 * 		if (item)
 * 		{
 * 			// ...
 * 		}
 * }
 */
tb_pointer_t 			tb_stack_itor_at(tb_stack_t* stack, tb_size_t itor);
tb_cpointer_t 	tb_stack_itor_const_at(tb_stack_t const* stack, tb_size_t itor);

tb_size_t 			tb_stack_itor_head(tb_stack_t const* stack);
tb_size_t 			tb_stack_itor_tail(tb_stack_t const* stack);
tb_size_t 			tb_stack_itor_last(tb_stack_t const* stack);
tb_size_t 			tb_stack_itor_next(tb_stack_t const* stack, tb_size_t itor);
tb_size_t 			tb_stack_itor_prev(tb_stack_t const* stack, tb_size_t itor);

// attributes
tb_size_t 			tb_stack_size(tb_stack_t const* stack);
tb_size_t 			tb_stack_maxn(tb_stack_t const* stack);



// c plus plus
#ifdef __cplusplus
}
#endif

#endif

