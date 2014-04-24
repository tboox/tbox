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
 * @file		uint8.c
 * @ingroup 	container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_item_func_uint8_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(size, 0);
	tb_assert_return_val(tb_ispow2(size), 0);

	// compute the crc hash
	return (tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, (tb_byte_t const*)&data, sizeof(tb_uint8_t)) & (size - 1));
}
static tb_long_t tb_item_func_uint8_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	// compare it
	return (tb_p2u8(ldata) > tb_p2u8(rdata)? 1 : (tb_p2u8(ldata) < tb_p2u8(rdata)? -1 : 0));
}
static tb_pointer_t tb_item_func_uint8_data(tb_item_func_t* func, tb_cpointer_t item)
{
	// check
	tb_assert_and_check_return_val(item, tb_null);

	// the item data
	return tb_u2p(*((tb_uint8_t*)item));
}
static tb_char_t const* tb_item_func_uint8_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(func && cstr, "");

	// format string
	tb_long_t n = tb_snprintf(cstr, maxn, "%u", (tb_uint8_t)(tb_size_t)data);
	if (n >= 0 && n < (tb_long_t)maxn) cstr[n] = '\0';

	// ok?
	return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_uint8_free(tb_item_func_t* func, tb_pointer_t item)
{
	// check
	tb_assert_and_check_return(item);

	// clear
	*((tb_uint8_t*)item) = 0;
}
static tb_void_t tb_item_func_uint8_copy(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(item);

	// copy item
	*((tb_uint8_t*)item) = tb_p2u8(data);
}
static tb_void_t tb_item_func_uint8_nfree(tb_item_func_t* func, tb_pointer_t item, tb_size_t size)
{
	// check
	tb_assert_and_check_return(item);

	// clear items
	if (size) tb_memset(item, 0, size);
}
static tb_void_t tb_item_func_uint8_ncopy(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(item);

	// copy items
	tb_memset(item, tb_p2u8(data), size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_item_func_t tb_item_func_uint8()
{
	// init func
	tb_item_func_t func = {0};
	func.type 	= TB_ITEM_TYPE_UINT8;
	func.hash 	= tb_item_func_uint8_hash;
	func.comp 	= tb_item_func_uint8_comp;
	func.data 	= tb_item_func_uint8_data;
	func.cstr 	= tb_item_func_uint8_cstr;
	func.free 	= tb_item_func_uint8_free;
	func.dupl 	= tb_item_func_uint8_copy;
	func.repl 	= tb_item_func_uint8_copy;
	func.copy 	= tb_item_func_uint8_copy;
	func.nfree 	= tb_item_func_uint8_nfree;
	func.ndupl 	= tb_item_func_uint8_ncopy;
	func.nrepl 	= tb_item_func_uint8_ncopy;
	func.ncopy 	= tb_item_func_uint8_ncopy;
	func.size 	= sizeof(tb_uint8_t);

	// ok?
	return func;
}
