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
 * @file		insert_sort.c
 * @ingroup 	algorithm
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "insert_sort.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

/*!the insertion sort
 *
 * <pre>
 * old:     5       2       6       2       8       6       1
 *
 *        (hole)
 * step1: ((5))     2       6       2       8       6       1
 *        (next) <=
 *
 *        (hole)  
 * step2: ((2))    (5)      6       2       8       6       1
 *                (next) <=
 *
 *                        (hole)
 * step3:   2       5     ((6))     2       8       6       1
 *                        (next) <=
 *
 *                 (hole)       
 * step4:   2      ((2))   (5)     (6)      8       6       1
 *                                (next) <=
 *
 *                                        (hole)
 * step5:   2       2       5       6     ((8))     6       1
 *                                        (next) <=
 *
 *                                        (hole) 
 * step6:   2       2       5       6     ((6))    (8)       1
 *                                                (next) <=
 *
 *        (hole)                                         
 * step7: ((1))    (2)     (2)     (5)     (6)     (6)      (8)       
 *                                                        (next)
 * </pre>
 */
tb_void_t tb_insert_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail, tb_iterator_comp_t comp)
{	
	// check
	tb_assert_and_check_return(iterator && (tb_iterator_mode(iterator) & TB_ITERATOR_MODE_BDIRECT));
	tb_check_return(head != tail);
	
	// init
	tb_size_t 		step = tb_iterator_step(iterator);
	tb_pointer_t 	temp = step > sizeof(tb_pointer_t)? tb_malloc(step) : tb_null;
	tb_assert_and_check_return(step <= sizeof(tb_pointer_t) || temp);

	// the comparer
	if (!comp) comp = tb_iterator_comp;

	// sort
	tb_size_t last, next;
	for (next = tb_iterator_next(iterator, head); next != tail; next = tb_iterator_next(iterator, next))
	{
		// save next
		if (step <= sizeof(tb_pointer_t)) temp = tb_iterator_item(iterator, next);
		else tb_memcpy(temp, tb_iterator_item(iterator, next), step);

		// look for hole and move elements[hole, next - 1] => [hole + 1, next]
		for (last = next; last != head && (last = tb_iterator_prev(iterator, last), comp(iterator, temp, tb_iterator_item(iterator, last)) < 0); next = last)
				tb_iterator_copy(iterator, next, tb_iterator_item(iterator, last));

		// item => hole
		tb_iterator_copy(iterator, next, temp);
	}

	// free
	if (temp && step > sizeof(tb_pointer_t)) tb_free(temp);
}
tb_void_t tb_insert_sort_all(tb_iterator_t* iterator, tb_iterator_comp_t comp)
{
	tb_insert_sort(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator), comp);
}

