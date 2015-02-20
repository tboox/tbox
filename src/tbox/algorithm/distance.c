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
 * @file        distance.c
 * @ingroup     algorithm
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "distance.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_distance(tb_iterator_ref_t iterator, tb_size_t head, tb_size_t tail)
{
    // check
    tb_assert_and_check_return_val(iterator, 0);

    // zero distance?
    tb_check_return_val(head != tail, 0);

    // the iterator mode
    tb_size_t mode = tb_iterator_mode(iterator);

    // random access iterator? 
    tb_size_t distance = 0;
    if (mode & TB_ITERATOR_MODE_RACCESS) 
    {
        // compute it fastly
        distance = tail - head;
    }
    // forward iterator?
    else if (mode & TB_ITERATOR_MODE_FORWARD) 
    {
        // whole container?
        if (tb_iterator_head(iterator) == head && tb_iterator_tail(iterator) == tail)
            distance = tb_iterator_size(iterator);
        else
        {
            // done
            tb_size_t itor = head;
            for (; itor != tail; itor = tb_iterator_next(iterator, itor)) distance++;
        }
    }
    // reverse iterator?
    else if (mode & TB_ITERATOR_MODE_REVERSE) 
    {
        // whole container?
        if (tb_iterator_head(iterator) == head && tb_iterator_tail(iterator) == tail)
            distance = tb_iterator_size(iterator);
        else
        {
            // done
            tb_size_t itor = tail;
            do
            {
                // update the distance
                distance++;

                // the previous 
                itor = tb_iterator_prev(iterator, itor);

            } while (itor != head);
        }
    }
    // unknown mode?
    else
    {
        // abort
        tb_assert_abort(0);
    }

    // ok?
    return distance;
}
