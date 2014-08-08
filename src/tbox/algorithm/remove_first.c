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
 * @file        remove_first.c
 * @ingroup     algorithm
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "remove_if.h"
#include "remove_first.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_long_t tb_remove_first_comp(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t priv)
{
    // the tuple
    tb_value_t* tuple = (tb_value_t*)priv;
    tb_assert_and_check_return_val(tuple, -1);

    // have been removed? break it
    tb_check_return_val(!tuple[1].b, -1);

    // done comp func
    tb_long_t ok = tb_iterator_comp(iterator, item, tuple[0].cptr);

    // removed?
    if (!ok) tuple[1].b = tb_true;

    // ok?
    return ok;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_remove_first(tb_iterator_ref_t iterator, tb_cpointer_t item)
{
    // init tuple
    tb_value_t tuple[3];
    tuple[0].cptr   = item;
    tuple[1].b      = tb_false;

    // remove it
    tb_remove_if(iterator, tb_remove_first_comp, (tb_cpointer_t)tuple);
}

