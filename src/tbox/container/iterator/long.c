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
 * @file        long.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_long_t tb_iterator_long_comp(tb_iterator_ref_t iterator, tb_cpointer_t litem, tb_cpointer_t ritem)
{
    return ((tb_long_t)litem < (tb_long_t)ritem)? -1 : ((tb_long_t)litem > (tb_long_t)ritem);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_iterator_ref_t tb_iterator_make_for_long(tb_array_iterator_ref_t iterator, tb_long_t* items, tb_size_t count)
{
    // make iterator for the pointer array
    if (!tb_iterator_make_for_ptr(iterator, (tb_pointer_t*)items, count)) return tb_null;

    // init
    iterator->base.comp = tb_iterator_long_comp;

    // ok
    return (tb_iterator_ref_t)iterator;
}
