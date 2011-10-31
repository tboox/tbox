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
#include "../utils/utils.h"
#include "../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * details
 */
static tb_void_t tb_hash_item_free(tb_void_t* item, tb_void_t* priv)
{
	tb_hash_t* hash = priv;
	TB_ASSERT_RETURN(hash);

	if (hash->name_func.free) hash->name_func.free(((tb_hash_item_t*)item)->name, hash->name_func.priv);
	if (hash->data_func.free) hash->data_func.free(((tb_hash_item_t*)item)->data, hash->data_func.priv);
}
static tb_size_t tb_hash_item_find(tb_hash_t* hash, tb_void_t const* name, tb_size_t* pprev, tb_size_t* pbuck)
{
	TB_ASSERT_RETURN_VAL(hash && hash->item_list && hash->name_func.hash && hash->name_func.comp, 0);

	// comupte hash from name
	tb_size_t i = hash->name_func.hash(name, hash->hash_size, hash->name_func.priv);
	TB_ASSERT_RETURN_VAL(i < hash->hash_size, 0);

	// find item
	tb_int_t 	retn = 1;
	tb_size_t 	prev = 0;
	tb_size_t 	itor = hash->hash_list[i];
	tb_size_t 	tail = hash->hash_list[i + 1];
	for (; itor != tail; prev = itor, itor = tb_slist_itor_next(hash->item_list, itor))
	{
		// get item
		tb_hash_item_t const* item = tb_slist_itor_const_at(hash->item_list, itor);
		if (!item) break;
		
		// compare it
		retn = hash->name_func.comp(name, item->name, hash->name_func.priv);
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
	TB_ASSERT_RETURN_VAL(hash && hash->hash_list, 0);
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

tb_hash_t* tb_hash_init(tb_size_t step, tb_size_t size, tb_item_func_t name_func, tb_item_func_t data_func)
{
	TB_ASSERT_RETURN_VAL(!(size % 2), TB_NULL);

	// alloc hash
	tb_hash_t* hash = (tb_hash_t*)tb_calloc(1, sizeof(tb_hash_t));
	TB_ASSERT_RETURN_VAL(hash, TB_NULL);

	// init hash func
	hash->name_func = name_func;
	hash->data_func = data_func;

	// init item list
	tb_slist_item_func_t func;
	func.free = tb_hash_item_free;
	func.priv = hash;
	hash->item_list = tb_slist_init(sizeof(tb_void_t*) << 1, size, &func); //!< name + data
	TB_ASSERT_GOTO(hash->item_list, fail);

	// init hash list
	hash->hash_size = size;
	hash->hash_list = tb_calloc(hash->hash_size + 1, sizeof(tb_size_t)); //!< + end
	TB_ASSERT_GOTO(hash->hash_list, fail);

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
	TB_ASSERT_RETURN(hash && hash->hash_list && hash->item_list);

	// clear hash list
	tb_memset(hash->hash_list, 0, (hash->hash_size + 1) * sizeof(tb_size_t));

	// clear item list
	tb_slist_clear(hash->item_list);
}
tb_void_t* tb_hash_at(tb_hash_t* hash, tb_void_t const* name)
{
	return (tb_void_t*)tb_hash_const_at(hash, name);
}
tb_void_t const* tb_hash_const_at(tb_hash_t const* hash, tb_void_t const* name)
{
	TB_ASSERT_RETURN_VAL(hash, TB_NULL);

	tb_size_t itor = tb_hash_item_find(hash, name, TB_NULL, TB_NULL);
	if (itor)
	{
		tb_hash_item_t* it = tb_hash_itor_at(hash, itor);
		if (it) return it->data;
	}
	return TB_NULL;
}
tb_void_t tb_hash_del(tb_hash_t* hash, tb_void_t const* name)
{
	TB_ASSERT_RETURN(hash && hash->item_list);

	// find it
	tb_size_t prev = 0;
	tb_size_t bukt = 0;
	tb_size_t size = 0;
	tb_size_t itor = tb_hash_item_find(hash, name, &prev, &bukt);
	if (itor) 
	{
		// find prev if at head
		if (!prev) prev = tb_hash_prev_find(hash, bukt, &size);
		TB_ASSERT(bukt + 1 >= size);

		// remove it
		tb_size_t next = tb_slist_remove_next(hash->item_list, prev);

		// update hash list
		if (size > 32 && sizeof(tb_size_t) == 4) 
			tb_memset_u32(hash->hash_list + bukt + 1 - size, next, size);
		else if (size) while (size--) hash->hash_list[bukt - size] = next;
	}
}
tb_void_t tb_hash_set(tb_hash_t* hash, tb_void_t const* name, tb_void_t const* data)
{
	TB_ASSERT_RETURN(hash && hash->item_list);

	// find it
	tb_size_t prev = 0;
	tb_size_t bukt = 0;
	tb_size_t size = 0;
	tb_size_t itor = tb_hash_item_find(hash, name, &prev, &bukt);
	if (itor) 
	{
		// update data if exists
		tb_hash_item_t* it = tb_hash_itor_at(hash, itor);
		if (it) 
		{
			// set data 
			if (hash->data_func.dupl) it->data = hash->data_func.dupl(data, hash->data_func.priv);
			else it->data = TB_NULL;
		}
	}
	else 
	{
		// find prev if at head
		if (!prev) prev = tb_hash_prev_find(hash, bukt, &size);
		TB_ASSERT(bukt + 1 >= size);

		// insert it and set data if not exists
		itor = tb_slist_insert_next(hash->item_list, prev, TB_NULL);
		//TB_DBG("%d %d %d %d", prev, bukt, size, itor);

		// ok?
		if (itor) 
		{
			// set item
			tb_hash_item_t* it = tb_slist_itor_at(hash->item_list, itor);
			if (it) 
			{
				// set name
				if (hash->name_func.dupl) it->name = hash->name_func.dupl(name, hash->name_func.priv);
				else it->name = TB_NULL;

				// set data
				if (hash->data_func.dupl) it->data = hash->data_func.dupl(data, hash->data_func.priv);
				else it->data = TB_NULL;
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
	TB_ASSERT_RETURN_VAL(hash && hash->item_list, 0);
	return tb_slist_size(hash->item_list);
}
tb_size_t tb_hash_maxn(tb_hash_t const* hash)
{
	TB_ASSERT_RETURN_VAL(hash && hash->item_list, 0);
	return tb_slist_maxn(hash->item_list);
}
tb_hash_item_t* tb_hash_itor_at(tb_hash_t* hash, tb_size_t itor)
{
	TB_ASSERT_RETURN_VAL(hash && hash->item_list && itor, TB_NULL);
	return tb_slist_itor_at(hash->item_list, itor);
}
tb_hash_item_t const* tb_hash_itor_const_at(tb_hash_t* hash, tb_size_t itor)
{	
	TB_ASSERT_RETURN_VAL(hash && hash->item_list && itor, TB_NULL);
	return tb_slist_itor_const_at(hash->item_list, itor);
}

tb_size_t tb_hash_itor_head(tb_hash_t const* hash)
{
	TB_ASSERT_RETURN_VAL(hash && hash->item_list, 0);
	return tb_slist_itor_head(hash->item_list);
}
tb_size_t tb_hash_itor_tail(tb_hash_t const* hash)
{
	TB_ASSERT_RETURN_VAL(hash && hash->item_list, 0);
	return tb_slist_itor_tail(hash->item_list);
}
tb_size_t tb_hash_itor_next(tb_hash_t const* hash, tb_size_t itor)
{
	TB_ASSERT_RETURN_VAL(hash && hash->item_list, 0);
	return tb_slist_itor_next(hash->item_list, itor);
}
#ifdef TB_DEBUG
tb_void_t tb_hash_dump(tb_hash_t const* hash)
{
	TB_ASSERT_RETURN(hash && hash->item_list && hash->hash_list);

	// dump hash list
	tb_printf("=========================================================\n");
	tb_printf("hash_list: size: %d\n", hash->hash_size);
	tb_printf("=========================================================\n");
	tb_size_t i = 0;
	tb_char_t name[4096];
	tb_char_t data[4096];
	for (i = 0; i < hash->hash_size; i++)
	{
		tb_size_t itor = hash->hash_list[i];
		tb_size_t tail = hash->hash_list[i + 1];
		if (itor != tail)
		{
			tb_size_t num = 0;
			for (; itor != tail; itor = tb_slist_itor_next(hash->item_list, itor))
			{
				tb_hash_item_t const* item = tb_slist_itor_const_at(hash->item_list, itor);

				if (hash->name_func.cstr && hash->data_func.cstr) 
					tb_printf("bucket[%d:%d] => [%d]:\t%s\t\t=> %s\n", i
						, hash->name_func.hash(item->name, hash->hash_size, hash->name_func.priv)
						, itor
						, hash->name_func.cstr(item->name, name, 4096, hash->name_func.priv)
						, hash->data_func.cstr(item->data, data, 4096, hash->data_func.priv));
				else if (hash->name_func.cstr) 
					tb_printf("bucket[%d:%d] => [%d]:\t%s\t\t=> %x\n", i
						, hash->name_func.hash(item->name, hash->hash_size, hash->name_func.priv)
						, itor
						, hash->name_func.cstr(item->name, name, 4096, hash->name_func.priv)
						, item->data);
				else if (hash->data_func.cstr) 
					tb_printf("bucket[%d:%d] => [%d]:\t%x\t\t=> %x\n", i
						, hash->name_func.hash(item->name, hash->hash_size, hash->name_func.priv)
						, itor
						, item->name
						, hash->data_func.cstr(item->data, data, 4096, hash->data_func.priv));
				else tb_printf("bucket[%d:%d] => [%d]:\t%x\t\t=> %x\n", i
						, hash->name_func.hash(item->name, hash->hash_size, hash->name_func.priv)
						, itor
						, item->name
						, item->data);

			}
		}
	}
	tb_printf("\n");

	// dump item list
	tb_printf("=========================================================\n");
	tb_printf("item_list: size: %d, maxn: %d\n", tb_slist_size(hash->item_list), tb_slist_maxn(hash->item_list));
	tb_printf("=========================================================\n");

	tb_size_t itor = tb_slist_itor_head(hash->item_list);
	tb_size_t tail = tb_slist_itor_tail(hash->item_list);
	for (; itor != tail; itor = tb_slist_itor_next(hash->item_list, itor))
	{
		tb_hash_item_t const* item = tb_slist_itor_const_at(hash->item_list, itor);

		if (hash->name_func.cstr && hash->data_func.cstr) 
			tb_printf("item[%d]:\t%s\t\t=> %s\n", itor
				, hash->name_func.cstr(item->name, name, 4096, hash->name_func.priv)
				, hash->data_func.cstr(item->data, data, 4096, hash->data_func.priv));
		else if (hash->name_func.cstr) 
			tb_printf("item[%d]:\t%s\t\t=> %x\n", itor
				, hash->name_func.cstr(item->name, name, 4096, hash->name_func.priv)
				, item->data);
		else if (hash->data_func.cstr) 
			tb_printf("item[%d]:\t%x\t\t=> %x\n", itor
				, item->name
				, hash->data_func.cstr(item->data, data, 4096, hash->data_func.priv));
		else tb_printf("item[%d]:\t%x\t\t=> %x\n", itor
				, item->name
				, item->data);
	}
	tb_printf("\n");
}
#endif
