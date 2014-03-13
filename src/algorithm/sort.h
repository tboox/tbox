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
 * @file		sort.h
 * @ingroup 	algorithm
 *
 */
#ifndef TB_ALGORITHM_SORT_H
#define TB_ALGORITHM_SORT_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the sorter
 *
 * @param iterator 	the iterator
 * @param head 		the iterator head
 * @param tail 		the iterator tail
 * @param comp 		the comparer
 */
tb_void_t 			tb_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail, tb_iterator_comp_t comp);

/*! the sorter for all
 *
 * @param iterator 	the iterator
 * @param head 		the iterator head
 * @param tail 		the iterator tail
 * @param comp 		the comparer
 */
tb_void_t 			tb_sort_all(tb_iterator_t* iterator, tb_iterator_comp_t comp);

/*! the bubble sorter, O(n^2)
 *
 * @param iterator 	the iterator
 * @param head 		the iterator head
 * @param tail 		the iterator tail
 * @param comp 		the comparer
 */
tb_void_t 			tb_bubble_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail, tb_iterator_comp_t comp);

/*! the bubble sorter for all
 *
 * @param iterator 	the iterator
 * @param head 		the iterator head
 * @param tail 		the iterator tail
 * @param comp 		the comparer
 */
tb_void_t 			tb_bubble_sort_all(tb_iterator_t* iterator, tb_iterator_comp_t comp);

/*! the insert sorter, O(n^2)
 *
 * @param iterator 	the iterator
 * @param head 		the iterator head
 * @param tail 		the iterator tail
 * @param comp 		the comparer
 */
tb_void_t 			tb_insert_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail, tb_iterator_comp_t comp);

/*! the insert sorter for all
 *
 * @param iterator 	the iterator
 * @param head 		the iterator head
 * @param tail 		the iterator tail
 * @param comp 		the comparer
 */
tb_void_t 			tb_insert_sort_all(tb_iterator_t* iterator, tb_iterator_comp_t comp);

/*! the quick sorter, O(nlog(n))
 *
 * @param iterator 	the iterator
 * @param head 		the iterator head
 * @param tail 		the iterator tail
 * @param comp 		the comparer
 */
tb_void_t 			tb_quick_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail, tb_iterator_comp_t comp);

/*! the quick sorter for all
 *
 * @param iterator 	the iterator
 * @param head 		the iterator head
 * @param tail 		the iterator tail
 * @param comp 		the comparer
 */
tb_void_t 			tb_quick_sort_all(tb_iterator_t* iterator, tb_iterator_comp_t comp);

/*! the heap sorter, O(nlog(n))
 *
 * @param iterator 	the iterator
 * @param head 		the iterator head
 * @param tail 		the iterator tail
 * @param comp 		the comparer
 */
tb_void_t 			tb_heap_sort(tb_iterator_t* iterator, tb_size_t head, tb_size_t tail, tb_iterator_comp_t comp);

/*! the heap sorter for all
 *
 * @param iterator 	the iterator
 * @param head 		the iterator head
 * @param tail 		the iterator tail
 * @param comp 		the comparer
 */
tb_void_t 			tb_heap_sort_all(tb_iterator_t* iterator, tb_iterator_comp_t comp);



#endif
