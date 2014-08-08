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
 * @file        long.c
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
static tb_long_t tb_item_func_long_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
    // compare it
    return ((tb_long_t)ldata > (tb_long_t)rdata? 1 : ((tb_long_t)ldata < (tb_long_t)rdata? -1 : 0));
}
static tb_pointer_t tb_item_func_long_data(tb_item_func_t* func, tb_cpointer_t buff)
{
    // check
    tb_assert_and_check_return_val(buff, tb_null);

    // the item data
    return (tb_pointer_t)*((tb_long_t*)buff);
}
static tb_char_t const* tb_item_func_long_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(func && cstr, "");

    // format string
    tb_long_t n = tb_snprintf(cstr, maxn, "%ld", (tb_long_t)data);
    if (n >= 0 && n < (tb_long_t)maxn) cstr[n] = '\0';

    // ok?
    return (tb_char_t const*)cstr;
}
static tb_bool_t tb_item_func_long_load(tb_item_func_t* func, tb_pointer_t buff, tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(buff && stream, tb_false);

    // load it
#if TB_CPU_BIT64
    *((tb_long_t*)buff) = (tb_long_t)tb_stream_bread_s64_be(stream);
#else
    *((tb_long_t*)buff) = (tb_long_t)tb_stream_bread_s32_be(stream);
#endif

    // ok
    return tb_true;
}
static tb_bool_t tb_item_func_long_save(tb_item_func_t* func, tb_cpointer_t data, tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream, tb_false);

    // save it
#if TB_CPU_BIT64
    return tb_stream_bwrit_s64_be(stream, (tb_sint64_t)data);
#else
    return tb_stream_bwrit_s32_be(stream, (tb_sint32_t)data);
#endif
}
static tb_void_t tb_item_func_long_free(tb_item_func_t* func, tb_pointer_t buff)
{
    // check
    tb_assert_and_check_return(buff);

    // clear
    *((tb_size_t*)buff) = 0;
}
static tb_void_t tb_item_func_long_copy(tb_item_func_t* func, tb_pointer_t buff, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(buff);

    // copy item
    *((tb_long_t*)buff) = (tb_long_t)data;
}
static tb_void_t tb_item_func_long_nfree(tb_item_func_t* func, tb_pointer_t buff, tb_size_t size)
{
    // check
    tb_assert_and_check_return(buff);

    // clear items
    if (size) tb_memset(buff, 0, size * sizeof(tb_long_t));
}
static tb_void_t tb_item_func_long_ncopy(tb_item_func_t* func, tb_pointer_t buff, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(buff);

    // copy items
    if (size) tb_memset_ptr(buff, data, size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_item_func_t tb_item_func_long()
{
    // the size func
    tb_item_func_t func_size = tb_item_func_size();

    // init func
    tb_item_func_t func = {0};
    func.type   = TB_ITEM_TYPE_LONG;
    func.flag   = 0;
    func.hash   = func_size.hash;
    func.comp   = tb_item_func_long_comp;
    func.data   = tb_item_func_long_data;
    func.cstr   = tb_item_func_long_cstr;
    func.load   = tb_item_func_long_load;
    func.save   = tb_item_func_long_save;
    func.free   = tb_item_func_long_free;
    func.dupl   = tb_item_func_long_copy;
    func.repl   = tb_item_func_long_copy;
    func.copy   = tb_item_func_long_copy;
    func.nfree  = tb_item_func_long_nfree;
    func.ndupl  = tb_item_func_long_ncopy;
    func.nrepl  = tb_item_func_long_ncopy;
    func.ncopy  = tb_item_func_long_ncopy;
    func.size   = sizeof(tb_long_t);

    // ok?
    return func;
}
