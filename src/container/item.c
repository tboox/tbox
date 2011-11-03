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
static tb_void_t tb_item_func_str_free(tb_void_t* item, tb_void_t* priv)
{
	if (item) 
	{
		if (priv) tb_spool_free((tb_spool_t*)priv, item);
		else tb_free(item);
	}
}
static tb_void_t* tb_item_func_str_dupl(tb_void_t const* item, tb_void_t* priv)
{
	return priv? tb_spool_strdup((tb_spool_t*)priv, item) : tb_strdup(item);
}
#if 1
static tb_size_t tb_item_func_str_hash(tb_void_t const* item, tb_size_t size, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(item && size, 0);

	tb_size_t h = 2166136261;
	tb_byte_t const* p = item;
	while (*p) h = 16777619 * h ^ (tb_size_t)(*p++);

	return (h & (size - 1));
}
#else
static tb_size_t tb_item_func_str_hash(tb_void_t const* item, tb_size_t size, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(item && size, 0);
	return (tb_strlen(item) & (size - 1));
}
#endif
static tb_int_t tb_item_func_str_comp(tb_void_t const* litem, tb_void_t const* ritem, tb_void_t* priv)
{
	return tb_strcmp(litem, ritem);
}

static tb_char_t const* tb_item_func_str_cstr(tb_void_t const* item, tb_char_t* data, tb_size_t maxn, tb_void_t* priv)
{
	return (tb_char_t const*)item;
}

// integer
static tb_void_t tb_item_func_int_free(tb_void_t* item, tb_void_t* priv)
{
}
static tb_void_t* tb_item_func_int_dupl(tb_void_t const* item, tb_void_t* priv)
{
	return (tb_void_t*)item;
}
static tb_size_t tb_item_func_int_hash(tb_void_t const* item, tb_size_t size, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(size, 0);
	return (((tb_size_t)item) ^ 0xdeadbeef) & (size - 1);
}
static tb_int_t tb_item_func_int_comp(tb_void_t const* litem, tb_void_t const* ritem, tb_void_t* priv)
{
	return (litem - ritem);
}
static tb_char_t const* tb_item_func_int_cstr(tb_void_t const* item, tb_char_t* data, tb_size_t maxn, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(data, TB_NULL);
	tb_int_t n = tb_snprintf(data, maxn, "%d", item);
	if (n > 0) data[n] = '\0';
	return (tb_char_t const*)data;
}

// pointer
static tb_void_t tb_item_func_ptr_free(tb_void_t* item, tb_void_t* priv)
{
}
static tb_void_t* tb_item_func_ptr_dupl(tb_void_t const* item, tb_void_t* priv)
{
	return item;
}
static tb_size_t tb_item_func_ptr_hash(tb_void_t const* item, tb_size_t size, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(size, 0);
	return (((tb_size_t)item) ^ 0xdeadbeef) & (size - 1);
}
static tb_int_t tb_item_func_ptr_comp(tb_void_t const* litem, tb_void_t const* ritem, tb_void_t* priv)
{
	return (litem - ritem);
}
static tb_char_t const* tb_item_func_ptr_cstr(tb_void_t const* item, tb_char_t* data, tb_size_t maxn, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(data, TB_NULL);
	tb_int_t n = tb_snprintf(data, maxn, "%x", item);
	if (n > 0) data[n] = '\0';
	return (tb_char_t const*)data;
}

// memory
static tb_void_t tb_item_func_mem_free(tb_void_t* item, tb_void_t* priv)
{
	if (item) tb_free(item);
}
static tb_void_t* tb_item_func_mem_dupl(tb_void_t const* item, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(priv, TB_NULL);
	
	tb_void_t* p = tb_malloc((tb_size_t)priv);
	TB_ASSERT_RETURN_VAL(p, TB_NULL);

	return tb_memcpy(p, item, (tb_size_t)priv);
}
static tb_size_t tb_item_func_mem_hash(tb_void_t const* item, tb_size_t size, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(item && size, 0);

	tb_size_t h = 2166136261;
	tb_byte_t const* p = item;
	tb_byte_t const* e = p + (tb_size_t)priv;
	while (p < e && *p) h = 16777619 * h ^ (tb_size_t)(*p++);

	return (h & (size - 1));
}
static tb_int_t tb_item_func_mem_comp(tb_void_t const* litem, tb_void_t const* ritem, tb_void_t* priv)
{
	return tb_memcmp(litem, ritem, (tb_size_t)priv);
}
static tb_char_t const* tb_item_func_mem_cstr(tb_void_t const* item, tb_char_t* data, tb_size_t maxn, tb_void_t* priv)
{
	TB_ASSERT_RETURN_VAL(item && data, TB_NULL);
	tb_byte_t const* p = item;
	tb_int_t n = tb_snprintf(data, maxn, "%x", tb_bits_get_u32_be(p));
	if (n > 0) data[n] = '\0';
	return (tb_char_t const*)data;
}
/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_item_func_t tb_item_func_str(tb_spool_t* spool)
{
	tb_item_func_t func;
	func.hash = tb_item_func_str_hash;
	func.comp = tb_item_func_str_comp;
	func.dupl = tb_item_func_str_dupl;
	func.cstr = tb_item_func_str_cstr;
	func.free = tb_item_func_str_free;
	func.priv = spool;
	return func;
}
tb_item_func_t tb_item_func_int()
{
	tb_item_func_t func;
	func.hash = tb_item_func_int_hash;
	func.comp = tb_item_func_int_comp;
	func.dupl = tb_item_func_int_dupl;
	func.cstr = tb_item_func_int_cstr;
	func.free = tb_item_func_int_free;
	func.priv = TB_NULL;
	return func;
}
tb_item_func_t tb_item_func_ptr()
{
	tb_item_func_t func;
	func.hash = tb_item_func_ptr_hash;
	func.comp = tb_item_func_ptr_comp;
	func.dupl = tb_item_func_ptr_dupl;
	func.cstr = tb_item_func_ptr_cstr;
	func.free = tb_item_func_ptr_free;
	func.priv = TB_NULL;
	return func;
}
tb_item_func_t tb_item_func_mem(tb_size_t size)
{
	tb_item_func_t func;
	func.hash = tb_item_func_mem_hash;
	func.comp = tb_item_func_mem_comp;
	func.dupl = tb_item_func_mem_dupl;
	func.cstr = tb_item_func_mem_cstr;
	func.free = tb_item_func_mem_free;
	func.priv = (tb_size_t)size;
	return func;
}

