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

/* /////////////////////////////////////////////////////////
 * details
 */

static tb_void_t tb_hash_item_free(tb_void_t* item, tb_void_t* priv)
{
	tb_hash_t* hash = priv;
	if (hash && hash->item_func.free)
	{
		hash->item_func.free((tb_byte_t*)item + 8, hash->item_func.priv);
	}
}
static tb_size_t tb_hash_find(tb_hash_t* hash, tb_void_t const* name)
{
	return 0;
}
/* /////////////////////////////////////////////////////////
 * func
 */

// cstring
static tb_void_t tb_hash_name_str_free_func(tb_void_t* name, tb_void_t* priv)
{
	if (name) tb_free(name);
}
static tb_void_t* tb_hash_name_str_dupl_func(tb_void_t const* name, tb_void_t* priv)
{
	return tb_strdup(name);
}
static tb_size_t tb_hash_name_str_hash_func(tb_void_t const* name, tb_size_t size, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(size, 0);

	tb_size_t h = 2166136261;
	tb_byte_t const* p = name;
	while (*p) h = 16777619 * h ^ (tb_size_t)(*p++);

	return (h & (size - 1));
}
static tb_int_t tb_hash_name_str_comp_func(tb_void_t const* lname, tb_void_t const* rname, tb_void_t* priv)
{
	return tb_strcmp(lname, rname);
}

// integer
static tb_void_t tb_hash_name_int_free_func(tb_void_t* name, tb_void_t* priv)
{
}
static tb_void_t* tb_hash_name_int_dupl_func(tb_void_t const* name, tb_void_t* priv)
{
	return name;
}
static tb_size_t tb_hash_name_int_hash_func(tb_void_t const* name, tb_size_t size, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(size, 0);
	return (((tb_size_t)name) ^ 0xdeadbeef) & (size - 1);
}
static tb_int_t tb_hash_name_int_comp_func(tb_void_t const* lname, tb_void_t const* rname, tb_void_t* priv)
{
	return (lname - rname);
}

// pointer
static tb_void_t tb_hash_name_ptr_free_func(tb_void_t* name, tb_void_t* priv)
{
}
static tb_void_t* tb_hash_name_ptr_dupl_func(tb_void_t const* name, tb_void_t* priv)
{
	return name;
}
static tb_size_t tb_hash_name_ptr_hash_func(tb_void_t const* name, tb_size_t size, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(size, 0);
	return (((tb_size_t)name) ^ 0xdeadbeef) & (size - 1);
}
static tb_int_t tb_hash_name_ptr_comp_func(tb_void_t const* lname, tb_void_t const* rname, tb_void_t* priv)
{
	return (lname - rname);
}

// memory
static tb_void_t tb_hash_name_mem_free_func(tb_void_t* name, tb_void_t* priv)
{
	if (name) tb_free(name);
}
static tb_void_t* tb_hash_name_mem_dupl_func(tb_void_t const* name, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(priv, TB_NULL);
	
	tb_void_t* p = tb_malloc((tb_size_t)priv);
	TB_ASSERT_RETURN_VAL(p, TB_NULL);

	return tb_memcpy(p, name, (tb_size_t)priv);
}
static tb_size_t tb_hash_name_mem_hash_func(tb_void_t const* name, tb_size_t size, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(size, 0);

	tb_size_t h = 2166136261;
	tb_byte_t const* p = name;
	tb_byte_t const* e = p + (tb_size_t)priv;
	while (p < e && *p) h = 16777619 * h ^ (tb_size_t)(*p++);

	return (h & (size - 1));
}
static tb_int_t tb_hash_name_mem_comp_func(tb_void_t const* lname, tb_void_t const* rname, tb_void_t* priv)
{
	return tb_memcmp(lname, rname, (tb_size_t)priv);
}

/* /////////////////////////////////////////////////////////
 * implemention
 */

tb_hash_t* tb_hash_init(tb_size_t step, tb_size_t size, tb_hash_name_func_t name_func, tb_hash_item_func_t const* item_func)
{
	tb_hash_t* hash = (tb_hash_t*)tb_calloc(1, sizeof(tb_hash_t));
	TB_ASSERT_RETURN_VAL(hash && !(size % 2), TB_NULL);

	// init hash func
	hash->name_func = name_func;
	if (item_func) hash->item_func = *item_func;

	// init item list
	tb_slist_item_func_t func;
	func.free = tb_hash_item_free;
	func.priv = hash;
	hash->item_list = tb_slist_init(step, size, &func);
	TB_ASSERT_GOTO(hash->item_list, fail);

	// init hash list
	hash->hash_size = size;
	hash->hash_list = tb_calloc(hash->hash_size, sizeof(tb_hash_bucket_t));
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
	tb_memset(hash->hash_list, 0, hash->hash_size * sizeof(tb_hash_bucket_t));

	// clear item list
	tb_slist_clear(hash->item_list);
}
tb_void_t* tb_hash_at(tb_hash_t* hash, tb_void_t const* name)
{
	//tb_size_t itor = tb_hash_find(hash, name);
	return TB_NULL;
}
tb_void_t const* tb_hash_const_at(tb_hash_t const* hash, tb_void_t const* name)
{
	TB_NOT_IMPLEMENT();
	return TB_NULL;
}
tb_void_t tb_hash_del(tb_hash_t* hash, tb_void_t const* name)
{
	TB_NOT_IMPLEMENT();
}
tb_void_t tb_hash_set(tb_hash_t* hash, tb_void_t const* name, tb_void_t const* item)
{
	TB_NOT_IMPLEMENT();
}
tb_void_t tb_hash_get(tb_hash_t* hash, tb_void_t const* name, tb_void_t* item)
{
	TB_NOT_IMPLEMENT();
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
tb_hash_name_func_t tb_hash_name_func_str()
{
	tb_hash_name_func_t func;
	func.hash = tb_hash_name_str_hash_func;
	func.comp = tb_hash_name_str_comp_func;
	func.dupl = tb_hash_name_str_dupl_func;
	func.free = tb_hash_name_str_free_func;
	func.priv = TB_NULL;
	return func;
}
tb_hash_name_func_t tb_hash_name_func_int()
{
	tb_hash_name_func_t func;
	func.hash = tb_hash_name_int_hash_func;
	func.comp = tb_hash_name_int_comp_func;
	func.dupl = tb_hash_name_int_dupl_func;
	func.free = tb_hash_name_int_free_func;
	func.priv = TB_NULL;
	return func;
}
tb_hash_name_func_t tb_hash_name_func_ptr()
{
	tb_hash_name_func_t func;
	func.hash = tb_hash_name_ptr_hash_func;
	func.comp = tb_hash_name_ptr_comp_func;
	func.dupl = tb_hash_name_ptr_dupl_func;
	func.free = tb_hash_name_ptr_free_func;
	func.priv = TB_NULL;
	return func;
}
tb_hash_name_func_t tb_hash_name_func_mem(tb_size_t size)
{
	tb_hash_name_func_t func;
	func.hash = tb_hash_name_mem_hash_func;
	func.comp = tb_hash_name_mem_comp_func;
	func.dupl = tb_hash_name_mem_dupl_func;
	func.free = tb_hash_name_mem_free_func;
	func.priv = (tb_size_t)size;
	return func;
}
tb_hash_pair_t tb_hash_itor_at(tb_hash_t* hash, tb_size_t itor)
{
	tb_hash_pair_t pair;
	pair.name = TB_NULL;
	pair.item = TB_NULL;
	return pair;
}
tb_size_t tb_hash_itor_head(tb_hash_t const* hash)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
tb_size_t tb_hash_itor_tail(tb_hash_t const* hash)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
tb_size_t tb_hash_itor_next(tb_hash_t const* hash, tb_size_t itor)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
tb_size_t tb_hash_itor_prev(tb_hash_t const* hash, tb_size_t itor)
{
	TB_NOT_IMPLEMENT();
	return 0;
}
#ifdef TB_DEBUG
tb_void_t tb_hash_dump(tb_hash_t const* hash)
{
	TB_NOT_IMPLEMENT();
}
#endif
