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
 * @file		ptr.c
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
static tb_long_t tb_item_func_ptr_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
	return ((tb_size_t)ldata > (tb_size_t)rdata? 1 : ((tb_size_t)ldata < (tb_size_t)rdata? -1 : 0));
}
static tb_pointer_t tb_item_func_ptr_data(tb_item_func_t* func, tb_cpointer_t item)
{
	// check
	tb_assert_and_check_return_val(item, tb_null);

	// the item data
	return *((tb_pointer_t*)item);
}
static tb_char_t const* tb_item_func_ptr_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(cstr, "");

	// format string
	tb_long_t n = tb_snprintf(cstr, maxn - 1, "%p", data);
	if (n >= 0 && n < (tb_long_t)maxn) cstr[n] = '\0';

	// ok?
	return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_ptr_free(tb_item_func_t* func, tb_pointer_t item)
{
	// check
	tb_assert_and_check_return(item);

	// clear it
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
	tb_assert_and_check_return(item);

	// copy it
	*((tb_cpointer_t*)item) = data;
}
static tb_void_t tb_item_func_ptr_dupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(item);

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
		while (n--) func->free(func, (tb_byte_t*)item + n * sizeof(tb_pointer_t));
	}

	// clear
	if (size) tb_memset(item, 0, size * sizeof(tb_pointer_t));
}
static tb_void_t tb_item_func_ptr_ndupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(item);

	// copy items
	if (func->ncopy) func->ncopy(func, item, data, size);
}
static tb_void_t tb_item_func_ptr_nrepl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(func && item);

	// free item
	if (func->nfree) func->nfree(func, item, size);

	// copy items
	if (func->ncopy) func->ncopy(func, item, data, size);
}
static tb_void_t tb_item_func_ptr_ncopy(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
	// check
	tb_assert_and_check_return(item);

	// fill items
	if (size) tb_memset_ptr(item, data, size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_item_func_t tb_item_func_ptr(tb_item_func_free_t free, tb_cpointer_t priv)
{
	// the size func
	tb_item_func_t func_size = tb_item_func_size();

	// init func
	tb_item_func_t func = {0};
	func.type 	= TB_ITEM_TYPE_PTR;
	func.hash 	= func_size.hash;
	func.comp 	= tb_item_func_ptr_comp;
	func.data 	= tb_item_func_ptr_data;
	func.cstr 	= tb_item_func_ptr_cstr;
	func.free 	= free? free : tb_item_func_ptr_free;
	func.dupl 	= tb_item_func_ptr_dupl;
	func.repl 	= tb_item_func_ptr_repl;
	func.copy 	= tb_item_func_ptr_copy;
	func.nfree 	= tb_item_func_ptr_nfree;
	func.ndupl 	= tb_item_func_ptr_ndupl;
	func.nrepl 	= tb_item_func_ptr_nrepl;
	func.ncopy 	= tb_item_func_ptr_ncopy;
	func.priv 	= priv;
	func.size 	= sizeof(tb_pointer_t);

	// ok?
	return func;
}
