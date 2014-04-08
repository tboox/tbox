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
 * @file		rfor.h
 * @ingroup 	algorithm
 *
 */
#ifndef TB_ALGORITHM_RFOR_H
#define TB_ALGORITHM_RFOR_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

/*! for items using iterator
 *
 * @code
 * tb_rfor(tb_char_t*, item, tb_iterator_head(iterator), tb_iterator_tail(iterator), iterator)
 * {
 * 		tb_trace_d("item: %s", item);
 * }
 *
 * tb_rfor(tb_size_t, item, tb_iterator_head(iterator), tb_iterator_tail(iterator), iterator)
 * {
 * 		tb_trace_d("item: %lu", item);
 * }
 *
 * tb_rfor(tb_hash_item_t*, item, tb_iterator_head(iterator), tb_iterator_tail(iterator), iterator)
 * {
 * 		if (item) tb_trace_d("item: %p => %p", item->name, item->data);
 * }
 * @endcode
 */
#define tb_rfor(type, item, head, tail, iterator) \
			/* iterator */ \
			tb_iterator_t* __iterator##item = (tb_iterator_t*)iterator; \
			tb_assert(!__iterator##item || (tb_iterator_mode(__iterator##item) & TB_ITERATOR_MODE_REVERSE)); \
			/* init */ \
			type item; \
			tb_size_t __head_##item = head; \
			tb_size_t __tail_##item = tail; \
			tb_size_t __itor_##item = (__iterator##item && __head_##item != __tail_##item)? tb_iterator_prev(__iterator##item, __tail_##item) : __tail_##item; \
			/* walk */ \
			if (__iterator##item && __itor_##item != __tail_##item) \
				for ( 	; \
						__itor_##item != __tail_##item && ((item = (type)tb_iterator_item(__iterator##item, __itor_##item)), 1); \
						__itor_##item = tb_iterator_prev(__iterator##item, __itor_##item)) 

/*! for all items using iterator
 *
 * @code
 *
 * tb_rfor_all(tb_char_t*, item, iterator)
 * {
 * 		tb_trace_d("item: %s", item);
 * }
 *
 * tb_rfor_all(tb_size_t, item, iterator)
 * {
 * 		tb_trace_d("item: %lu", item);
 * }
 *
 * tb_rfor_all(tb_hash_item_t*, item, iterator)
 * {
 * 		if (item) tb_trace_d("item: %p => %p", item->name, item->data);
 * }
 * @endcode
 */
#define tb_rfor_all(type, item, iterator) \
			tb_rfor(type, item, tb_iterator_head((tb_iterator_t*)iterator), tb_iterator_tail((tb_iterator_t*)iterator), iterator)

#endif
