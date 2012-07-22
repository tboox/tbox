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
 * @file		iterator.c
 * @ingroup 	algorithm
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "iterator.h"

/* ///////////////////////////////////////////////////////////////////////
 * integer
 */
static tb_size_t tb_iterator_int_head(tb_iterator_t* iterator)
{
	return 0;
}
static tb_size_t tb_iterator_int_tail(tb_iterator_t* iterator)
{
	return iterator->size;
}
static tb_size_t tb_iterator_int_next(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_assert_return_val(itor < iterator->size, iterator->size);
	return itor + 1;
}
static tb_size_t tb_iterator_int_prev(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_assert_return_val(itor, 0);
	return itor - 1;
}
static tb_size_t tb_iterator_int_midd(tb_iterator_t* iterator, tb_size_t ltor, tb_size_t rtor)
{
	tb_assert_return(ltor < iterator->size && rtor < iterator->size);
	return (ltor + rtor) >> 1;
}
static tb_long_t tb_iterator_int_diff(tb_iterator_t* iterator, tb_size_t ltor, tb_size_t rtor)
{
	tb_assert_return(ltor < iterator->size && rtor < iterator->size);
	return ((tb_long_t)ltor - (tb_long_t)rtor);
}
static tb_pointer_t tb_iterator_int_item(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_assert_return_val(itor < iterator->size, TB_NULL);
	return &(((tb_long_t*)iterator->data)[itor]);
}
static tb_pointer_t tb_iterator_int_save(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_assert_return_val(itor < iterator->size, TB_NULL);
	iterator->priv = (tb_pointer_t)((tb_long_t*)iterator->data)[itor];
	return (tb_pointer_t)&iterator->priv;
}
static tb_void_t tb_iterator_int_swap(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
	tb_long_t item = *((tb_long_t*)ltem);
	*((tb_long_t*)ltem) = *((tb_long_t*)rtem);
	*((tb_long_t*)rtem) = item;
}
static tb_void_t tb_iterator_int_copy(tb_iterator_t* iterator, tb_cpointer_t dtem, tb_cpointer_t stem)
{
	*((tb_long_t*)dtem) = *((tb_long_t*)stem);
}
static tb_long_t tb_iterator_int_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
	return *((tb_long_t*)ltem) - *((tb_long_t*)rtem);
}
/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_iterator_head(tb_iterator_t* iterator)
{
	tb_assert_and_check_return_val(iterator && iterator->head, 0);
	return iterator->head(iterator);
}
tb_size_t tb_iterator_tail(tb_iterator_t* iterator)
{
	tb_assert_and_check_return_val(iterator && iterator->tail, 0);
	return iterator->tail(iterator);
}
tb_size_t tb_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_assert_return_val(iterator && iterator->prev, 0);
	return iterator->prev(iterator, itor);
}
tb_size_t tb_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_assert_return_val(iterator && iterator->next, 0);
	return iterator->next(iterator, itor);
}
tb_size_t tb_iterator_midd(tb_iterator_t* iterator, tb_size_t ltor, tb_size_t rtor)
{
	tb_assert_return_val(iterator && iterator->midd, 0);
	return iterator->midd(iterator, ltor, rtor);
}
tb_long_t tb_iterator_diff(tb_iterator_t* iterator, tb_size_t ltor, tb_size_t rtor)
{
	tb_assert_return_val(iterator && iterator->diff, 0);
	return iterator->diff(iterator, ltor, rtor);
}
tb_pointer_t tb_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_assert_return_val(iterator && iterator->item, TB_NULL);
	return iterator->item(iterator, itor);
}
tb_pointer_t tb_iterator_save(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_assert_return_val(iterator && iterator->save, TB_NULL);
	return iterator->save(iterator, itor);
}
tb_void_t tb_iterator_swap(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
	tb_assert_return(iterator && iterator->swap && ltem && rtem);
	return iterator->swap(iterator, ltem, rtem);
}
tb_void_t tb_iterator_copy(tb_iterator_t* iterator, tb_cpointer_t dtem, tb_cpointer_t stem)
{
	tb_assert_return(iterator && iterator->copy && dtem && stem);
	return iterator->copy(iterator, dtem, stem);
}
tb_long_t tb_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
	tb_assert_return_val(iterator && iterator->comp && ltem && rtem, 0);
	return iterator->comp(iterator, ltem, rtem);
}
tb_iterator_t tb_iterator_int(tb_long_t* data, tb_size_t size)
{
	// check
	tb_assert(data && size);

	// init
	tb_iterator_t itor;
	itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS;
	itor.data = (tb_pointer_t)data;
	itor.size = size;
	itor.priv = TB_NULL;
	itor.step = sizeof(tb_long_t);
	itor.head = tb_iterator_int_head;
	itor.tail = tb_iterator_int_tail;
	itor.prev = tb_iterator_int_prev;
	itor.next = tb_iterator_int_next;
	itor.midd = tb_iterator_int_midd;
	itor.diff = tb_iterator_int_diff;
	itor.item = tb_iterator_int_item;
	itor.save = tb_iterator_int_save;
	itor.swap = tb_iterator_int_swap;
	itor.copy = tb_iterator_int_copy;
	itor.comp = tb_iterator_int_comp;

	// ok
	return itor;
}
tb_iterator_t tb_iterator_str(tb_char_t** data, tb_size_t size)
{	
	// check
	tb_assert(data && size);

	// init
	tb_iterator_t itor;
	itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS;
	itor.data = (tb_pointer_t)data;
	itor.size = size;
	itor.step = sizeof(tb_char_t*);

	// ok
	return itor;
}
tb_iterator_t tb_iterator_ptr(tb_pointer_t* data, tb_size_t size)
{
	// check
	tb_assert(data && size);

	// init
	tb_iterator_t itor;
	itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS;
	itor.data = (tb_pointer_t)data;
	itor.size = size;
	itor.step = sizeof(tb_pointer_t*);

	// ok
	return itor;
}
tb_iterator_t tb_iterator_mem(tb_pointer_t data, tb_size_t size, tb_size_t step)
{
	// check
	tb_assert(data && size && step);

	// init
	tb_iterator_t itor;
	itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS;
	itor.data = (tb_pointer_t)data;
	itor.size = size;
	itor.step = step;

	// ok
	return itor;
}

