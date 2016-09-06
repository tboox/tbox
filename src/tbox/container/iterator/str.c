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
 * @file        str.c
 * @ingroup     container
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_long_t tb_iterator_str_comp(tb_iterator_ref_t iterator, tb_cpointer_t litem, tb_cpointer_t ritem)
{
    // check
    tb_assert(litem && ritem);

    // compare it
    return tb_strcmp((tb_char_t const*)litem, (tb_char_t const*)ritem);
}
static tb_long_t tb_iterator_istr_comp(tb_iterator_ref_t iterator, tb_cpointer_t litem, tb_cpointer_t ritem)
{
    // check
    tb_assert(litem && ritem);

    // compare it
    return tb_stricmp((tb_char_t const*)litem, (tb_char_t const*)ritem);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_iterator_ref_t tb_iterator_make_for_str(tb_array_iterator_ref_t iterator, tb_char_t** items, tb_size_t count)
{
    // make iterator for the pointer array
    if (!tb_iterator_make_for_ptr(iterator, (tb_pointer_t*)items, count)) return tb_null;

    // init
    iterator->base.comp = tb_iterator_str_comp;

    // ok
    return (tb_iterator_ref_t)iterator;
}
tb_iterator_ref_t tb_iterator_make_for_istr(tb_array_iterator_ref_t iterator, tb_char_t** items, tb_size_t count)
{
    // make iterator for the pointer array
    if (!tb_iterator_make_for_ptr(iterator, (tb_pointer_t*)items, count)) return tb_null;

    // init
    iterator->base.comp = tb_iterator_istr_comp;

    // ok
    return (tb_iterator_ref_t)iterator;
}
