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
 * @file		slist.c
 * @ingroup 	container
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "slist.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the slist item type
typedef struct __tb_slist_item_t
{
	// the item next
	tb_size_t 			next;

}tb_slist_item_t;

/*! the single list type
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
typedef struct __tb_slist_impl_t
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

}tb_slist_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_slist_iterator_head(tb_iterator_t* iterator)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)iterator;
	tb_assert_and_check_return_val(slist, 0);

	// head
	return slist->head;
}
static tb_size_t tb_slist_iterator_tail(tb_iterator_t* iterator)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)iterator;
	tb_assert_and_check_return_val(slist, 0);

	// tail
	return 0;
}
static tb_size_t tb_slist_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)iterator;
	tb_assert_and_check_return_val(slist && itor, 0);

	// next
	return ((tb_slist_item_t const*)itor)->next;
}
static tb_size_t tb_slist_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)iterator;
	tb_assert_and_check_return_val(slist, 0);

	// is tail?
	tb_size_t prev = 0;
	if (!itor) prev = slist->last;
	// is head?
	else if (itor == slist->head) prev = 0;
	// find it
	else
	{
		tb_size_t node = tb_slist_iterator_head(iterator);
		tb_size_t tail = tb_slist_iterator_tail(iterator);
		for (prev = node; node != tail && node != itor; prev = node, node = tb_slist_iterator_next(iterator, node)) ;
	}

	return prev;
}
static tb_pointer_t tb_slist_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)iterator;
	tb_assert_and_check_return_val(slist && itor, tb_null);

	// data
	return slist->func.data(&slist->func, &((tb_slist_item_t const*)itor)[1]);
}
static tb_void_t tb_slist_iterator_copy(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)iterator;
	tb_assert_and_check_return(slist && itor);

	// copy
	slist->func.copy(&slist->func, (tb_pointer_t)&((tb_slist_item_t const*)itor)[1], item);
}
static tb_long_t tb_slist_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)iterator;
	tb_assert_and_check_return_val(slist && slist->func.comp, 0);

	// comp
	return slist->func.comp(&slist->func, ltem, rtem);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_slist_attach_next(tb_slist_impl_t* slist, tb_size_t itor, tb_size_t node)
{
	// check
	tb_assert_and_check_return_val(slist && slist->pool && node, 0);

	// the prev node
	tb_size_t prev = itor;

	// init node, inode => 0
	tb_slist_item_t* pnode = (tb_slist_item_t*)node;
	pnode->next = 0;

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
			tb_slist_item_t* pprev = (tb_slist_item_t*)prev;
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
			tb_slist_item_t* pprev = (tb_slist_item_t*)prev;
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
static tb_size_t tb_slist_detach_next(tb_slist_impl_t* slist, tb_size_t itor)
{
	// check
	tb_assert_and_check_return_val(slist && slist->pool, 0);

	// non-empty?
	tb_check_return_val(slist->head, 0);

	// the prev node
	tb_size_t prev = itor;

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
		else slist->head = next = tb_iterator_next((tb_iterator_t*)slist, midd);
	}
	// remove body?
	else
	{
		// the midd node
		midd = tb_iterator_next((tb_iterator_t*)slist, prev);

		// get the prev data
		tb_slist_item_t* pprev = (tb_slist_item_t*)prev;
		tb_assert_and_check_return_val(pprev, 0);

		// the next node
		next = tb_iterator_next((tb_iterator_t*)slist, midd);

		// prev => next
		pprev->next = next;

		// update last if midd is last
		if (midd == slist->last) slist->last = next = prev;
	}

	// ok?
	return midd;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_slist_t* tb_slist_init(tb_size_t grow, tb_item_func_t func)
{
	// check
	tb_assert_and_check_return_val(grow, tb_null);
	tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.repl, tb_null);

	// alloc slist
	tb_slist_impl_t* slist = (tb_slist_impl_t*)tb_malloc0(sizeof(tb_slist_impl_t));
	tb_assert_and_check_return_val(slist, tb_null);

	// init slist
	slist->head = 0;
	slist->last = 0;
	slist->func = func;

	// init iterator
	slist->itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
	slist->itor.size = 0;
	slist->itor.priv = tb_null;
	slist->itor.step = func.size;
	slist->itor.head = tb_slist_iterator_head;
	slist->itor.tail = tb_slist_iterator_tail;
	slist->itor.prev = tb_slist_iterator_prev;
	slist->itor.next = tb_slist_iterator_next;
	slist->itor.item = tb_slist_iterator_item;
	slist->itor.copy = tb_slist_iterator_copy;
	slist->itor.comp = tb_slist_iterator_comp;

	// init pool, step = next + data
	slist->pool = tb_fixed_pool_init(grow, sizeof(tb_slist_item_t) + func.size, 0);
	tb_assert_and_check_goto(slist->pool, fail);

	// ok
	return slist;
fail:
	if (slist) tb_slist_exit(slist);
	return tb_null;
}

tb_void_t tb_slist_exit(tb_slist_t* handle)
{
	tb_slist_impl_t* slist = (tb_slist_impl_t*)handle;
	if (slist)
	{
		// clear data
		tb_slist_clear(slist);

		// free pool
		if (slist->pool) tb_fixed_pool_exit(slist->pool);

		// free it
		tb_free(slist);
	}
}
tb_void_t tb_slist_clear(tb_slist_t* handle)
{
	tb_slist_impl_t* slist = (tb_slist_impl_t*)handle;
	if (slist) 
	{
		// free items
		if (slist->func.free)
		{
			tb_size_t itor = slist->head;
			while (itor)
			{
				// item
				tb_slist_item_t* item = (tb_slist_item_t*)itor;

				// free 
				slist->func.free(&slist->func, &item[1]);
		
				// next
				itor = item->next;
			}
		}

		// clear pool
		if (slist->pool) tb_fixed_pool_clear(slist->pool);

		// reset it
		slist->head = 0;
		slist->last = 0;
	}
}
tb_pointer_t tb_slist_head(tb_slist_t const* handle)
{
	return tb_iterator_item((tb_iterator_t*)handle, tb_iterator_head((tb_iterator_t*)handle));
}
tb_pointer_t tb_slist_last(tb_slist_t const* handle)
{
	return tb_iterator_item((tb_iterator_t*)handle, tb_iterator_last((tb_iterator_t*)handle));
}
tb_size_t tb_slist_size(tb_slist_t const* handle)
{
	tb_slist_impl_t const* slist = (tb_slist_impl_t const*)handle;
	tb_assert_and_check_return_val(slist && slist->pool, 0);
	return tb_fixed_pool_size(slist->pool);
}
tb_size_t tb_slist_maxn(tb_slist_t const* handle)
{
	tb_slist_impl_t const* slist = (tb_slist_impl_t const*)handle;
	tb_assert_and_check_return_val(slist, 0);
	return TB_MAXU32;
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
tb_size_t tb_slist_insert_prev(tb_slist_t* handle, tb_size_t itor, tb_cpointer_t data)
{
	return tb_slist_insert_next(handle, tb_iterator_prev(handle, itor), data);
}
tb_size_t tb_slist_insert_next(tb_slist_t* handle, tb_size_t itor, tb_cpointer_t data)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)handle;
	tb_assert_and_check_return_val(slist && slist->pool, 0);

	// make the node data
	tb_slist_item_t* pnode = tb_fixed_pool_malloc(slist->pool);
	tb_assert_and_check_return_val(pnode, 0);

	// init node, inode => 0
	pnode->next = 0;

	// init data
	slist->func.dupl(&slist->func, &pnode[1], data);

	// attach next
	return tb_slist_attach_next(slist, itor, (tb_size_t)pnode);
}
tb_size_t tb_slist_insert_head(tb_slist_t* handle, tb_cpointer_t data)
{
	return tb_slist_insert_prev(handle, tb_iterator_head(handle), data);
}
tb_size_t tb_slist_insert_tail(tb_slist_t* handle, tb_cpointer_t data)
{
	return tb_slist_insert_prev(handle, tb_iterator_tail(handle), data);
}
tb_size_t tb_slist_ninsert_prev(tb_slist_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	return tb_slist_ninsert_next(handle, tb_iterator_prev(handle, itor), data, size);
}
tb_size_t tb_slist_ninsert_next(tb_slist_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)handle;
	tb_assert_and_check_return_val(slist && size, 0);

	// insert items
	tb_size_t node = itor;
	while (size--) node = tb_slist_insert_next(slist, itor, data);

	// return the first itor
	return node;
}
tb_size_t tb_slist_ninsert_head(tb_slist_t* handle, tb_cpointer_t data, tb_size_t size)
{
	return tb_slist_ninsert_prev(handle, tb_iterator_head(handle), data, size);
}
tb_size_t tb_slist_ninsert_tail(tb_slist_t* handle, tb_cpointer_t data, tb_size_t size)
{
	return tb_slist_ninsert_prev(handle, tb_iterator_tail(handle), data, size);
}
tb_size_t tb_slist_replace(tb_slist_t* handle, tb_size_t itor, tb_cpointer_t data)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)handle;
	tb_assert_and_check_return_val(slist && itor, itor);

	// the item
	tb_slist_item_t* item = (tb_slist_item_t*)itor;

	// replace data
	slist->func.repl(&slist->func, &item[1], data);

	// ok
	return itor;
}
tb_size_t tb_slist_replace_head(tb_slist_t* handle, tb_cpointer_t data)
{
	return tb_slist_replace(handle, tb_iterator_head(handle), data);
}
tb_size_t tb_slist_replace_last(tb_slist_t* handle, tb_cpointer_t data)
{
	return tb_slist_replace(handle, tb_iterator_last(handle), data);
}
tb_size_t tb_slist_nreplace(tb_slist_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)handle;
	tb_assert_and_check_return_val(slist && size, itor);

	tb_size_t head = itor;
	tb_size_t tail = tb_iterator_tail((tb_iterator_t*)slist);
	for (; size-- && itor != tail; itor = tb_iterator_next((tb_iterator_t*)slist, itor)) 
		tb_slist_replace(slist, itor, data);
	return head;
}
tb_size_t tb_slist_nreplace_head(tb_slist_t* handle, tb_cpointer_t data, tb_size_t size)
{
	return tb_slist_nreplace(handle, tb_iterator_head(handle), data, size);
}
tb_size_t tb_slist_nreplace_last(tb_slist_t* handle, tb_cpointer_t data, tb_size_t size)
{
	// compute offset
	tb_size_t n = tb_slist_size(handle);
	tb_size_t o = n > size? n - size : 0;
	
	// seek
	tb_size_t itor = tb_iterator_head(handle);
	tb_size_t tail = tb_iterator_tail(handle);
	for (; o-- && itor != tail; itor = tb_iterator_next(handle, itor)) ;

	// replace
	return tb_slist_nreplace(handle, itor, data, size);
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
tb_size_t tb_slist_remove(tb_slist_t* handle, tb_size_t itor)
{	
	return tb_slist_remove_next(handle, tb_iterator_prev(handle, itor));
}
tb_size_t tb_slist_remove_next(tb_slist_t* handle, tb_size_t itor)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)handle;
	tb_assert_and_check_return_val(slist && slist->pool, itor);

	// detach next
	tb_size_t node = tb_slist_detach_next(slist, itor);
	tb_assert_and_check_return_val(node, itor);

	// next item
	tb_size_t next = tb_iterator_next((tb_iterator_t*)slist, node);

	// free item
	if (slist->func.free)
		slist->func.free(&slist->func, &((tb_slist_item_t*)node)[1]);

	// free node
	tb_fixed_pool_free(slist->pool, (tb_pointer_t)node);

	// return next node
	return next;
}
tb_size_t tb_slist_remove_head(tb_slist_t* handle)
{
	return tb_slist_remove(handle, tb_iterator_head(handle));
}
tb_size_t tb_slist_remove_last(tb_slist_t* handle)
{
	return tb_slist_remove(handle, tb_iterator_last(handle));
}
tb_size_t tb_slist_nremove(tb_slist_t* handle, tb_size_t itor, tb_size_t size)
{
	return tb_slist_nremove_next(handle, tb_iterator_prev(handle, itor), size);
}
tb_size_t tb_slist_nremove_next(tb_slist_t* handle, tb_size_t itor, tb_size_t size)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)handle;
	tb_assert_and_check_return_val(slist && size, itor);

	tb_size_t next = itor;
	while (size--) next = tb_slist_remove_next(slist, itor);
	return next;
}
tb_size_t tb_slist_nremove_head(tb_slist_t* handle, tb_size_t size)
{
	tb_slist_nremove(handle, tb_iterator_head(handle), size);
	return tb_iterator_head(handle);
}
tb_size_t tb_slist_nremove_last(tb_slist_t* handle, tb_size_t size)
{
	// compute offset
	tb_size_t n = tb_slist_size(handle);
	tb_size_t o = n > size? n - size : 0;
	
	// seek
	tb_size_t prev = 0;
	tb_size_t itor = tb_iterator_head(handle);
	tb_size_t tail = tb_iterator_tail(handle);
	for (; o-- && itor != tail; prev = itor, itor = tb_iterator_next(handle, itor)) ;

	// remove
	tb_slist_nremove_next(handle, prev, size);

	// ok?
	return tb_iterator_last(handle);
}
tb_size_t tb_slist_moveto_prev(tb_slist_t* handle, tb_size_t itor, tb_size_t move)
{
	return tb_slist_moveto_next(handle, tb_iterator_prev(handle, itor), move);
}
tb_size_t tb_slist_moveto_next(tb_slist_t* handle, tb_size_t itor, tb_size_t move)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)handle;
	tb_assert_and_check_return_val(slist && slist->pool && move, move);

	// detach move
	tb_size_t node = tb_slist_detach_next(slist, tb_iterator_prev((tb_iterator_t*)slist, move));
	tb_assert_and_check_return_val(node && node == move, move);

	// attach move to next
	return tb_slist_attach_next(slist, itor, node);
}
tb_size_t tb_slist_moveto_head(tb_slist_t* handle, tb_size_t move)
{
	return tb_slist_moveto_prev(handle, tb_iterator_head(handle), move);
}
tb_size_t tb_slist_moveto_tail(tb_slist_t* handle, tb_size_t move)
{
	return tb_slist_moveto_prev(handle, tb_iterator_tail(handle), move);
}
tb_void_t tb_slist_walk(tb_slist_t* handle, tb_bool_t (*func)(tb_slist_t* slist, tb_pointer_t item, tb_bool_t* bdel, tb_pointer_t priv), tb_pointer_t priv)
{
	// check
	tb_slist_impl_t* slist = (tb_slist_impl_t*)handle;
	tb_assert_and_check_return(slist && slist->pool && func);

	// pool
	tb_handle_t pool = slist->pool;

	// step
	tb_size_t 	step = slist->func.size;
	tb_assert_and_check_return(step);

	// walk
	tb_size_t 	base = -1;
	tb_size_t 	prev = 0;
	tb_bool_t 	bdel = tb_false;
	tb_size_t 	itor = slist->head;
	tb_bool_t 	stop = tb_false;
	while (itor)
	{
		// node
		tb_slist_item_t* node = (tb_slist_item_t*)itor;

		// item
		tb_pointer_t item = slist->func.data(&slist->func, &node[1]);

		// next
		tb_size_t next = node->next;
	
		// bdel
		bdel = tb_false;

		// callback: item
		if (!func(slist, item, &bdel, priv)) stop = tb_true;

		// free it?
		if (bdel)
		{
			// save
			if (base == -1) base = prev;

			// free item
			if (slist->func.free)
				slist->func.free(&slist->func, &((tb_slist_item_t*)itor)[1]);

			// free item
			tb_fixed_pool_free(pool, (tb_pointer_t)itor);
		}
		
		// remove items?
		if (!bdel || !next || stop)
		{
			// has deleted items?
			if (base != -1)
			{
				// remove part
				if (base)
				{
					// base => next
					((tb_slist_item_t*)base)->next = next;

					// update last
					if (!next) slist->last = base;
				}
				// remove all
				else
				{
					slist->head = 0;
					slist->last = 0;
				}
			}

			// reset
			base = -1;

			// stop?
			tb_check_goto(!stop, end);
		}
	
		// next
		prev = itor;
		itor = next;
	}

end:
	return ;
}
