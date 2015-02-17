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
 * @file        predicate.c
 * @ingroup     algorithm
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "predicate.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_predicate_eq(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value)
{
    // check
    tb_assert_abort(iterator);

    // item == value?
    return !tb_iterator_comp(iterator, item, value);
}
tb_bool_t tb_predicate_le(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value)
{
    // check
    tb_assert_abort(iterator);

    // item < value?
    return tb_iterator_comp(iterator, item, value) < 0;
}
tb_bool_t tb_predicate_be(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value)
{
    // check
    tb_assert_abort(iterator);

    // item > value?
    return tb_iterator_comp(iterator, item, value) > 0;
}
tb_bool_t tb_predicate_leq(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value)
{
    // check
    tb_assert_abort(iterator);

    // item <= value?
    return tb_iterator_comp(iterator, item, value) <= 0;
}
tb_bool_t tb_predicate_beq(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t value)
{
    // check
    tb_assert_abort(iterator);

    // item >= value?
    return tb_iterator_comp(iterator, item, value) >= 0;
}
