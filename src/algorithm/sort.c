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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		sort.c
 * @ingroup 	algorithm
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "sort.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_void_t tb_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail)
{
	if (iterator->mode & TB_ITERATOR_MODE_RACCESS) tb_quick_sort(iterator, head, tail);
	else tb_bubble_sort(iterator, head, tail);
}
tb_void_t tb_sort_all(tb_iterator_t* iterator)
{
	tb_sort(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator));
}
tb_void_t tb_bubble_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail)
{
	// check
	tb_assert_and_check_return(iterator && iterator->mode & TB_ITERATOR_MODE_FORWARD);
	tb_check_return(head != tail);

	// init
	tb_size_t 		itor1;
	tb_size_t 		itor2;
	tb_pointer_t 	item1 = TB_NULL;
	tb_pointer_t 	item2 = TB_NULL;

	// sort
	for (itor1 = head; itor1 != tail; itor1 = tb_iterator_next(iterator, itor1))
	{
		for (itor2 = itor1, itor2 = tb_iterator_next(iterator, itor2); itor2 != tail; itor2 = tb_iterator_next(iterator, itor2))
		{
			item1 = tb_iterator_item(iterator, itor1);
			item2 = tb_iterator_item(iterator, itor2);
			if (tb_iterator_comp(iterator, item2, item1) < 0)
				tb_iterator_swap(iterator, item2, item1);
		}
	}
}
tb_void_t tb_bubble_sort_all(tb_iterator_t* iterator)
{
	tb_bubble_sort(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator));
}

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
tb_void_t tb_insert_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail)
{	
	// check
	tb_assert_and_check_return(iterator && iterator->mode & TB_ITERATOR_MODE_BDIRECT);
	tb_check_return(head != tail);

	tb_size_t last;
	tb_size_t next;
	for (next = tb_iterator_next(iterator, head); next != tail; next = tb_iterator_next(iterator, next))
	{
		// save item
		tb_cpointer_t item = tb_iterator_save(iterator, next);

		// look for hole and move elements[hole, next - 1] => [hole + 1, next]
		for (last = next; last != head && (last = tb_iterator_prev(iterator, last), tb_iterator_comp(iterator, item, tb_iterator_item(iterator, last)) < 0); next = last)
				tb_iterator_copy(iterator, tb_iterator_item(iterator, next), tb_iterator_item(iterator, last));

		// item => hole
		tb_iterator_copy(iterator, tb_iterator_item(iterator, next), item);
	}
}
tb_void_t tb_insert_sort_all(tb_iterator_t* iterator)
{
	tb_insert_sort(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator));
}
tb_void_t tb_quick_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail)
{	
	// check
	tb_assert_and_check_return(iterator && iterator->mode & TB_ITERATOR_MODE_BDIRECT);
	tb_check_return(head != tail);

	// key
	tb_cpointer_t key = tb_iterator_save(iterator, head);

}
tb_void_t tb_quick_sort_all(tb_iterator_t* iterator)
{
	tb_quick_sort(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator));
}
tb_void_t tb_heap_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail)
{	

}
tb_void_t tb_heap_sort_all(tb_iterator_t* iterator)
{
	tb_heap_sort(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator));
}
