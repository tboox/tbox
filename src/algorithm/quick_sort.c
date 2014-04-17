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
 * @author		ruki
 * @file		quick_sort.c
 * @ingroup 	algorithm
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "quick_sort.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_void_t tb_quick_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail, tb_iterator_comp_t comp)
{	
	// check
	tb_assert_and_check_return(iterator && (tb_iterator_mode(iterator) & TB_ITERATOR_MODE_RACCESS));
	tb_check_return(head != tail);

	// init
	tb_size_t 		step = tb_iterator_step(iterator);
	tb_pointer_t 	key = step > sizeof(tb_pointer_t)? tb_malloc(step) : tb_null;
	tb_assert_and_check_return(step <= sizeof(tb_pointer_t) || key);

	// the comparer
	if (!comp) comp = tb_iterator_comp;

	// hole => key
	if (step <= sizeof(tb_pointer_t)) key = tb_iterator_item(iterator, head);
	else tb_memcpy(key, tb_iterator_item(iterator, head), step);

	// quick_sort
	tb_size_t l = head;
	tb_size_t r = tail - 1;
	while (r > l)
	{
		// find: <= 
		for (; r != l; r--)
			if (comp(iterator, tb_iterator_item(iterator, r), key) < 0) break;
		if (r != l) 
		{
			tb_iterator_copy(iterator, l, tb_iterator_item(iterator, r));
			l++;
		}

		// find: =>
		for (; l != r; l++)
			if (comp(iterator, tb_iterator_item(iterator, l), key) > 0) break;
		if (l != r) 
		{
			tb_iterator_copy(iterator, r, tb_iterator_item(iterator, l));
			r--;
		}
	}

	// key => hole
	tb_iterator_copy(iterator, l, key);

	// quick_sort [head, hole - 1]
	tb_quick_sort(iterator, head, l, comp);

	// quick_sort [hole + 1, tail]
	tb_quick_sort(iterator, ++l, tail, comp);

	// free
	if (key && step > sizeof(tb_pointer_t)) tb_free(key);
}
tb_void_t tb_quick_sort_all(tb_iterator_t* iterator, tb_iterator_comp_t comp)
{
	tb_quick_sort(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator), comp);
}

