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
 * head
 */
static __tb_inline__ tb_bool_t tb_heap_check(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail)
{
	if (head != tail)
	{
		tb_size_t root;
		for (root = head; ++head != tail; ++root)
		{
			// root < left?
			if (tb_iterator_comp(iterator, tb_iterator_item(iterator, root), tb_iterator_item(iterator, head)) < 0) return TB_FALSE;
			// end?
			else if (++head == tail) break;
			// root < right?
			else if (tb_iterator_comp(iterator, tb_iterator_item(iterator, root), tb_iterator_item(iterator, head)) < 0) return TB_FALSE;
		}
	}

	// ok
	return TB_TRUE;
}
/*!push heap
 *
 * <pre>
 * hole: bottom => top
 * init:
 *                                          16(top)
 *                               -------------------------
 *                              |                         |
 *                              14                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       8(parent)      7           9             3
 *                   ---------      
 *                  |         |     
 *                  2      (hole) <= 11(val)
 * after:
 *                                          16(top)
 *                               -------------------------
 *                              |                         |
 *                              14(parent)                10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       11(hole)       7           9             3
 *                   ---------      
 *                  |         |    
 *                  2         8 
 * </pre>
 */
static __tb_inline__ tb_void_t tb_heap_push(tb_iterator_t* iterator, tb_size_t head, tb_size_t hole, tb_size_t top, tb_cpointer_t item)
{
	// (hole - 1) / 2: the parent node of the hole
	// finds the final hole
	tb_size_t i = 0;
	for (i = (hole - 1) / 2; hole > top && (tb_iterator_comp(iterator, tb_iterator_item(iterator, head + i), item) < 0); i = (hole - 1) / 2)
	{	
		// copy item: parent => hole
		tb_iterator_copy(iterator, head + i, item);

		// move node: hole => parent
		hole = i;
	}

	// copy item
	tb_iterator_copy(iterator, head + hole, item);
}
/*!adjust heap
 *
 * <pre>
 * hole: top => bottom
 * init:
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                           (hole)                       10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       8(larger)      7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2         4     1(bottom - 1)
 *
 * after:
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                              8                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                      (hole)          7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2 (larger)4     1(bottom - 1)
 *
 * after:
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                              8                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       4              7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2      (hole)   1(bottom - 1)
 * </pre>
 */
static __tb_inline__ tb_void_t tb_heap_adjust(tb_iterator_t* iterator, tb_size_t head, tb_size_t hole, tb_size_t bottom, tb_cpointer_t item)
{
	// save top position
	tb_size_t top = hole;

	// 2 * hole + 2: the right child node of hole
	tb_size_t i = (hole << 1) + 2;

	for (; i < bottom; i = 2 * i + 2)
	{	
		// gets the larger child node
		if (tb_iterator_comp(iterator, tb_iterator_item(iterator, head + i), tb_iterator_item(iterator, head + i - 1)) < 0) --i;

		// larger child => hole
		tb_iterator_copy(iterator, head + hole, tb_iterator_item(iterator, head + i));

		// move the hole down to it's larger child node 
		hole = i;
	}

	if (i == bottom)
	{	
		// bottom child => hole
		tb_iterator_copy(iterator, head + hole, tb_iterator_item(iterator, head + bottom - 1));

		// move hole down to bottom
		hole = bottom - 1;
	}

	// push item into the hole
	tb_heap_push(iterator, head, hole, top, item);
}
/*!make heap
 *
 * <pre>
 * heap:    16      14      10      8       7       9       3       2       4       1
 *
 *                                          16(head)
 *                               -------------------------
 *                              |                         |
 *                              14                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       8       (bottom / 2 - 1)7  9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2         4     1(bottom - 1)
 * </pre>
 */
static __tb_inline__ tb_void_t tb_heap_make(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail)
{
	// make
	tb_size_t hole;
	tb_size_t bottom = tail - head;
	for (hole = bottom / 2; hole > 0; )
	{
		// reheap top half, bottom to top
		--hole;
		tb_heap_adjust(iterator, head, hole, bottom, tb_iterator_save(iterator, head + hole));
	}

	// check
	tb_assert(tb_heap_check(iterator, head, tail));
}

/*!pop the top of heap to last and reheap
 *
 * <pre>
 *                                          16(head) 
 *                               ----------------|--------
 *                              |                |        |
 *                              14               |        10
 *                        --------------         |   -------------
 *                       |              |        |  |             |
 *                       8              7        |  9             3
 *                   ---------       ----        |
 *                  |         |     |            |
 *                  2         4     1(last)<-----
 *                                (hole)
 * </pre>
 */   
static __tb_inline__ tb_void_t tb_heap_pop0(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail)
{
	// item
	tb_cpointer_t item = tb_iterator_save(iterator, tail - 1);

	// top => last
	tb_iterator_copy(iterator, tail - 1, tb_iterator_item(iterator, head));

	// reheap it
	tb_heap_adjust(iterator, head, 0, tail - head - 1, item);

	// check
//	tb_assert(tb_heap_check(iterator, head, tail - head - 1));
}
/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_void_t tb_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail)
{
	if (iterator->mode & TB_ITERATOR_MODE_RACCESS) 
	{
		if (tail > head + 100000) tb_heap_sort(iterator, head, tail);
		else tb_quick_sort(iterator, head, tail); //!< @note the recursive stack size is limit
	}
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

	// sort
	for (itor1 = head; itor1 != tail; itor1 = tb_iterator_next(iterator, itor1))
	{
		for (itor2 = itor1, itor2 = tb_iterator_next(iterator, itor2); itor2 != tail; itor2 = tb_iterator_next(iterator, itor2))
		{
			if (tb_iterator_comp(iterator, tb_iterator_item(iterator, itor2), tb_iterator_item(iterator, itor1)) < 0)
				tb_iterator_swap(iterator, itor2, itor1);
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
				tb_iterator_copy(iterator, next, tb_iterator_item(iterator, last));

		// item => hole
		tb_iterator_copy(iterator, next, item);
	}
}
tb_void_t tb_insert_sort_all(tb_iterator_t* iterator)
{
	tb_insert_sort(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator));
}
tb_void_t tb_quick_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail)
{	
	// check
	tb_assert_and_check_return(iterator && iterator->mode & TB_ITERATOR_MODE_RACCESS);
	tb_check_return(head != tail);

	// hole => key
	tb_cpointer_t key = tb_iterator_save(iterator, head);

	// init
	tb_size_t l = head;
	tb_size_t r = tail - 1;
	while (r > l)
	{
		// find: <= 
		for (; r != l; r--)
			if (tb_iterator_comp(iterator, tb_iterator_item(iterator, r), key) < 0) break;
		if (r != l) 
		{
			tb_iterator_copy(iterator, l, tb_iterator_item(iterator, r));
			l++;
		}

		// find: =>
		for (; l != r; l++)
			if (tb_iterator_comp(iterator, tb_iterator_item(iterator, l), key) > 0) break;
		if (l != r) 
		{
			tb_iterator_copy(iterator, r, tb_iterator_item(iterator, l));
			r--;
		}
	}

	// key => hole
	tb_iterator_copy(iterator, l, key);

	// sort [head, hole - 1]
	tb_quick_sort(iterator, head, l);

	// sort [hole + 1, tail]
	tb_quick_sort(iterator, ++l, tail);
}
tb_void_t tb_quick_sort_all(tb_iterator_t* iterator)
{
	tb_quick_sort(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator));
}
/*!the heap sort 
 * 
 * <pre>
 * init:
 *
 *                                           16(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       14             7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2         8     1(last - 1)
 * 
 * make_heap:
 *
 *                                           16(head)
 *                               -------------------------
 *                              |                         |
 *                              14                        10
 *                        --------------             -------------
 *                       |              |           |             |
 *                       8              7           9             3
 *                   ---------       ----
 *                  |         |     |
 *                  2         4     1(last - 1)
 * pop_heap:
 *
 *                                          16(head)--------------------------
 *                               -------------------------                     |
 *                              |                         |                    |
 *                              4                         10                   |
 *                        --------------             -------------             |
 *                       |              |           |             |            | 
 *                       14             7           9             3            |
 *                   ---------       ----                                      |
 *                  |         |     |                                          |
 *                  2         8     1(last - 1) <------------------------------ 
 *
 *                                          (hole)(head)
 *                               -------------------------               
 *                              |                         |                  
 *                              4                         10                 
 *                        --------------             -------------           
 *                       |              |           |             |          (val = 1)
 *                       14             7           9             3         
 *                   ---------       ----                                    
 *                  |         |     |                                       
 *                  2         8     16(last - 1)
 *                           
 * adjust_heap:
 *                                          14(head)
 *                               -------------------------               
 *                              |                         |                  
 *                              8                        10                 
 *                        --------------             -------------           
 *                       |              |           |             |           (val = 1)         
 *                       4              7           9             3         
 *                   ---------                                         
 *                  |         |                                            
 *                  2      (hole)(last - 1)   16
 *
 *
 * push_heap:
 *                                          14(head)
 *                               -------------------------               
 *                              |                         |                  
 *                              8                        10                 
 *                        --------------             -------------           
 *                       |              |           |             |           (val = 1)         
 *                       4              7           9             3              |
 *                   ---------                                                   |
 *                  |         | /-----------------------------------------------
 *                  2      (hole)(last - 1)   16
 *
 *                                          14(head)
 *                               -------------------------               
 *                              |                         |                  
 *                              8                        10                 
 *                        --------------             -------------           
 *                       |              |           |             |           (val = 1)         
 *                       4              7           9             3            
 *                   ---------                                                   
 *                  |         |  
 *                  2       1(last - 1)   16
 *
 * pop_heap adjust_heap push_heap ...
 *
 * final_heap:
 *                                           1(head)
 *                            
 *                         
 *                              2                         3               
 *                               
 *                              
 *                       4              7           8             9           
 *                                                            
 *             
 *                  10       14      16
 *     
 * result: 1 2 3 4 7 8 9 10 14 16
 * </pre>
 */
tb_void_t tb_heap_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail)
{
	// check
	tb_assert_and_check_return(iterator && iterator->mode & TB_ITERATOR_MODE_RACCESS);
	tb_check_return(head != tail);

	// make
	tb_heap_make(iterator, head, tail);

	// pop0
	for (; tail > head + 1; tail--)
		tb_heap_pop0(iterator, head, tail);
}
tb_void_t tb_heap_sort_all(tb_iterator_t* iterator)
{
	tb_heap_sort(iterator, tb_iterator_head(iterator), tb_iterator_tail(iterator));
}
