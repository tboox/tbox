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
 * @file        size.c
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
static tb_size_t tb_item_func_size_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t mask, tb_size_t index)
{
#if TB_CPU_BIT64
    tb_assert_static(sizeof(tb_size_t) == sizeof(tb_uint64_t));
    return tb_item_func_hash_uint64((tb_uint64_t)data, mask, index);
#else
    tb_assert_static(sizeof(tb_size_t) == sizeof(tb_uint32_t));
    return tb_item_func_hash_uint32((tb_uint32_t)data, mask, index);
#endif
}
static tb_long_t tb_item_func_size_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
    // compare it
    return ((tb_size_t)ldata > (tb_size_t)rdata? 1 : ((tb_size_t)ldata < (tb_size_t)rdata? -1 : 0));
}
static tb_pointer_t tb_item_func_size_data(tb_item_func_t* func, tb_cpointer_t buff)
{
    // check
    tb_assert_and_check_return_val(buff, tb_null);

    // the item data
    return (tb_pointer_t)*((tb_size_t*)buff);
}
static tb_char_t const* tb_item_func_size_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(func && cstr, "");

    // format string
    tb_long_t n = tb_snprintf(cstr, maxn, "%lu", (tb_size_t)data);
    if (n >= 0 && n < (tb_long_t)maxn) cstr[n] = '\0';

    // ok?
    return (tb_char_t const*)cstr;
}
static tb_bool_t tb_item_func_size_load(tb_item_func_t* func, tb_pointer_t buff, tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(buff && stream, tb_false);

    // load it
#if TB_CPU_BIT64
    *((tb_size_t*)buff) = (tb_size_t)tb_stream_bread_u64_be(stream);
#else
    *((tb_size_t*)buff) = (tb_size_t)tb_stream_bread_u32_be(stream);
#endif

    // ok
    return tb_true;
}
static tb_bool_t tb_item_func_size_save(tb_item_func_t* func, tb_cpointer_t data, tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream, tb_false);

    // save it
#if TB_CPU_BIT64
    return tb_stream_bwrit_u64_be(stream, (tb_uint64_t)data);
#else
    return tb_stream_bwrit_u32_be(stream, (tb_uint32_t)data);
#endif
}
static tb_void_t tb_item_func_size_free(tb_item_func_t* func, tb_pointer_t buff)
{
    // check
    tb_assert_and_check_return(buff);

    // clear
    *((tb_size_t*)buff) = 0;
}
static tb_void_t tb_item_func_size_copy(tb_item_func_t* func, tb_pointer_t buff, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(buff);

    // copy item
    *((tb_size_t*)buff) = (tb_size_t)data;
}
static tb_void_t tb_item_func_size_nfree(tb_item_func_t* func, tb_pointer_t buff, tb_size_t size)
{
    // check
    tb_assert_and_check_return(buff);

    // clear items
    if (size) tb_memset(buff, 0, size * sizeof(tb_size_t));
}
static tb_void_t tb_item_func_size_ncopy(tb_item_func_t* func, tb_pointer_t buff, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(buff);

    // copy items
    if (size) tb_memset_ptr(buff, data, size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_item_func_t tb_item_func_size()
{
    // init func
    tb_item_func_t func = {0};
    func.type   = TB_ITEM_TYPE_SIZE;
    func.flag   = 0;
    func.hash   = tb_item_func_size_hash;
    func.comp   = tb_item_func_size_comp;
    func.data   = tb_item_func_size_data;
    func.cstr   = tb_item_func_size_cstr;
    func.load   = tb_item_func_size_load;
    func.save   = tb_item_func_size_save;
    func.free   = tb_item_func_size_free;
    func.dupl   = tb_item_func_size_copy;
    func.repl   = tb_item_func_size_copy;
    func.copy   = tb_item_func_size_copy;
    func.nfree  = tb_item_func_size_nfree;
    func.ndupl  = tb_item_func_size_ncopy;
    func.nrepl  = tb_item_func_size_ncopy;
    func.ncopy  = tb_item_func_size_ncopy;
    func.size   = sizeof(tb_size_t);

    // ok?
    return func;
}
