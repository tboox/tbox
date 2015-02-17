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
 * @file        find_if.c
 * @ingroup     algorithm
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "find_if.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_find_if(tb_iterator_ref_t iterator, tb_size_t head, tb_size_t tail, tb_predicate_unary_t pred, tb_cpointer_t value)
{
    // check
    tb_assert_and_check_return_val(pred && iterator && (tb_iterator_mode(iterator) & TB_ITERATOR_MODE_FORWARD), tb_iterator_tail(iterator));

    // null?
    tb_check_return_val(head != tail, tb_iterator_tail(iterator));

    // find
    tb_size_t itor = head;
    tb_bool_t find = tb_false;
    for (; itor != tail; itor = tb_iterator_next(iterator, itor)) 
        if ((find = pred(iterator, tb_iterator_item(iterator, itor), value))) break;

    // ok?
    return find? itor : tb_iterator_tail(iterator);
} 
tb_size_t tb_find_all_if(tb_iterator_ref_t iterator, tb_predicate_unary_t pred, tb_cpointer_t value)
{
    return tb_find_if(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator), pred, value);
}

