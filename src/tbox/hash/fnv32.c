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
 * Copyright (C) 2016, Olexander Yermakov and ruki All rights reserved.
 *
 * @author      alexyer, ruki
 * @file        fnv32.c
 * @ingroup     hash
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fnv32.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the pnv32 prime and offset basis
#define TB_FNV32_PRIME          (16777619)
#define TB_FNV32_OFFSET_BASIS   (2166136261)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_uint32_t tb_fnv32_make(tb_byte_t const* data, tb_size_t size, tb_uint32_t seed)
{
    // check
    tb_assert_and_check_return_val(data && size, 0);

    // init value
    tb_uint32_t value = TB_FNV32_OFFSET_BASIS;
    if (seed) value = (value * TB_FNV32_PRIME) ^ seed;

    // generate it
    while (size)
    {
        value *= TB_FNV32_PRIME;
        value ^= (tb_uint32_t)*data++;
        size--;
    }
    return value;
}
tb_uint32_t tb_fnv32_make_from_cstr(tb_char_t const* cstr, tb_uint32_t seed)
{
    // check
    tb_assert_and_check_return_val(cstr, 0);

    // make it
    return tb_fnv32_make((tb_byte_t const*)cstr, tb_strlen(cstr) + 1, seed);
}
tb_uint32_t tb_fnv32_1a_make(tb_byte_t const* data, tb_size_t size, tb_uint32_t seed)
{
    // check
    tb_assert_and_check_return_val(data && size, 0);

    // init value
    tb_uint32_t value = TB_FNV32_OFFSET_BASIS;
    if (seed) value = (value * TB_FNV32_PRIME) ^ seed;

    // generate it
    while (size)
    {
        value ^= (tb_uint32_t)*data++;
        value *= TB_FNV32_PRIME;
        size--;
    }
    return value;
}
tb_uint32_t tb_fnv32_1a_make_from_cstr(tb_char_t const* cstr, tb_uint32_t seed)
{
    // check
    tb_assert_and_check_return_val(cstr, 0);

    // make it
    return tb_fnv32_1a_make((tb_byte_t const*)cstr, tb_strlen(cstr) + 1, seed);
}
