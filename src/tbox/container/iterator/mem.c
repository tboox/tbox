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
 * @file        mem.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_pointer_t tb_iterator_mem_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_assert_abort(iterator && itor < ((tb_array_iterator_ref_t)iterator)->count);

    // the item
    return (tb_pointer_t)((tb_byte_t*)((tb_array_iterator_ref_t)iterator)->items + itor * iterator->step);
}
static tb_void_t tb_iterator_mem_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_assert_abort(iterator && itor < ((tb_array_iterator_ref_t)iterator)->count);

    // copy
    tb_memcpy((tb_byte_t*)((tb_array_iterator_ref_t)iterator)->items + itor * iterator->step, item, iterator->step);
}
static tb_long_t tb_iterator_mem_comp(tb_iterator_ref_t iterator, tb_cpointer_t litem, tb_cpointer_t ritem)
{
    // check
    tb_assert_abort(litem && ritem);

    // compare it
    return tb_memcmp(litem, ritem, iterator->step);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_iterator_ref_t tb_iterator_make_for_mem(tb_array_iterator_ref_t iterator, tb_pointer_t items, tb_size_t count, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(size, tb_null);

    // make iterator for the pointer array
    if (!tb_iterator_make_for_ptr(iterator, (tb_pointer_t*)items, count)) return tb_null;

    // init
    iterator->base.step = size;
    iterator->base.item = tb_iterator_mem_item;
    iterator->base.copy = tb_iterator_mem_copy;
    iterator->base.comp = tb_iterator_mem_comp;

    // ok
    return (tb_iterator_ref_t)iterator;
}
