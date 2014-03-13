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
 * @file		dlist.c
 * @ingroup 	container
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "dlist.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the dlist item type
typedef struct __tb_dlist_item_t
{
	// the item next
	tb_size_t 			next;

	// the item prev
	tb_size_t 			prev;

}tb_dlist_item_t;

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
typedef struct __tb_dlist_impl_t
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

}tb_dlist_impl_t;

/* ///////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_dlist_iterator_head(tb_iterator_t* iterator)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)iterator;
	tb_assert_and_check_return_val(dlist, 0);

	// head
	return dlist->head;
}
static tb_size_t tb_dlist_iterator_tail(tb_iterator_t* iterator)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)iterator;
	tb_assert_and_check_return_val(dlist, 0);

	// tail
	return 0;
}
static tb_size_t tb_dlist_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)iterator;
	tb_assert_and_check_return_val(dlist, 0);

	// next
	if (!itor) return dlist->head;
	else return ((tb_dlist_item_t const*)itor)->next;
}
static tb_size_t tb_dlist_iterator_prev(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)iterator;
	tb_assert_and_check_return_val(dlist, 0);

	// prev
	if (!itor) return dlist->last;
	else return ((tb_dlist_item_t const*)itor)->prev;
}
static tb_pointer_t tb_dlist_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)iterator;
	tb_assert_and_check_return_val(dlist && itor, tb_null);

	// data
	return dlist->func.data(&dlist->func, &((tb_dlist_item_t const*)itor)[1]);
}
static tb_void_t tb_dlist_iterator_copy(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t item)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)iterator;
	tb_assert_and_check_return(dlist && itor);

	// copy
	dlist->func.copy(&dlist->func, &((tb_dlist_item_t const*)itor)[1], item);
}
static tb_long_t tb_dlist_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)iterator;
	tb_assert_and_check_return_val(dlist && dlist->func.comp, 0);

	// comp
	return dlist->func.comp(&dlist->func, ltem, rtem);
}

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_dlist_attach_prev(tb_dlist_impl_t* dlist, tb_size_t itor, tb_size_t node)
{
	// check
	tb_dlist_item_t* pnode = (tb_dlist_item_t*)node;
	tb_assert_and_check_return_val(dlist && dlist->pool && pnode, 0);

	// init node
	pnode->prev = 0;
	pnode->next = 0;

	// is null?
	if (!dlist->head && !dlist->last)
	{
		/* dlist: 0 => node => 0
		 *       tail  head   tail
		 *             last
		 */
		dlist->head = node;
		dlist->last = node;
	}
	else
	{
		// check
		tb_assert_and_check_return_val(dlist->head && dlist->last, 0);

		// insert to tail
		if (!itor)
		{
			// the last node
			tb_size_t last = dlist->last;
		
			// the last data
			tb_dlist_item_t* plast = (tb_dlist_item_t*)last;
			tb_assert_and_check_return_val(plast, 0);

			// last <=> node <=> 0
			plast->next = node;
			pnode->prev = last;

			// update the last node
			dlist->last = node;
		}
		// insert to head
		else if (itor == dlist->head)
		{
			// the head node
			tb_size_t head = dlist->head;
		
			// the head data
			tb_dlist_item_t* phead = (tb_dlist_item_t*)head;
			tb_assert_and_check_return_val(phead, 0);

			// 0 <=> node <=> head
			phead->prev = node;
			pnode->next = head;

			// update the head node
			dlist->head = node;
		}
		// insert to body
		else
		{
			// the body node
			tb_size_t body = itor;
		
			// the body data
			tb_dlist_item_t* pbody = (tb_dlist_item_t*)body;
			tb_assert_and_check_return_val(pbody, 0);

			// the prev node 
			tb_size_t prev = pbody->prev;

			// the prev data
			tb_dlist_item_t* pprev = (tb_dlist_item_t*)prev;
			tb_assert_and_check_return_val(pprev, 0);

			/* 0 <=> ... <=> prev <=> body <=> ... <=> 0
			 * 0 <=> ... <=> prev <=> node <=> body <=> ... <=> 0
			 */
			pnode->next = body;
			pnode->prev = prev;
			pprev->next = node;
			pbody->prev = node;
		}
	}

	// return the new node
	return node;
}
static tb_size_t tb_dlist_detach_item(tb_dlist_impl_t* dlist, tb_size_t itor)
{
	// check
	tb_assert_and_check_return_val(dlist && dlist->pool && itor, 0);

	// not empty?
	tb_check_return_val(dlist->head && dlist->last, 0);

	// only one?
	if (dlist->head == dlist->last)
	{
		tb_assert_and_check_return_val(dlist->head == itor, 0);
		dlist->head = 0;
		dlist->last = 0;
	}
	else
	{
		// remove head?
		if (itor == dlist->head)
		{
			// the next node
			tb_size_t next = tb_iterator_next((tb_iterator_t*)dlist, itor);

			// the next data
			tb_dlist_item_t* pnext = (tb_dlist_item_t*)next;
			tb_assert_and_check_return_val(pnext, 0);

			/* 0 <=> node <=> next <=> ... <=> 0
			 * 0 <=> next <=> ... <=> 0
			 */
			dlist->head = next;
			pnext->prev = 0;
		}
		// remove last?
		else if (itor == dlist->last)
		{
			// the prev node
			tb_size_t prev = tb_iterator_prev((tb_iterator_t*)dlist, itor);

			// the prev data
			tb_dlist_item_t* pprev = (tb_dlist_item_t*)prev;
			tb_assert_and_check_return_val(pprev, 0);

			/* 0 <=> ... <=> prev <=> node <=> 0
			 * 0 <=> ... <=> prev <=> 0
			 */
			pprev->next = 0;
			dlist->last = prev;
		}
		// remove body?
		else
		{
			// the body node
			tb_size_t body = itor;

			// the body data
			tb_dlist_item_t* pbody = (tb_dlist_item_t*)body;
			tb_assert_and_check_return_val(pbody, 0);

			// the next node
			tb_size_t next = pbody->next;

			// the next data
			tb_dlist_item_t* pnext = (tb_dlist_item_t*)next;
			tb_assert_and_check_return_val(pnext, 0);

			// the prev node
			tb_size_t prev = pbody->prev;

			// the prev data
			tb_dlist_item_t* pprev = (tb_dlist_item_t*)prev;
			tb_assert_and_check_return_val(pprev, 0);

			/* 0 <=> ... <=> prev <=> body <=> next <=> ... <=> 0
			 * 0 <=> ... <=> prev <=> next <=> ... <=> 0
			 */
			pprev->next = next;
			pnext->prev = prev;
		}
	}

	// ok?
	return itor;
}
/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_dlist_t* tb_dlist_init(tb_size_t grow, tb_item_func_t func)
{
	// check
	tb_assert_and_check_return_val(grow, tb_null);
	tb_assert_and_check_return_val(func.size && func.data && func.dupl && func.repl, tb_null);

	// alloc dlist
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)tb_malloc0(sizeof(tb_dlist_impl_t));
	tb_assert_and_check_return_val(dlist, tb_null);

	// init dlist
	dlist->head = 0;
	dlist->last = 0;
	dlist->func = func;

	// init iterator
	dlist->itor.mode = TB_ITERATOR_MODE_FORWARD | TB_ITERATOR_MODE_REVERSE;
	dlist->itor.size = 0;
	dlist->itor.priv = tb_null;
	dlist->itor.step = func.size;
	dlist->itor.head = tb_dlist_iterator_head;
	dlist->itor.tail = tb_dlist_iterator_tail;
	dlist->itor.prev = tb_dlist_iterator_prev;
	dlist->itor.next = tb_dlist_iterator_next;
	dlist->itor.item = tb_dlist_iterator_item;
	dlist->itor.copy = tb_dlist_iterator_copy;
	dlist->itor.comp = tb_dlist_iterator_comp;

	// init pool, step = next + prev + data
	dlist->pool = tb_rpool_init(grow, sizeof(tb_dlist_item_t) + func.size, 0);
	tb_assert_and_check_goto(dlist->pool, fail);

	// ok
	return dlist;
fail:
	if (dlist) tb_dlist_exit(dlist);
	return tb_null;
}

tb_void_t tb_dlist_exit(tb_dlist_t* handle)
{
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)handle;
	if (dlist)
	{
		// clear data
		tb_dlist_clear(dlist);

		// free pool
		if (dlist->pool) tb_rpool_exit(dlist->pool);

		// free it
		tb_free(dlist);
	}
}
tb_void_t tb_dlist_clear(tb_dlist_t* handle)
{
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)handle;
	if (dlist) 
	{
		// free items
		if (dlist->func.free)
		{
			tb_size_t itor = dlist->head;
			while (itor)
			{
				// item
				tb_dlist_item_t* item = (tb_dlist_item_t*)itor;

				// free 
				dlist->func.free(&dlist->func, &item[1]);
		
				// next
				itor = item->next;
			}
		}

		// clear pool
		if (dlist->pool) tb_rpool_clear(dlist->pool);

		// reset it
		dlist->head = 0;
		dlist->last = 0;
	}
}
tb_pointer_t tb_dlist_head(tb_dlist_t const* handle)
{
	return tb_iterator_item(handle, tb_iterator_head(handle));
}
tb_pointer_t tb_dlist_last(tb_dlist_t const* handle)
{
	return tb_iterator_item(handle, tb_iterator_last(handle));
}
tb_size_t tb_dlist_size(tb_dlist_t const* handle)
{
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)handle;
	tb_assert_and_check_return_val(dlist && dlist->pool, 0);
	return tb_rpool_size(dlist->pool);
}
tb_size_t tb_dlist_maxn(tb_dlist_t const* handle)
{
	tb_assert_and_check_return_val(handle, 0);
	return TB_MAXU32;
}
tb_size_t tb_dlist_insert_prev(tb_dlist_t* handle, tb_size_t itor, tb_cpointer_t data)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)handle;
	tb_assert_and_check_return_val(dlist && dlist->pool, 0);

	// make the node data
	tb_dlist_item_t* pnode = tb_rpool_malloc(dlist->pool);
	tb_assert_and_check_return_val(pnode, 0);

	// init node
	pnode->prev = 0;
	pnode->next = 0;
	dlist->func.dupl(&dlist->func, &pnode[1], data);

	// attach node
	return tb_dlist_attach_prev(dlist, itor, (tb_size_t)pnode);
}
tb_size_t tb_dlist_insert_next(tb_dlist_t* handle, tb_size_t itor, tb_cpointer_t data)
{
	return tb_dlist_insert_prev(handle, tb_iterator_next(handle, itor), data);
}
tb_size_t tb_dlist_insert_head(tb_dlist_t* handle, tb_cpointer_t data)
{
	return tb_dlist_insert_prev(handle, tb_iterator_head(handle), data);
}
tb_size_t tb_dlist_insert_tail(tb_dlist_t* handle, tb_cpointer_t data)
{
	return tb_dlist_insert_prev(handle, tb_iterator_tail(handle), data);
}
tb_size_t tb_dlist_ninsert_prev(tb_dlist_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)handle;
	tb_assert_and_check_return_val(dlist && size, 0);

	// insert items
	tb_size_t node = itor;
	while (size--) node = tb_dlist_insert_prev(dlist, node, data);

	// return the first itor
	return node;
}
tb_size_t tb_dlist_ninsert_next(tb_dlist_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	return tb_dlist_ninsert_prev(handle, tb_iterator_next(handle, itor), data, size);
}
tb_size_t tb_dlist_ninsert_head(tb_dlist_t* handle, tb_cpointer_t data, tb_size_t size)
{
	return tb_dlist_ninsert_prev(handle, tb_iterator_head(handle), data, size);
}
tb_size_t tb_dlist_ninsert_tail(tb_dlist_t* handle, tb_cpointer_t data, tb_size_t size)
{
	return tb_dlist_ninsert_prev(handle, tb_iterator_tail(handle), data, size);
}
tb_size_t tb_dlist_replace(tb_dlist_t* handle, tb_size_t itor, tb_cpointer_t data)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)handle;
	tb_assert_and_check_return_val(dlist && itor, itor);

	// the item
	tb_dlist_item_t* item = (tb_dlist_item_t*)itor;

	// replace data
	dlist->func.repl(&dlist->func, &item[1], data);

	// ok
	return itor;
}
tb_size_t tb_dlist_replace_head(tb_dlist_t* handle, tb_cpointer_t data)
{
	return tb_dlist_replace(handle, tb_iterator_head(handle), data);
}
tb_size_t tb_dlist_replace_last(tb_dlist_t* handle, tb_cpointer_t data)
{
	return tb_dlist_replace(handle, tb_iterator_last(handle), data);
}
tb_size_t tb_dlist_nreplace(tb_dlist_t* handle, tb_size_t itor, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)handle;
	tb_assert_and_check_return_val(dlist && data && size, itor);

	tb_size_t head = itor;
	tb_size_t tail = tb_iterator_tail((tb_iterator_t*)dlist);
	for (; size-- && itor != tail; itor = tb_iterator_next((tb_iterator_t*)dlist, itor)) 
		tb_dlist_replace(dlist, itor, data);
	return head;
}
tb_size_t tb_dlist_nreplace_head(tb_dlist_t* handle, tb_cpointer_t data, tb_size_t size)
{
	return tb_dlist_nreplace(handle, tb_iterator_head(handle), data, size);
}
tb_size_t tb_dlist_nreplace_last(tb_dlist_t* handle, tb_cpointer_t data, tb_size_t size)
{
	tb_size_t node = 0;
	tb_size_t itor = tb_iterator_last(handle);
	tb_size_t tail = tb_iterator_tail(handle);
	for (; size-- && itor != tail; itor = tb_iterator_prev(handle, itor)) 
		node = tb_dlist_replace(handle, itor, data);

	return node;
}
tb_size_t tb_dlist_remove(tb_dlist_t* handle, tb_size_t itor)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)handle;
	tb_assert_and_check_return_val(dlist && dlist->pool && itor, itor);

	// detach item
	tb_size_t node = tb_dlist_detach_item(dlist, itor);
	tb_assert_and_check_return_val(node && node == itor, itor);

	// next item
	tb_size_t next = tb_iterator_next(dlist, node);

	// free item
	if (dlist->func.free)
		dlist->func.free(&dlist->func, &((tb_dlist_item_t*)node)[1]);

	// free node
	tb_rpool_free(dlist->pool, (tb_pointer_t)node);

	// return next node
	return next;
}
tb_size_t tb_dlist_remove_next(tb_dlist_t* handle, tb_size_t itor)
{
	return tb_dlist_remove(handle, tb_iterator_next(handle, itor));
}
tb_size_t tb_dlist_remove_head(tb_dlist_t* handle)
{
	return tb_dlist_remove(handle, tb_iterator_head(handle));
}
tb_size_t tb_dlist_remove_last(tb_dlist_t* handle)
{
	return tb_dlist_remove(handle, tb_iterator_last(handle));
}
tb_size_t tb_dlist_nremove(tb_dlist_t* handle, tb_size_t itor, tb_size_t size)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)handle;
	tb_assert_and_check_return_val(dlist && size, itor);

	tb_size_t next = itor;
	while (size--) next = tb_dlist_remove(dlist, next);
	return next;
}
tb_size_t tb_dlist_nremove_head(tb_dlist_t* handle, tb_size_t size)
{
	while (size-- && tb_dlist_size(handle)) tb_dlist_remove_head(handle);
	return tb_iterator_head(handle);
}
tb_size_t tb_dlist_nremove_last(tb_dlist_t* handle, tb_size_t size)
{
	while (size-- && tb_dlist_size(handle)) tb_dlist_remove_last(handle);
	return tb_iterator_last(handle);
}
tb_size_t tb_dlist_moveto_prev(tb_dlist_t* handle, tb_size_t itor, tb_size_t move)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)handle;
	tb_assert_and_check_return_val(dlist && dlist->pool && move, move);

	// detach move
	tb_size_t node = tb_dlist_detach_item(dlist, move);
	tb_assert_and_check_return_val(node && node == move, move);

	// attach move to prev
	return tb_dlist_attach_prev(dlist, itor, node);
}
tb_size_t tb_dlist_moveto_next(tb_dlist_t* handle, tb_size_t itor, tb_size_t move)
{
	return tb_dlist_moveto_prev(handle, tb_iterator_next(handle, itor), move);
}
tb_size_t tb_dlist_moveto_head(tb_dlist_t* handle, tb_size_t move)
{
	return tb_dlist_moveto_prev(handle, tb_iterator_head(handle), move);
}
tb_size_t tb_dlist_moveto_tail(tb_dlist_t* handle, tb_size_t move)
{
	return tb_dlist_moveto_prev(handle, tb_iterator_tail(handle), move);
}
tb_void_t tb_dlist_walk(tb_dlist_t* handle, tb_bool_t (*func)(tb_dlist_t* handle, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data)
{
	// check
	tb_dlist_impl_t* dlist = (tb_dlist_impl_t*)handle;
	tb_assert_and_check_return(dlist && dlist->pool && func);

	// pool
	tb_handle_t pool = dlist->pool;

	// step
	tb_size_t 	step = dlist->func.size;
	tb_assert_and_check_return(step);

	// walk
	tb_size_t 	base = -1;
	tb_size_t 	prev = 0;
	tb_bool_t 	bdel = tb_false;
	tb_size_t 	itor = dlist->head;
	tb_bool_t 	stop = tb_false;
	while (itor)
	{
		// node
		tb_dlist_item_t* node = (tb_dlist_item_t*)itor;

		// item
		tb_pointer_t item = dlist->func.data(&dlist->func, &node[1]);

		// next
		tb_size_t next = node->next;
	
		// bdel
		bdel = tb_false;

		// callback: item
		if (!func(dlist, &item, &bdel, data)) stop = tb_true;

		// free it?
		if (bdel)
		{
			// save
			if (base == -1) base = prev;

			// free item
			if (dlist->func.free)
				dlist->func.free(&dlist->func, &((tb_dlist_item_t*)itor)[1]);

			// free item
			tb_rpool_free(pool, (tb_pointer_t)itor);
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
					// get the base data
					tb_dlist_item_t* pbase = (tb_dlist_item_t*)base;

					// base => next
					pbase->next = next;

					// has next
					if (next) 
					{
						// next => base
						((tb_dlist_item_t*)next)->prev = base;
					}
					// tail
					else 
					{
						// update last
						dlist->last = base;
					}
				}
				// remove all
				else
				{
					dlist->head = 0;
					dlist->last = 0;
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

	// callback: tail
	if (!func(dlist, tb_null, &bdel, data)) goto end;

end:
	return ;
}
