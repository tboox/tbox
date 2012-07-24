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
 * @file		slist.h
 * @ingroup 	container
 *
 */
#ifndef TB_CONTAINER_SLIST_H
#define TB_CONTAINER_SLIST_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "item.h"
#include "iterator.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/*!the single list type
 *
 * <pre>
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
 * </pre>
 *
 */
typedef struct __tb_slist_t
{
	/// the itor
	tb_iterator_t 			itor;

	/// the rpool
	tb_handle_t 			pool;

	/// the head item 
	tb_size_t 				head;

	/// the last item
	tb_size_t 				last;

	/// the item func
	tb_item_func_t 			func;

}tb_slist_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_slist_t* 		tb_slist_init(tb_size_t grow, tb_item_func_t func);
tb_void_t 			tb_slist_exit(tb_slist_t* slist);

// accessors & modifiors
tb_pointer_t 		tb_slist_head(tb_slist_t* slist);
tb_pointer_t 		tb_slist_last(tb_slist_t* slist);

tb_void_t 			tb_slist_clear(tb_slist_t* slist);

tb_size_t 			tb_slist_insert(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data);
tb_size_t 			tb_slist_insert_next(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data);
tb_size_t 			tb_slist_insert_head(tb_slist_t* slist, tb_cpointer_t data);
tb_size_t 			tb_slist_insert_tail(tb_slist_t* slist, tb_cpointer_t data);

tb_size_t 			tb_slist_ninsert(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data, tb_size_t size);
tb_size_t 			tb_slist_ninsert_next(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data, tb_size_t size);
tb_size_t 			tb_slist_ninsert_head(tb_slist_t* slist, tb_cpointer_t data, tb_size_t size);
tb_size_t 			tb_slist_ninsert_tail(tb_slist_t* slist, tb_cpointer_t data, tb_size_t size);

tb_size_t 			tb_slist_replace(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data);
tb_size_t 			tb_slist_replace_head(tb_slist_t* slist, tb_cpointer_t data);
tb_size_t 			tb_slist_replace_last(tb_slist_t* slist, tb_cpointer_t data);

tb_size_t 			tb_slist_nreplace(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data, tb_size_t size);
tb_size_t 			tb_slist_nreplace_head(tb_slist_t* slist, tb_cpointer_t data, tb_size_t size);
tb_size_t 			tb_slist_nreplace_last(tb_slist_t* slist, tb_cpointer_t data, tb_size_t size);

tb_size_t 			tb_slist_remove(tb_slist_t* slist, tb_size_t itor);
tb_size_t 			tb_slist_remove_next(tb_slist_t* slist, tb_size_t itor);
tb_size_t 			tb_slist_remove_head(tb_slist_t* slist);
tb_size_t 			tb_slist_remove_last(tb_slist_t* slist);

tb_size_t 			tb_slist_nremove(tb_slist_t* slist, tb_size_t itor, tb_size_t size);
tb_size_t 			tb_slist_nremove_next(tb_slist_t* slist, tb_size_t itor, tb_size_t size);
tb_size_t 			tb_slist_nremove_head(tb_slist_t* slist, tb_size_t size);
tb_size_t 			tb_slist_nremove_last(tb_slist_t* slist, tb_size_t size);

// attributes
tb_size_t 			tb_slist_size(tb_slist_t const* slist);
tb_size_t 			tb_slist_maxn(tb_slist_t const* slist);

/*!walk
 *
 * be faster than the iterator mode, optimizate to remove items for walking
 *
 * @code
 * tb_bool_t tb_slist_item_func(tb_slist_t* slist, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
 * {
 *  	tb_assert_and_check_return_val(slist && bdel, TB_FALSE);
 *
 * 		// is tail?
 * 		if (!item) ;
 *
 * 		// delete it?
 * 		// *bdel = TB_TRUE;
 *
 * 		// ok
 * 		return TB_TRUE;
 *
 * fail:
 * 		// break
 * 		return TB_FALSE;
 * }
 * @endcode
 *
 */
tb_void_t 			tb_slist_walk(tb_slist_t* slist, tb_bool_t (*func)(tb_slist_t* slist, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data);



#endif

