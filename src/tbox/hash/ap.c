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
 * Copyright (C) 2016, Olexander Yermakov All rights reserved.
 *
 * @author      ruki
 * @file        ap.c
 * @ingroup     hash
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "ap.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_ap_make(tb_byte_t const* data, tb_size_t size, tb_size_t seed)
{
    // check
    tb_assert_and_check_return_val(data && size, 0);

    // init value
    tb_size_t v = 0xAAAAAAAA;
    if (seed) v ^= seed;

    // generate it
    tb_size_t           i = 0;
    tb_byte_t const*    p = data;
    for (i = 0; i < size; i++, p++) 
    {  
        v ^= (!(i & 1)) ? ((v << 7) ^ ((*p) * (v >> 3))) : (~(((v << 11) + (*p)) ^ (v >> 5)));  
    }
    return v;  
}
tb_size_t tb_ap_make_from_cstr(tb_char_t const* cstr, tb_size_t seed)
{
    // check
    tb_assert_and_check_return_val(cstr, 0);

    // make it
    return tb_ap_make((tb_byte_t const*)cstr, tb_strlen(cstr), seed);
}
