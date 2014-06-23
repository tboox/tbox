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
 * @file        string_pool.c
 * @ingroup     container
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_item_func_string_pool_free(tb_item_func_t* func, tb_pointer_t item)
{
    // check
    tb_assert_and_check_return(func && item);

    // free item
    if (*((tb_pointer_t*)item)) tb_object_string_pool_del(tb_object_string_pool(), *((tb_char_t const**)item));
}
static tb_void_t tb_item_func_string_pool_dupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(func && item);
 
    // dupl item
    if (data) *((tb_pointer_t*)item) = (tb_pointer_t)tb_object_string_pool_put(tb_object_string_pool(), (tb_char_t const*)data);
    else *((tb_pointer_t*)item) = tb_null;
}
static tb_void_t tb_item_func_string_pool_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(func && item);
 
    // copy it, refn++
    tb_pointer_t copy = data? (tb_pointer_t)tb_object_string_pool_put(tb_object_string_pool(), (tb_char_t const*)data) : tb_null;
 
    // free it, refn--
    if (func->free) func->free(func, item);
 
    // save it
    *((tb_pointer_t*)item) = copy;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_item_func_t tb_item_func_string_pool(tb_handle_t spool)
{
    // check
    tb_assert(spool);

    // the str func
    tb_item_func_t func_str = tb_item_func_str(tb_true, tb_null);

    // init func
    tb_item_func_t func = {0};
    func.type   = TB_ITEM_TYPE_STRING_POOL;
    func.hash   = func_str.hash;
    func.comp   = func_str.comp;
    func.data   = func_str.data;
    func.cstr   = func_str.cstr;
    func.free   = tb_item_func_string_pool_free;
    func.dupl   = tb_item_func_string_pool_dupl;
    func.repl   = tb_item_func_string_pool_repl;
    func.copy   = func_str.copy;
    func.nfree  = func_str.nfree;
    func.ndupl  = func_str.ndupl;
    func.nrepl  = func_str.nrepl;
    func.ncopy  = func_str.ncopy;
    func.pool   = spool;
    func.size   = sizeof(tb_char_t*);

    // ok?
    return func;
}
