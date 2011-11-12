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
 * \file		dlist.h
 *
 */
#ifndef TB_CONTAINER_DLIST_H
#define TB_CONTAINER_DLIST_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "fpool.h"
#include "item.h"

/* /////////////////////////////////////////////////////////
 * types
 */

/* the double list type
 *
 *
 * dlist: 0 => |-----| => |-------------------------------------------------=> |------| => |------| => 0
 *       tail   head                                                                         last     tail
 *        |                                                                                            |
 *        ---------------------------------------------------------------------------------------------
 *
 * head: => the first item
 * last: => the last item
 * tail: => behind the last item, no item

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
 *
 */
typedef struct __tb_dlist_t
{
	// the pool
	tb_fpool_t* 			pool;

	// the head item
	tb_size_t 				head;

	// the last item
	tb_size_t 				last;

	// the func
	tb_item_func_t 			func;

}tb_dlist_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_dlist_t* 		tb_dlist_init(tb_size_t grow, tb_item_func_t func);
tb_void_t 			tb_dlist_exit(tb_dlist_t* dlist);

// accessors
tb_void_t* 			tb_dlist_at_head(tb_dlist_t* dlist);
tb_void_t* 			tb_dlist_at_last(tb_dlist_t* dlist);

tb_void_t const* 	tb_dlist_const_at_head(tb_dlist_t const* dlist);
tb_void_t const* 	tb_dlist_const_at_last(tb_dlist_t const* dlist);

// modifiors
tb_void_t 			tb_dlist_clear(tb_dlist_t* dlist);

tb_size_t 			tb_dlist_insert(tb_dlist_t* dlist, tb_size_t itor, tb_void_t const* data);
tb_size_t 			tb_dlist_insert_head(tb_dlist_t* dlist, tb_void_t const* data);
tb_size_t 			tb_dlist_insert_tail(tb_dlist_t* dlist, tb_void_t const* data);

tb_size_t 			tb_dlist_ninsert(tb_dlist_t* dlist, tb_size_t itor, tb_void_t const* data, tb_size_t size);
tb_size_t 			tb_dlist_ninsert_head(tb_dlist_t* dlist, tb_void_t const* data, tb_size_t size);
tb_size_t 			tb_dlist_ninsert_tail(tb_dlist_t* dlist, tb_void_t const* data, tb_size_t size);

tb_size_t 			tb_dlist_replace(tb_dlist_t* dlist, tb_size_t itor, tb_void_t const* data);
tb_size_t 			tb_dlist_replace_head(tb_dlist_t* dlist, tb_void_t const* data);
tb_size_t 			tb_dlist_replace_last(tb_dlist_t* dlist, tb_void_t const* data);

tb_size_t 			tb_dlist_nreplace(tb_dlist_t* dlist, tb_size_t itor, tb_void_t const* data, tb_size_t size);
tb_size_t 			tb_dlist_nreplace_head(tb_dlist_t* dlist, tb_void_t const* data, tb_size_t size);
tb_size_t 			tb_dlist_nreplace_last(tb_dlist_t* dlist, tb_void_t const* data, tb_size_t size);

tb_size_t 			tb_dlist_remove(tb_dlist_t* dlist, tb_size_t itor);
tb_size_t 			tb_dlist_remove_head(tb_dlist_t* dlist);
tb_size_t 			tb_dlist_remove_last(tb_dlist_t* dlist);

tb_size_t 			tb_dlist_nremove(tb_dlist_t* dlist, tb_size_t itor, tb_size_t size);
tb_size_t 			tb_dlist_nremove_head(tb_dlist_t* dlist, tb_size_t size);
tb_size_t 			tb_dlist_nremove_last(tb_dlist_t* dlist, tb_size_t size);

/* iterator
 * 
 * tb_size_t itor = tb_dlist_itor_head(dlist);
 * tb_size_t tail = tb_dlist_itor_tail(dlist);
 * for (; itor != tail; itor = tb_dlist_itor_next(dlist, itor))
 * {
 * 		tb_byte_t const* item = tb_dlist_itor_const_at(dlist, itor);
 * 		if (item)
 * 		{
 * 			// ...
 * 		}
 * }
 *
 */
tb_void_t* 			tb_dlist_itor_at(tb_dlist_t* dlist, tb_size_t itor);
tb_void_t const* 	tb_dlist_itor_const_at(tb_dlist_t const* dlist, tb_size_t itor);

tb_size_t 			tb_dlist_itor_head(tb_dlist_t const* dlist);
tb_size_t 			tb_dlist_itor_tail(tb_dlist_t const* dlist);
tb_size_t 			tb_dlist_itor_last(tb_dlist_t const* dlist);
tb_size_t 			tb_dlist_itor_next(tb_dlist_t const* dlist, tb_size_t itor);
tb_size_t 			tb_dlist_itor_prev(tb_dlist_t const* dlist, tb_size_t itor);

// attributes
tb_size_t 			tb_dlist_size(tb_dlist_t const* dlist);
tb_size_t 			tb_dlist_maxn(tb_dlist_t const* dlist);


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

