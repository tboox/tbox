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
 * @author      alexyer, ruki
 * @file        fnv64.c
 * @ingroup     hash
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fnv64.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the pnv64 prime and offset basis
#define TB_FNV64_PRIME          (1099511628211ULL)
#define TB_FNV64_OFFSET_BASIS   (14695981039346656037ULL)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_uint64_t tb_fnv64_make(tb_byte_t const* data, tb_size_t size, tb_uint64_t seed)
{
    // check
    tb_assert_and_check_return_val(data && size, 0);

    // init value
    tb_uint64_t value = TB_FNV64_OFFSET_BASIS;
    if (seed) value = (value * TB_FNV64_PRIME) ^ seed;

    // generate it
    while (size)
    {
        value *= TB_FNV64_PRIME;
        value ^= (tb_uint64_t)*data++;
        size--;
    }
    return value;
}
tb_uint64_t tb_fnv64_make_from_cstr(tb_char_t const* cstr, tb_uint64_t seed)
{
    // check
    tb_assert_and_check_return_val(cstr, 0);

    // make it
    return tb_fnv64_make((tb_byte_t const*)cstr, tb_strlen(cstr) + 1, seed);
}
tb_uint64_t tb_fnv64_1a_make(tb_byte_t const* data, tb_size_t size, tb_uint64_t seed)
{
    // check
    tb_assert_and_check_return_val(data && size, 0);

    // init value
    tb_uint64_t value = TB_FNV64_OFFSET_BASIS;
    if (seed) value = (value * TB_FNV64_PRIME) ^ seed;

    // generate it
    while (size)
    {
        value ^= (tb_uint64_t)*data++;
        value *= TB_FNV64_PRIME;
        size--;
    }
    return value;
}
tb_uint64_t tb_fnv64_1a_make_from_cstr(tb_char_t const* cstr, tb_uint64_t seed)
{
    // check
    tb_assert_and_check_return_val(cstr, 0);

    // make it
    return tb_fnv64_1a_make((tb_byte_t const*)cstr, tb_strlen(cstr) + 1, seed);
}
