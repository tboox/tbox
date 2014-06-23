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
 * implementation
 */
static tb_size_t tb_iterator_init_ptr_size(tb_iterator_t* iterator)
{
    // check
    tb_assert_return_val(iterator, 0);

    // the size
    return (tb_size_t)iterator->priv;
}
static tb_size_t tb_iterator_init_ptr_head(tb_iterator_t* iterator)
{
    return 0;
}
static tb_size_t tb_iterator_init_ptr_tail(tb_iterator_t* iterator)
{
    // check
    tb_assert_return_val(iterator, 0);

    // the tail
    return (tb_size_t)iterator->priv;
}
static tb_size_t tb_iterator_init_ptr_next(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_assert_return_val(iterator, 0);
    tb_assert_and_check_return_val(itor < (tb_size_t)iterator->priv, (tb_size_t)iterator->priv);

    // the next
    return itor + 1;
}
static tb_size_t tb_iterator_init_ptr_prev(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_assert_and_check_return_val(itor, 0);

    // the prev
    return itor - 1;
}
static tb_pointer_t tb_iterator_init_ptr_item(tb_iterator_t* iterator, tb_size_t itor)
{
    // check
    tb_assert_return_val(iterator, tb_object_null);
    tb_assert_and_check_return_val(itor < (tb_size_t)iterator->priv, tb_object_null);

    // the item
    return ((tb_pointer_t*)iterator->data)[itor];
}
static tb_void_t tb_iterator_init_ptr_copy(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_assert_return(iterator);
    tb_assert_and_check_return(itor < (tb_size_t)iterator->priv);

    // copy
    ((tb_cpointer_t*)iterator->data)[itor] = item;
}
static tb_long_t tb_iterator_init_ptr_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    return ((tb_size_t)ltem > (tb_size_t)rtem? 1 : ((tb_size_t)ltem < (tb_size_t)rtem? -1 : 0));
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_iterator_t tb_iterator_init_ptr(tb_pointer_t* data, tb_size_t size)
{
    // check
    tb_assert(data && size);

    // init
    tb_iterator_t itor = {0};
    itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS;
    itor.data = (tb_pointer_t)data;
    itor.priv = tb_u2p(size);
    itor.step = sizeof(tb_pointer_t);
    itor.size = tb_iterator_init_ptr_size;
    itor.head = tb_iterator_init_ptr_head;
    itor.tail = tb_iterator_init_ptr_tail;
    itor.prev = tb_iterator_init_ptr_prev;
    itor.next = tb_iterator_init_ptr_next;
    itor.item = tb_iterator_init_ptr_item;
    itor.copy = tb_iterator_init_ptr_copy;
    itor.comp = tb_iterator_init_ptr_comp;

    // ok
    return itor;
}

