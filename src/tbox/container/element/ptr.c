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
 * @file        ptr.c
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
static tb_long_t tb_element_ptr_comp(tb_element_ref_t element, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
    return (ldata < rdata)? -1 : (ldata > rdata);
}
static tb_pointer_t tb_element_ptr_data(tb_element_ref_t element, tb_cpointer_t buff)
{
    // check
    tb_assert_and_check_return_val(buff, tb_null);

    // the element data
    return *((tb_pointer_t*)buff);
}
static tb_char_t const* tb_element_ptr_cstr(tb_element_ref_t element, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(cstr, "");

    // format string
    tb_long_t n = tb_snprintf(cstr, maxn - 1, "%p", data);
    if (n >= 0 && n < (tb_long_t)maxn) cstr[n] = '\0';

    // ok?
    return (tb_char_t const*)cstr;
}
static tb_void_t tb_element_ptr_free(tb_element_ref_t element, tb_pointer_t buff)
{
    // check
    tb_assert_and_check_return(buff);

    // clear it
    *((tb_pointer_t*)buff) = tb_null;
}
static tb_void_t tb_element_ptr_repl(tb_element_ref_t element, tb_pointer_t buff, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(element && buff);

    // the free is hooked? free it 
    if (element->free != tb_element_ptr_free && element->free)
        element->free(element, buff);

    // copy it
    *((tb_cpointer_t*)buff) = data;
}
static tb_void_t tb_element_ptr_copy(tb_element_ref_t element, tb_pointer_t buff, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(buff);

    // copy it
    *((tb_cpointer_t*)buff) = data;
}
static tb_void_t tb_element_ptr_dupl(tb_element_ref_t element, tb_pointer_t buff, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(buff);

    // dupl it
    *((tb_cpointer_t*)buff) = data;
}
static tb_void_t tb_element_ptr_nfree(tb_element_ref_t element, tb_pointer_t buff, tb_size_t size)
{
    // check
    tb_assert_and_check_return(element && buff);

    // the free is hooked? free it 
    if (element->free != tb_element_ptr_free && element->free)
    {
        tb_size_t n = size;
        while (n--) element->free(element, (tb_byte_t*)buff + n * sizeof(tb_pointer_t));
    }

    // clear
    if (size) tb_memset(buff, 0, size * sizeof(tb_pointer_t));
}
static tb_void_t tb_element_ptr_ndupl(tb_element_ref_t element, tb_pointer_t buff, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(buff);

    // copy elements
    if (element->ncopy) element->ncopy(element, buff, data, size);
}
static tb_void_t tb_element_ptr_nrepl(tb_element_ref_t element, tb_pointer_t buff, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(element && buff);

    // free element
    if (element->nfree) element->nfree(element, buff, size);

    // copy elements
    if (element->ncopy) element->ncopy(element, buff, data, size);
}
static tb_void_t tb_element_ptr_ncopy(tb_element_ref_t element, tb_pointer_t buff, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(buff);

    // fill elements
    if (size) tb_memset_ptr(buff, data, size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_element_t tb_element_ptr(tb_element_free_func_t free, tb_cpointer_t priv)
{
    // the size element
    tb_element_t element_size = tb_element_size();

    // init element
    tb_element_t element = {0};
    element.type   = TB_ELEMENT_TYPE_PTR;
    element.flag   = 0;
    element.hash   = element_size.hash;
    element.comp   = tb_element_ptr_comp;
    element.data   = tb_element_ptr_data;
    element.cstr   = tb_element_ptr_cstr;
    element.free   = free? free : tb_element_ptr_free;
    element.dupl   = tb_element_ptr_dupl;
    element.repl   = tb_element_ptr_repl;
    element.copy   = tb_element_ptr_copy;
    element.nfree  = tb_element_ptr_nfree;
    element.ndupl  = tb_element_ptr_ndupl;
    element.nrepl  = tb_element_ptr_nrepl;
    element.ncopy  = tb_element_ptr_ncopy;
    element.priv   = priv;
    element.size   = sizeof(tb_pointer_t);

    // ok?
    return element;
}
