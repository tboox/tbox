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
 * @file		hash.c
 * @ingroup 	container
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "hash.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// index
#if TB_CPU_BIT64
# 	define TB_HASH_INDEX_MAKE(buck, item) 			(((tb_size_t)((item) & 0xffffffff) << 32) | (buck) & 0xffffffff)
# 	define TB_HASH_INDEX_BUCK(index) 				((index) & 0xffffffff)
# 	define TB_HASH_INDEX_ITEM(index) 				(((index) >> 32) & 0xffffffff)
#else
# 	define TB_HASH_INDEX_MAKE(buck, item) 			(((tb_size_t)((item) & 0xffff) << 16) | (buck) & 0xffff)
# 	define TB_HASH_INDEX_BUCK(index) 				((index) & 0xffff)
# 	define TB_HASH_INDEX_ITEM(index) 				(((index) >> 16) & 0xffff)
#endif

// grow
#define TB_HASH_GROW_MIN 							(8)

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the hash item list type
typedef struct __tb_hash_item_list_t
{
	tb_size_t 				size;
	tb_size_t 				maxn;

}tb_hash_item_list_t;

/*!the hash type
 *
 * <pre>
 *                 0        1        3       ...     ...                n       n + 1
 * hash_list: |--------|--------|--------|--------|--------|--------|--------|--------|
 *                         |
 *                       -----    
 * item_list:           |     |       key:0                                      
 *                       -----   
 *                      |     |       key:1                                              
 *                       -----               <= insert by binary search algorithm
 *                      |     |       key:2                                               
 *                       -----  
 *                      |     |       key:3                                               
 *                       -----   
 *                      |     |       key:4                                               
 *                       -----  
 *                      |     |                                              
 *                       -----  
 *                      |     |                                              
 *                       -----  
 *                      |     |                                              
 *                       -----  
 *
 * </pre>
 *
 * @note the itor of the same item is mutable
 */
typedef struct __tb_hash_t
{
	// the item itor
	tb_iterator_t 			item_itor;

	/// the hash list
	tb_hash_item_list_t** 	hash_list;
	tb_size_t 				hash_size;

	/// the item size
	tb_size_t 				item_size;

	/// the item maxn
	tb_size_t 				item_maxn;

	/// the hash item
	tb_hash_item_t 			hash_item;

	/// the hash func
	tb_item_func_t 			name_func;
	tb_item_func_t 			data_func;

}tb_hash_t;

/* ///////////////////////////////////////////////////////////////////////
 * finder
 */

#if 0
static tb_bool_t tb_hash_item_find(tb_hash_t* hash, tb_cpointer_t name, tb_size_t* pbuck, tb_size_t* pitem)
{
	tb_assert_and_check_return_val(hash && hash->hash_list && hash->hash_size, tb_false);
	
	// get step
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return_val(step, tb_false);

	// comupte hash from name
	tb_size_t buck = hash->name_func.hash(&hash->name_func, name, hash->hash_size);
	tb_assert_and_check_return_val(buck < hash->hash_size, tb_false);

	// update bucket
	if (pbuck) *pbuck = buck;

	// get list
	tb_hash_item_list_t* list = hash->hash_list[buck];
	tb_check_return_val(list && list->size, tb_false);

	// find item
	tb_long_t 	r = 1;
	tb_size_t 	i = 0;
	tb_size_t 	n = list->size;
	for (i = 0; i < n; i++)
	{
		// get item
		tb_byte_t const* item = ((tb_byte_t*)&list[1]) + i * step;

		// compare it
		r = hash->name_func.comp(&hash->name_func, name, hash->name_func.data(&hash->name_func, item));
		if (r <= 0) break;
	}

	// update item
	if (pitem) *pitem = i;

	// ok?
	return !r? tb_true : tb_false;
}
#else
static tb_bool_t tb_hash_item_find(tb_hash_t* hash, tb_cpointer_t name, tb_size_t* pbuck, tb_size_t* pitem)
{
	tb_assert_and_check_return_val(hash && hash->hash_list && hash->hash_size, tb_false);
	
	// get step
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return_val(step, tb_false);

	// comupte hash from name
	tb_size_t buck = hash->name_func.hash(&hash->name_func, name, hash->hash_size);
	tb_assert_and_check_return_val(buck < hash->hash_size, tb_false);

	// update bucket
	if (pbuck) *pbuck = buck;

	// get list
	tb_hash_item_list_t* list = hash->hash_list[buck];
	tb_check_return_val(list && list->size, tb_false);

	// find item
	tb_long_t 	t = 1;
	tb_size_t 	l = 0;
	tb_size_t 	r = list->size;
	tb_size_t 	m = (l + r) >> 1;
	while (l < r)
	{
		// get item
		tb_byte_t const* item = ((tb_byte_t*)&list[1]) + m * step;

		// compare it
		t = hash->name_func.comp(&hash->name_func, name, hash->name_func.data(&hash->name_func, item));
		if (t < 0) r = m;
		else if (t > 0) l = m + 1;
		else break;
	
		// next
		m = (l + r) >> 1;
	}

	// update item
	if (pitem) *pitem = m;

	// ok?
	return !t? tb_true : tb_false;
}
#endif
static tb_bool_t tb_hash_item_at(tb_hash_t* hash, tb_size_t buck, tb_size_t item, tb_pointer_t* pname, tb_pointer_t* pdata)
{
	tb_assert_and_check_return_val(hash && hash->hash_list && hash->hash_size && buck < hash->hash_size, tb_false);
	
	// get step
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return_val(step, tb_false);

	// get list
	tb_hash_item_list_t* list = hash->hash_list[buck];
	tb_check_return_val(list && list->size && item < list->size, tb_false);

	// get name
	if (pname) *pname = hash->name_func.data(&hash->name_func, ((tb_byte_t*)&list[1]) + item * step);
	
	// get data
	if (pdata) *pdata = hash->data_func.data(&hash->data_func, ((tb_byte_t*)&list[1]) + item * step + hash->name_func.size);

	// ok
	return tb_true;
}

/* ///////////////////////////////////////////////////////////////////////
 * iterator
 */
static tb_size_t tb_hash_iterator_head(tb_iterator_t* iterator)
{
	tb_hash_t* hash = (tb_hash_t*)iterator;
	tb_assert_and_check_return_val(hash, 0);

	// find the head
	tb_size_t i = 0;
	tb_size_t n = hash->hash_size;
	for (i = 0; i < n; i++)
	{
		tb_hash_item_list_t* list = hash->hash_list[i];
		if (list && list->size) return TB_HASH_INDEX_MAKE(i + 1, 1);
	}
	return 0;
}
static tb_size_t tb_hash_iterator_tail(tb_iterator_t* iterator)
{
	tb_hash_t* hash = (tb_hash_t*)iterator;
	tb_assert_and_check_return_val(hash, 0);

	return 0;
}
static tb_size_t tb_hash_iterator_next(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_hash_t* hash = (tb_hash_t*)iterator;
	tb_assert_and_check_return_val(hash && hash->hash_list && hash->hash_size, 0);

	// buck & item
	tb_size_t buck = TB_HASH_INDEX_BUCK(itor);
	tb_size_t item = TB_HASH_INDEX_ITEM(itor);
	tb_assert_and_check_return_val(buck && item, 0);

	// the current bucket & item
	buck--;
	item--;
	tb_assert_and_check_return_val(buck < hash->hash_size && (item + 1) < 65536, 0);

	// find the next from the current bucket first
	if (hash->hash_list[buck] && item + 1 < hash->hash_list[buck]->size) return TB_HASH_INDEX_MAKE(buck + 1, item + 2);

	// find the next from the next buckets
	tb_size_t i;
	tb_size_t n = hash->hash_size;
	for (i = buck + 1; i < n; i++)
	{
		tb_hash_item_list_t* list = hash->hash_list[i];
		if (list && list->size) return TB_HASH_INDEX_MAKE(i + 1, 1);
	}
	return 0;
}
static tb_pointer_t tb_hash_iterator_item(tb_iterator_t* iterator, tb_size_t itor)
{
	tb_hash_t* hash = (tb_hash_t*)iterator;
	tb_assert_and_check_return_val(hash && itor, 0);

	// get buck & item
	tb_size_t buck = TB_HASH_INDEX_BUCK(itor);
	tb_size_t item = TB_HASH_INDEX_ITEM(itor);
	tb_assert_and_check_return_val(buck && item, tb_null);

	// get item
	if (tb_hash_item_at(hash, buck - 1, item - 1, &((tb_hash_t*)hash)->hash_item.name, &((tb_hash_t*)hash)->hash_item.data))
		return &(hash->hash_item);
	return tb_null;
}
static tb_void_t tb_hash_iterator_delt(tb_iterator_t* iterator, tb_size_t itor)
{
	// check
	tb_hash_t* hash = (tb_hash_t*)iterator;
	tb_assert_return(hash && hash->hash_list && hash->hash_size);
	
	// buck & item
	tb_size_t buck = TB_HASH_INDEX_BUCK(itor);
	tb_size_t item = TB_HASH_INDEX_ITEM(itor);
	tb_assert_and_check_return(buck && item); buck--; item--;
	tb_assert_and_check_return(buck < hash->hash_size);

	// the step
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return(step);

	// get list
	tb_hash_item_list_t* list = hash->hash_list[buck];
	tb_assert_and_check_return(list && list->size && item < list->size);

	// free item
	if (hash->name_func.free) hash->name_func.free(&hash->name_func, ((tb_byte_t*)&list[1]) + item * step);
	if (hash->data_func.free) hash->data_func.free(&hash->data_func, ((tb_byte_t*)&list[1]) + item * step + hash->name_func.size);

	// remove item from the list
	if (list->size > 1)
	{
		// move items
		if (item < list->size - 1) tb_memmov(((tb_byte_t*)&list[1]) + item * step, ((tb_byte_t*)&list[1]) + (item + 1) * step, (list->size - item - 1) * step);

		// update size
		list->size--;
	}
	// remove list
	else 
	{
		// free it
		tb_free(list);

		// reset
		hash->hash_list[buck] = tb_null;
	}

	// update the hash item size
	hash->item_size--;
}
static tb_void_t tb_hash_iterator_move(tb_iterator_t* iterator, tb_size_t itor, tb_cpointer_t data)
{
	tb_hash_t* hash = (tb_hash_t*)iterator;
	tb_assert_return(hash && hash->hash_list && hash->hash_size);
	
	// buck & item
	tb_size_t b = TB_HASH_INDEX_BUCK(itor);
	tb_size_t i = TB_HASH_INDEX_ITEM(itor);
	tb_assert_return(b && i); b--; i--;
	tb_assert_return(b < hash->hash_size);

	// step
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return(step);

	// list
	tb_hash_item_list_t* list = hash->hash_list[b];
	tb_check_return(list && list->size && i < list->size);

	// note: copy data only, will destroy hash index if copy name
	hash->data_func.copy(&hash->data_func, ((tb_byte_t*)&list[1]) + i * step + hash->name_func.size, data);
}
static tb_long_t tb_hash_iterator_comp(tb_iterator_t* iterator, tb_cpointer_t ltem, tb_cpointer_t rtem)
{
	tb_hash_t* hash = (tb_hash_t*)iterator;
	tb_assert_and_check_return_val(hash && hash->name_func.comp && ltem && rtem, 0);
	
	return hash->name_func.comp(&hash->name_func, ((tb_hash_item_t*)ltem)->name, ((tb_hash_item_t*)rtem)->name);
}
/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_hash_init(tb_size_t size, tb_item_func_t name_func, tb_item_func_t data_func)
{
	// check
	tb_assert_and_check_return_val(size, tb_null);
	tb_assert_and_check_return_val(name_func.size && name_func.hash && name_func.comp && name_func.data && name_func.dupl, tb_null);
	tb_assert_and_check_return_val(data_func.size && data_func.data && data_func.dupl && data_func.copy, tb_null);

	// alloc hash
	tb_hash_t* hash = (tb_hash_t*)tb_malloc0(sizeof(tb_hash_t));
	tb_assert_and_check_return_val(hash, tb_null);

	// init hash func
	hash->name_func = name_func;
	hash->data_func = data_func;

	// init item itor
	hash->item_itor.mode = TB_ITERATOR_MODE_FORWARD;
	hash->item_itor.size = 0;
	hash->item_itor.priv = tb_null;
	hash->item_itor.step = sizeof(tb_hash_item_t);
	hash->item_itor.head = tb_hash_iterator_head;
	hash->item_itor.tail = tb_hash_iterator_tail;
	hash->item_itor.prev = tb_null;
	hash->item_itor.next = tb_hash_iterator_next;
	hash->item_itor.item = tb_hash_iterator_item;
	hash->item_itor.move = tb_hash_iterator_move;
	hash->item_itor.delt = tb_hash_iterator_delt;
	hash->item_itor.comp = tb_hash_iterator_comp;

	// init hash size
	hash->hash_size = tb_align_pow2(size);
	tb_assert_and_check_goto(hash->hash_size <= 65536, fail);

	// init hash list
	hash->hash_list = tb_nalloc0(hash->hash_size, sizeof(tb_size_t));
	tb_assert_and_check_goto(hash->hash_list, fail);

	// ok
	return (tb_handle_t)hash;
fail:
	if (hash) tb_hash_exit(hash);
	return tb_null;
}
tb_void_t tb_hash_exit(tb_handle_t handle)
{
	tb_hash_t* hash = (tb_hash_t*)handle;
	if (hash)
	{
		// clear it
		tb_hash_clear(hash);

		// free hash list
		if (hash->hash_list) tb_free(hash->hash_list);

		// free it
		tb_free(hash);
	}
}
tb_void_t tb_hash_clear(tb_handle_t handle)
{
	// check
	tb_hash_t* hash = (tb_hash_t*)handle;
	tb_assert_and_check_return(hash && hash->hash_list);

	// step
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return(step);

	// clear hash
	tb_size_t i = 0;
	tb_size_t n = hash->hash_size;
	for (i = 0; i < n; i++)
	{
		tb_hash_item_list_t* list = hash->hash_list[i];
		if (list)
		{
			// free items
			if (hash->name_func.free || hash->data_func.free)
			{
				tb_size_t j = 0;
				tb_size_t m = list->size;
				for (j = 0; j < m; j++)
				{
					tb_byte_t* item = ((tb_byte_t*)&list[1]) + j * step;
					if (hash->name_func.free) hash->name_func.free(&hash->name_func, item);
					if (hash->data_func.free) hash->data_func.free(&hash->data_func, item + hash->name_func.size);
				}
			}

			// free list
			tb_free(list);
		}
		hash->hash_list[i] = tb_null;
	}

	// reset info
	hash->item_size = 0;
	hash->item_maxn = 0;
	tb_memset(&hash->hash_item, 0, sizeof(tb_hash_item_t));
}
tb_size_t tb_hash_itor(tb_handle_t handle, tb_cpointer_t name)
{
	// check
	tb_hash_t* hash = (tb_hash_t*)handle;
	tb_assert_and_check_return_val(hash, 0);

	// find
	tb_size_t buck = 0;
	tb_size_t item = 0;
	return tb_hash_item_find(hash, name, &buck, &item)? TB_HASH_INDEX_MAKE(buck + 1, item + 1) : 0;
}
tb_pointer_t tb_hash_get(tb_handle_t handle, tb_cpointer_t name)
{
	// check
	tb_hash_t* hash = (tb_hash_t*)handle;
	tb_assert_and_check_return_val(hash, tb_null);

	// find
	tb_size_t buck = 0;
	tb_size_t item = 0;
	if (tb_hash_item_find(hash, name, &buck, &item))
	{
		tb_pointer_t data = tb_null;
		if (tb_hash_item_at(hash, buck, item, tb_null, &data)) return (tb_pointer_t)data;
	}
	return tb_null;
}
tb_void_t tb_hash_del(tb_handle_t handle, tb_cpointer_t name)
{
	// check
	tb_hash_t* hash = (tb_hash_t*)handle;
	tb_assert_and_check_return(hash);

	// find it
	tb_size_t buck = 0;
	tb_size_t item = 0;
	if (tb_hash_item_find(hash, name, &buck, &item))
		tb_hash_iterator_delt((tb_iterator_t*)hash, TB_HASH_INDEX_MAKE(buck + 1, item + 1));
}
tb_size_t tb_hash_set(tb_handle_t handle, tb_cpointer_t name, tb_cpointer_t data)
{
	// check
	tb_hash_t* hash = (tb_hash_t*)handle;
	tb_assert_and_check_return_val(hash, 0);

	// the step
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return_val(step, 0);

	// find it
	tb_size_t buck = 0;
	tb_size_t item = 0;
	if (tb_hash_item_find(hash, name, &buck, &item))
	{
		// check
		tb_assert_and_check_return_val(buck < hash->hash_size, 0);

		// get list
		tb_hash_item_list_t* list = hash->hash_list[buck];
		tb_assert_and_check_return_val(list && list->size && item < list->size, 0);

		// copy data
		hash->data_func.copy(&hash->data_func, ((tb_byte_t*)&list[1]) + item * step + hash->name_func.size, data);
	}
	else
	{
		// check
		tb_assert_and_check_return_val(buck < hash->hash_size, 0);

		// get list
		tb_hash_item_list_t* list = hash->hash_list[buck];
		
		// insert item
		if (list)
		{
			// grow?
			if (list->size >= list->maxn)
			{
				tb_size_t maxn = tb_align_pow2(list->maxn + 1);
				tb_assert_and_check_return_val(maxn > list->maxn, 0);

				// realloc it
				list = tb_ralloc(list, sizeof(tb_hash_item_list_t) + maxn * step);	
				tb_assert_and_check_return_val(list, 0);

				// update the hash item maxn
				hash->item_maxn += maxn - list->maxn;

				// update maxn
				list->maxn = maxn;

				// reattach list
				hash->hash_list[buck] = list;
			}
			tb_assert_and_check_return_val(item <= list->size && list->size < list->maxn, 0);

			// move items
			if (item != list->size) tb_memmov(((tb_byte_t*)&list[1]) + (item + 1) * step, ((tb_byte_t*)&list[1]) + item * step, (list->size - item) * step);

			// dupl item
			list->size++;
			hash->name_func.dupl(&hash->name_func, ((tb_byte_t*)&list[1]) + item * step, name);
			hash->data_func.dupl(&hash->data_func, ((tb_byte_t*)&list[1]) + item * step + hash->name_func.size, data);
		}
		// create list for adding item
		else
		{
			// alloc list
			list = tb_malloc0(sizeof(tb_hash_item_list_t) + TB_HASH_GROW_MIN * step);
			tb_assert_and_check_return_val(list, 0);

			// init list
			list->size = 1;
			list->maxn = TB_HASH_GROW_MIN;
			hash->name_func.dupl(&hash->name_func, ((tb_byte_t*)&list[1]), name);
			hash->data_func.dupl(&hash->data_func, ((tb_byte_t*)&list[1]) + hash->name_func.size, data);

			// attach list
			hash->hash_list[buck] = list;

			// update the hash item maxn
			hash->item_maxn += list->maxn;
		}

		// update the hash item size
		hash->item_size++;
	}

	// ok?
	return TB_HASH_INDEX_MAKE(buck + 1, item + 1);
}
tb_size_t tb_hash_size(tb_handle_t handle)
{
	tb_hash_t* hash = (tb_hash_t*)handle;
	tb_assert_and_check_return_val(hash, 0);
	return hash->item_size;
}
tb_size_t tb_hash_maxn(tb_handle_t handle)
{
	tb_hash_t* hash = (tb_hash_t*)handle;
	tb_assert_and_check_return_val(hash, 0);
	return hash->item_maxn;
}
tb_void_t tb_hash_walk(tb_handle_t handle, tb_bool_t (*func)(tb_handle_t hash, tb_hash_item_t* item, tb_bool_t* bdel, tb_pointer_t data), tb_pointer_t data)
{
	// check
	tb_hash_t* hash = (tb_hash_t*)handle;
	tb_assert_and_check_return(hash && hash->hash_list && func);

	// step
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return(step);

	// bdel
	tb_bool_t bdel = tb_false;

	// item
	tb_hash_item_t item;

	// walk
	tb_size_t i = 0;
	tb_size_t n = hash->hash_size;
	for (i = 0; i < n; i++)
	{
		tb_hash_item_list_t* list = hash->hash_list[i];
		if (list)
		{
			tb_size_t j = 0;
			tb_size_t b = -1;
			tb_size_t l = list->size;
			for (j = 0; j < l; j++)
			{
				// init item
				tb_byte_t* it = ((tb_byte_t*)&list[1]) + j * step;
				item.name = hash->name_func.data(&hash->name_func, it);
				item.data = hash->data_func.data(&hash->data_func, it + hash->name_func.size);

				// init bdel
				bdel = tb_false;

				// callback: item
				if (!func(hash, &item, &bdel, data)) goto end;

				// free it?
				if (bdel)
				{
					// save
					if (b == -1) b = j;

					// free item
					if (hash->name_func.free) hash->name_func.free(&hash->name_func, it);
					if (hash->data_func.free) hash->data_func.free(&hash->data_func, it + hash->name_func.size);
				}

				// remove items?
				if (!bdel || j + 1 == l)
				{
					// has deleted items?
					if (b != -1)
					{
						// the removed items end
						tb_size_t e = !bdel? j : j + 1;
						if (e > b)
						{
							// the items number
							tb_size_t m = e - b;
							tb_assert(l >= m);
//							tb_trace("del: b: %u, e: %u, d: %u", b, e, bdel);

							// remove items
							if (e < l) tb_memmov(((tb_byte_t*)&list[1]) + b * step, ((tb_byte_t*)&list[1]) + e * step, (l - e) * step);

							// remove all?
							if (l > m) 
							{
								// update the list size
								l -= m;
								list->size = l;

								// update j
								j = b;
							}
							else
							{
								// update the list size
								l = 0;

								// free it
								tb_free(list);

								// reset
								list = tb_null;
								hash->hash_list[i] = tb_null;
							}

							// update the hash item size
							hash->item_size -= m;
						}
					}

					// reset
					b = -1;
				}
			}
		}
	}

	// callback: tail
	if (!func(hash, tb_null, &bdel, data)) goto end;

end:

	return ;
}
#ifdef __tb_debug__
tb_void_t tb_hash_dump(tb_handle_t handle)
{
	// check
	tb_hash_t* hash = (tb_hash_t*)handle;
	tb_assert_and_check_return(hash && hash->hash_list);

	// the step
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return(step);

	// trace
	tb_print("=========================================================");
	tb_print("hash_list: hash_size: %d, item_size: %u, item_maxn: %u", hash->hash_size, hash->item_size, hash->item_maxn);
	tb_print("=========================================================");

	// dump
	tb_size_t i = 0;
	tb_char_t name[4096];
	tb_char_t data[4096];
	for (i = 0; i < hash->hash_size; i++)
	{
		tb_hash_item_list_t* list = hash->hash_list[i];
		if (list)
		{
			tb_size_t j = 0;
			for (j = 0; j < list->size; j++)
			{
				tb_byte_t const* item = ((tb_byte_t*)&list[1]) + j * step;
				tb_pointer_t item_name = hash->name_func.data(&hash->name_func, item);
				tb_pointer_t item_data = hash->data_func.data(&hash->data_func, item + hash->name_func.size);

				if (hash->name_func.cstr && hash->data_func.cstr) 
					tb_print("bucket[%d:%d] => [%d]:\t%s\t\t=> %s", i, j
						, hash->name_func.hash(&hash->name_func, item_name, hash->hash_size)
						, hash->name_func.cstr(&hash->name_func, item_name, name, 4096)
						, hash->data_func.cstr(&hash->data_func, item_data, data, 4096));
				else if (hash->name_func.cstr) 
					tb_print("bucket[%d:%d] => [%d]:\t%s\t\t=> %x", i, j
						, hash->name_func.hash(&hash->name_func, item_name, hash->hash_size)
						, hash->name_func.cstr(&hash->name_func, item_name, name, 4096)
						, item_data);
				else if (hash->data_func.cstr) 
					tb_print("bucket[%d:%d] => [%d]:\t%x\t\t=> %x", i, j
						, hash->name_func.hash(&hash->name_func, item_name, hash->hash_size)
						, item_name
						, hash->data_func.cstr(&hash->data_func, item_data, data, 4096));
				else tb_print("bucket[%d:%d] => [%d]:\t%x\t\t=> %x", i, j
						, hash->name_func.hash(&hash->name_func, item_name, hash->hash_size)
						, item_name
						, item_data);
			}

			tb_print("bucket[%u]: size: %u, maxn: %u", i, list->size, list->maxn);
		}
	}

	tb_print("");
	tb_size_t itor = tb_iterator_head((tb_iterator_t*)hash);
	tb_size_t tail = tb_iterator_tail((tb_iterator_t*)hash);
	for (; itor != tail; itor = tb_iterator_next((tb_iterator_t*)hash, itor))
	{
		tb_hash_item_t const* item = tb_iterator_item((tb_iterator_t*)hash, itor);
		if (item)
		{
			if (hash->name_func.cstr && hash->data_func.cstr) 
				tb_print("item[%d] => [%d]:\t%s\t\t=> %s", itor
					, hash->name_func.hash(&hash->name_func, item->name, hash->hash_size)
					, hash->name_func.cstr(&hash->name_func, item->name, name, 4096)
					, hash->data_func.cstr(&hash->data_func, item->data, data, 4096));
			else if (hash->name_func.cstr) 
				tb_print("item[%d] => [%d]:\t%s\t\t=> %x", itor
					, hash->name_func.hash(&hash->name_func, item->name, hash->hash_size)
					, hash->name_func.cstr(&hash->name_func, item->name, name, 4096)
					, item->data);
			else if (hash->data_func.cstr) 
				tb_print("item[%d] => [%d]:\t%x\t\t=> %x", itor
					, hash->name_func.hash(&hash->name_func, item->name, hash->hash_size)
					, item->name
					, hash->data_func.cstr(&hash->data_func, item->data, data, 4096));
			else tb_print("item[%d] => [%d]:\t%x\t\t=> %x", itor
					, hash->name_func.hash(&hash->name_func, item->name, hash->hash_size)
					, item->name
					, item->data);
		}
	}
}
#endif
