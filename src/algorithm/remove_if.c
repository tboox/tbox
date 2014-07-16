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
 * @file        remove_if.c
 * @ingroup     algorithm
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "remove_if.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_remove_all_if(tb_iterator_ref_t iterator, tb_iterator_comp_t comp, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(iterator && comp);
    tb_assert_and_check_return((tb_iterator_mode(iterator) & TB_ITERATOR_MODE_FORWARD));
    tb_assert_and_check_return(!(tb_iterator_mode(iterator) & TB_ITERATOR_MODE_READONLY));

    // done
    tb_long_t ok = 1;
    tb_size_t itor = tb_iterator_head(iterator);
    while (itor != tb_iterator_tail(iterator) && ok > 0)
    {
        // save next
        tb_size_t next = tb_iterator_next(iterator, itor);

        // remove it?
        if (!(ok = comp(iterator, tb_iterator_item(iterator, itor), priv)))
            tb_iterator_delt(iterator, itor);

        // next
        itor = next;
    }
}

