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
 * @file		iterator.h
 * @ingroup 	container
 *
 */
#ifndef TB_CONTAINER_ITERATOR_H
#define TB_CONTAINER_ITERATOR_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
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

	/// the iterator step
	tb_size_t 				step;

	/// the iterator size
	tb_size_t 				size;

	/// the iterator data
	tb_pointer_t 			data;

	/// the iterator priv
	tb_pointer_t 			priv;

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

	/// the iterator comp
	tb_long_t 				(*comp)(struct __tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem);

	/// the iterator copy
	tb_void_t 				(*copy)(struct __tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item);

	/// the iterator delt
	tb_void_t 				(*delt)(struct __tb_iterator_t* iterator, tb_size_t itor);

}tb_iterator_t;

/// the iterator comp func type
typedef tb_long_t 			(*tb_iterator_comp_t)(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/// the integer iterator
tb_iterator_t 	tb_iterator_int(tb_long_t* data, tb_size_t size);

/// the string iterator
tb_iterator_t 	tb_iterator_str(tb_char_t** data, tb_size_t size, tb_bool_t bcase);

/// the pointer iterator
tb_iterator_t 	tb_iterator_ptr(tb_pointer_t* data, tb_size_t size);

/// the memory iterator 
tb_iterator_t 	tb_iterator_mem(tb_pointer_t data, tb_size_t size, tb_size_t step);

/// the iterator mode
tb_size_t 		tb_iterator_mode(tb_iterator_t* iterator);

/// the iterator step
tb_size_t 		tb_iterator_step(tb_iterator_t* iterator);

/// the iterator head
tb_size_t 		tb_iterator_head(tb_iterator_t* iterator);

/// the iterator last
tb_size_t 		tb_iterator_last(tb_iterator_t* iterator);

/// the iterator tail
tb_size_t 		tb_iterator_tail(tb_iterator_t* iterator);

/// the iterator prev
tb_size_t 		tb_iterator_prev(tb_iterator_t* iterator, tb_size_t itor);

/// the iterator next
tb_size_t 		tb_iterator_next(tb_iterator_t* iterator, tb_size_t itor);

/// the iterator item
tb_pointer_t 	tb_iterator_item(tb_iterator_t* iterator, tb_size_t itor);

/// the iterator delt
tb_void_t 		tb_iterator_delt(tb_iterator_t* iterator, tb_size_t itor);

/// the iterator copy
tb_void_t 		tb_iterator_copy(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item);

/// the iterator comp
tb_long_t 		tb_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem);

#endif
