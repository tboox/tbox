/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		slist.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "slist.h"
#include "../math/math.h"
#include "../memory/memory.h"
#include "../bits.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_slist_t* tb_slist_create(tb_size_t step, tb_size_t grow, void (*ctor)(void* , void* ), void (*dtor)(void* , void* ), void* priv)
{
	tb_slist_t* slist = (tb_slist_t*)tb_calloc(1, sizeof(tb_slist_t));
	TB_ASSERT_RETURN_VAL(slist, TB_NULL);

	// init slist
	slist->ctor = ctor;
	slist->dtor = dtor;
	slist->priv = priv;
	slist->step = step;
	slist->head = 0;
	slist->last = 0;

	// create pool, capacity = grow, size = 0, step = next + data
	slist->pool = tb_pool_create(sizeof(tb_size_t) + step, grow, grow);
	TB_ASSERT_GOTO(slist->pool, fail);

	return slist;
fail:
	if (slist) tb_slist_destroy(slist);
	return TB_NULL;
}

void tb_slist_destroy(tb_slist_t* slist)
{
	if (slist)
	{
		// clear data
		tb_slist_clear(slist);

		// free pool
		if (slist->pool) tb_pool_destroy(slist->pool);

		// free it
		tb_free(slist);
	}
}
void tb_slist_clear(tb_slist_t* slist)
{
	if (slist) 
	{
		if (slist->pool)
		{
			// do dtor
			if (slist->dtor)
			{
				tb_size_t itor = tb_slist_head(slist);
				tb_size_t tail = tb_slist_tail(slist);
				for (; itor != tail; itor = tb_slist_next(slist, itor))
				{
					tb_byte_t* item = tb_slist_at(slist, itor);
					if (item) slist->dtor(item, slist->priv);
				}
			}
			
			// clear pool
			tb_pool_clear(slist->pool);
		}

		// reset it
		slist->head = 0;
		slist->last = 0;
	}
}
tb_byte_t* tb_slist_at(tb_slist_t* slist, tb_size_t index)
{
	TB_ASSERTA(slist && slist->pool);
	tb_byte_t* data = tb_pool_get(slist->pool, index);
	TB_ASSERTA(data);
	return (data + sizeof(tb_size_t));
}
tb_byte_t const* tb_slist_const_at(tb_slist_t const* slist, tb_size_t index)
{
	TB_ASSERTA(slist && slist->pool);
	tb_byte_t const* data = tb_pool_get(slist->pool, index);
	TB_ASSERTA(data);
	return (data + sizeof(tb_size_t));
}
tb_size_t tb_slist_head(tb_slist_t const* slist)
{
	TB_ASSERTA(slist);
	return slist->head;
}
tb_size_t tb_slist_last(tb_slist_t const* slist)
{
	TB_ASSERTA(slist);
	return slist->last;
}
tb_size_t tb_slist_tail(tb_slist_t const* slist)
{
	TB_ASSERTA(slist);
	return 0;
}
tb_size_t tb_slist_next(tb_slist_t const* slist, tb_size_t index)
{
	TB_ASSERTA(slist && slist->pool);
	tb_byte_t* data = tb_pool_get(slist->pool, index);
	TB_ASSERTA(data);
	return tb_bits_get_u32_ne(data);
}
tb_size_t tb_slist_prev(tb_slist_t const* slist, tb_size_t index)
{
	TB_ASSERTA(slist);

	// is tail?
	tb_size_t prev = 0;
	if (!index) prev = slist->last;
	// is head?
	else if (index == slist->head) prev = 0;
	// find it
	else
	{
		tb_size_t itor = tb_slist_head(slist);
		tb_size_t tail = tb_slist_tail(slist);
		for (prev = itor; itor != tail && itor != index; prev = itor, itor = tb_slist_next(slist, itor)) ;
	}

	return prev;
}
tb_size_t tb_slist_size(tb_slist_t const* slist)
{
	TB_ASSERT_RETURN_VAL(slist && slist->pool, 0);
	return slist->pool->size;
}
tb_size_t tb_slist_maxn(tb_slist_t const* slist)
{
	TB_ASSERT_RETURN_VAL(slist && slist->pool, 0);
	return slist->pool->maxn;
}
/* insert node:
 *
 * index == a: 
 * before: [a] => ... => [b] => [0]
 *         head         last   tail
 *
 * after:  [node] => [a] => ... => [b] => [0]
 *          head                  last    tail
 *
 * index == b: 
 * before: [a] => ... => [b] => [c] => [0]
 *         head                last   tail
 *
 * after:  [a] => ... => [node] => [b] => [c] => [0]
 *         head                          last   tail
 *
 * index == 0: 
 * before: [a] => ... => [b] => [0]
 *         head         last   tail
 *
 * after:  [a] => ... => [b] => [node] => [0]
 *         head                  last    tail
 *
 */
tb_size_t tb_slist_insert(tb_slist_t* slist, tb_size_t index, tb_byte_t const* item)
{
	return tb_slist_insert_next(slist, tb_slist_prev(slist, index), item);
}

tb_size_t tb_slist_insert_next(tb_slist_t* slist, tb_size_t index, tb_byte_t const* item)
{
	TB_ASSERT_RETURN_VAL(slist && slist->pool && item, index);

	// alloc a new node
	tb_size_t node = tb_pool_alloc(slist->pool);
	TB_ASSERT_RETURN_VAL(node, index);

	// get the node data
	tb_byte_t* pnode = tb_pool_get(slist->pool, node);
	TB_ASSERTA(pnode);

	// init node, inode => 0
	tb_bits_set_u32_ne(pnode, 0);

	// copy the item data
	tb_memcpy(pnode + sizeof(tb_size_t), item, slist->step);

	// the prev node
	tb_size_t prev = index;

	// non-empty?
	if (slist->head)
	{
		// is head?
		if (!prev)
		{
			// node => head
			tb_bits_set_u32_ne(pnode, slist->head);

			// update head
			slist->head = node;
		}
		// is last?
		else if (prev == slist->last)
		{
			// the prev data
			tb_byte_t* pprev = tb_pool_get(slist->pool, prev);
			TB_ASSERTA(pprev);

			// last => node => null
			tb_bits_set_u32_ne(pprev, node);

			// update last
			slist->last = node;
		}
		// is body?
		else
		{
			// the prev data
			tb_byte_t* pprev = tb_pool_get(slist->pool, prev);
			TB_ASSERTA(pprev);

			// node => next
			tb_bits_set_u32_ne(pnode, tb_bits_get_u32_ne(pprev));

			// prev => node
			tb_bits_set_u32_ne(pprev, node);
		}
	}
	// empty?
	else
	{
		// must be zero
		TB_ASSERT(!prev);

		// update head
		slist->head = node;

		// update last
		slist->last = node;
	}

	// return the new node
	return node;
}

tb_size_t tb_slist_insert_head(tb_slist_t* slist, tb_byte_t const* item)
{
	return tb_slist_insert(slist, tb_slist_head(slist), item);
}
tb_size_t tb_slist_insert_tail(tb_slist_t* slist, tb_byte_t const* item)
{
	return tb_slist_insert(slist, tb_slist_tail(slist), item);
}
tb_size_t tb_slist_ninsert(tb_slist_t* slist, tb_size_t index, tb_byte_t const* item, tb_size_t size)
{
	return tb_slist_ninsert_next(slist, tb_slist_prev(slist, index), item, size);
}
tb_size_t tb_slist_ninsert_next(tb_slist_t* slist, tb_size_t index, tb_byte_t const* item, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(slist && item && size, index);

	// insert items
	tb_size_t node = index;
	while (size--) node = tb_slist_insert_next(slist, index, item);

	// return the first index
	return node;
}
tb_size_t tb_slist_ninsert_head(tb_slist_t* slist, tb_byte_t const* item, tb_size_t size)
{
	return tb_slist_ninsert(slist, tb_slist_head(slist), item, size);
}
tb_size_t tb_slist_ninsert_tail(tb_slist_t* slist, tb_byte_t const* item, tb_size_t size)
{
	return tb_slist_ninsert(slist, tb_slist_tail(slist), item, size);
}
tb_size_t tb_slist_replace(tb_slist_t* slist, tb_size_t index, tb_byte_t const* item)
{
	tb_byte_t* data = tb_slist_at(slist, index);
	TB_ASSERT_RETURN_VAL(data && item, index);
	
	// do dtor
	if (slist->dtor) slist->dtor(data, slist->priv);

	// copy data
	tb_memcpy(data, item, slist->step);
	return index;
}
tb_size_t tb_slist_replace_head(tb_slist_t* slist, tb_byte_t const* item)
{
	return tb_slist_replace(slist, tb_slist_head(slist), item);
}
tb_size_t tb_slist_replace_last(tb_slist_t* slist, tb_byte_t const* item)
{
	return tb_slist_replace(slist, tb_slist_last(slist), item);
}
tb_size_t tb_slist_nreplace(tb_slist_t* slist, tb_size_t index, tb_byte_t const* item, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(slist && item && size, index);

	tb_size_t itor = index;
	tb_size_t tail = tb_slist_tail(slist);
	for (; size-- && itor != tail; itor = tb_slist_next(slist, itor)) 
		tb_slist_replace(slist, itor, item);
	return index;
}
tb_size_t tb_slist_nreplace_head(tb_slist_t* slist, tb_byte_t const* item, tb_size_t size)
{
	return tb_slist_nreplace(slist, tb_slist_head(slist), item, size);
}
tb_size_t tb_slist_nreplace_last(tb_slist_t* slist, tb_byte_t const* item, tb_size_t size)
{
	// compute offset
	tb_size_t n = tb_slist_size(slist);
	tb_size_t o = n > size? n - size : 0;
	
	// seek
	tb_size_t itor = tb_slist_head(slist);
	tb_size_t tail = tb_slist_tail(slist);
	for (; o-- && itor != tail; itor = tb_slist_next(slist, itor)) ;

	// replace
	return tb_slist_nreplace(slist, itor, item, size);
}
/* remove node:
 *
 * index == node: 
 * before: [node] => [a] => ... => [b] => [0]
 *          head                  last    tail

 * after:  [a] => ... => [b] => [0]
 *         head         last   tail
 *
 *
 * index == node: 
 * before: [a] => ... => [node] => [b] => [c] => [0]
 *         head                          last   tail
 * after:  [a] => ... => [b] => [c] => [0]
 *         head                last   tail
 *
 * index == node: 
 * before: [a] => ... => [b] => [node] => [0]
 *         head                  last    tail
 * after:  [a] => ... => [b] => [0]
 *         head         last   tail
 *
 *
 */
tb_size_t tb_slist_remove(tb_slist_t* slist, tb_size_t index)
{	
	return tb_slist_remove_next(slist, tb_slist_prev(slist, index));
}
tb_size_t tb_slist_remove_next(tb_slist_t* slist, tb_size_t index)
{
	TB_ASSERT_RETURN_VAL(slist && slist->pool, index);

	// the prev node
	tb_size_t prev = index;

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
			else slist->head = next = tb_slist_next(slist, midd);
		}
		// remove body?
		else
		{
			// the midd node
			midd = tb_slist_next(slist, prev);

			// get the prev data
			tb_byte_t* pprev = tb_pool_get(slist->pool, prev);
			TB_ASSERTA(pprev);

			// the next node
			next = tb_slist_next(slist, midd);

			// prev => next
			tb_bits_set_u32_ne(pprev, next);

			// update last if midd is last
			if (midd == slist->last) slist->last = next = prev;
		}

		// check
		TB_ASSERTA(midd);

		// do dtor
		if (slist->dtor)
		{
			tb_byte_t* data = tb_slist_at(slist, midd);
			TB_ASSERTA(data);

			slist->dtor(data, slist->priv);
		}

		// free node
		tb_pool_free(slist->pool, midd);

		// return next node
		return next;
	}

	return index;
}
tb_size_t tb_slist_remove_head(tb_slist_t* slist)
{
	return tb_slist_remove(slist, tb_slist_head(slist));
}
tb_size_t tb_slist_remove_last(tb_slist_t* slist)
{
	return tb_slist_remove(slist, tb_slist_last(slist));
}
tb_size_t tb_slist_nremove(tb_slist_t* slist, tb_size_t index, tb_size_t size)
{
	return tb_slist_nremove_next(slist, tb_slist_prev(slist, index), size);
}
tb_size_t tb_slist_nremove_next(tb_slist_t* slist, tb_size_t index, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(slist && size, index);

	tb_size_t next = index;
	while (size--) next = tb_slist_remove_next(slist, index);
	return next;
}
tb_size_t tb_slist_nremove_head(tb_slist_t* slist, tb_size_t size)
{
	return tb_slist_nremove(slist, tb_slist_head(slist), size);
}
tb_size_t tb_slist_nremove_last(tb_slist_t* slist, tb_size_t size)
{
	// compute offset
	tb_size_t n = tb_slist_size(slist);
	tb_size_t o = n > size? n - size : 0;
	
	// seek
	tb_size_t prev = 0;
	tb_size_t itor = tb_slist_head(slist);
	tb_size_t tail = tb_slist_tail(slist);
	for (; o-- && itor != tail; prev = itor, itor = tb_slist_next(slist, itor)) ;

	// remove
	return tb_slist_nremove_next(slist, prev, size);
}
