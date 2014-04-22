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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		item.c
 * @ingroup 	container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "item.h"
#include "../libc/libc.h"
#include "../utils/utils.h"
#include "../memory/memory.h"
#include "../object/object.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * the item for true
 */
static tb_size_t tb_item_func_true_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t size)
{
	tb_trace_noimpl();
	return 0;
}
static tb_long_t tb_item_func_true_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	tb_trace_noimpl();
	return 0;
}
static tb_pointer_t tb_item_func_true_data(tb_item_func_t* func, tb_cpointer_t item)
{
	return (tb_pointer_t)tb_true;
}
static tb_char_t const* tb_item_func_true_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func && cstr && maxn, "");
	tb_strlcpy(cstr, "true", maxn - 1); cstr[maxn - 1] = '\0';
	return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_true_free(tb_item_func_t* func, tb_pointer_t item)
{
}
static tb_void_t tb_item_func_true_nfree(tb_item_func_t* func, tb_pointer_t item, tb_size_t size)
{
}
static tb_void_t tb_item_func_true_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	tb_assert((tb_bool_t)(tb_size_t)data == tb_true);
}
static tb_void_t tb_item_func_true_nrepl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	tb_assert((tb_bool_t)(tb_size_t)data == tb_true);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * the item for long
 */
static tb_long_t tb_item_func_long_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	return ((tb_long_t)ldata > (tb_long_t)rdata? 1 : ((tb_long_t)ldata < (tb_long_t)rdata? -1 : 0));
	//return (ldata - rdata); //!< maybe overflow for int32
}
static tb_pointer_t tb_item_func_long_data(tb_item_func_t* func, tb_cpointer_t item)
{
	tb_assert_and_check_return_val(func && item, tb_null);

	return (tb_pointer_t)*((tb_long_t*)item);
}
static tb_char_t const* tb_item_func_long_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func && cstr, "");
	tb_long_t n = tb_snprintf(cstr, maxn, "%ld", (tb_long_t)data);
	if (n > 0) cstr[n] = '\0';
	return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_long_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	tb_assert_and_check_return(func && item);
	*((tb_long_t*)item) = (tb_long_t)data;
}
static tb_void_t tb_item_func_long_nrepl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(func && item);

	if (func->size == 4) tb_memset_u32(item, (tb_uint32_t)(tb_size_t)data, size);
	else while (size--) ((tb_long_t*)item)[size] = (tb_long_t)data;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * the item for size
 */
static tb_size_t tb_item_func_size_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_return_val(tb_ispow2(size), 0);
	tb_assert_and_check_return_val(func, 0);
	return (tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, (tb_byte_t const*)&data, sizeof(tb_size_t)) & (size - 1));
}
static tb_long_t tb_item_func_size_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	return ((tb_size_t)ldata > (tb_size_t)rdata? 1 : ((tb_size_t)ldata < (tb_size_t)rdata? -1 : 0));
}
static tb_pointer_t tb_item_func_size_data(tb_item_func_t* func, tb_cpointer_t item)
{
	tb_assert_and_check_return_val(func && item, tb_null);
	return (tb_pointer_t)*((tb_size_t*)item);
}
static tb_char_t const* tb_item_func_size_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func && cstr, "");
	tb_long_t n = tb_snprintf(cstr, maxn, "%lu", (tb_size_t)data);
	if (n > 0) cstr[n] = '\0';
	return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_size_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_assert_and_check_return(func && item);
	*((tb_size_t*)item) = 0;
}
static tb_void_t tb_item_func_size_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	tb_assert_and_check_return(func && item);
	*((tb_size_t*)item) = (tb_size_t)data;
}
static tb_void_t tb_item_func_size_nfree(tb_item_func_t* func, tb_pointer_t item, tb_size_t size)
{
	tb_assert_and_check_return(func && item);
	if (size) tb_memset(item, 0, size * func->size);
}
static tb_void_t tb_item_func_size_nrepl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(func && item);

	if (func->size == 4) tb_memset_u32(item, (tb_uint32_t)(tb_size_t)data, size);
	else while (size--) ((tb_size_t*)item)[size] = (tb_size_t)data;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * the item for uint8
 */
static tb_size_t tb_item_func_uint8_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_return_val(tb_ispow2(size), 0);
	tb_assert_and_check_return_val(func, 0);
	return (tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, (tb_byte_t const*)&data, sizeof(tb_uint8_t)) & (size - 1));
}
static tb_long_t tb_item_func_uint8_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	return (tb_p2u8(ldata) > tb_p2u8(rdata)? 1 : (tb_p2u8(ldata) < tb_p2u8(rdata)? -1 : 0));
}
static tb_pointer_t tb_item_func_uint8_data(tb_item_func_t* func, tb_cpointer_t item)
{
	tb_assert_and_check_return_val(func && item, tb_null);
	return (tb_pointer_t)(tb_size_t)*((tb_uint8_t*)item);
}
static tb_char_t const* tb_item_func_uint8_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func && cstr, "");
	tb_long_t n = tb_snprintf(cstr, maxn, "%u", (tb_uint8_t)(tb_size_t)data);
	if (n > 0) cstr[n] = '\0';
	return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_uint8_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_assert_and_check_return(func && item);
	*((tb_uint8_t*)item) = 0;
}
static tb_void_t tb_item_func_uint8_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	tb_assert_and_check_return(func && item);
	*((tb_uint8_t*)item) = (tb_uint8_t)(tb_size_t)data;
}
static tb_void_t tb_item_func_uint8_nrepl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(func && item);
	tb_memset(item, (tb_uint8_t)(tb_size_t)data, size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * the item for uint16
 */
static tb_size_t tb_item_func_uint16_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_return_val(tb_ispow2(size), 0);
	tb_assert_and_check_return_val(func, 0);
	return (tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, (tb_byte_t const*)&data, sizeof(tb_uint16_t)) & (size - 1));
}
static tb_long_t tb_item_func_uint16_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	return (tb_p2u16(ldata) > tb_p2u16(rdata)? 1 : (tb_p2u16(ldata) < tb_p2u16(rdata)? -1 : 0));
}
static tb_pointer_t tb_item_func_uint16_data(tb_item_func_t* func, tb_cpointer_t item)
{
	tb_assert_and_check_return_val(func && item, tb_null);
	return (tb_pointer_t)(tb_size_t)*((tb_uint16_t*)item);
}
static tb_char_t const* tb_item_func_uint16_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func && cstr, "");
	tb_long_t n = tb_snprintf(cstr, maxn, "%u", (tb_uint16_t)(tb_size_t)data);
	if (n > 0) cstr[n] = '\0';
	return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_uint16_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_assert_and_check_return(func && item);
	*((tb_uint16_t*)item) = 0;
}
static tb_void_t tb_item_func_uint16_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	tb_assert_and_check_return(func && item);
	*((tb_uint16_t*)item) = (tb_uint16_t)(tb_size_t)data;
}
static tb_void_t tb_item_func_uint16_nrepl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(func && item);
	tb_memset_u16(item, (tb_uint16_t)(tb_size_t)data, size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * the item for uint32
 */
static tb_size_t tb_item_func_uint32_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_return_val(tb_ispow2(size), 0);
	tb_assert_and_check_return_val(func, 0);
	return (tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, (tb_byte_t const*)&data, sizeof(tb_uint32_t)) & (size - 1));
}
static tb_long_t tb_item_func_uint32_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	return (tb_p2u32(ldata) > tb_p2u32(rdata)? 1 : (tb_p2u32(ldata) < tb_p2u32(rdata)? -1 : 0));
}
static tb_pointer_t tb_item_func_uint32_data(tb_item_func_t* func, tb_cpointer_t item)
{
	tb_assert_and_check_return_val(func && item, tb_null);
	return (tb_pointer_t)(tb_size_t)*((tb_uint32_t*)item);
}
static tb_char_t const* tb_item_func_uint32_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func && cstr, "");
	tb_long_t n = tb_snprintf(cstr, maxn, "%u", (tb_uint32_t)(tb_size_t)data);
	if (n > 0) cstr[n] = '\0';
	return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_uint32_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_assert_and_check_return(func && item);
	*((tb_uint32_t*)item) = 0;
}
static tb_void_t tb_item_func_uint32_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	tb_assert_and_check_return(func && item);
	*((tb_uint32_t*)item) = (tb_uint32_t)(tb_size_t)data;
}
static tb_void_t tb_item_func_uint32_nrepl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(func && item);
	tb_memset_u32(item, (tb_uint32_t)(tb_size_t)data, size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * the item for string
 */
static tb_size_t tb_item_func_str_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_return_val(tb_ispow2(size), 0);
	tb_assert_and_check_return_val(func && data, 0);

#if 1
	tb_size_t 			h = 2166136261;
	tb_byte_t const* 	p = data;
	while (*p) h = 16777619 * h ^ (tb_size_t)(*p++);
	return (h & (size - 1));
#else
	return (tb_crc_encode_cstr(TB_CRC_MODE_32_IEEE_LE, 0, data) & (size - 1));
#endif
}
static tb_long_t tb_item_func_str_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	tb_assert_and_check_return_val(func, 0);
	return func->priv? tb_strcmp(ldata, rdata) : tb_stricmp(ldata, rdata);
}
static tb_char_t const* tb_item_func_str_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func, "");
	return (tb_char_t const*)data;
}
static tb_void_t tb_item_func_str_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_assert_and_check_return(func && item);
	if (*((tb_pointer_t*)item)) 
	{
		if (func->pool) tb_block_pool_free(func->pool, *((tb_pointer_t*)item));
		else tb_free(*((tb_pointer_t*)item));
		*((tb_pointer_t*)item) = tb_null;
	}
}
static tb_void_t tb_item_func_str_dupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	tb_assert_and_check_return(func && item);

	if (data) *((tb_pointer_t*)item) = func->pool? tb_block_pool_strdup(func->pool, data) : tb_strdup(data);
	else *((tb_pointer_t*)item) = tb_null;
}
static tb_void_t tb_item_func_str_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	tb_assert_and_check_return(func && item);

	// free it
	if (func->free) func->free(func, item);

	// copy it
	if (func->pool) *((tb_pointer_t*)item) = data? tb_block_pool_strdup(func->pool, data) : tb_null;
	else *((tb_pointer_t*)item) = data? tb_strdup(data) : tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * the item for pointer
 */
static tb_size_t tb_item_func_ptr_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_return_val(tb_ispow2(size), 0);
	tb_assert_and_check_return_val(func, 0);
	return (tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, (tb_byte_t const*)&data, sizeof(tb_size_t)) & (size - 1));
}
static tb_long_t tb_item_func_ptr_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	return ((tb_long_t)ldata > (tb_long_t)rdata? 1 : ((tb_long_t)ldata < (tb_long_t)rdata? -1 : 0));
}
static tb_pointer_t tb_item_func_ptr_data(tb_item_func_t* func, tb_cpointer_t item)
{
	tb_assert_and_check_return_val(func && item, tb_null);

	return *((tb_pointer_t*)item);
}
static tb_char_t const* tb_item_func_ptr_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func && cstr, "");
	tb_long_t n = tb_snprintf(cstr, maxn, "%p", data);
	if (n > 0) cstr[n] = '\0';
	return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_ptr_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_assert_and_check_return(func && item);
	*((tb_pointer_t*)item) = tb_null;
}
static tb_void_t tb_item_func_ptr_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(func && item);

	// the free is hooked? free it 
	if (func->free != tb_item_func_ptr_free && func->free)
		func->free(func, item);

	// copy it
	*((tb_cpointer_t*)item) = data;
}
static tb_void_t tb_item_func_ptr_copy(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(func && item);

	// copy it
	*((tb_cpointer_t*)item) = data;
}
static tb_void_t tb_item_func_ptr_dupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(func && item);

	// dupl it
	*((tb_cpointer_t*)item) = data;
}
static tb_void_t tb_item_func_ptr_nfree(tb_item_func_t* func, tb_pointer_t item, tb_size_t size)
{
	// check
	tb_assert_and_check_return(func && item);

	// the free is hooked? free it 
	if (func->free != tb_item_func_ptr_free && func->free)
	{
		tb_size_t n = size;
		while (n--) func->free(func, (tb_byte_t*)item + n * func->size);
	}

	// clear
	if (size) tb_memset(item, 0, size * func->size);
}
static tb_void_t tb_item_func_ptr_ndupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(func && item);

	// dupl items
	if (func->size == 4) tb_memset_u32(item, (tb_uint32_t)(tb_size_t)data, size);
	else while (size--) ((tb_cpointer_t*)item)[size] = data;
}
static tb_void_t tb_item_func_ptr_nrepl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(func && item);

	// the free is hooked? free items 
	if (func->free != tb_item_func_ptr_free && func->free)
	{
		tb_size_t n = size;
		while (n--) func->free(func, (tb_byte_t*)item + n * func->size);
	}

	// copy items
	if (func->size == 4) tb_memset_u32(item, (tb_uint32_t)(tb_size_t)data, size);
	else while (size--) ((tb_cpointer_t*)item)[size] = data;
}
static tb_void_t tb_item_func_ptr_ncopy(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(func && item);

	// copy items
	if (func->size == 4) tb_memset_u32(item, (tb_uint32_t)(tb_size_t)data, size);
	else while (size--) ((tb_cpointer_t*)item)[size] = data;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * the item for object
 */
static tb_char_t const* tb_item_func_obj_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(func && cstr, "");

	// format
	tb_long_t n = tb_snprintf(cstr, maxn, "<object: %p>", data);
	if (n > 0) cstr[n] = '\0';
	return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_obj_free(tb_item_func_t* func, tb_pointer_t item)
{
	// check
	tb_assert_and_check_return(func && item);

	// exit
	tb_object_t* object = *((tb_object_t**)item);
	if (object)
	{
		tb_object_exit(object);
		*((tb_object_t**)item) = tb_null;
	}
}
static tb_void_t tb_item_func_obj_dupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(func && item);

	// refn++
	if (data) tb_object_inc((tb_object_t*)data);

	// copy it
	*((tb_cpointer_t*)item) = data;
}
static tb_void_t tb_item_func_obj_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(func && item);

	// save the previous object
	tb_object_t* object = *((tb_object_t**)item);

	// refn++
	if (data) tb_object_inc((tb_object_t*)data);

	// copy it
	*((tb_cpointer_t*)item) = data;

	// refn--
	if (object) tb_object_dec(object);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * the item for the external fixed memory
 */
static tb_void_t tb_item_func_efm_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_assert_and_check_return(func && item);
	if (*((tb_pointer_t*)item)) 
	{
		if (func->pool) tb_fixed_pool_free(func->pool, *((tb_pointer_t*)item));
		else tb_free(*((tb_pointer_t*)item));
		*((tb_pointer_t*)item) = tb_null;
	}
}
static tb_pointer_t tb_item_func_efm_data(tb_item_func_t* func, tb_cpointer_t item)
{
	tb_assert_and_check_return_val(func && item, tb_null);
	return *((tb_pointer_t*)item);
}
static tb_void_t tb_item_func_efm_dupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	tb_assert_and_check_return(func && item);

	if (func->pool) *((tb_pointer_t*)item) = data? tb_fixed_pool_memdup(func->pool, data) : 0;
	else if (func->priv) *((tb_pointer_t*)item) = data? tb_memdup(data, (tb_size_t)func->priv) : tb_null;
}
static tb_void_t tb_item_func_efm_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(func && item);

	// copy it
	if (*((tb_pointer_t*)item) && func->priv) 
	{
		if (data) tb_memcpy(*((tb_pointer_t*)item), data, (tb_size_t)func->priv);
		else tb_memset(*((tb_pointer_t*)item), 0, (tb_size_t)func->priv);
	}
}
static tb_size_t tb_item_func_efm_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_return_val(tb_ispow2(size), 0);
	tb_assert_and_check_return_val(func && func->priv && data, 0);
	return (tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, data, (tb_size_t)func->priv) & (size - 1));
}
static tb_long_t tb_item_func_efm_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	tb_assert_and_check_return_val(func && func->priv && ldata && rdata, 0);
	return tb_memcmp(ldata, rdata, (tb_size_t)func->priv);
}
static tb_char_t const* tb_item_func_efm_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	tb_assert_and_check_return_val(func && func->priv && cstr && data, "");

	// data => string
	tb_long_t n = tb_snprintf(cstr, maxn, "0x%x", tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, data, (tb_size_t)func->priv));
	if (n > 0) cstr[n] = '\0';
	return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_efm_nfree(tb_item_func_t* func, tb_pointer_t item, tb_size_t size)
{
	tb_assert_and_check_return(func && func->size && item);
	if (func->free) while (size--) func->free(func, (tb_byte_t*)item + size * func->size);
}
static tb_void_t tb_item_func_efm_ndupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(func && func->size && item);
	if (func->dupl)
		while (size--) func->dupl(func, (tb_byte_t*)item + size * func->size, data);
}
static tb_void_t tb_item_func_efm_nrepl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	tb_assert_and_check_return(func && func->size && item);
	if (func->repl) while (size--) func->repl(func, (tb_byte_t*)item + size * func->size, data);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * the item for the internal fixed memory
 */
static tb_size_t tb_item_func_ifm_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t size)
{	
	// check
	tb_assert_return_val(tb_ispow2(size), 0);
	tb_assert_and_check_return_val(func && func->size && data, 0);

	// hash
	return (tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, data, func->size) & (size - 1));
}
static tb_long_t tb_item_func_ifm_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	// check
	tb_assert_and_check_return_val(func && func->size && ldata && rdata, 0);

	// comp
	return tb_memcmp(ldata, rdata, func->size);
}
static tb_pointer_t tb_item_func_ifm_data(tb_item_func_t* func, tb_cpointer_t item)
{
	// check
	tb_assert_and_check_return_val(func && item, tb_null);
	return (tb_pointer_t)item;
}
static tb_char_t const* tb_item_func_ifm_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(func && func->size && cstr && data, "");

	// data => string
	tb_long_t n = tb_snprintf(cstr, maxn, "0x%x", tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, data, func->size));
	if (n > 0) cstr[n] = '\0';
	return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_ifm_dupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(func && func->size && item);

	// copy item
	if (data) tb_memcpy(item, data, func->size);
	else tb_memset(item, 0, func->size);
}
static tb_void_t tb_item_func_ifm_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(func && func->size && item);

	// free item
	if (func->free) func->free(func, item);

	// copy item
	if (data) tb_memcpy(item, data, func->size);
	else tb_memset(item, 0, func->size);
}
static tb_void_t tb_item_func_ifm_copy(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(func && func->size && item);

	// copy item
	if (data) tb_memcpy(item, data, func->size);
	else tb_memset(item, 0, func->size);
}
static tb_void_t tb_item_func_ifm_nfree(tb_item_func_t* func, tb_pointer_t item, tb_size_t size)
{
	// check
	tb_assert_and_check_return(func && func->size && item);

	// free
	if (func->free) while (size--) func->free(func, (tb_byte_t*)item + size * func->size);
}
static tb_void_t tb_item_func_ifm_ndupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(func && func->size && item);

	// copy item
	if (data) while (size--) tb_memcpy((tb_byte_t*)item + size * func->size, data, func->size);
	else tb_memset(item, 0, func->size * size);
}
static tb_void_t tb_item_func_ifm_nrepl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(func && func->size && item);

	// free item
	if (func->nfree) func->nfree(func, item, size);

	// copy item
	if (func->ndupl) func->ndupl(func, item, data, size);
}
static tb_void_t tb_item_func_ifm_ncopy(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(func && func->size && item);

	// copy item
	if (func->ndupl) func->ndupl(func, item, data, size);
}
// the string for string pool 
static tb_void_t tb_item_func_string_pool_free(tb_item_func_t* func, tb_pointer_t item)
{
	// check
	tb_assert_and_check_return(func && func->pool && item);
	if (*((tb_pointer_t*)item)) tb_string_pool_del(func->pool, *((tb_pointer_t*)item));
}
static tb_void_t tb_item_func_string_pool_dupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(func && func->pool && item);

	if (data) *((tb_cpointer_t*)item) = tb_string_pool_put(func->pool, data);
	else *((tb_pointer_t*)item) = tb_null;
}
static tb_void_t tb_item_func_string_pool_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(func && func->pool && item);

	// copy it, refn++
	tb_cpointer_t copy = data? tb_string_pool_put(func->pool, data) : tb_null;

	// free it, refn--
	if (func->free) func->free(func, item);

	// save it
	*((tb_cpointer_t*)item) = copy;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_item_func_t tb_item_func_true()
{
	tb_item_func_t func = {0};
	func.type = TB_ITEM_TYPE_TRUE;

	func.hash = tb_item_func_true_hash;
	func.comp = tb_item_func_true_comp;

	func.data = tb_item_func_true_data;
	func.cstr = tb_item_func_true_cstr;

	func.free = tb_item_func_true_free;
	func.dupl = tb_item_func_true_repl;
	func.repl = tb_item_func_true_repl;
	func.copy = tb_item_func_true_repl;

	func.nfree = tb_item_func_true_nfree;
	func.ndupl = tb_item_func_true_nrepl;
	func.nrepl = tb_item_func_true_nrepl;
	func.ncopy = tb_item_func_true_nrepl;

	func.size = 0;

	return func;
}
tb_item_func_t tb_item_func_long()
{
	tb_item_func_t func = {0};
	func.type = TB_ITEM_TYPE_LONG;

	func.hash = tb_item_func_size_hash;
	func.comp = tb_item_func_long_comp;

	func.data = tb_item_func_long_data;
	func.cstr = tb_item_func_long_cstr;

	func.free = tb_item_func_size_free;
	func.dupl = tb_item_func_long_repl;
	func.repl = tb_item_func_long_repl;
	func.copy = tb_item_func_long_repl;

	func.nfree = tb_item_func_size_nfree;
	func.ndupl = tb_item_func_long_nrepl;
	func.nrepl = tb_item_func_long_nrepl;
	func.ncopy = tb_item_func_long_nrepl;

	func.size = sizeof(tb_long_t);

	return func;
}
tb_item_func_t tb_item_func_size()
{
	tb_item_func_t func = {0};
	func.type = TB_ITEM_TYPE_SIZE;

	func.hash = tb_item_func_size_hash;
	func.comp = tb_item_func_size_comp;

	func.data = tb_item_func_size_data;
	func.cstr = tb_item_func_size_cstr;

	func.free = tb_item_func_size_free;
	func.dupl = tb_item_func_size_repl;
	func.repl = tb_item_func_size_repl;
	func.copy = tb_item_func_size_repl;

	func.nfree = tb_item_func_size_nfree;
	func.ndupl = tb_item_func_size_nrepl;
	func.nrepl = tb_item_func_size_nrepl;
	func.ncopy = tb_item_func_size_nrepl;

	func.size = sizeof(tb_size_t);

	return func;
}
tb_item_func_t tb_item_func_uint8()
{
	tb_item_func_t func = {0};
	func.type = TB_ITEM_TYPE_UINT8;

	func.hash = tb_item_func_uint8_hash;
	func.comp = tb_item_func_uint8_comp;

	func.data = tb_item_func_uint8_data;
	func.cstr = tb_item_func_uint8_cstr;

	func.free = tb_item_func_uint8_free;
	func.dupl = tb_item_func_uint8_repl;
	func.repl = tb_item_func_uint8_repl;
	func.copy = tb_item_func_uint8_repl;

	func.nfree = tb_item_func_size_nfree;
	func.ndupl = tb_item_func_uint8_nrepl;
	func.nrepl = tb_item_func_uint8_nrepl;
	func.ncopy = tb_item_func_uint8_nrepl;

	func.size = sizeof(tb_uint8_t);

	return func;
}
tb_item_func_t tb_item_func_uint16()
{
	tb_item_func_t func = {0};
	func.type = TB_ITEM_TYPE_UINT16;

	func.hash = tb_item_func_uint16_hash;
	func.comp = tb_item_func_uint16_comp;

	func.data = tb_item_func_uint16_data;
	func.cstr = tb_item_func_uint16_cstr;

	func.free = tb_item_func_uint16_free;
	func.dupl = tb_item_func_uint16_repl;
	func.repl = tb_item_func_uint16_repl;
	func.copy = tb_item_func_uint16_repl;

	func.nfree = tb_item_func_size_nfree;
	func.ndupl = tb_item_func_uint16_nrepl;
	func.nrepl = tb_item_func_uint16_nrepl;
	func.ncopy = tb_item_func_uint16_nrepl;

	func.size = sizeof(tb_uint16_t);

	return func;
}
tb_item_func_t tb_item_func_uint32()
{
	tb_item_func_t func = {0};
	func.type = TB_ITEM_TYPE_UINT32;

	func.hash = tb_item_func_uint32_hash;
	func.comp = tb_item_func_uint32_comp;

	func.data = tb_item_func_uint32_data;
	func.cstr = tb_item_func_uint32_cstr;

	func.free = tb_item_func_uint32_free;
	func.dupl = tb_item_func_uint32_repl;
	func.repl = tb_item_func_uint32_repl;
	func.copy = tb_item_func_uint32_repl;

	func.nfree = tb_item_func_size_nfree;
	func.ndupl = tb_item_func_uint32_nrepl;
	func.nrepl = tb_item_func_uint32_nrepl;
	func.ncopy = tb_item_func_uint32_nrepl;

	func.size = sizeof(tb_uint32_t);

	return func;
}
tb_item_func_t tb_item_func_str(tb_bool_t bcase, tb_handle_t bpool)
{
	tb_item_func_t func = {0};
	func.type = TB_ITEM_TYPE_STR;
	func.hash = tb_item_func_str_hash;
	func.comp = tb_item_func_str_comp;

	func.data = tb_item_func_ptr_data;
	func.cstr = tb_item_func_str_cstr;

	func.free = tb_item_func_str_free;
	func.dupl = tb_item_func_str_dupl;
	func.repl = tb_item_func_str_repl;
	func.copy = tb_item_func_ptr_copy;

	func.nfree = tb_item_func_efm_nfree;
	func.ndupl = tb_item_func_efm_ndupl;
	func.nrepl = tb_item_func_efm_nrepl;
	func.ncopy = tb_item_func_ptr_ncopy;

	func.size = sizeof(tb_pointer_t);
	func.pool = bpool;
	func.priv = tb_b2p(bcase);

	return func;
}
tb_item_func_t tb_item_func_ptr(tb_item_func_free_t free, tb_cpointer_t priv)
{
	tb_item_func_t func = {0};
	func.type = TB_ITEM_TYPE_PTR;

	func.hash = tb_item_func_ptr_hash;
	func.comp = tb_item_func_ptr_comp;

	func.data = tb_item_func_ptr_data;
	func.cstr = tb_item_func_ptr_cstr;

	func.free = free? free : tb_item_func_ptr_free;
	func.dupl = tb_item_func_ptr_dupl;
	func.repl = tb_item_func_ptr_repl;
	func.copy = tb_item_func_ptr_copy;

	func.nfree = tb_item_func_ptr_nfree;
	func.ndupl = tb_item_func_ptr_ndupl;
	func.nrepl = tb_item_func_ptr_nrepl;
	func.ncopy = tb_item_func_ptr_ncopy;

	func.size = sizeof(tb_pointer_t);
	func.priv = (tb_pointer_t)priv;

	return func;
}
tb_item_func_t tb_item_func_obj()
{
	tb_item_func_t func = {0};
	func.type = TB_ITEM_TYPE_OBJ;

	func.hash = tb_item_func_ptr_hash;
	func.comp = tb_item_func_ptr_comp;

	func.data = tb_item_func_ptr_data;
	func.cstr = tb_item_func_obj_cstr;

	func.free = tb_item_func_obj_free;
	func.dupl = tb_item_func_obj_dupl;
	func.repl = tb_item_func_obj_repl;
	func.copy = tb_item_func_ptr_copy;

	func.nfree = tb_item_func_efm_nfree;
	func.ndupl = tb_item_func_efm_ndupl;
	func.nrepl = tb_item_func_efm_nrepl;
	func.ncopy = tb_item_func_ptr_ncopy;

	func.size = sizeof(tb_object_t*);

	return func;
}
tb_item_func_t tb_item_func_efm(tb_size_t size, tb_handle_t fpool)
{
	tb_item_func_t func = {0};
	func.type = TB_ITEM_TYPE_EFM;

	func.hash = tb_item_func_efm_hash;
	func.comp = tb_item_func_efm_comp;

	func.data = tb_item_func_efm_data;
	func.cstr = tb_item_func_efm_cstr;

	func.free = tb_item_func_efm_free;
	func.dupl = tb_item_func_efm_dupl;
	func.repl = tb_item_func_efm_repl;
	func.copy = tb_item_func_ptr_copy;

	func.nfree = tb_item_func_efm_nfree;
	func.ndupl = tb_item_func_efm_ndupl;
	func.nrepl = tb_item_func_efm_nrepl;
	func.ncopy = tb_item_func_ptr_ncopy;

	func.size = sizeof(tb_pointer_t);
	func.pool = fpool;
	func.priv = (tb_pointer_t)size;

	return func;
}
tb_item_func_t tb_item_func_ifm(tb_size_t size, tb_item_func_free_t free, tb_cpointer_t priv)
{
	tb_item_func_t func = {0};
	func.type = TB_ITEM_TYPE_IFM;

	func.hash = tb_item_func_ifm_hash;
	func.comp = tb_item_func_ifm_comp;

	func.data = tb_item_func_ifm_data;
	func.cstr = tb_item_func_ifm_cstr;

	func.free = free;
	func.dupl = tb_item_func_ifm_dupl;
	func.repl = tb_item_func_ifm_repl;
	func.copy = tb_item_func_ifm_copy;

	func.nfree = tb_item_func_ifm_nfree;
	func.ndupl = tb_item_func_ifm_ndupl;
	func.nrepl = tb_item_func_ifm_nrepl;
	func.ncopy = tb_item_func_ifm_ncopy;

	func.size = size;
	func.priv = (tb_pointer_t)priv;

	return func;
}
tb_item_func_t tb_item_func_string_pool(tb_handle_t spool)
{
	tb_item_func_t func = {0};
	func.type = TB_ITEM_TYPE_SPOOL;
	func.hash = tb_item_func_str_hash;
	func.comp = tb_item_func_str_comp;

	func.data = tb_item_func_ptr_data;
	func.cstr = tb_item_func_str_cstr;

	func.free = tb_item_func_string_pool_free;
	func.dupl = tb_item_func_string_pool_dupl;
	func.repl = tb_item_func_string_pool_repl;
	func.copy = tb_item_func_ptr_copy;

	func.nfree = tb_item_func_efm_nfree;
	func.ndupl = tb_item_func_efm_ndupl;
	func.nrepl = tb_item_func_efm_nrepl;
	func.ncopy = tb_item_func_ptr_ncopy;

	func.size = sizeof(tb_pointer_t);
	func.pool = spool;
	tb_assert(spool);

	return func;
}
