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
 * \file		item.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "item.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../platform/platform.h"

/* /////////////////////////////////////////////////////////
 * details
 */

// cstring
static tb_void_t tb_item_func_str_free(tb_item_func_t* func, tb_void_t* item)
{
	tb_assert_and_check_return(func);
	if (item) 
	{
		if (func->pool) tb_spool_free(func->pool, item);
		else tb_free(item);
	}
}
static tb_void_t* tb_item_func_str_dupl(tb_item_func_t* func, tb_void_t const* data)
{
	tb_assert_and_check_return_val(func, TB_NULL);
	return func->pool? tb_spool_strdup(func->pool, data) : tb_strdup(data);
}
static tb_void_t* tb_item_func_str_copy(tb_item_func_t* func, tb_void_t* item, tb_void_t const* data)
{
	tb_assert_and_check_return_val(func && data, item);
	if (func->pool) 
	{
		tb_spool_free(func->pool, item);
		return tb_spool_strdup(func->pool, data);
	}
	else
	{
		if (item) tb_free(item);
		return tb_strdup(data);
	}
	return item;
}
static tb_size_t tb_item_func_str_hash(tb_item_func_t* func, tb_void_t const* data, tb_size_t size)
{
	tb_assert_and_check_return_val(func && data && tb_ispow2(size), 0);

	tb_size_t h = 2166136261;
	tb_byte_t const* p = data;
	while (*p) h = 16777619 * h ^ (tb_size_t)(*p++);

	return (h & (size - 1));
}
static tb_int_t tb_item_func_str_comp(tb_item_func_t* func, tb_void_t const* ldata, tb_void_t const* rdata)
{
	tb_assert_and_check_return_val(func, 0);
	return tb_strcmp(ldata, rdata);
}
static tb_char_t const* tb_item_func_str_cstr(tb_item_func_t* func, tb_void_t const* data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func, "");
	return (tb_char_t const*)data;
}

// integer
static tb_size_t tb_item_func_int_hash(tb_item_func_t* func, tb_void_t const* data, tb_size_t size)
{
	tb_assert_and_check_return_val(func && tb_ispow2(size), 0);
	return (((tb_size_t)data) ^ 0xdeadbeef) & (size - 1);
}
static tb_int_t tb_item_func_int_comp(tb_item_func_t* func, tb_void_t const* ldata, tb_void_t const* rdata)
{
	return (ldata - rdata);
}
static tb_char_t const* tb_item_func_int_cstr(tb_item_func_t* func, tb_void_t const* data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func && cstr, "");
	tb_int_t n = tb_snprintf(cstr, maxn, "%d", data);
	if (n > 0) cstr[n] = '\0';
	return (tb_char_t const*)cstr;
}

// pointer
static tb_size_t tb_item_func_ptr_hash(tb_item_func_t* func, tb_void_t const* data, tb_size_t size)
{
	tb_assert_and_check_return_val(func && tb_ispow2(size), 0);
	return (((tb_size_t)data) ^ 0xdeadbeef) & (size - 1);
}
static tb_int_t tb_item_func_ptr_comp(tb_item_func_t* func, tb_void_t const* ldata, tb_void_t const* rdata)
{
	return (ldata - rdata);
}
static tb_char_t const* tb_item_func_ptr_cstr(tb_item_func_t* func, tb_void_t const* data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func && cstr, "");
	tb_int_t n = tb_snprintf(cstr, maxn, "%x", data);
	if (n > 0) cstr[n] = '\0';
	return (tb_char_t const*)cstr;
}

// memory
static tb_void_t tb_item_func_efm_free(tb_item_func_t* func, tb_void_t* item)
{
	tb_assert_and_check_return(func);
	if (func->pool) tb_fpool_del(func->pool, item);
	else if (item) tb_free(item);
}
static tb_void_t* tb_item_func_efm_data(tb_item_func_t* func, tb_void_t const* item)
{
	tb_assert_and_check_return_val(func, 0);

	if (func->pool) return tb_fpool_get(func->pool, (tb_size_t)item);
	else return item;
}
static tb_void_t* tb_item_func_efm_dupl(tb_item_func_t* func, tb_void_t const* data)
{
	tb_assert_and_check_return_val(func && data, TB_NULL);

	if (func->pool) return tb_fpool_put(func->pool, data);
	else if (func->size)
	{
		tb_void_t* 	dupl = tb_malloc(func->size);
		tb_assert_and_check_return_val(dupl, TB_NULL);

		return tb_memcpy(dupl, data, func->size);
	}
	return TB_NULL;
}
static tb_void_t* tb_item_func_efm_copy(tb_item_func_t* func, tb_void_t* item, tb_void_t const* data)
{
	tb_assert_and_check_return_val(func && data, item);
	if (func->pool) tb_fpool_set(func->pool, item, data);
	else if (item && func->size) tb_memcpy(item, data, func->size);
	return item;
}
static tb_size_t tb_item_func_efm_hash(tb_item_func_t* func, tb_void_t const* data, tb_size_t size)
{
	tb_assert_and_check_return_val(func && func->size && data && tb_ispow2(size), 0);
	return (tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, data, func->size) & (size - 1));
}
static tb_int_t tb_item_func_efm_comp(tb_item_func_t* func, tb_void_t const* ldata, tb_void_t const* rdata)
{
	tb_assert_and_check_return_val(func && func->size && ldata && rdata, 0);
	return tb_memcmp(ldata, rdata, func->size);
}
static tb_char_t const* tb_item_func_efm_cstr(tb_item_func_t* func, tb_void_t const* data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func && func->size && cstr && data, "");

	// data => string
	tb_int_t n = tb_snprintf(cstr, maxn, "0x%x", tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, data, func->size));
	if (n > 0) cstr[n] = '\0';
	return (tb_char_t const*)cstr;
}
/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_item_func_t tb_item_func_str(tb_spool_t* spool)
{
	tb_item_func_t func;
	tb_memset(&func, 0, sizeof(tb_item_func_t));
	func.hash = tb_item_func_str_hash;
	func.comp = tb_item_func_str_comp;
	func.dupl = tb_item_func_str_dupl;
	func.copy = tb_item_func_str_copy;
	func.cstr = tb_item_func_str_cstr;
	func.free = tb_item_func_str_free;
	func.pool = spool;
	return func;
}
tb_item_func_t tb_item_func_int()
{
	tb_item_func_t func;
	tb_memset(&func, 0, sizeof(tb_item_func_t));
	func.hash = tb_item_func_int_hash;
	func.comp = tb_item_func_int_comp;
	func.cstr = tb_item_func_int_cstr;
	return func;
}
tb_item_func_t tb_item_func_ptr()
{
	tb_item_func_t func;
	tb_memset(&func, 0, sizeof(tb_item_func_t));
	func.hash = tb_item_func_ptr_hash;
	func.comp = tb_item_func_ptr_comp;
	func.cstr = tb_item_func_ptr_cstr;
	return func;
}
tb_item_func_t tb_item_func_efm(tb_size_t size, tb_fpool_t* fpool)
{
	tb_item_func_t func;
	tb_memset(&func, 0, sizeof(tb_item_func_t));
	func.hash = tb_item_func_efm_hash;
	func.comp = tb_item_func_efm_comp;
	func.dupl = tb_item_func_efm_dupl;
	func.copy = tb_item_func_efm_copy;
	func.cstr = tb_item_func_efm_cstr;
	func.free = tb_item_func_efm_free;
	func.data = tb_item_func_efm_data;
	func.size = size;
	func.pool = fpool;
	return func;
}

