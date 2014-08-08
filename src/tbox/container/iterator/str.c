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
 * @file        str.c
 * @ingroup     container
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_long_t tb_iterator_init_str_comp(tb_iterator_ref_t iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
    // check
    tb_assert_and_check_return_val(ltem && rtem, 0);

    // compare it
    return iterator->priv? tb_strcmp((tb_char_t const*)ltem, (tb_char_t const*)rtem) : tb_stricmp((tb_char_t const*)ltem, (tb_char_t const*)rtem);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_iterator_t tb_iterator_init_str(tb_char_t** data, tb_size_t size)
{   
    // check
    tb_assert(data && size);

    // the ptr iterator
    tb_iterator_t ptr = tb_iterator_init_ptr((tb_pointer_t*)data, size);

    // init
    tb_iterator_t itor = {0};
    itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE | TB_ITERATOR_MODE_RACCESS | TB_ITERATOR_MODE_MUTABLE;
    itor.data = (tb_pointer_t)data;
    itor.priv = tb_u2p(size);
    itor.step = sizeof(tb_char_t*);
    itor.size = ptr.size;
    itor.head = ptr.head;
    itor.tail = ptr.tail;
    itor.prev = ptr.prev;
    itor.next = ptr.next;
    itor.item = ptr.item;
    itor.copy = ptr.copy;
    itor.comp = tb_iterator_init_str_comp;

    // ok
    return itor;
}
