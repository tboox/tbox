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
 * \file		slist.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "slist.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the slist item type
typedef struct __tb_slist_item_t
{
	// the item next
	tb_size_t 			next;

}tb_slist_item_t;

/* /////////////////////////////////////////////////////////
 * details
 */

static tb_void_t tb_slist_item_free(tb_pointer_t item, tb_pointer_t priv)
{
	tb_slist_t* slist = priv;
	if (slist && slist->func.free && item)
		slist->func.free(&slist->func, &((tb_slist_item_t*)item)[1]);
}

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_slist_t* tb_slist_init(tb_size_t grow, tb_item_func_t func)
{
	// check
	tb_assert_and_check_return_val(grow, TB_NULL);
	tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.copy, TB_NULL);

	// alloc slist
	tb_slist_t* slist = (tb_slist_t*)tb_calloc(1, sizeof(tb_slist_t));
	tb_assert_and_check_return_val(slist, TB_NULL);

	// init slist
	slist->head = 0;
	slist->last = 0;
	slist->func = func;

	// init pool, step = next + data
	tb_fpool_item_func_t pool_func;
	pool_func.free = tb_slist_item_free;
	pool_func.priv = slist;
	slist->pool = tb_fpool_init(sizeof(tb_slist_item_t) + func.size, grow, grow, &pool_func);
	tb_assert_and_check_goto(slist->pool, fail);

	return slist;
fail:
	if (slist) tb_slist_exit(slist);
	return TB_NULL;
}

tb_void_t tb_slist_exit(tb_slist_t* slist)
{
	if (slist)
	{
		// clear data
		tb_slist_clear(slist);

		// free pool
		if (slist->pool) tb_fpool_exit(slist->pool);

		// free it
		tb_free(slist);
	}
}
tb_void_t tb_slist_clear(tb_slist_t* slist)
{
	if (slist) 
	{
		// clear pool
		if (slist->pool) tb_fpool_clear(slist->pool);

		// reset it
		slist->head = 0;
		slist->last = 0;
	}
}
tb_pointer_t tb_slist_itor_at(tb_slist_t* slist, tb_size_t itor)
{
	return (tb_pointer_t)tb_slist_itor_const_at(slist, itor);
}
tb_pointer_t tb_slist_at_head(tb_slist_t* slist)
{
	return tb_slist_itor_at(slist, tb_slist_itor_head(slist));
}
tb_pointer_t tb_slist_at_last(tb_slist_t* slist)
{
	return tb_slist_itor_at(slist, tb_slist_itor_last(slist));
}
tb_cpointer_t tb_slist_itor_const_at(tb_slist_t const* slist, tb_size_t itor)
{
	tb_assert_and_check_return_val(slist && slist->pool, TB_NULL);
	tb_slist_item_t const* item = tb_fpool_get(slist->pool, itor);
	return item? slist->func.data(&slist->func, &item[1]) : TB_NULL;
}
tb_cpointer_t tb_slist_const_at_head(tb_slist_t const* slist)
{
	return tb_slist_itor_const_at(slist, tb_slist_itor_head(slist));
}
tb_cpointer_t tb_slist_const_at_last(tb_slist_t const* slist)
{
	return tb_slist_itor_const_at(slist, tb_slist_itor_last(slist));
}
tb_size_t tb_slist_itor_head(tb_slist_t const* slist)
{
	tb_assert_and_check_return_val(slist, 0);
	return slist->head;
}
tb_size_t tb_slist_itor_last(tb_slist_t const* slist)
{
	tb_assert_and_check_return_val(slist, 0);
	return slist->last;
}
tb_size_t tb_slist_itor_tail(tb_slist_t const* slist)
{
	return 0;
}
tb_size_t tb_slist_itor_next(tb_slist_t const* slist, tb_size_t itor)
{
	tb_assert_and_check_return_val(slist && slist->pool, 0);
	tb_slist_item_t const* item = tb_fpool_get(slist->pool, itor);
	return item? item->next : 0;
}
tb_size_t tb_slist_itor_prev(tb_slist_t const* slist, tb_size_t itor)
{
	tb_assert_and_check_return_val(slist, 0);

	// is tail?
	tb_size_t prev = 0;
	if (!itor) prev = slist->last;
	// is head?
	else if (itor == slist->head) prev = 0;
	// find it
	else
	{
		tb_size_t node = tb_slist_itor_head(slist);
		tb_size_t tail = tb_slist_itor_tail(slist);
		for (prev = node; node != tail && node != itor; prev = node, node = tb_slist_itor_next(slist, node)) ;
	}

	return prev;
}
tb_size_t tb_slist_size(tb_slist_t const* slist)
{
	tb_assert_and_check_return_val(slist && slist->pool, 0);
	return tb_fpool_size(slist->pool);
}
tb_size_t tb_slist_maxn(tb_slist_t const* slist)
{
	tb_assert_and_check_return_val(slist && slist->pool, 0);
	return tb_fpool_maxn(slist->pool);
}
/* insert node:
 *
 * itor == a: 
 * before: [a] => ... => [b] => [0]
 *         head         last   tail
 *
 * after:  [node] => [a] => ... => [b] => [0]
 *          head                  last    tail
 *
 * itor == b: 
 * before: [a] => ... => [b] => [c] => [0]
 *         head                last   tail
 *
 * after:  [a] => ... => [node] => [b] => [c] => [0]
 *         head                          last   tail
 *
 * itor == 0: 
 * before: [a] => ... => [b] => [0]
 *         head         last   tail
 *
 * after:  [a] => ... => [b] => [node] => [0]
 *         head                  last    tail
 *
 */
tb_size_t tb_slist_insert(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data)
{
	return tb_slist_insert_next(slist, tb_slist_itor_prev(slist, itor), data);
}

tb_size_t tb_slist_insert_next(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data)
{
	tb_assert_and_check_return_val(slist && slist->pool, 0);

	// alloc a new node
	tb_size_t node = tb_fpool_put(slist->pool, TB_NULL);
	tb_assert_and_check_return_val(node, 0);

	// get the node data
	tb_slist_item_t* pnode = tb_fpool_get(slist->pool, node);
	tb_assert_and_check_return_val(pnode, 0);

	// init node, inode => 0
	pnode->next = 0;
	slist->func.dupl(&slist->func, &pnode[1], data);

	// the prev node
	tb_size_t prev = itor;

	// non-empty?
	if (slist->head)
	{
		// is head?
		if (!prev)
		{
			// node => head
			pnode->next = slist->head;

			// update head
			slist->head = node;
		}
		// is last?
		else if (prev == slist->last)
		{
			// the prev data
			tb_slist_item_t* pprev = tb_fpool_get(slist->pool, prev);
			tb_assert_and_check_return_val(pprev, 0);

			// last => node => null
			pprev->next = node;

			// update last
			slist->last = node;
		}
		// is body?
		else
		{
			// the prev data
			tb_slist_item_t* pprev = tb_fpool_get(slist->pool, prev);
			tb_assert_and_check_return_val(pprev, 0);

			// node => next
			pnode->next = pprev->next;

			// prev => node
			pprev->next = node;
		}
	}
	// empty?
	else
	{
		// must be zero
		tb_assert_and_check_return_val(!prev, 0);

		// update head
		slist->head = node;

		// update last
		slist->last = node;
	}

	// return the new node
	return node;
}

tb_size_t tb_slist_insert_head(tb_slist_t* slist, tb_cpointer_t data)
{
	return tb_slist_insert(slist, tb_slist_itor_head(slist), data);
}
tb_size_t tb_slist_insert_tail(tb_slist_t* slist, tb_cpointer_t data)
{
	return tb_slist_insert(slist, tb_slist_itor_tail(slist), data);
}
tb_size_t tb_slist_ninsert(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	return tb_slist_ninsert_next(slist, tb_slist_itor_prev(slist, itor), data, size);
}
tb_size_t tb_slist_ninsert_next(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return_val(slist && size, 0);

	// insert items
	tb_size_t node = itor;
	while (size--) node = tb_slist_insert_next(slist, itor, data);

	// return the first itor
	return node;
}
tb_size_t tb_slist_ninsert_head(tb_slist_t* slist, tb_cpointer_t data, tb_size_t size)
{
	return tb_slist_ninsert(slist, tb_slist_itor_head(slist), data, size);
}
tb_size_t tb_slist_ninsert_tail(tb_slist_t* slist, tb_cpointer_t data, tb_size_t size)
{
	return tb_slist_ninsert(slist, tb_slist_itor_tail(slist), data, size);
}
tb_size_t tb_slist_replace(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data)
{
	tb_assert_and_check_return_val(slist && slist->pool, itor);

	// get item
	tb_slist_item_t* item = tb_fpool_get(slist->pool, itor);
	tb_assert_and_check_return_val(item, itor);

	// copy data to item
	slist->func.copy(&slist->func, &item[1], data);

	return itor;
}
tb_size_t tb_slist_replace_head(tb_slist_t* slist, tb_cpointer_t data)
{
	return tb_slist_replace(slist, tb_slist_itor_head(slist), data);
}
tb_size_t tb_slist_replace_last(tb_slist_t* slist, tb_cpointer_t data)
{
	return tb_slist_replace(slist, tb_slist_itor_last(slist), data);
}
tb_size_t tb_slist_nreplace(tb_slist_t* slist, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return_val(slist && size, itor);

	tb_size_t head = itor;
	tb_size_t tail = tb_slist_itor_tail(slist);
	for (; size-- && itor != tail; itor = tb_slist_itor_next(slist, itor)) 
		tb_slist_replace(slist, itor, data);
	return head;
}
tb_size_t tb_slist_nreplace_head(tb_slist_t* slist, tb_cpointer_t data, tb_size_t size)
{
	return tb_slist_nreplace(slist, tb_slist_itor_head(slist), data, size);
}
tb_size_t tb_slist_nreplace_last(tb_slist_t* slist, tb_cpointer_t data, tb_size_t size)
{
	// compute offset
	tb_size_t n = tb_slist_size(slist);
	tb_size_t o = n > size? n - size : 0;
	
	// seek
	tb_size_t itor = tb_slist_itor_head(slist);
	tb_size_t tail = tb_slist_itor_tail(slist);
	for (; o-- && itor != tail; itor = tb_slist_itor_next(slist, itor)) ;

	// replace
	return tb_slist_nreplace(slist, itor, data, size);
}
/* remove node:
 *
 * itor == node: 
 * before: [node] => [a] => ... => [b] => [0]
 *          head                  last    tail

 * after:  [a] => ... => [b] => [0]
 *         head         last   tail
 *
 *
 * itor == node: 
 * before: [a] => ... => [node] => [b] => [c] => [0]
 *         head                          last   tail
 * after:  [a] => ... => [b] => [c] => [0]
 *         head                last   tail
 *
 * itor == node: 
 * before: [a] => ... => [b] => [node] => [0]
 *         head                  last    tail
 * after:  [a] => ... => [b] => [0]
 *         head         last   tail
 *
 *
 */
tb_size_t tb_slist_remove(tb_slist_t* slist, tb_size_t itor)
{	
	return tb_slist_remove_next(slist, tb_slist_itor_prev(slist, itor));
}
tb_size_t tb_slist_remove_next(tb_slist_t* slist, tb_size_t itor)
{
	tb_assert_and_check_return_val(slist && slist->pool, itor);

	// the prev node
	tb_size_t prev = itor;

	// non-empty?
	if (slist->head)
	{
		// the next node
		tb_size_t next = 0;

		// the midd node
		tb_size_t midd = 0;
		
		// remove head?
		if (!prev)
		{
			// the midd node
			midd = slist->head;

			// only one item?
			if (midd == slist->last)
			{
				slist->head = 0;
				slist->last = 0;
			}
			// update head
			else slist->head = next = tb_slist_itor_next(slist, midd);
		}
		// remove body?
		else
		{
			// the midd node
			midd = tb_slist_itor_next(slist, prev);

			// get the prev data
			tb_slist_item_t* pprev = tb_fpool_get(slist->pool, prev);
			tb_assert_and_check_return_val(pprev, 0);

			// the next node
			next = tb_slist_itor_next(slist, midd);

			// prev => next
			pprev->next = next;

			// update last if midd is last
			if (midd == slist->last) slist->last = next = prev;
		}

		// check
		tb_assert_and_check_return_val(midd, 0);

		// free node
		tb_fpool_del(slist->pool, midd);

		// return next node
		return next;
	}

	return itor;
}
tb_size_t tb_slist_remove_head(tb_slist_t* slist)
{
	return tb_slist_remove(slist, tb_slist_itor_head(slist));
}
tb_size_t tb_slist_remove_last(tb_slist_t* slist)
{
	return tb_slist_remove(slist, tb_slist_itor_last(slist));
}
tb_size_t tb_slist_nremove(tb_slist_t* slist, tb_size_t itor, tb_size_t size)
{
	return tb_slist_nremove_next(slist, tb_slist_itor_prev(slist, itor), size);
}
tb_size_t tb_slist_nremove_next(tb_slist_t* slist, tb_size_t itor, tb_size_t size)
{
	tb_assert_and_check_return_val(slist && size, itor);

	tb_size_t next = itor;
	while (size--) next = tb_slist_remove_next(slist, itor);
	return next;
}
tb_size_t tb_slist_nremove_head(tb_slist_t* slist, tb_size_t size)
{
	tb_slist_nremove(slist, tb_slist_itor_head(slist), size);
	return tb_slist_itor_head(slist);
}
tb_size_t tb_slist_nremove_last(tb_slist_t* slist, tb_size_t size)
{
	// compute offset
	tb_size_t n = tb_slist_size(slist);
	tb_size_t o = n > size? n - size : 0;
	
	// seek
	tb_size_t prev = 0;
	tb_size_t itor = tb_slist_itor_head(slist);
	tb_size_t tail = tb_slist_itor_tail(slist);
	for (; o-- && itor != tail; prev = itor, itor = tb_slist_itor_next(slist, itor)) ;

	// remove
	tb_slist_nremove_next(slist, prev, size);

	return tb_slist_itor_last(slist);
}
