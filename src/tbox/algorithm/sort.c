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
 * @file        sort.c
 * @ingroup     algorithm
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sort.h"
#include "distance.h"
#include "heap_sort.h"
#include "quick_sort.h"
#include "insert_sort.h"
#include "bubble_sort.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_sort(tb_iterator_ref_t iterator, tb_size_t head, tb_size_t tail, tb_iterator_comp_t comp)
{
    // check
    tb_assert_and_check_return(iterator);

    // no elements?
    tb_check_return(head != tail);

    // readonly?
    tb_assert_and_check_return(!(tb_iterator_mode(iterator) & TB_ITERATOR_MODE_READONLY));

#ifdef TB_CONFIG_EMBED_ENABLE
    // random access iterator?
    tb_assert_and_check_return(tb_iterator_mode(iterator) & TB_ITERATOR_MODE_RACCESS);

    // sort it
    tb_quick_sort(iterator, head, tail, comp);
#else
    // random access iterator? 
    if (tb_iterator_mode(iterator) & TB_ITERATOR_MODE_RACCESS) 
    {
        if (tb_distance(iterator, head, tail) > 100000) tb_heap_sort(iterator, head, tail, comp);
        else tb_quick_sort(iterator, head, tail, comp); //!< @note the recursive stack size is limit
    }
    else tb_bubble_sort(iterator, head, tail, comp);
#endif
}
tb_void_t tb_sort_all(tb_iterator_ref_t iterator, tb_iterator_comp_t comp)
{
    tb_sort(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator), comp);
}

