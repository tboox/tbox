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
 * @author      alexyer
 * @file        fnv32.c
 * @ingroup     utils
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fnv32.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// FNV prime
#define FNV_32_PRIME (0x01000193)
#define FNV0 (0x811c9dc5)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_uint32_t tb_fnv32_encode(tb_byte_t const* data)
{
    tb_uint32_t hval = FNV0;

    while (*data)
    {
        hval *= FNV_32_PRIME;
        hval ^= (unsigned int)*data++;
    }

    return hval;
}
