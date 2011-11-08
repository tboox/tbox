/*!The Tiny Box Library
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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		slist.h
 *
 */
#ifndef TB_CONTAINER_SLIST_H
#define TB_CONTAINER_SLIST_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "fpool.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the item func
typedef tb_void_t 	(*tb_slist_item_free_func_t)(tb_void_t* item, tb_void_t* priv);	

// the slist item func type
typedef struct __tb_slist_item_func_t
{
	// the item func
	tb_slist_item_free_func_t 	free;

	// the priv data
	tb_void_t* 					priv;

}tb_slist_item_func_t;

/* the single list type
 *
 *
 * slist: |-----| => |-------------------------------------------------=> |------| => |------| => null
 *         head                                                                         last      tail
 *
 * head: => the first item
 * last: => the last item
 * tail: => behind the last item, no item
 
 * performance: 
 *
 * insert:
 * insert midd: slow
 * insert head: fast
 * insert tail: fast
 * insert next: fast
 * 
 * ninsert:
 * ninsert midd: slow
 * ninsert head: fast
 * ninsert tail: fast
 * ninsert next: fast
 *
 * remove:
 * remove midd: slow
 * remove head: fast
 * remove last: slow
 * remove next: fast
 *
 * nremove:
 * nremove midd: slow
 * nremove head: fast
 * nremove last: slow
 * nremove next: fast
 *
 * iterator:
 * next: fast
 * prev: slow
 *
 */
typedef struct __tb_slist_t
{
	// the pool
	tb_fpool_t* 			pool;

	// the head item 
	tb_size_t 				head;

	// the last item
	tb_size_t 				last;

	// the step size
	tb_size_t 				step;

	// the item func
	tb_slist_item_func_t 	func;

}tb_slist_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_slist_t* 		tb_slist_init(tb_size_t step, tb_size_t grow, tb_slist_item_func_t const* func);
tb_void_t 			tb_slist_exit(tb_slist_t* slist);

// accessors & modifiors
tb_void_t* 			tb_slist_at_head(tb_slist_t* slist);
tb_void_t* 			tb_slist_at_last(tb_slist_t* slist);

tb_void_t const* 	tb_slist_const_at_head(tb_slist_t const* slist);
tb_void_t const* 	tb_slist_const_at_last(tb_slist_t const* slist);

tb_void_t 			tb_slist_clear(tb_slist_t* slist);

tb_size_t 			tb_slist_insert(tb_slist_t* slist, tb_size_t index, tb_void_t const* item);
tb_size_t 			tb_slist_insert_next(tb_slist_t* slist, tb_size_t index, tb_void_t const* item);
tb_size_t 			tb_slist_insert_head(tb_slist_t* slist, tb_void_t const* item);
tb_size_t 			tb_slist_insert_tail(tb_slist_t* slist, tb_void_t const* item);

tb_size_t 			tb_slist_ninsert(tb_slist_t* slist, tb_size_t index, tb_void_t const* item, tb_size_t size);
tb_size_t 			tb_slist_ninsert_next(tb_slist_t* slist, tb_size_t index, tb_void_t const* item, tb_size_t size);
tb_size_t 			tb_slist_ninsert_head(tb_slist_t* slist, tb_void_t const* item, tb_size_t size);
tb_size_t 			tb_slist_ninsert_tail(tb_slist_t* slist, tb_void_t const* item, tb_size_t size);

tb_size_t 			tb_slist_replace(tb_slist_t* slist, tb_size_t index, tb_void_t const* item);
tb_size_t 			tb_slist_replace_head(tb_slist_t* slist, tb_void_t const* item);
tb_size_t 			tb_slist_replace_last(tb_slist_t* slist, tb_void_t const* item);

tb_size_t 			tb_slist_nreplace(tb_slist_t* slist, tb_size_t index, tb_void_t const* item, tb_size_t size);
tb_size_t 			tb_slist_nreplace_head(tb_slist_t* slist, tb_void_t const* item, tb_size_t size);
tb_size_t 			tb_slist_nreplace_last(tb_slist_t* slist, tb_void_t const* item, tb_size_t size);

tb_size_t 			tb_slist_remove(tb_slist_t* slist, tb_size_t index);
tb_size_t 			tb_slist_remove_next(tb_slist_t* slist, tb_size_t index);
tb_size_t 			tb_slist_remove_head(tb_slist_t* slist);
tb_size_t 			tb_slist_remove_last(tb_slist_t* slist);

tb_size_t 			tb_slist_nremove(tb_slist_t* slist, tb_size_t index, tb_size_t size);
tb_size_t 			tb_slist_nremove_next(tb_slist_t* slist, tb_size_t index, tb_size_t size);
tb_size_t 			tb_slist_nremove_head(tb_slist_t* slist, tb_size_t size);
tb_size_t 			tb_slist_nremove_last(tb_slist_t* slist, tb_size_t size);

/* iterator
 * 
 * tb_size_t itor = tb_slist_itor_head(slist);
 * tb_size_t tail = tb_slist_itor_tail(slist);
 * for (; index != tail; itor = tb_slist_itor_next(slist, itor))
 * {
 * 		tb_byte_t const* item = tb_slist_itor_const_at(slist, itor);
 * 		if (item)
 * 		{
 * 			// ...
 * 		}
 * }
 *
*/
tb_void_t* 			tb_slist_itor_at(tb_slist_t* slist, tb_size_t itor);
tb_void_t const* 	tb_slist_itor_const_at(tb_slist_t const* slist, tb_size_t itor);

tb_size_t 			tb_slist_itor_head(tb_slist_t const* slist);
tb_size_t 			tb_slist_itor_tail(tb_slist_t const* slist);
tb_size_t 			tb_slist_itor_last(tb_slist_t const* slist);
tb_size_t 			tb_slist_itor_next(tb_slist_t const* slist, tb_size_t itor);
tb_size_t 			tb_slist_itor_prev(tb_slist_t const* slist, tb_size_t itor);

// attributes
tb_size_t 			tb_slist_size(tb_slist_t const* slist);
tb_size_t 			tb_slist_maxn(tb_slist_t const* slist);


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

