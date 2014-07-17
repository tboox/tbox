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
tb_void_t tb_remove_if(tb_iterator_ref_t iterator, tb_iterator_comp_t comp, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(iterator && comp);

    // the iterator mode
    tb_size_t   mode = tb_iterator_mode(iterator);
    tb_assert_and_check_return((mode & TB_ITERATOR_MODE_FORWARD));
    tb_assert_and_check_return(!(mode & TB_ITERATOR_MODE_READONLY));

    // done
    tb_long_t   ok = 1;
    tb_size_t   size = 0;
    tb_size_t   base = tb_iterator_tail(iterator);
    tb_size_t   prev = tb_iterator_tail(iterator);
    tb_size_t   itor = tb_iterator_head(iterator);
    tb_bool_t   stop = tb_false;
    tb_bool_t   need = tb_false;
    tb_bool_t   mutable = (mode & TB_ITERATOR_MODE_MUTABLE)? tb_true : tb_false;
    while (itor != tb_iterator_tail(iterator))
    {
        // save next
        tb_size_t next = tb_iterator_next(iterator, itor);
   
        // done func
        if ((ok = comp(iterator, tb_iterator_item(iterator, itor), priv)) < 0) stop = tb_true;

        // remove it? 
        if (!ok)
        {
            // is the first removed item?
            if (!need)
            {
                // save the removed range base
                base = prev;

                // need remove items
                need = tb_true;
            }

            // update size
            size++;
        }
       
        // the removed range have been passed or stop or end?
        if (ok || next == tb_iterator_tail(iterator))
        {
            // need remove items?
            if (need) 
            {
                // check
                tb_assert_abort(size);

                // remove items
                tb_iterator_remove_range(iterator, base, !ok? next : itor, size);

                // reset state
                need = tb_false;
                size = 0;

                // is the mutable iterator?
                if (mutable)
                {
                    // update itor
                    prev = base;

                    // the next itor
                    itor = tb_iterator_next(iterator, base);

                    // the last item be not removed? skip the last walked item
                    if (ok)
                    {
                        prev = itor;
                        itor = tb_iterator_next(iterator, itor);
                    }

                    // stop?
                    tb_check_break(!stop);

                    // continue?
                    continue ;
                }
            }

            // stop?
            tb_check_break(!stop);
        }
    
        // next
        prev = itor;
        itor = next;
    }
}

