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
 * @author      ruki
 * @file        rfor_if.h
 * @ingroup     algorithm
 *
 */
#ifndef TB_ALGORITHM_RFOR_IF_H
#define TB_ALGORITHM_RFOR_IF_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/*! for items using iterator
 *
 * @code
 * tb_rfor_if(tb_char_t*, item, tb_iterator_head(iterator), tb_iterator_tail(iterator), iterator, item)
 * {
 *      tb_trace_d("item: %s", item);
 * }
 *
 * tb_rfor_if(tb_size_t, item, tb_iterator_head(iterator), tb_iterator_tail(iterator), iterator, item > 10)
 * {
 *      tb_trace_d("item: %lu", item);
 * }
 *
 * tb_rfor_if(tb_hash_item_t*, item, tb_iterator_head(iterator), tb_iterator_tail(iterator), iterator, item != tb_null)
 * {
 *      tb_trace_d("item: %p => %p", item->name, item->data);
 * }
 * @endcode
 */
#define tb_rfor_if(type, item, head, tail, iterator, cond) \
            /* iterator */ \
            tb_iterator_t* item##_iterator = (tb_iterator_t*)iterator; \
            tb_assert(!item##_iterator || (tb_iterator_mode(item##_iterator) & TB_ITERATOR_MODE_REVERSE)); \
            /* init */ \
            type item; \
            tb_size_t item##_itor; \
            tb_size_t item##_head = head; \
            tb_size_t item##_tail = tail; \
            /* walk */ \
            if (item##_iterator && item##_head != item##_tail) \
                for (   item##_itor = tb_iterator_prev(item##_iterator, item##_tail); \
                        item##_itor != item##_tail && ((item = (type)tb_iterator_item(item##_iterator, item##_itor)), item##_itor = item##_itor != item##_head? item##_itor : item##_tail, 1); \
                        item##_itor = item##_itor != item##_tail? tb_iterator_prev(item##_iterator, item##_itor) : item##_tail) if ((cond))

/*! for all items using iterator
 *
 * @code
 *
 * tb_rfor_all(tb_char_t*, item, iterator, item)
 * {
 *      tb_trace_d("item: %s", item);
 * }
 *
 * tb_rfor_all(tb_size_t, item, iterator, itme > 10)
 * {
 *      tb_trace_d("item: %lu", item);
 * }
 *
 * tb_rfor_all(tb_hash_item_t*, item, iterator, item != tb_null)
 * {
 *      tb_trace_d("item: %p => %p", item->name, item->data);
 * }
 * @endcode
 */
#define tb_rfor_all_if(type, item, iterator, cond) \
            tb_rfor_if(type, item, tb_iterator_head((tb_iterator_t*)iterator), tb_iterator_tail((tb_iterator_t*)iterator), iterator, cond)

#endif
