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
 * @file        mem.c
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
static tb_size_t tb_item_func_mem_hash(tb_item_func_t* func, tb_cpointer_t data, tb_size_t mask, tb_size_t index)
{   
    return tb_item_func_hash_data(data, func->size, mask, index);
}
static tb_long_t tb_item_func_mem_comp(tb_item_func_t* func, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
    // check
    tb_assert_and_check_return_val(func && func->size && ldata && rdata, 0);

    // comp
    return tb_memcmp(ldata, rdata, func->size);
}
static tb_pointer_t tb_item_func_mem_data(tb_item_func_t* func, tb_cpointer_t item)
{
    // check
    tb_assert_and_check_return_val(func && item, tb_null);

    // the item data
    return (tb_pointer_t)item;
}
static tb_char_t const* tb_item_func_mem_cstr(tb_item_func_t* func, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(func && func->size && cstr && data, "");

    // format string
    tb_long_t n = tb_snprintf(cstr, maxn - 1, "0x%x", tb_crc_encode(TB_CRC_MODE_32_IEEE_LE, 0, data, func->size));
    if (n >= 0 && n < (tb_long_t)maxn) cstr[n] = '\0';

    // ok?
    return (tb_char_t const*)cstr;
}
static tb_void_t tb_item_func_mem_free(tb_item_func_t* func, tb_pointer_t item)
{
    // check
    tb_assert_and_check_return(func && func->size && item);

    // clear it
    tb_memset(item, 0, func->size);
}
static tb_void_t tb_item_func_mem_dupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(func && func->size && item && data);

    // copy item
    tb_memcpy(item, data, func->size);
}
static tb_void_t tb_item_func_mem_repl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(func && func->size && item && data);

    // the free is hooked? free it 
    if (func->free != tb_item_func_mem_free && func->free)
        func->free(func, item);

    // copy item
    tb_memcpy(item, data, func->size);
}
static tb_void_t tb_item_func_mem_copy(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(func && func->size && item && data);

    // copy item
    tb_memcpy(item, data, func->size);
}
static tb_void_t tb_item_func_mem_nfree(tb_item_func_t* func, tb_pointer_t item, tb_size_t size)
{
    // check
    tb_assert_and_check_return(func && func->size && item);

    // the free is hooked? free it 
    if (func->free != tb_item_func_mem_free && func->free)
    {
        tb_size_t n = size;
        while (n--) func->free(func, (tb_byte_t*)item + n * func->size);
    }

    // clear
    if (size) tb_memset(item, 0, size * func->size);
}
static tb_void_t tb_item_func_mem_ndupl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(func && func->size && item && data);

    // copy items
    if (func->ncopy) func->ncopy(func, item, data, size);
}
static tb_void_t tb_item_func_mem_nrepl(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(func && func->size && item && data);

    // free items
    if (func->nfree) func->nfree(func, item, size);

    // copy items
    if (func->ncopy) func->ncopy(func, item, data, size);
}
static tb_void_t tb_item_func_mem_ncopy(tb_item_func_t* func, tb_pointer_t item, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(func && func->size && item && data);

    // copy items
    while (size--) tb_memcpy((tb_byte_t*)item + size * func->size, data, func->size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_item_func_t tb_item_func_mem(tb_size_t size, tb_item_func_free_t free, tb_cpointer_t priv)
{
    // init func
    tb_item_func_t func = {0};
    func.type   = TB_ITEM_TYPE_MEM;
    func.hash   = tb_item_func_mem_hash;
    func.comp   = tb_item_func_mem_comp;
    func.data   = tb_item_func_mem_data;
    func.cstr   = tb_item_func_mem_cstr;
    func.free   = free? free : tb_item_func_mem_free;
    func.dupl   = tb_item_func_mem_dupl;
    func.repl   = tb_item_func_mem_repl;
    func.copy   = tb_item_func_mem_copy;
    func.nfree  = tb_item_func_mem_nfree;
    func.ndupl  = tb_item_func_mem_ndupl;
    func.nrepl  = tb_item_func_mem_nrepl;
    func.ncopy  = tb_item_func_mem_ncopy;
    func.size   = size;
    func.priv   = priv;

    // ok?
    return func;
}
