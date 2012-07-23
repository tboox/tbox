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
 * @file		iterator.h
 * @ingroup 	algorithm
 *
 */
#ifndef TB_ALGORITHM_ITERATOR_H
#define TB_ALGORITHM_ITERATOR_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the iterator mode type
typedef enum __tb_iterator_mode_t
{
	TB_ITERATOR_MODE_FORWARD 		= 1 //!< forward iterator
,	TB_ITERATOR_MODE_REVERSE 		= 2 //!< reverse iterator
,	TB_ITERATOR_MODE_BDIRECT 		= 3 //!< bidirectional iterator
,	TB_ITERATOR_MODE_RACCESS 		= 4 //!< random access iterator

}tb_iterator_mode_t;

/// the iterator type
typedef struct __tb_iterator_t
{
	/// the iterator mode
	tb_size_t 				mode;

	/// the iterator data
	tb_pointer_t 			data;

	/// the iterator size
	tb_pointer_t 			size;

	/// the iterator temp
	tb_pointer_t 			temp;

	/// the iterator priv
	tb_pointer_t 			priv;

	/// the iterator step
	tb_size_t 				step;

	/// the iterator head
	tb_size_t 				(*head)(struct __tb_iterator_t* iterator);

	/// the iterator tail
	tb_size_t 				(*tail)(struct __tb_iterator_t* iterator);

	/// the iterator prev
	tb_size_t 				(*prev)(struct __tb_iterator_t* iterator, tb_size_t itor);

	/// the iterator next
	tb_size_t 				(*next)(struct __tb_iterator_t* iterator, tb_size_t itor);

	/// the iterator item
	tb_pointer_t 			(*item)(struct __tb_iterator_t* iterator, tb_size_t itor);

	/// the iterator save
	tb_pointer_t 			(*save)(struct __tb_iterator_t* iterator, tb_size_t itor);

	/// the iterator swap
	tb_void_t 				(*swap)(struct __tb_iterator_t* iterator, tb_size_t ltor, tb_size_t rtor);

	/// the iterator copy
	tb_void_t 				(*copy)(struct __tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item);

	/// the iterator comp
	tb_long_t 				(*comp)(struct __tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem);

}tb_iterator_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/// the iterator head
tb_size_t 		tb_iterator_head(tb_iterator_t* iterator);

/// the iterator tail
tb_size_t 		tb_iterator_tail(tb_iterator_t* iterator);

/// the iterator prev
tb_size_t 		tb_iterator_prev(tb_iterator_t* iterator, tb_size_t itor);

/// the iterator next
tb_size_t 		tb_iterator_next(tb_iterator_t* iterator, tb_size_t itor);

/// the iterator item
tb_pointer_t 	tb_iterator_item(tb_iterator_t* iterator, tb_size_t itor);

/// the iterator save
tb_pointer_t 	tb_iterator_save(tb_iterator_t* iterator, tb_size_t itor);

/// the iterator swap
tb_void_t 		tb_iterator_swap(tb_iterator_t* iterator, tb_size_t ltor, tb_size_t rtor);

/// the iterator copy
tb_void_t 		tb_iterator_copy(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item);

/// the iterator comp
tb_long_t 		tb_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem);

/// the integer iterator
tb_iterator_t 	tb_iterator_int(tb_long_t* data, tb_size_t size);

/// the string iterator
tb_iterator_t 	tb_iterator_str(tb_char_t** data, tb_size_t size, tb_bool_t bcase);

/// the pointer iterator
tb_iterator_t 	tb_iterator_ptr(tb_pointer_t* data, tb_size_t size);

/// the memory iterator
tb_iterator_t 	tb_iterator_mem(tb_pointer_t data, tb_size_t size, tb_size_t step, tb_pointer_t temp);


#endif
