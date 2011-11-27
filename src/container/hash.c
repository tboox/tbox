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
 * macros
 */
#define TB_HASH_INDEX_MAKE(buck, item) 			((((item) & 0xff) << (TB_CPU_BITSIZE >> 1)) | (buck) & 0xff)
#define TB_HASH_INDEX_BUCK(index) 				((index) & 0xff)
#define TB_HASH_INDEX_ITEM(index) 				(((index) >> (TB_CPU_BITSIZE >> 1)) & 0xff)

/* /////////////////////////////////////////////////////////
 * details
 */

static tb_bool_t tb_hash_item_find(tb_hash_t* hash, tb_cpointer_t name, tb_size_t* pbuck, tb_size_t* pitem)
{
	tb_trace_noimpl();
	return TB_FALSE;
}
static tb_bool_t tb_hash_item_at(tb_hash_t* hash, tb_size_t buck, tb_size_t item, tb_size_t* pname, tb_size_t* pdata)
{
	tb_trace_noimpl();
	return TB_FALSE;
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
	for (i = 0; i < hash->hash_size; i++)
	{
		tb_hash_item_list_t* list = hash->hash_list[i];
		if (list)
		{
			// free items
			if (hash->name_func.free || hash->data_func.free)
			{
				tb_size_t j = 0;
				for (j = 0; j < list->size; j++)
				{
					tb_byte_t* item = list->data + j * step;
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
tb_void_t tb_hash_del(tb_hash_t* hash, tb_cpointer_t name)
{
	tb_assert_and_check_return(hash);
	tb_trace_noimpl();
}
tb_void_t tb_hash_set(tb_hash_t* hash, tb_cpointer_t name, tb_cpointer_t data)
{
	tb_assert_and_check_return(hash);
	tb_trace_noimpl();
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
	if (tb_hash_item_at(hash, TB_HASH_INDEX_BUCK(itor), TB_HASH_INDEX_ITEM(itor), &((tb_hash_t*)hash)->hash_item.name, &((tb_hash_t*)hash)->hash_item.data))
		return &(hash->hash_item);
	return TB_NULL;
}
tb_size_t tb_hash_itor_head(tb_hash_t const* hash)
{
	tb_assert_and_check_return_val(hash, 0);
	tb_trace_noimpl();
	return 0;
}
tb_size_t tb_hash_itor_tail(tb_hash_t const* hash)
{
	tb_assert_and_check_return_val(hash, 0);
	tb_trace_noimpl();
	return 0;
}
tb_size_t tb_hash_itor_next(tb_hash_t const* hash, tb_size_t itor)
{
	tb_assert_and_check_return_val(hash, 0);
	tb_trace_noimpl();
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
				tb_byte_t const* item = list->data + j * step;
				tb_pointer_t item_name = hash->name_func.data(&hash->name_func, item);
				tb_pointer_t item_data = hash->data_func.data(&hash->data_func, item + hash->name_func.size);

				if (hash->name_func.cstr && hash->data_func.cstr) 
					tb_print("bucket[%d:%d] => [%d]:\t%s\t\t=> %s", i, j
						, hash->name_func.hash(&hash->name_func, name, hash->hash_size)
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
}
#endif
