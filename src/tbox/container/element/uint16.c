/*!The Treasure Box Library
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2009 - 2018, TBOOX Open Source Group.
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
static tb_size_t tb_element_uint16_hash(tb_element_ref_t element, tb_cpointer_t data, tb_size_t mask, tb_size_t index)
{
    return tb_element_hash_uint16(tb_p2u16(data), mask, index);
}
static tb_long_t tb_element_uint16_comp(tb_element_ref_t element, tb_cpointer_t ldata, tb_cpointer_t rdata)
{
    // compare it
    return ((tb_p2u16(ldata) < tb_p2u16(rdata))? -1 : (tb_p2u16(ldata) > tb_p2u16(rdata)));
}
static tb_pointer_t tb_element_uint16_data(tb_element_ref_t element, tb_cpointer_t buff)
{
    // check
    tb_assert_and_check_return_val(buff, tb_null);

    // the element data
    return tb_u2p(*((tb_uint16_t*)buff));
}
static tb_char_t const* tb_element_uint16_cstr(tb_element_ref_t element, tb_cpointer_t data, tb_char_t* cstr, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(element && cstr, "");

    // format string
    tb_long_t n = tb_snprintf(cstr, maxn, "%u", (tb_uint16_t)(tb_size_t)data);
    if (n >= 0 && n < (tb_long_t)maxn) cstr[n] = '\0';

    // ok?
    return (tb_char_t const*)cstr;
}
static tb_void_t tb_element_uint16_free(tb_element_ref_t element, tb_pointer_t buff)
{
    // check
    tb_assert_and_check_return(buff);

    // clear
    *((tb_uint16_t*)buff) = 0;
}
static tb_void_t tb_element_uint16_copy(tb_element_ref_t element, tb_pointer_t buff, tb_cpointer_t data)
{
    // check
    tb_assert_and_check_return(buff);

    // copy element
    *((tb_uint16_t*)buff) = tb_p2u16(data);
}
static tb_void_t tb_element_uint16_nfree(tb_element_ref_t element, tb_pointer_t buff, tb_size_t size)
{
    // check
    tb_assert_and_check_return(buff);

    // clear elements
    if (size) tb_memset(buff, 0, size * sizeof(tb_uint16_t));
}
static tb_void_t tb_element_uint16_ncopy(tb_element_ref_t element, tb_pointer_t buff, tb_cpointer_t data, tb_size_t size)
{
    // check
    tb_assert_and_check_return(buff);

    // copy elements
    tb_memset_u16(buff, tb_p2u16(data), size);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_element_t tb_element_uint16()
{
    // init element
    tb_element_t element = {0};
    element.type   = TB_ELEMENT_TYPE_UINT16;
    element.flag   = 0;
    element.hash   = tb_element_uint16_hash;
    element.comp   = tb_element_uint16_comp;
    element.data   = tb_element_uint16_data;
    element.cstr   = tb_element_uint16_cstr;
    element.free   = tb_element_uint16_free;
    element.dupl   = tb_element_uint16_copy;
    element.repl   = tb_element_uint16_copy;
    element.copy   = tb_element_uint16_copy;
    element.nfree  = tb_element_uint16_nfree;
    element.ndupl  = tb_element_uint16_ncopy;
    element.nrepl  = tb_element_uint16_ncopy;
    element.ncopy  = tb_element_uint16_ncopy;
    element.size   = sizeof(tb_uint16_t);

    // ok?
    return element;
}
