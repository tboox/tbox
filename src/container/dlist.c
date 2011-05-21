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
 * \file		dlist.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "dlist.h"
#include "../math/math.h"
#include "../memops.h"
#include "../bits.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_dlist_t* tb_dlist_create(tb_size_t step, tb_size_t grow, void (*ctor)(void* , void* ), void (*dtor)(void* , void* ), void* priv)
{
	tb_dlist_t* dlist = (tb_dlist_t*)tb_calloc(1, sizeof(tb_dlist_t));
	TB_ASSERT_RETURN_VAL(dlist, TB_NULL);

	TB_ASSERT_RETURN_VAL(sizeof(tb_size_t) == 4);

	// init dlist
	dlist->ctor = ctor;
	dlist->dtor = dtor;
	dlist->priv = priv;
	dlist->step = step;
	dlist->next = 0;
	dlist->prev = 0;

	// create pool, capacity = grow, size = 0, step = next + prev + data
	dlist->pool = tb_pool_create(8 + step, grow, grow);
	TB_ASSERT_GOTO(dlist->pool, fail);

	return dlist;
fail:
	if (dlist) tb_dlist_destroy(dlist);
	return TB_NULL;
}

void tb_dlist_destroy(tb_dlist_t* dlist)
{
	if (dlist)
	{
		// clear data
		tb_dlist_clear(dlist);

		// free pool
		if (dlist->pool) tb_pool_destroy(dlist->pool);

		// free it
		tb_free(dlist);
	}
}
void tb_dlist_clear(tb_dlist_t* dlist)
{
	if (dlist) 
	{
		if (dlist->pool)
		{
			// do dtor
			if (dlist->dtor)
			{
				tb_size_t itor = tb_dlist_head(dlist);
				tb_size_t tail = tb_dlist_tail(dlist);
				for (; itor != tail; itor = tb_dlist_next(dlist, itor))
				{
					tb_byte_t* item = tb_dlist_at(dlist, itor);
					if (item) dlist->dtor(item, dlist->priv);
				}
			}
			
			// clear pool
			tb_pool_clear(dlist->pool);
		}

		// reset it
		dlist->next = 0;
		dlist->prev = 0;
	}
}
tb_byte_t* tb_dlist_at(tb_dlist_t* dlist, tb_size_t index)
{
	TB_ASSERTA(dlist && dlist->pool);
	tb_byte_t* data = tb_pool_get(dlist->pool, index);
	TB_ASSERTA(data);
	return (data + 8);
}
tb_byte_t const* tb_dlist_const_at(tb_dlist_t const* dlist, tb_size_t index)
{
	TB_ASSERTA(dlist && dlist->pool);
	tb_byte_t const* data = tb_pool_get(dlist->pool, index);
	TB_ASSERTA(data);
	return (data + 8);
}
tb_size_t tb_dlist_head(tb_dlist_t const* dlist)
{
	TB_ASSERTA(dlist);
	return dlist->next;
}
tb_size_t tb_dlist_last(tb_dlist_t const* dlist)
{
	TB_ASSERTA(dlist);
	return dlist->prev;
}
tb_size_t tb_dlist_tail(tb_dlist_t const* dlist)
{
	TB_ASSERTA(dlist);
	return 0;
}
tb_size_t tb_dlist_next(tb_dlist_t const* dlist, tb_size_t index)
{
	TB_ASSERTA(dlist && dlist->pool);

	if (!index) return dlist->next;
	else
	{
		tb_byte_t* data = tb_pool_get(dlist->pool, index);
		TB_ASSERTA(data);
		return tb_bits_get_u32_ne(data);
	}
}
tb_size_t tb_dlist_prev(tb_dlist_t const* dlist, tb_size_t index)
{
	TB_ASSERTA(dlist && dlist->pool);

	if (!index) return dlist->prev;
	else
	{
		tb_byte_t* data = tb_pool_get(dlist->pool, index);
		TB_ASSERTA(data);
		data += 4;
		return tb_bits_get_u32_ne(data);
	}
}
tb_size_t tb_dlist_size(tb_dlist_t const* dlist)
{
	TB_ASSERT_RETURN_VAL(dlist && dlist->pool, 0);
	return dlist->pool->size;
}
tb_size_t tb_dlist_maxn(tb_dlist_t const* dlist)
{
	TB_ASSERT_RETURN_VAL(dlist && dlist->pool, 0);
	return dlist->pool->maxn;
}
tb_size_t tb_dlist_insert(tb_dlist_t* dlist, tb_size_t index, tb_byte_t const* item)
{
	TB_ASSERT_RETURN_VAL(dlist && dlist->pool && item, index);

	// alloc a new node
	tb_size_t node = tb_pool_alloc(dlist->pool);
	TB_ASSERT_RETURN_VAL(node, index);

	// get the node data
	tb_byte_t* pnode = tb_pool_get(dlist->pool, node);
	TB_ASSERTA(pnode);

	// init node, node <=> 0
	tb_bits_set_u32_ne(pnode, 0);
	tb_bits_set_u32_ne(pnode + 4, 0);

	// copy the item data
	tb_memcpy(pnode + 8, item, dlist->step);

	// is null?
	if (!dlist->next && !dlist->prev)
	{
		/* dlist: 0 => node => 0
		 *       tail  head   tail
		 *             last
		 */
		dlist->next = node;
		dlist->prev = node;
	}
	else
	{
		TB_ASSERTA(dlist->next && dlist->prev);

		// insert to tail
		if (!index)
		{
			// the last node
			tb_size_t last = dlist->prev;
		
			// the last data
			tb_byte_t* plast = tb_pool_get(dlist->pool, last);
			TB_ASSERTA(plast);

			// last <=> node <=> 0
			tb_bits_set_u32_ne(plast, node);
			tb_bits_set_u32_ne(pnode + 4, last);
		}
		// insert to head
		else if (index == dlist->next)
		{
			// the head node
			tb_size_t head = dlist->next;
		
			// the head data
			tb_byte_t* phead = tb_pool_get(dlist->pool, head);
			TB_ASSERTA(phead);

			// 0 <=> node <=> head
			tb_bits_set_u32_ne(phead + 4, node);
			tb_bits_set_u32_ne(pnode, head);
		}
		// insert to body
		else
		{
			// the body node
			tb_size_t body = index;
		
			// the body data
			tb_byte_t* pbody = tb_pool_get(dlist->pool, body);
			TB_ASSERTA(pbody);

			// the prev node 
			tb_size_t prev = tb_bits_get_u32_ne(pbody + 4);

			// the prev data
			tb_byte_t* pprev = tb_pool_get(dlist->pool, prev);
			TB_ASSERTA(pprev);

			/* 0 <=> ... <=> prev <=> body <=> ... <=> 0
			 * 0 <=> ... <=> prev <=> node <=> body <=> ... <=> 0
			 */
			tb_bits_set_u32_ne(pnode, body);
			tb_bits_set_u32_ne(pnode + 4, prev);
			tb_bits_set_u32_ne(pprev, node);
			tb_bits_set_u32_ne(pbody + 4, node);
		}
	}

	// return the new node
	return node;
}

tb_size_t tb_dlist_insert_head(tb_dlist_t* dlist, tb_byte_t const* item)
{
	return tb_dlist_insert(dlist, tb_dlist_head(dlist), item);
}
tb_size_t tb_dlist_insert_tail(tb_dlist_t* dlist, tb_byte_t const* item)
{
	return tb_dlist_insert(dlist, tb_dlist_tail(dlist), item);
}
tb_size_t tb_dlist_ninsert(tb_dlist_t* dlist, tb_size_t index, tb_byte_t const* item, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(dlist && item && size, index);

	// insert items
	tb_size_t node = index;
	while (size--) node = tb_dlist_insert(dlist, index, item);

	// return the first index
	return node;
}
tb_size_t tb_dlist_ninsert_head(tb_dlist_t* dlist, tb_byte_t const* item, tb_size_t size)
{
	return tb_dlist_ninsert(dlist, tb_dlist_head(dlist), item, size);
}
tb_size_t tb_dlist_ninsert_tail(tb_dlist_t* dlist, tb_byte_t const* item, tb_size_t size)
{
	return tb_dlist_ninsert(dlist, tb_dlist_tail(dlist), item, size);
}
tb_size_t tb_dlist_replace(tb_dlist_t* dlist, tb_size_t index, tb_byte_t const* item)
{
	tb_byte_t* data = tb_dlist_at(dlist, index);
	TB_ASSERT_RETURN_VAL(data && item, index);
	
	// do dtor
	if (dlist->dtor) dlist->dtor(data, dlist->priv);

	// copy data
	tb_memcpy(data, item, dlist->step);
	return index;
}
tb_size_t tb_dlist_replace_head(tb_dlist_t* dlist, tb_byte_t const* item)
{
	return tb_dlist_replace(dlist, tb_dlist_head(dlist), item);
}
tb_size_t tb_dlist_replace_last(tb_dlist_t* dlist, tb_byte_t const* item)
{
	return tb_dlist_replace(dlist, tb_dlist_last(dlist), item);
}
tb_size_t tb_dlist_nreplace(tb_dlist_t* dlist, tb_size_t index, tb_byte_t const* item, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(dlist && item && size, index);

	tb_size_t itor = index;
	tb_size_t tail = tb_dlist_tail(dlist);
	for (; size-- && itor != tail; itor = tb_dlist_next(dlist, itor)) 
		tb_dlist_replace(dlist, itor, item);
	return index;
}
tb_size_t tb_dlist_nreplace_head(tb_dlist_t* dlist, tb_byte_t const* item, tb_size_t size)
{
	return tb_dlist_nreplace(dlist, tb_dlist_head(dlist), item, size);
}
tb_size_t tb_dlist_nreplace_last(tb_dlist_t* dlist, tb_byte_t const* item, tb_size_t size)
{
	tb_size_t node = 0;
	tb_size_t itor = tb_dlist_last(dlist);
	tb_size_t tail = tb_dlist_tail(dlist);
	for (; size-- && itor != tail; itor = tb_dlist_prev(dlist, itor)) 
		node = tb_dlist_replace(dlist, itor, item);

	return node;
}
tb_size_t tb_dlist_remove(tb_dlist_t* dlist, tb_size_t index)
{
	TB_ASSERT_RETURN_VAL(dlist && dlist->pool && index, index);

	// not null?
	if (dlist->next && dlist->prev)
	{
		// only one?
		if (dlist->next == dlist->prev)
		{
			TB_ASSERTA(dlist->next == index);
			dlist->next = 0;
			dlist->prev = 0;
		}
		else
		{
			// remove head?
			if (index == dlist->next)
			{
				// the next node
				tb_size_t next = tb_dlist_next(dlist, index);

				// the next data
				tb_byte_t* pnext = tb_pool_get(dlist->pool, next);
				TB_ASSERTA(pnext);

				/* 0 <=> node <=> next <=> ... <=> 0
				 * 0 <=> next <=> ... <=> 0
				 */
				dlist->next = next;
				tb_bits_set_u32_ne(pnext + 4, 0);
			}
			// remove last?
			else if (index == dlist->prev)
			{
				// the prev node
				tb_size_t prev = tb_dlist_prev(dlist, index);

				// the prev data
				tb_byte_t* pprev = tb_pool_get(dlist->pool, prev);
				TB_ASSERTA(pprev);

				/* 0 <=> ... <=> prev <=> node <=> 0
				 * 0 <=> ... <=> prev <=> 0
				 */
				tb_bits_set_u32_ne(pprev, 0);
				dlist->prev = prev;
			}
			// remove body?
			else
			{
				// the body node
				tb_size_t body = index;
	
				// the body data
				tb_byte_t* pbody = tb_pool_get(dlist->pool, body);
				TB_ASSERTA(pbody);

				// the next node
				tb_size_t next = tb_bits_get_u32_ne(pbody);

				// the next data
				tb_byte_t* pnext = tb_pool_get(dlist->pool, next);
				TB_ASSERTA(pnext);

				// the prev node
				tb_size_t prev = tb_bits_get_u32_ne(pbody + 4);

				// the prev data
				tb_byte_t* pprev = tb_pool_get(dlist->pool, prev);
				TB_ASSERTA(pprev);

				/* 0 <=> ... <=> prev <=> body <=> next <=> ... <=> 0
				 * 0 <=> ... <=> prev <=> next <=> ... <=> 0
				 */
				tb_bits_set_u32_ne(pprev, next);
				tb_bits_set_u32_ne(pnext + 4, prev);
			}
		}

		// do dtor
		if (dlist->dtor)
		{
			tb_byte_t* data = tb_dlist_at(dlist, index);
			TB_ASSERTA(data);

			dlist->dtor(data, dlist->priv);
		}

		// free node
		tb_pool_free(dlist->pool, index);
	}

	return index;
}
tb_size_t tb_dlist_remove_head(tb_dlist_t* dlist)
{
	return tb_dlist_remove(dlist, tb_dlist_head(dlist));
}
tb_size_t tb_dlist_remove_last(tb_dlist_t* dlist)
{
	return tb_dlist_remove(dlist, tb_dlist_last(dlist));
}
tb_size_t tb_dlist_nremove(tb_dlist_t* dlist, tb_size_t index, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(dlist && size, index);

	tb_size_t next = index;
	while (size--) next = tb_dlist_remove(dlist, index);
	return next;
}
tb_size_t tb_dlist_nremove_head(tb_dlist_t* dlist, tb_size_t size)
{
	return tb_dlist_nremove(dlist, tb_dlist_head(dlist), size);
}
tb_size_t tb_dlist_nremove_last(tb_dlist_t* dlist, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(dlist && size, index);

	while (size--) next = tb_dlist_remove(dlist, dlist->prev);
	return dlist->prev;
}
