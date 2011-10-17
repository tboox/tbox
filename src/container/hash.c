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
 * implemention
 */

tb_hash_t* tb_hash_init(tb_size_t step, tb_size_t size, tb_hash_name_func_t name_func, tb_hash_item_func_t const* item_func)
{
	return TB_NULL;
}
tb_void_t tb_hash_exit(tb_hash_t* hash)
{
}
tb_void_t tb_hash_clear(tb_hash_t* hash)
{
}
tb_byte_t* tb_hash_at(tb_hash_t* hash, tb_void_t const* name)
{
	return TB_NULL;
}
tb_byte_t const* tb_hash_const_at(tb_hash_t const* hash, tb_void_t const* name)
{
	return TB_NULL;
}
tb_void_t tb_hash_del(tb_hash_t* hash, tb_void_t const* name)
{
}
tb_void_t tb_hash_set(tb_hash_t* hash, tb_void_t const* name, tb_byte_t const* item)
{
}
tb_void_t tb_hash_get(tb_hash_t* hash, tb_void_t const* name, tb_byte_t* item)
{
}
tb_size_t tb_hash_size(tb_hash_t const* hash)
{
	return 0;
}
tb_size_t tb_hash_maxn(tb_hash_t const* hash)
{
	return 0;
}
tb_hash_name_func_t tb_hash_name_func_str(tb_void_t* priv)
{
	tb_hash_name_func_t func;
	func.hash = TB_NULL;
	func.comp = TB_NULL;
	func.dupl = TB_NULL;
	func.free = TB_NULL;
	func.priv = priv;
	return func;
}
tb_hash_name_func_t tb_hash_name_func_int(tb_void_t* priv)
{
	tb_hash_name_func_t func;
	func.hash = TB_NULL;
	func.comp = TB_NULL;
	func.dupl = TB_NULL;
	func.free = TB_NULL;
	func.priv = priv;
	return func;
}
tb_hash_name_func_t tb_hash_name_func_ptr(tb_void_t* priv)
{
	tb_hash_name_func_t func;
	func.hash = TB_NULL;
	func.comp = TB_NULL;
	func.dupl = TB_NULL;
	func.free = TB_NULL;
	func.priv = priv;
	return func;
}
tb_hash_name_func_t tb_hash_name_func_mem(tb_void_t* priv)
{
	tb_hash_name_func_t func;
	func.hash = TB_NULL;
	func.comp = TB_NULL;
	func.dupl = TB_NULL;
	func.free = TB_NULL;
	func.priv = priv;
	return func;
}
tb_hash_pair_t tb_hash_itor_at(tb_hash_t* hash, tb_hash_itor_t itor)
{
	tb_hash_pair_t pair;
	pair.name = TB_NULL;
	pair.item = TB_NULL;
	return pair;
}
tb_hash_itor_t tb_hash_itor_head(tb_hash_t const* hash)
{
	return 0;
}
tb_hash_itor_t tb_hash_itor_tail(tb_hash_t const* hash)
{
	return 0;
}
tb_hash_itor_t tb_hash_itor_next(tb_hash_t const* hash, tb_hash_itor_t itor)
{
	return 0;
}
tb_hash_itor_t tb_hash_itor_prev(tb_hash_t const* hash, tb_hash_itor_t itor)
{
	return 0;
}

