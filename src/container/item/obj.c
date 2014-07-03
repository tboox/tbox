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
 * @file        obj.c
 * @ingroup     container
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_char_t const* tb_item_func_obj_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(cstr, "");

    // format string
    tb_long_t n = tb_snprintf(cstr, maxn, "<object: %p>", data);
    if (n >= 0 && n < (tb_long_t)maxn) cstr[n] = '\0';

    // ok?
    return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_obj_free(tb_item_func_t* func, tb_pointer_t item)
{
    // check
    tb_assert_and_check_return(func && item);

    // exit
    tb_object_ref_t object = *((tb_object_ref_t*)item);
    if (object)
    {
        tb_object_exit(object);
        *((tb_object_ref_t*)item) = tb_null;
    }
}
static tb_void_t tb_item_func_obj_dupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(func && item);

    // refn++
    if (data) tb_object_inc((tb_object_ref_t)data);

    // copy it
    *((tb_cpointer_t*)item) = data;
}
static tb_void_t tb_item_func_obj_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(func && item);

    // save the previous object
    tb_object_ref_t object = *((tb_object_ref_t*)item);

    // refn++
    if (data) tb_object_inc((tb_object_ref_t)data);

    // copy it
    *((tb_cpointer_t*)item) = data;

    // refn--
    if (object) tb_object_dec(object);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_item_func_t tb_item_func_obj()
{
    // the ptr func
    tb_item_func_t func_ptr = tb_item_func_ptr(tb_null, tb_null);

    // the str func
    tb_item_func_t func_str = tb_item_func_str(tb_true, tb_null);

    // init func
    tb_item_func_t func = {0};
    func.type   = TB_ITEM_TYPE_OBJ;
    func.flag   = 0;
    func.hash   = func_ptr.hash;
    func.comp   = func_ptr.comp;
    func.data   = func_ptr.data;
    func.cstr   = tb_item_func_obj_cstr;
    func.free   = tb_item_func_obj_free;
    func.dupl   = tb_item_func_obj_dupl;
    func.repl   = tb_item_func_obj_repl;
    func.copy   = func_ptr.copy;
    func.nfree  = func_str.nfree;
    func.ndupl  = func_str.ndupl;
    func.nrepl  = func_str.nrepl;
    func.ncopy  = func_ptr.ncopy;
    func.size   = sizeof(tb_object_ref_t);
    
    // ok?
    return func;
}
