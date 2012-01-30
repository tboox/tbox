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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		hash.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "hash.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// index
#define TB_HASH_INDEX_MAKE(buck, item) 			((((item) & 0xff) << (TB_CPU_BITSIZE >> 1)) | (buck) & 0xff)
#define TB_HASH_INDEX_BUCK(index) 				((index) & 0xff)
#define TB_HASH_INDEX_ITEM(index) 				(((index) >> (TB_CPU_BITSIZE >> 1)) & 0xff)

// grow
#define TB_HASH_GROW_MIN 						(8)

/* ///////////////////////////////////////////////////////////////////////
 * details
 */

#if 0
static tb_bool_t tb_hash_item_find(tb_hash_t* hash, tb_cpointer_t name, tb_size_t* pbuck, tb_size_t* pitem)
{
	tb_assert_and_check_return_val(hash && hash->hash_list && hash->hash_size, TB_FALSE);
	
	// get step
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return_val(step, TB_FALSE);

	// comupte hash from name
	tb_size_t buck = hash->name_func.hash(&hash->name_func, name, hash->hash_size);
	tb_assert_and_check_return_val(buck < hash->hash_size, TB_FALSE);

	// update bucket
	if (pbuck) *pbuck = buck;

	// get list
	tb_hash_item_list_t* list = hash->hash_list[buck];
	tb_check_return_val(list && list->size, TB_FALSE);

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
	return !r? TB_TRUE : TB_FALSE;
}
#else
static tb_bool_t tb_hash_item_find(tb_hash_t* hash, tb_cpointer_t name, tb_size_t* pbuck, tb_size_t* pitem)
{
	tb_assert_and_check_return_val(hash && hash->hash_list && hash->hash_size, TB_FALSE);
	
	// get step
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return_val(step, TB_FALSE);

	// comupte hash from name
	tb_size_t buck = hash->name_func.hash(&hash->name_func, name, hash->hash_size);
	tb_assert_and_check_return_val(buck < hash->hash_size, TB_FALSE);

	// update bucket
	if (pbuck) *pbuck = buck;

	// get list
	tb_hash_item_list_t* list = hash->hash_list[buck];
	tb_check_return_val(list && list->size, TB_FALSE);

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
	return !t? TB_TRUE : TB_FALSE;
}
#endif
static tb_bool_t tb_hash_item_at(tb_hash_t* hash, tb_size_t buck, tb_size_t item, tb_pointer_t* pname, tb_pointer_t* pdata)
{
	tb_assert_and_check_return_val(hash && hash->hash_list && hash->hash_size && buck < hash->hash_size, TB_FALSE);
	
	// get step
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return_val(step, TB_FALSE);

	// get list
	tb_hash_item_list_t* list = hash->hash_list[buck];
	tb_check_return_val(list && list->size && item < list->size, TB_FALSE);

	// get name
	if (pname) *pname = hash->name_func.data(&hash->name_func, ((tb_byte_t*)&list[1]) + item * step);
	
	// get data
	if (pdata) *pdata = hash->data_func.data(&hash->data_func, ((tb_byte_t*)&list[1]) + item * step + hash->name_func.size);

	return TB_TRUE;
}
/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */

tb_hash_t* tb_hash_init(tb_size_t size, tb_item_func_t name_func, tb_item_func_t data_func)
{
	// check
	tb_assert_and_check_return_val(size, TB_NULL);
	tb_assert_and_check_return_val(name_func.size && name_func.hash && name_func.comp && name_func.data && name_func.dupl, TB_NULL);
	tb_assert_and_check_return_val(data_func.size && data_func.data && data_func.dupl && data_func.copy, TB_NULL);

	// alloc hash
	tb_hash_t* hash = (tb_hash_t*)tb_calloc(1, sizeof(tb_hash_t));
	tb_assert_and_check_return_val(hash, TB_NULL);

	// init hash func
	hash->name_func = name_func;
	hash->data_func = data_func;

	// init hash list
	hash->hash_size = tb_align_pow2(size);
	hash->hash_list = tb_calloc(hash->hash_size, sizeof(tb_size_t));
	tb_assert_and_check_goto(hash->hash_list, fail);

	return hash;
fail:
	if (hash) tb_hash_exit(hash);
	return TB_NULL;
}
tb_void_t tb_hash_exit(tb_hash_t* hash)
{
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
tb_void_t tb_hash_clear(tb_hash_t* hash)
{
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
		hash->hash_list[i] = TB_NULL;
	}

	// reset info
	hash->item_size = 0;
	hash->item_maxn = 0;
	tb_memset(&hash->hash_item, 0, sizeof(tb_hash_item_t));
}
tb_pointer_t tb_hash_at(tb_hash_t* hash, tb_cpointer_t name)
{
	return (tb_pointer_t)tb_hash_const_at(hash, name);
}
tb_cpointer_t tb_hash_const_at(tb_hash_t const* hash, tb_cpointer_t name)
{
	tb_assert_and_check_return_val(hash, TB_NULL);
	tb_size_t buck = 0;
	tb_size_t item = 0;
	if (tb_hash_item_find(hash, name, &buck, &item))
	{
		tb_pointer_t data = TB_NULL;
		if (tb_hash_item_at(hash, buck, item, TB_NULL, &data)) return (tb_cpointer_t)data;
	}
	return TB_NULL;
}
tb_pointer_t tb_hash_get(tb_hash_t* hash, tb_cpointer_t name)
{
	return (tb_pointer_t)tb_hash_const_at(hash, name);
}
tb_void_t tb_hash_del(tb_hash_t* hash, tb_cpointer_t name)
{
	tb_assert_and_check_return(hash);

	tb_size_t buck = 0;
	tb_size_t item = 0;
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return(step);

	// find it
	if (tb_hash_item_find(hash, name, &buck, &item))
	{
		// check
		tb_assert_and_check_return(buck < hash->hash_size);

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
			hash->hash_list[buck] = TB_NULL;
		}

		// update the hash item size
		hash->item_size--;
	}
}
tb_void_t tb_hash_set(tb_hash_t* hash, tb_cpointer_t name, tb_cpointer_t data)
{
	tb_assert_and_check_return(hash);

	tb_size_t buck = 0;
	tb_size_t item = 0;
	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return(step);

	// find it
	if (tb_hash_item_find(hash, name, &buck, &item))
	{
		// check
		tb_assert_and_check_return(buck < hash->hash_size);

		// get list
		tb_hash_item_list_t* list = hash->hash_list[buck];
		tb_assert_and_check_return(list && list->size && item < list->size);

		// copy data
		hash->data_func.copy(&hash->data_func, ((tb_byte_t*)&list[1]) + item * step + hash->name_func.size, data);
	}
	else
	{
		// check
		tb_assert_and_check_return(buck < hash->hash_size);

		// get list
		tb_hash_item_list_t* list = hash->hash_list[buck];
		
		// insert item
		if (list)
		{
			// grow?
			if (list->size >= list->maxn)
			{
				tb_size_t maxn = tb_align_pow2(list->maxn + 1);
				tb_assert_and_check_return(maxn > list->maxn);

				// realloc it
				list = tb_realloc(list, sizeof(tb_hash_item_list_t) + maxn * step);	
				tb_assert_and_check_return(list);

				// update the hash item maxn
				hash->item_maxn += maxn - list->maxn;

				// update maxn
				list->maxn = maxn;

				// reattach list
				hash->hash_list[buck] = list;
			}
			tb_assert_and_check_return(item <= list->size && list->size < list->maxn);

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
			list = tb_calloc(1, sizeof(tb_hash_item_list_t) + TB_HASH_GROW_MIN * step);
			tb_assert_and_check_return(list);

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
}
tb_size_t tb_hash_size(tb_hash_t const* hash)
{
	tb_assert_and_check_return_val(hash, 0);
	return hash->item_size;
}
tb_size_t tb_hash_maxn(tb_hash_t const* hash)
{
	tb_assert_and_check_return_val(hash, 0);
	return hash->item_maxn;
}
tb_hash_item_t* tb_hash_itor_at(tb_hash_t* hash, tb_size_t itor)
{
	return (tb_hash_item_t*)tb_hash_itor_const_at(hash, itor);
}
tb_hash_item_t const* tb_hash_itor_const_at(tb_hash_t const* hash, tb_size_t itor)
{	
	tb_assert_and_check_return_val(hash && itor, TB_NULL);

	// get buck & item
	tb_size_t buck = TB_HASH_INDEX_BUCK(itor);
	tb_size_t item = TB_HASH_INDEX_ITEM(itor);
	tb_assert_and_check_return_val(buck && item, TB_NULL);

	// get item
	if (tb_hash_item_at(hash, buck - 1, item - 1, &((tb_hash_t*)hash)->hash_item.name, &((tb_hash_t*)hash)->hash_item.data))
		return &(hash->hash_item);
	return TB_NULL;
}
tb_size_t tb_hash_itor_head(tb_hash_t const* hash)
{
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
tb_size_t tb_hash_itor_tail(tb_hash_t const* hash)
{
	tb_assert_and_check_return_val(hash, 0);
	return 0;
}
tb_size_t tb_hash_itor_next(tb_hash_t const* hash, tb_size_t itor)
{
	tb_assert_and_check_return_val(hash && hash->hash_list && hash->hash_size, 0);

	// get buck & item
	tb_size_t buck = TB_HASH_INDEX_BUCK(itor);
	tb_size_t item = TB_HASH_INDEX_ITEM(itor);
	tb_assert_and_check_return_val(buck && item, 0);

	// get the current bucket & item
	buck--;
	item--;
	tb_assert_and_check_return_val(buck < hash->hash_size, 0);

	// find the next from the current bucket first
	if (hash->hash_list[buck] && item + 1 < hash->hash_list[buck]->size) return TB_HASH_INDEX_MAKE(buck + 1, item + 2);

	// find the next from the next buckets
	tb_size_t i;
	tb_size_t j;
	tb_size_t n = hash->hash_size;
	for (i = buck + 1; i < n; i++)
	{
		tb_hash_item_list_t* list = hash->hash_list[i];
		if (list && list->size) return TB_HASH_INDEX_MAKE(i + 1, 1);
	}
	return 0;
}
#ifdef TB_DEBUG
tb_void_t tb_hash_dump(tb_hash_t const* hash)
{
	tb_assert_and_check_return(hash && hash->hash_list);

	tb_size_t step = hash->name_func.size + hash->data_func.size;
	tb_assert_and_check_return(step);

	tb_print("=========================================================");
	tb_print("hash_list: hash_size: %d, item_size: %u, item_maxn: %u", hash->hash_size, hash->item_size, hash->item_maxn);
	tb_print("=========================================================");

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
	tb_size_t itor = tb_hash_itor_head(hash);
	tb_size_t tail = tb_hash_itor_tail(hash);
	for (; itor != tail; itor = tb_hash_itor_next(hash, itor))
	{
		tb_hash_item_t const* item = tb_hash_itor_const_at(hash, itor);
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
