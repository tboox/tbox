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
 * @file        uint16.c
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
static tb_size_t tb_item_func_uint16_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t mask, tb_size_t index)
{
    return tb_item_func_hash_uint16(tb_p2u16(data), mask, index);
}
static tb_long_t tb_item_func_uint16_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
    // compare it
    return (tb_p2u16(ldata) > tb_p2u16(rdata)? 1 : (tb_p2u16(ldata) < tb_p2u16(rdata)? -1 : 0));
}
static tb_pointer_t tb_item_func_uint16_data(tb_item_func_t* func, tb_cpointer_t buff)
{
    // check
    tb_assert_and_check_return_val(buff, tb_null);

    // the item data
    return tb_u2p(*((tb_uint16_t*)buff));
}
static tb_char_t const* tb_item_func_uint16_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(func && cstr, "");

    // format string
    tb_long_t n = tb_snprintf(cstr, maxn, "%u", (tb_uint16_t)(tb_size_t)data);
    if (n >= 0 && n < (tb_long_t)maxn) cstr[n] = '\0';

    // ok?
    return (tb_char_t const*)cstr;
}
static tb_bool_t tb_item_func_uint16_load(tb_item_func_t* func, tb_pointer_t buff, tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(buff && stream, tb_false);

    // load it
    *((tb_uint16_t*)buff) = tb_stream_bread_u16_be(stream);

    // ok
    return tb_true;
}
static tb_bool_t tb_item_func_uint16_save(tb_item_func_t* func, tb_cpointer_t data, tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream, tb_false);

    // save it
    return tb_stream_bwrit_u16_be(stream, (tb_uint16_t)data);
}
static tb_void_t tb_item_func_uint16_free(tb_item_func_t* func, tb_pointer_t buff)
{
    // check
    tb_assert_and_check_return(buff);

    // clear
    *((tb_uint16_t*)buff) = 0;
}
static tb_void_t tb_item_func_uint16_copy(tb_item_func_t* func, tb_pointer_t buff, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(buff);

    // copy item
    *((tb_uint16_t*)buff) = tb_p2u16(data);
}
static tb_void_t tb_item_func_uint16_nfree(tb_item_func_t* func, tb_pointer_t buff, tb_size_t size)
{
    // check
    tb_assert_and_check_return(buff);

    // clear items
    if (size) tb_memset(buff, 0, size * sizeof(tb_uint16_t));
}
static tb_void_t tb_item_func_uint16_ncopy(tb_item_func_t* func, tb_pointer_t buff, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(buff);

    // copy items
    tb_memset_u16(buff, tb_p2u16(data), size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_item_func_t tb_item_func_uint16()
{
    // init func
    tb_item_func_t func = {0};
    func.type   = TB_ITEM_TYPE_UINT16;
    func.flag   = 0;
    func.hash   = tb_item_func_uint16_hash;
    func.comp   = tb_item_func_uint16_comp;
    func.data   = tb_item_func_uint16_data;
    func.cstr   = tb_item_func_uint16_cstr;
    func.load   = tb_item_func_uint16_load;
    func.save   = tb_item_func_uint16_save;
    func.free   = tb_item_func_uint16_free;
    func.dupl   = tb_item_func_uint16_copy;
    func.repl   = tb_item_func_uint16_copy;
    func.copy   = tb_item_func_uint16_copy;
    func.nfree  = tb_item_func_uint16_nfree;
    func.ndupl  = tb_item_func_uint16_ncopy;
    func.nrepl  = tb_item_func_uint16_ncopy;
    func.ncopy  = tb_item_func_uint16_ncopy;
    func.size   = sizeof(tb_uint16_t);

    // ok?
    return func;
}
