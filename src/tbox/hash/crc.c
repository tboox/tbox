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
 * @file        crc.c
 * @ingroup     hash
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "crc.h"
#include "crc.g"
#if defined(TB_ARCH_ARM)
#   include "impl/crc_arm.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_uint32_t tb_crc_make(tb_crc_mode_t mode, tb_byte_t const* data, tb_size_t size, tb_uint32_t seed)
{
    // check
    tb_assert_and_check_return_val(mode < TB_CRC_MODE_MAX && data, 0);

    // init value
    tb_uint32_t crc = seed;

    // done
#ifdef tb_crc32_make
#   ifndef __tb_small__
    if (mode == TB_CRC_MODE_32_IEEE_LE || mode == TB_CRC_MODE_32_IEEE)
#   else
    if (mode == TB_CRC_MODE_32_IEEE_LE)
#   endif
    {
        crc = tb_crc32_make(crc, data, size, (tb_uint32_t const*)&g_crc_table[mode]);
    }
    else
    {
        tb_byte_t const*    ie = data + size;
        tb_uint32_t const*  pt = (tb_uint32_t const*)&g_crc_table[mode];
        while (data < ie) crc = pt[((tb_uint8_t)crc) ^ *data++] ^ (crc >> 8);
    }
#else
    tb_byte_t const*    ie = data + size;
    tb_uint32_t const*  pt = (tb_uint32_t const*)&g_crc_table[mode];
    while (data < ie) crc = pt[((tb_uint8_t)crc) ^ *data++] ^ (crc >> 8);
#endif

    // ok?
    return crc;
}
tb_uint32_t tb_crc_make_from_cstr(tb_crc_mode_t mode, tb_char_t const* cstr, tb_uint32_t seed)
{
    // check
    tb_assert_and_check_return_val(cstr, 0);

    // make it
    return tb_crc_make(mode, (tb_byte_t const*)cstr, tb_strlen(cstr), seed);
}
