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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        remove_first_if.c
 * @ingroup     algorithm
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "remove_first_if.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_remove_first_if(tb_iterator_ref_t iterator, tb_predicate_ref_t pred, tb_cpointer_t value)
{
    // check
    tb_assert_and_check_return(iterator && pred);

    // the iterator mode
    tb_size_t mode = tb_iterator_mode(iterator);
    tb_assert_and_check_return((mode & TB_ITERATOR_MODE_FORWARD));
    tb_assert_and_check_return(!(mode & TB_ITERATOR_MODE_READONLY));

    // done
    tb_size_t itor = tb_iterator_head(iterator);
    while (itor != tb_iterator_tail(iterator))
    {
        // done predicate
        if (pred(iterator, tb_iterator_item(iterator, itor), value))
        {
            // remove it
            tb_iterator_remove(iterator, itor);
            break;
        }
    
        // next
        itor = tb_iterator_next(iterator, itor);
    }
}

