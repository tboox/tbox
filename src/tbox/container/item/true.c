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
 * @file        true.c
 * @ingroup     container
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_size_t tb_item_func_true_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t size, tb_size_t index)
{
    return 0;
}
static tb_long_t tb_item_func_true_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
    // always be equal
    return 0;
}
static tb_pointer_t tb_item_func_true_data(tb_item_func_t* func, tb_cpointer_t item)
{
    // the item data
    return (tb_pointer_t)tb_true;
}
static tb_char_t const* tb_item_func_true_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(func && cstr && maxn, "");

    // format string
    tb_strlcpy(cstr, "true", maxn - 1); cstr[maxn - 1] = '\0';

    // ok?
    return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_true_free(tb_item_func_t* func, tb_pointer_t buff)
{
}
static tb_void_t tb_item_func_true_nfree(tb_item_func_t* func, tb_pointer_t buff, tb_size_t size)
{
}
static tb_void_t tb_item_func_true_repl(tb_item_func_t* func, tb_pointer_t buff, tb_cpointer_t data)
{
    tb_assert_abort((tb_bool_t)(tb_size_t)data == tb_true);
}
static tb_void_t tb_item_func_true_nrepl(tb_item_func_t* func, tb_pointer_t buff, tb_cpointer_t data, tb_size_t size)
{
    tb_assert_abort((tb_bool_t)(tb_size_t)data == tb_true);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_item_func_t tb_item_func_true()
{
    // init func
    tb_item_func_t func = {0};
    func.type   = TB_ITEM_TYPE_TRUE;
    func.flag   = 0;
    func.hash   = tb_item_func_true_hash;
    func.comp   = tb_item_func_true_comp;
    func.data   = tb_item_func_true_data;
    func.cstr   = tb_item_func_true_cstr;
    func.free   = tb_item_func_true_free;
    func.dupl   = tb_item_func_true_repl;
    func.repl   = tb_item_func_true_repl;
    func.copy   = tb_item_func_true_repl;
    func.nfree  = tb_item_func_true_nfree;
    func.ndupl  = tb_item_func_true_nrepl;
    func.nrepl  = tb_item_func_true_nrepl;
    func.ncopy  = tb_item_func_true_nrepl;
    func.size   = 0;

    // ok?
    return func;
}
