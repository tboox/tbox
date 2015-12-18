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
 * @file        iterator.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "iterator.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_iterator_mode(tb_iterator_ref_t iterator)
{
    // check
    tb_assert(iterator);

    // mode
    return iterator->mode;
}
tb_size_t tb_iterator_step(tb_iterator_ref_t iterator)
{
    // check
    tb_assert(iterator);

    // step
    return iterator->step;
}
tb_size_t tb_iterator_size(tb_iterator_ref_t iterator)
{
    // check
    tb_assert(iterator && iterator->size);

    // size
    return iterator->size(iterator);
}
tb_size_t tb_iterator_head(tb_iterator_ref_t iterator)
{
    // check
    tb_assert(iterator && iterator->head);
    
    // head
    return iterator->head(iterator);
}
tb_size_t tb_iterator_last(tb_iterator_ref_t iterator)
{
    // check
    tb_assert(iterator && iterator->last);
    
    // last
    return iterator->last(iterator);
}
tb_size_t tb_iterator_tail(tb_iterator_ref_t iterator)
{
    // check
    tb_assert(iterator && iterator->tail);

    // tail
    return iterator->tail(iterator);
}
tb_size_t tb_iterator_prev(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_assert(iterator && iterator->prev);

    // prev
    return iterator->prev(iterator, itor);
}
tb_size_t tb_iterator_next(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_assert(iterator && iterator->next);

    // next
    return iterator->next(iterator, itor);
}
tb_pointer_t tb_iterator_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_assert(iterator && iterator->item);

    // item
    return iterator->item(iterator, itor);
}
tb_void_t tb_iterator_remove(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_assert(iterator && iterator->remove);

    // remove
    return iterator->remove(iterator, itor);
}
tb_void_t tb_iterator_remove_range(tb_iterator_ref_t iterator, tb_size_t prev, tb_size_t next, tb_size_t size)
{
    // check
    tb_assert(iterator && iterator->remove_range);

    // remove range
    return iterator->remove_range(iterator, prev, next, size);
}
tb_void_t tb_iterator_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_assert(iterator && iterator->copy);

    // copy
    return iterator->copy(iterator, itor, item);
}
tb_long_t tb_iterator_comp(tb_iterator_ref_t iterator, tb_cpointer_t litem, tb_cpointer_t ritem)
{
    // check
    tb_assert(iterator && iterator->comp);

    // comp
    return iterator->comp(iterator, litem, ritem);
}

