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
#include "pool.h"

/* /////////////////////////////////////////////////////////
 * types
 */

/* the single list type
 *
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
	tb_pool_t* 		pool;

	// the head item 
	tb_size_t 		head;

	// the last item
	tb_size_t 		last;

	// the step size
	tb_size_t 		step;

	// free
	tb_void_t 		(*free)(tb_void_t* data, tb_void_t* priv);
	tb_void_t* 		priv;

}tb_slist_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_slist_t* 		tb_slist_init(tb_size_t step, tb_size_t grow, tb_void_t (*free)(tb_void_t* , tb_void_t* ), tb_void_t* priv);
tb_void_t 			tb_slist_exit(tb_slist_t* slist);

// accessors
tb_byte_t* 			tb_slist_at(tb_slist_t* slist, tb_size_t index);
tb_byte_t* 			tb_slist_at_head(tb_slist_t* slist);
tb_byte_t* 			tb_slist_at_last(tb_slist_t* slist);

tb_byte_t const* 	tb_slist_const_at(tb_slist_t const* slist, tb_size_t index);
tb_byte_t const* 	tb_slist_const_at_head(tb_slist_t const* slist);
tb_byte_t const* 	tb_slist_const_at_last(tb_slist_t const* slist);

// modifiors
tb_void_t 			tb_slist_clear(tb_slist_t* slist);

tb_size_t 			tb_slist_insert(tb_slist_t* slist, tb_size_t index, tb_byte_t const* item);
tb_size_t 			tb_slist_insert_next(tb_slist_t* slist, tb_size_t index, tb_byte_t const* item);
tb_size_t 			tb_slist_insert_head(tb_slist_t* slist, tb_byte_t const* item);
tb_size_t 			tb_slist_insert_tail(tb_slist_t* slist, tb_byte_t const* item);

tb_size_t 			tb_slist_ninsert(tb_slist_t* slist, tb_size_t index, tb_byte_t const* item, tb_size_t size);
tb_size_t 			tb_slist_ninsert_next(tb_slist_t* slist, tb_size_t index, tb_byte_t const* item, tb_size_t size);
tb_size_t 			tb_slist_ninsert_head(tb_slist_t* slist, tb_byte_t const* item, tb_size_t size);
tb_size_t 			tb_slist_ninsert_tail(tb_slist_t* slist, tb_byte_t const* item, tb_size_t size);

tb_size_t 			tb_slist_replace(tb_slist_t* slist, tb_size_t index, tb_byte_t const* item);
tb_size_t 			tb_slist_replace_head(tb_slist_t* slist, tb_byte_t const* item);
tb_size_t 			tb_slist_replace_last(tb_slist_t* slist, tb_byte_t const* item);

tb_size_t 			tb_slist_nreplace(tb_slist_t* slist, tb_size_t index, tb_byte_t const* item, tb_size_t size);
tb_size_t 			tb_slist_nreplace_head(tb_slist_t* slist, tb_byte_t const* item, tb_size_t size);
tb_size_t 			tb_slist_nreplace_last(tb_slist_t* slist, tb_byte_t const* item, tb_size_t size);

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
 * tb_size_t itor = tb_slist_head(slist);
 * tb_size_t tail = tb_slist_tail(slist);
 * for (; itor != tail; itor = tb_slist_next(slist, itor))
 * {
 * 		tb_byte_t const* item = tb_slist_const_at(slist, itor);
 * 		if (item)
 * 		{
 * 			// ...
 * 		}
 * }
 *
 *
 * slist: |-----| => |-------------------------------------------------=> |------| => |------| => null
 *         head                                                                         last      tail
 *
 * head: => the first item
 * last: => the last item
 * tail: => behind the last item, no item
 */
tb_size_t 			tb_slist_head(tb_slist_t const* slist);
tb_size_t 			tb_slist_tail(tb_slist_t const* slist);
tb_size_t 			tb_slist_last(tb_slist_t const* slist);
tb_size_t 			tb_slist_size(tb_slist_t const* slist);
tb_size_t 			tb_slist_next(tb_slist_t const* slist, tb_size_t index);
tb_size_t 			tb_slist_prev(tb_slist_t const* slist, tb_size_t index);

// attributes
tb_size_t 			tb_slist_maxn(tb_slist_t const* slist);


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

