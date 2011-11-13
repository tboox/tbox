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
 * \file		hash.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "hash.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * details
 */
static tb_void_t tb_hash_item_free(tb_pointer_t item, tb_pointer_t priv)
{
	tb_hash_t* hash = priv;
	tb_assert_and_check_return(hash);

	if (hash->name_func.free) hash->name_func.free(&hash->name_func, item);
	if (hash->data_func.free) hash->data_func.free(&hash->data_func, (tb_byte_t*)item + hash->name_func.size);
}
static tb_size_t tb_hash_item_find(tb_hash_t* hash, tb_cpointer_t name, tb_size_t* pprev, tb_size_t* pbuck)
{
	tb_assert_and_check_return_val(hash && hash->item_list, 0);
	
	// comupte hash from name
	tb_size_t i = hash->name_func.hash(&hash->name_func, name, hash->hash_size);
	tb_assert_and_check_return_val(i < hash->hash_size, 0);

	// find item
	tb_int_t 	retn = 1;
	tb_size_t 	prev = 0;
	tb_size_t 	itor = hash->hash_list[i];
	tb_size_t 	tail = hash->hash_list[i + 1];
	for (; itor != tail; prev = itor, itor = tb_slist_itor_next(hash->item_list, itor))
	{
		// get item
		tb_byte_t const* item = tb_slist_itor_const_at(hash->item_list, itor);
		if (!item) break;

		// compare it
		retn = hash->name_func.comp(&hash->name_func, name, hash->name_func.data(&hash->name_func, item));
		if (retn <= 0) break;
	}

	// update prev
	if (pprev) *pprev = prev;

	// update bucket
	if (pbuck) *pbuck = i;

	// return the item
	return (!retn? itor : 0);
}
static tb_size_t tb_hash_prev_find(tb_hash_t* hash, tb_size_t bukt, tb_size_t* psize)
{
	tb_assert_and_check_return_val(hash && hash->hash_list, 0);
	tb_size_t 		 q = hash->hash_list[bukt];
	tb_size_t const* b = hash->hash_list;
	tb_size_t const* e = hash->hash_list + bukt;
	tb_size_t const* p = e;

	if (q == *b)
	{
		// update size
		if (psize) *psize = e - b + 1;
	}
	else
	{
		// find prev
		for (; p >= b && *p == q; p--) ;

		// update size
		if (psize) *psize = e - p;

		// return prev
		if (p >= b)
		{
			tb_size_t itor = p[0];
			tb_size_t tail = p[1];
			tb_size_t last = itor;
			for (; itor != tail; last = itor, itor = tb_slist_itor_next(hash->item_list, itor)) ;
			return last;
		}
	}

	return 0;
}

/* /////////////////////////////////////////////////////////
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

	// init item list
	tb_slist_item_func_t func;
	func.free = tb_hash_item_free;
	func.priv = hash;
	hash->item_list = tb_slist_init(name_func.size + data_func.size, size, &func);
	tb_assert_and_check_goto(hash->item_list, fail);

	// init hash list
	hash->hash_size = tb_align_pow2(size);
	hash->hash_list = tb_calloc(hash->hash_size + 1, sizeof(tb_size_t)); //!< + end
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

		// free item list
		if (hash->item_list) tb_slist_exit(hash->item_list);

		// free it
		tb_free(hash);
	}
}
tb_void_t tb_hash_clear(tb_hash_t* hash)
{
	tb_assert_and_check_return(hash && hash->hash_list && hash->item_list);

	// clear hash list
	tb_memset(hash->hash_list, 0, (hash->hash_size + 1) * sizeof(tb_size_t));

	// clear item list
	tb_slist_clear(hash->item_list);
}
tb_pointer_t tb_hash_at(tb_hash_t* hash, tb_cpointer_t name)
{
	return (tb_pointer_t)tb_hash_const_at(hash, name);
}
tb_cpointer_t tb_hash_const_at(tb_hash_t const* hash, tb_cpointer_t name)
{
	tb_assert_and_check_return_val(hash, TB_NULL);

	tb_size_t itor = tb_hash_item_find(hash, name, TB_NULL, TB_NULL);
	if (itor)
	{
		tb_hash_item_t* it = tb_hash_itor_at(hash, itor);
		if (it) return it->data;
	}
	return TB_NULL;
}
tb_void_t tb_hash_del(tb_hash_t* hash, tb_cpointer_t name)
{
	tb_assert_and_check_return(hash && hash->item_list);

	// find it
	tb_size_t prev = 0;
	tb_size_t bukt = 0;
	tb_size_t size = 0;
	tb_size_t itor = tb_hash_item_find(hash, name, &prev, &bukt);
	if (itor) 
	{
		// find prev if at head
		if (!prev) prev = tb_hash_prev_find(hash, bukt, &size);
		tb_assert(bukt + 1 >= size);

		// remove it
		tb_size_t next = tb_slist_remove_next(hash->item_list, prev);

		// update hash list
		if (size > 32 && sizeof(tb_size_t) == 4) 
			tb_memset_u32(hash->hash_list + bukt + 1 - size, next, size);
		else if (size) while (size--) hash->hash_list[bukt - size] = next;
	}
}
tb_void_t tb_hash_set(tb_hash_t* hash, tb_cpointer_t name, tb_cpointer_t data)
{
	tb_assert_and_check_return(hash && hash->item_list);

	// find it
	tb_size_t prev = 0;
	tb_size_t bukt = 0;
	tb_size_t size = 0;
	tb_size_t itor = tb_hash_item_find(hash, name, &prev, &bukt);
	if (itor) 
	{
		// update data if exists
		tb_byte_t* item = tb_slist_itor_at(hash->item_list, itor);
		if (item) 
		{
			// set data 
			hash->data_func.copy(&hash->data_func, item + hash->name_func.size, data);
		}
	}
	else 
	{
		// find prev if at head
		if (!prev) prev = tb_hash_prev_find(hash, bukt, &size);
		tb_assert(bukt + 1 >= size);

		// insert it and set data if not exists
		itor = tb_slist_insert_next(hash->item_list, prev, TB_NULL);
		//tb_trace("%d %d %d %d", prev, bukt, size, itor);

		// ok?
		if (itor) 
		{
			// set item
			tb_byte_t* item = tb_slist_itor_at(hash->item_list, itor);
			if (item) 
			{
				// set name
				hash->name_func.dupl(&hash->name_func, item, name);

				// set data
				hash->data_func.dupl(&hash->data_func, item + hash->name_func.size, data);
			}

			// update hash list
			if (size > 32 && sizeof(tb_size_t) == 4) 
				tb_memset_u32(hash->hash_list + bukt + 1 - size, itor, size);
			else if (size) while (size--) hash->hash_list[bukt - size] = itor;
		}
	}
}
tb_size_t tb_hash_size(tb_hash_t const* hash)
{
	tb_assert_and_check_return_val(hash && hash->item_list, 0);
	return tb_slist_size(hash->item_list);
}
tb_size_t tb_hash_maxn(tb_hash_t const* hash)
{
	tb_assert_and_check_return_val(hash && hash->item_list, 0);
	return tb_slist_maxn(hash->item_list);
}
tb_hash_item_t* tb_hash_itor_at(tb_hash_t* hash, tb_size_t itor)
{
	return (tb_hash_item_t*)tb_hash_itor_const_at(hash, itor);
}
tb_hash_item_t const* tb_hash_itor_const_at(tb_hash_t const* hash, tb_size_t itor)
{	
	tb_assert_and_check_return_val(hash && hash->item_list && itor, TB_NULL);
	tb_byte_t const* item = tb_slist_itor_const_at(hash->item_list, itor);
	if (item) 
	{
		((tb_hash_t*)hash)->hash_item.name = hash->name_func.data(&hash->name_func, item);
		((tb_hash_t*)hash)->hash_item.data = hash->data_func.data(&hash->data_func, item + hash->name_func.size);
		return &(hash->hash_item);
	}
	return TB_NULL;
}
tb_size_t tb_hash_itor_head(tb_hash_t const* hash)
{
	tb_assert_and_check_return_val(hash && hash->item_list, 0);
	return tb_slist_itor_head(hash->item_list);
}
tb_size_t tb_hash_itor_tail(tb_hash_t const* hash)
{
	tb_assert_and_check_return_val(hash && hash->item_list, 0);
	return tb_slist_itor_tail(hash->item_list);
}
tb_size_t tb_hash_itor_next(tb_hash_t const* hash, tb_size_t itor)
{
	tb_assert_and_check_return_val(hash && hash->item_list, 0);
	return tb_slist_itor_next(hash->item_list, itor);
}
#ifdef TB_DEBUG
tb_void_t tb_hash_dump(tb_hash_t const* hash)
{
	tb_assert_and_check_return(hash && hash->item_list && hash->hash_list);

	// dump hash list
	tb_print("=========================================================");
	tb_print("hash_list: size: %d", hash->hash_size);
	tb_print("=========================================================");
	tb_size_t i = 0;
	tb_char_t name[4096];
	tb_char_t data[4096];
	for (i = 0; i < hash->hash_size; i++)
	{
		tb_size_t itor = hash->hash_list[i];
		tb_size_t tail = hash->hash_list[i + 1];
		if (itor != tail)
		{
			tb_size_t n = 0;
			for (; itor != tail; itor = tb_slist_itor_next(hash->item_list, itor), n++)
			{
				tb_byte_t const* item = tb_slist_itor_const_at(hash->item_list, itor);
				tb_pointer_t item_name = hash->name_func.data(&hash->name_func, item);
				tb_pointer_t item_data = hash->data_func.data(&hash->data_func, item + hash->name_func.size);

				if (hash->name_func.cstr && hash->data_func.cstr) 
					tb_print("bucket[%d:%d] => [%d]:\t%s\t\t=> %s", i
						, hash->name_func.hash(&hash->name_func, name, hash->hash_size)
						, itor
						, hash->name_func.cstr(&hash->name_func, item_name, name, 4096)
						, hash->data_func.cstr(&hash->data_func, item_data, data, 4096));
				else if (hash->name_func.cstr) 
					tb_print("bucket[%d:%d] => [%d]:\t%s\t\t=> %x", i
						, hash->name_func.hash(&hash->name_func, item_name, hash->hash_size)
						, itor
						, hash->name_func.cstr(&hash->name_func, item_name, name, 4096)
						, item_data);
				else if (hash->data_func.cstr) 
					tb_print("bucket[%d:%d] => [%d]:\t%x\t\t=> %x", i
						, hash->name_func.hash(&hash->name_func, item_name, hash->hash_size)
						, itor
						, item_name
						, hash->data_func.cstr(&hash->data_func, item_data, data, 4096));
				else tb_print("bucket[%d:%d] => [%d]:\t%x\t\t=> %x", i
						, hash->name_func.hash(&hash->name_func, item_name, hash->hash_size)
						, itor
						, item_name
						, item_data);

			}
			tb_print("bucket[%u]: size: %u", i, n);
		}
	}
	tb_print("");

	// dump item list
	tb_print("=========================================================");
	tb_print("item_list: size: %d, maxn: %d", tb_slist_size(hash->item_list), tb_slist_maxn(hash->item_list));
	tb_print("=========================================================");

	tb_size_t itor = tb_slist_itor_head(hash->item_list);
	tb_size_t tail = tb_slist_itor_tail(hash->item_list);
	for (; itor != tail; itor = tb_slist_itor_next(hash->item_list, itor))
	{
		tb_byte_t const* item = tb_slist_itor_const_at(hash->item_list, itor);
		tb_pointer_t item_name = hash->name_func.data(&hash->name_func, item);
		tb_pointer_t item_data = hash->data_func.data(&hash->data_func, item + hash->name_func.size);

		if (hash->name_func.cstr && hash->data_func.cstr) 
			tb_print("item[%d]:\t%s\t\t=> %s", itor
				, hash->name_func.cstr(&hash->name_func, item_name, name, 4096)
				, hash->data_func.cstr(&hash->data_func, item_data, data, 4096));
		else if (hash->name_func.cstr) 
			tb_print("item[%d]:\t%s\t\t=> %x", itor
				, hash->name_func.cstr(&hash->name_func, item_name, name, 4096)
				, item_data);
		else if (hash->data_func.cstr) 
			tb_print("item[%d]:\t%x\t\t=> %x", itor
				, item_name
				, hash->data_func.cstr(&hash->data_func, item_data, data, 4096));
		else tb_print("item[%d]:\t%x\t\t=> %x", itor
				, item_name
				, item_data);
	}
	tb_print("");
}
#endif
