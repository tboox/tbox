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
 * @file		dlist.h
 * @ingroup 	container
 *
 */
#ifndef TB_CONTAINER_DLIST_H
#define TB_CONTAINER_DLIST_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "item.h"
#include "iterator.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/*!the double list type
 *
 *
 * <pre>
 * dlist: 0 => |-----| => |-------------------------------------------------=> |------| => |------| => 0
 *       tail   head                                                                         last     tail
 *        |                                                                                            |
 *        ---------------------------------------------------------------------------------------------
 *
 * head: => the first item
 * last: => the last item
 * tail: => behind the last item, no item
 *
 * performance: 
 *
 * insert:
 * insert midd: fast
 * insert head: fast
 * insert tail: fast
 * insert next: fast
 * 
 * ninsert:
 * ninsert midd: fast
 * ninsert head: fast
 * ninsert tail: fast
 * ninsert next: fast
 *
 * remove:
 * remove midd: fast
 * remove head: fast
 * remove last: fast
 * remove next: fast
 *
 * nremove:
 * nremove midd: fast
 * nremove head: fast
 * nremove last: fast
 * nremove next: fast
 *
 * iterator:
 * next: fast
 * prev: fast
 * </pre>
 *
 */
typedef struct __tb_dlist_t
{
	/// the itor
	tb_iterator_t 			itor;

	// the pool
	tb_handle_t 			pool;

	// the head item
	tb_size_t 				head;

	// the last item
	tb_size_t 				last;

	// the func
	tb_item_func_t 			func;

}tb_dlist_t;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_dlist_t* 		tb_dlist_init(tb_size_t grow, tb_item_func_t func);
tb_void_t 			tb_dlist_exit(tb_dlist_t* dlist);

// accessors
tb_pointer_t 		tb_dlist_head(tb_dlist_t* dlist);
tb_pointer_t 		tb_dlist_last(tb_dlist_t* dlist);

// modifiors
tb_void_t 			tb_dlist_clear(tb_dlist_t* dlist);

tb_size_t 			tb_dlist_insert(tb_dlist_t* dlist, tb_size_t itor, tb_cpointer_t data);
tb_size_t 			tb_dlist_insert_head(tb_dlist_t* dlist, tb_cpointer_t data);
tb_size_t 			tb_dlist_insert_tail(tb_dlist_t* dlist, tb_cpointer_t data);

tb_size_t 			tb_dlist_ninsert(tb_dlist_t* dlist, tb_size_t itor, tb_cpointer_t data, tb_size_t size);
tb_size_t 			tb_dlist_ninsert_head(tb_dlist_t* dlist, tb_cpointer_t data, tb_size_t size);
tb_size_t 			tb_dlist_ninsert_tail(tb_dlist_t* dlist, tb_cpointer_t data, tb_size_t size);

tb_size_t 			tb_dlist_replace(tb_dlist_t* dlist, tb_size_t itor, tb_cpointer_t data);
tb_size_t 			tb_dlist_replace_head(tb_dlist_t* dlist, tb_cpointer_t data);
tb_size_t 			tb_dlist_replace_last(tb_dlist_t* dlist, tb_cpointer_t data);

tb_size_t 			tb_dlist_nreplace(tb_dlist_t* dlist, tb_size_t itor, tb_cpointer_t data, tb_size_t size);
tb_size_t 			tb_dlist_nreplace_head(tb_dlist_t* dlist, tb_cpointer_t data, tb_size_t size);
tb_size_t 			tb_dlist_nreplace_last(tb_dlist_t* dlist, tb_cpointer_t data, tb_size_t size);

tb_size_t 			tb_dlist_remove(tb_dlist_t* dlist, tb_size_t itor);
tb_size_t 			tb_dlist_remove_head(tb_dlist_t* dlist);
tb_size_t 			tb_dlist_remove_last(tb_dlist_t* dlist);

tb_size_t 			tb_dlist_nremove(tb_dlist_t* dlist, tb_size_t itor, tb_size_t size);
tb_size_t 			tb_dlist_nremove_head(tb_dlist_t* dlist, tb_size_t size);
tb_size_t 			tb_dlist_nremove_last(tb_dlist_t* dlist, tb_size_t size);

// attributes
tb_size_t 			tb_dlist_size(tb_dlist_t const* dlist);
tb_size_t 			tb_dlist_maxn(tb_dlist_t const* dlist);

/*!walk
 *
 * be faster than the iterator mode, optimizate to remove items for walking
 *
 * @code
 * tb_bool_t tb_dlist_item_func(tb_dlist_t* dlist, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
 * {
 * 		tb_assert_and_check_return_val(dlist && bdel, TB_FALSE);
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
tb_void_t 			tb_dlist_walk(tb_dlist_t* dlist, tb_bool_t (*func)(tb_dlist_t* dlist, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data);

#endif

