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
 * @file        ptr.c
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
static tb_size_t tb_iterator_ptr_size(tb_iterator_ref_t iterator)
{
    // check
    tb_assert_abort(iterator);

    // the size
    return ((tb_array_iterator_ref_t)iterator)->count;
}
static tb_size_t tb_iterator_ptr_head(tb_iterator_ref_t iterator)
{
    return 0;
}
static tb_size_t tb_iterator_ptr_tail(tb_iterator_ref_t iterator)
{
    // check
    tb_assert_abort(iterator);

    // the tail
    return ((tb_array_iterator_ref_t)iterator)->count;
}
static tb_size_t tb_iterator_ptr_next(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_assert_abort(iterator && itor < ((tb_array_iterator_ref_t)iterator)->count);

    // the next
    return itor + 1;
}
static tb_size_t tb_iterator_ptr_prev(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_assert_abort(iterator && itor);

    // the prev
    return itor - 1;
}
static tb_pointer_t tb_iterator_ptr_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_assert_abort(iterator && itor < ((tb_array_iterator_ref_t)iterator)->count);

    // the item
    return ((tb_pointer_t*)((tb_array_iterator_ref_t)iterator)->items)[itor];
}
static tb_void_t tb_iterator_ptr_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_assert_abort(iterator && itor < ((tb_array_iterator_ref_t)iterator)->count);

    // copy
    ((tb_cpointer_t*)((tb_array_iterator_ref_t)iterator)->items)[itor] = item;
}
static tb_long_t tb_iterator_ptr_comp(tb_iterator_ref_t iterator, tb_cpointer_t litem, tb_cpointer_t ritem)
{
    return (litem < ritem)? -1 : (litem > ritem);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_iterator_ref_t tb_iterator_make_for_ptr(tb_array_iterator_ref_t iterator, tb_pointer_t* items, tb_size_t count)
{
    // check
    tb_assert(iterator && items && count);

    // init
    iterator->base.mode     = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS | TB_ITERATOR_MODE_MUTABLE;
    iterator->base.priv     = tb_null;
    iterator->base.step     = sizeof(tb_pointer_t);
    iterator->base.size     = tb_iterator_ptr_size;
    iterator->base.head     = tb_iterator_ptr_head;
    iterator->base.tail     = tb_iterator_ptr_tail;
    iterator->base.prev     = tb_iterator_ptr_prev;
    iterator->base.next     = tb_iterator_ptr_next;
    iterator->base.item     = tb_iterator_ptr_item;
    iterator->base.copy     = tb_iterator_ptr_copy;
    iterator->base.comp     = tb_iterator_ptr_comp;
    iterator->items      = items;
    iterator->count         = count;

    // ok
    return (tb_iterator_ref_t)iterator;
}

