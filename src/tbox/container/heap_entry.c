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
 * @file        heap_entry.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "heap_entry.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_iterator_ref_t tb_heap_entry_itor(tb_heap_entry_head_ref_t heap)
{
    // check
    tb_assert_and_check_return_val(heap, tb_null);

    // the iterator
    return &heap->itor;
}
tb_void_t tb_heap_entry_init_(tb_heap_entry_head_ref_t heap, tb_size_t entry_offset, tb_size_t entry_size, tb_heap_entry_copy_t copy)
{
    // check
    tb_assert_and_check_return(heap && entry_size > sizeof(tb_heap_entry_t));

    // init it
    heap->next          = (tb_heap_entry_ref_t)heap;
    heap->prev          = (tb_heap_entry_ref_t)heap;
    heap->size          = 0;
    heap->eoff          = entry_offset;
    heap->copy          = copy;

}
tb_void_t tb_heap_entry_exit(tb_heap_entry_head_ref_t heap)
{
    // check
    tb_assert_and_check_return(heap);

    // exit it
    heap->next = (tb_heap_entry_ref_t)heap;
    heap->prev = (tb_heap_entry_ref_t)heap;
    heap->size = 0;
}


