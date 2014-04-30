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
 * @file		mem.c
 * @ingroup 	container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_pointer_t tb_iterator_init_mem_item(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_assert_return_val(iterator, tb_null);
	tb_assert_and_check_return_val(itor < (tb_size_t)iterator->priv, tb_null);

	// the item
	return (tb_pointer_t)((tb_byte_t*)iterator->data + itor * iterator->step);
}
static tb_void_t tb_iterator_init_mem_copy(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item)
{
	// check
	tb_assert_return(iterator);
	tb_assert_and_check_return(itor < (tb_size_t)iterator->priv && item);

	// copy
	tb_memcpy((tb_byte_t*)iterator->data + itor * iterator->step, item, iterator->step);
}
static tb_long_t tb_iterator_init_mem_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
	// check
	tb_assert_and_check_return_val(ltem && rtem, 0);

	// compare it
	return tb_memcmp(ltem, rtem, iterator->step);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_iterator_t tb_iterator_init_mem(tb_pointer_t data, tb_size_t size, tb_size_t step)
{
	// check
	tb_assert(data && size);

	// the ptr iterator
	tb_iterator_t ptr = tb_iterator_init_ptr((tb_pointer_t*)data, size);

	// init
	tb_iterator_t itor;
	itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS;
	itor.data = (tb_pointer_t)data;
	itor.priv = tb_u2p(size);
	itor.step = step;
	itor.size = ptr.size;
	itor.head = ptr.head;
	itor.tail = ptr.tail;
	itor.prev = ptr.prev;
	itor.next = ptr.next;
	itor.item = tb_iterator_init_mem_item;
	itor.copy = tb_iterator_init_mem_copy;
	itor.comp = tb_iterator_init_mem_comp;

	// ok
	return itor;
}
