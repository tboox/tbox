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
 * @file        single_list_entry.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "single_list_entry.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator implementation
 */
static tb_size_t tb_single_list_entry_itor_size(tb_iterator_ref_t iterator)
{
    // check
    tb_single_list_entry_head_ref_t list = tb_container_of(tb_single_list_entry_head_t, itor, iterator);
    tb_assert_return_val(list, 0);

    // the size
    return list->size;
}
static tb_size_t tb_single_list_entry_itor_head(tb_iterator_ref_t iterator)
{
    // check
    tb_single_list_entry_head_ref_t list = tb_container_of(tb_single_list_entry_head_t, itor, iterator);
    tb_assert_return_val(list, 0);

    // head
    return (tb_size_t)list->next;
}
static tb_size_t tb_single_list_entry_itor_last(tb_iterator_ref_t iterator)
{
    // check
    tb_single_list_entry_head_ref_t list = tb_container_of(tb_single_list_entry_head_t, itor, iterator);
    tb_assert_return_val(list, 0);

    // last
    return (tb_size_t)list->last;
}
static tb_size_t tb_single_list_entry_itor_tail(tb_iterator_ref_t iterator)
{
    // tail
    return (tb_size_t)0;
}
static tb_size_t tb_single_list_entry_itor_next(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_assert_return_val(itor, 0);

    // next
    return (tb_size_t)((tb_single_list_entry_ref_t)itor)->next;
}
static tb_pointer_t tb_single_list_entry_itor_item(tb_iterator_ref_t iterator, tb_size_t itor)
{
    // check
    tb_single_list_entry_head_ref_t list = tb_container_of(tb_single_list_entry_head_t, itor, iterator);
    tb_assert_return_val(list && list->eoff < itor, tb_null);

    // data
    return (tb_pointer_t)(itor - list->eoff);
}
static tb_void_t tb_single_list_entry_itor_copy(tb_iterator_ref_t iterator, tb_size_t itor, tb_cpointer_t item)
{
    // check
    tb_single_list_entry_head_ref_t list = tb_container_of(tb_single_list_entry_head_t, itor, iterator);
    tb_assert_return(list && list->copy);
    tb_assert_return(list->eoff < itor && item);

    // copy it
    list->copy((tb_pointer_t)(itor - list->eoff), (tb_pointer_t)item);
}
static tb_void_t tb_single_list_entry_itor_remove_range(tb_iterator_ref_t iterator, tb_size_t prev, tb_size_t next, tb_size_t size)
{
    // check
    tb_single_list_entry_head_ref_t list = tb_container_of(tb_single_list_entry_head_t, itor, iterator);
    tb_assert_return(list && prev && next);

    // no size?
    tb_check_return(size);

    // the entry
    tb_single_list_entry_ref_t prev_entry = (tb_single_list_entry_ref_t)prev;
    tb_single_list_entry_ref_t next_entry = (tb_single_list_entry_ref_t)next;

    // remove entries
    prev_entry->next = next_entry;

    // update size
    list->size -= size;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_iterator_ref_t tb_single_list_entry_itor(tb_single_list_entry_head_ref_t list)
{
    // check
    tb_assert_and_check_return_val(list, tb_null);

    // the iterator
    return &list->itor;
}
tb_void_t tb_single_list_entry_init_(tb_single_list_entry_head_ref_t list, tb_size_t entry_offset, tb_size_t entry_size, tb_entry_copy_t copy)
{
    // check
    tb_assert_and_check_return(list && entry_size > sizeof(tb_single_list_entry_t));

    // init it
    list->next = tb_null;
    list->last = tb_null;
    list->size = 0;
    list->eoff = entry_offset;
    list->copy = copy;
 
    // init iterator
    list->itor.mode         = TB_ITERATOR_MODE_FORWARD;
    list->itor.priv         = tb_null;
    list->itor.step         = entry_size;
    list->itor.size         = tb_single_list_entry_itor_size;
    list->itor.head         = tb_single_list_entry_itor_head;
    list->itor.last         = tb_single_list_entry_itor_last;
    list->itor.tail         = tb_single_list_entry_itor_tail;
    list->itor.next         = tb_single_list_entry_itor_next;
    list->itor.item         = tb_single_list_entry_itor_item;
    list->itor.copy         = tb_single_list_entry_itor_copy;
    list->itor.remove_range = tb_single_list_entry_itor_remove_range;
    list->itor.comp         = tb_null;
}
tb_void_t tb_single_list_entry_exit(tb_single_list_entry_head_ref_t list)
{
    // check
    tb_assert_and_check_return(list);

    // exit it
    list->next          = tb_null;
    list->last          = tb_null;
    list->size = 0;
}


