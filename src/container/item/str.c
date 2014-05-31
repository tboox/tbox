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
 * @author      ruki
 * @file        str.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "hash.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_item_func_str_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t mask, tb_size_t index)
{
    return tb_item_func_hash_cstr(data, mask, index);
}
static tb_long_t tb_item_func_str_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
    // check
    tb_assert_and_check_return_val(func && ldata && rdata, 0);

    // compare it
    return func->priv? tb_strcmp(ldata, rdata) : tb_stricmp(ldata, rdata);
}
static tb_pointer_t tb_item_func_str_data(tb_item_func_t* func, tb_cpointer_t item)
{
    // check
    tb_assert_and_check_return_val(item, tb_null);

    // the item data
    return *((tb_pointer_t*)item);
}
static tb_char_t const* tb_item_func_str_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
    return (tb_char_t const*)data;
}
static tb_void_t tb_item_func_str_free(tb_item_func_t* func, tb_pointer_t item)
{
    // check
    tb_assert_and_check_return(func && item);

    // exists?
    if (*((tb_pointer_t*)item)) 
    {
        // free it
        if (func->pool) tb_block_pool_free(func->pool, *((tb_pointer_t*)item));
        else tb_free(*((tb_pointer_t*)item));

        // clear it
        *((tb_pointer_t*)item) = tb_null;
    }
}
static tb_void_t tb_item_func_str_dupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(func && item);

    // duplicate it
    if (data) *((tb_pointer_t*)item) = func->pool? tb_block_pool_strdup(func->pool, data) : tb_strdup(data);
    // clear it
    else *((tb_pointer_t*)item) = tb_null;
}
static tb_void_t tb_item_func_str_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(func && item);

    // free it
    if (func->free) func->free(func, item);

    // dupl it
    if (func->dupl) func->dupl(func, item, data);
}
static tb_void_t tb_item_func_str_copy(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(item);

    // copy it
    *((tb_cpointer_t*)item) = data;
}
static tb_void_t tb_item_func_str_nfree(tb_item_func_t* func, tb_pointer_t item, tb_size_t size)
{
    // check
    tb_assert_and_check_return(func && item);

    // free items 
    if (func->free)
    {
        tb_size_t n = size;
        while (n--) func->free(func, (tb_byte_t*)item + n * sizeof(tb_char_t*));
    }

    // clear
    if (size) tb_memset(item, 0, size * sizeof(tb_char_t*));
}
static tb_void_t tb_item_func_str_ndupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(func && item);

    // dupl items
    if (func->dupl) while (size--) func->dupl(func, (tb_byte_t*)item + size * sizeof(tb_char_t*), data);
}
static tb_void_t tb_item_func_str_nrepl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(func && item && data);

    // free items
    if (func->nfree) func->nfree(func, item, size);

    // dupl items
    if (func->ndupl) func->ndupl(func, item, data, size);
}
static tb_void_t tb_item_func_str_ncopy(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(item);

    // fill items
    if (size) tb_memset_ptr(item, data, size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_item_func_t tb_item_func_str(tb_bool_t bcase, tb_handle_t bpool)
{
    // init func
    tb_item_func_t func = {0};
    func.type   = TB_ITEM_TYPE_STR;
    func.hash   = tb_item_func_str_hash;
    func.comp   = tb_item_func_str_comp;
    func.data   = tb_item_func_str_data;
    func.cstr   = tb_item_func_str_cstr;
    func.free   = tb_item_func_str_free;
    func.dupl   = tb_item_func_str_dupl;
    func.repl   = tb_item_func_str_repl;
    func.copy   = tb_item_func_str_copy;
    func.nfree  = tb_item_func_str_nfree;
    func.ndupl  = tb_item_func_str_ndupl;
    func.nrepl  = tb_item_func_str_nrepl;
    func.ncopy  = tb_item_func_str_ncopy;
    func.pool   = bpool;
    func.priv   = tb_b2p(bcase);
    func.size   = sizeof(tb_char_t*);

    // ok?
    return func;
}
