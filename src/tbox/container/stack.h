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
 * @file        stack.h
 * @ingroup     container
 *
 */
#ifndef TB_CONTAINER_STACK_H
#define TB_CONTAINER_STACK_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "vector.h"
#include "item.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the stack ref type 
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
 * push:    fast
 * pop:     fast
 *
 * iterator:
 * next:    fast
 * prev:    fast
 * </pre>
 *
 * @note the itor of the same item is fixed
 *
 */
typedef tb_vector_ref_t tb_stack_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init stack
 *
 * @param grow      the item grow
 * @param func      the item func
 *
 * @return          the stack
 */
tb_stack_ref_t      tb_stack_init(tb_size_t grow, tb_item_func_t func);

/*! exit stack
 *
 * @param stack     the stack
 */
tb_void_t           tb_stack_exit(tb_stack_ref_t stack);

/*! the stack head item
 *
 * @param stack     the stack
 *
 * @return          the head item
 */
tb_pointer_t        tb_stack_head(tb_stack_ref_t stack);

/*! the stack last item
 *
 * @param stack     the stack
 *
 * @return          the last item
 */
tb_pointer_t        tb_stack_last(tb_stack_ref_t stack);

/*! clear the stack
 *
 * @param stack     the stack
 */
tb_void_t           tb_stack_clear(tb_stack_ref_t stack);

/*! copy the stack
 *
 * @param stack     the stack
 * @param copy      the copied stack
 */
tb_void_t           tb_stack_copy(tb_stack_ref_t stack, tb_stack_ref_t copy);

/*! put the stack item
 *
 * @param stack     the stack
 * @param data      the item data
 */
tb_void_t           tb_stack_put(tb_stack_ref_t stack, tb_cpointer_t data);

/*! pop the stack item
 *
 * @param stack     the stack
 */
tb_void_t           tb_stack_pop(tb_stack_ref_t stack);

/*! the stack top item
 *
 * @param stack     the stack
 *
 * @return          the stack top item
 */
tb_pointer_t        tb_stack_top(tb_stack_ref_t stack);

/*! load stack from the stream
 *
 * @param stack     the stack
 * @param stream    the stream
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_stack_load(tb_stack_ref_t stack, tb_stream_ref_t stream);

/*! save stack to the stream
 *
 * @param stack     the stack
 * @param stream    the stream
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_stack_save(tb_stack_ref_t stack, tb_stream_ref_t stream);

/*! the stack size
 *
 * @param stack     the stack
 *
 * @return          the stack size
 */
tb_size_t           tb_stack_size(tb_stack_ref_t stack);

/*! the stack maxn
 *
 * @param stack     the stack
 *
 * @return          the stack maxn
 */
tb_size_t           tb_stack_maxn(tb_stack_ref_t stack);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

