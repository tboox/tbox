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
 * @file		stack.h
 * @ingroup 	container
 *
 */
#ifndef TB_CONTAINER_STACK_H
#define TB_CONTAINER_STACK_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "vector.h"
#include "item.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/*!the stack type
 *
 * <pre>
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
 * </pre>
 *
 * @note the itor of the same item is fixed
 *
 */
typedef tb_vector_t 			tb_stack_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_stack_t* 		tb_stack_init(tb_size_t grow, tb_item_func_t func);
tb_void_t 			tb_stack_exit(tb_stack_t* stack);

// accessors
tb_pointer_t 		tb_stack_head(tb_stack_t* stack);
tb_pointer_t 		tb_stack_last(tb_stack_t* stack);

// modifiors
tb_void_t 			tb_stack_clear(tb_stack_t* stack);
tb_void_t 			tb_stack_copy(tb_stack_t* stack, tb_stack_t* copy);

tb_void_t 	 		tb_stack_put(tb_stack_t* stack, tb_cpointer_t data);
tb_void_t 	 		tb_stack_pop(tb_stack_t* stack);
tb_pointer_t 	 	tb_stack_top(tb_stack_t* stack);

// attributes
tb_size_t 			tb_stack_size(tb_stack_t const* stack);
tb_size_t 			tb_stack_maxn(tb_stack_t const* stack);

/*!walk
 *
 * be faster than the iterator mode, optimizate to remove items for walking
 *
 * @code
 * tb_bool_t tb_stack_item_func(tb_stack_t* stack, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
 * {
 * 		tb_assert_and_check_return_val(stack && bdel, TB_FALSE);
 *
 * 		// is tail?
 * 		if (!item) ;
 *
 * 		// delete it?
 * 		// *bdel = TB_TRUE;
 *
 * 		// ok
 * 		return TB_TRUE;
 *
 * fail:
 * 		// break
 * 		return TB_FALSE;
 * }
 * @endcode
 *
 */
tb_void_t 			tb_stack_walk(tb_stack_t* stack, tb_bool_t (*func)(tb_stack_t* stack, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data);


#endif

