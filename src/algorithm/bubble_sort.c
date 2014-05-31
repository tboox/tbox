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
 * @file        bubble_sort.c
 * @ingroup     algorithm
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "bubble_sort.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_void_t tb_bubble_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail, tb_iterator_comp_t comp)
{
    // check
    tb_assert_and_check_return(iterator && (tb_iterator_mode(iterator) & TB_ITERATOR_MODE_FORWARD));
    tb_check_return(head != tail);

    // init
    tb_size_t       step = tb_iterator_step(iterator);
    tb_pointer_t    temp = step > sizeof(tb_pointer_t)? tb_malloc(step) : tb_null;
    tb_assert_and_check_return(step <= sizeof(tb_pointer_t) || temp);

    // the comparer
    if (!comp) comp = tb_iterator_comp;

    // bubble_sort
    tb_size_t       itor1, itor2;
    for (itor1 = head; itor1 != tail; itor1 = tb_iterator_next(iterator, itor1))
    {
        for (itor2 = itor1, itor2 = tb_iterator_next(iterator, itor2); itor2 != tail; itor2 = tb_iterator_next(iterator, itor2))
        {
            if (comp(iterator, tb_iterator_item(iterator, itor2), tb_iterator_item(iterator, itor1)) < 0)
            {
                if (step <= sizeof(tb_pointer_t)) temp = tb_iterator_item(iterator, itor1);
                else tb_memcpy(temp, tb_iterator_item(iterator, itor1), step);
                tb_iterator_copy(iterator, itor1, tb_iterator_item(iterator, itor2));
                tb_iterator_copy(iterator, itor2, temp);
            }
        }
    }

    // free
    if (temp && step > sizeof(tb_pointer_t)) tb_free(temp);
}
tb_void_t tb_bubble_sort_all(tb_iterator_t* iterator, tb_iterator_comp_t comp)
{
    tb_bubble_sort(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator), comp);
}

