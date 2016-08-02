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
#include "../crc8.h"
#include "../crc16.h"
#include "../crc32.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_uint32_t tb_crc_encode(tb_crc_mode_t mode, tb_uint32_t crc, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(data, 0);

    // trace
    tb_trace_deprecated();

    // done
    switch (mode)
    {
    case TB_CRC_MODE_32_IEEE_LE:
        return tb_crc32_le_make(data, size, crc);
    case TB_CRC_MODE_16_CCITT:
        return tb_crc16_ccitt_make(data, size, crc);
#ifndef __tb_small__
    case TB_CRC_MODE_32_IEEE:
        return tb_crc32_make(data, size, crc);
    case TB_CRC_MODE_16_ANSI:
        return tb_crc16_make(data, size, crc);
    case TB_CRC_MODE_8_ATM:
        return tb_crc8_make(data, size, crc);
    default:
        break;
#endif
    }

    // failed
    return 0;
}
tb_uint32_t tb_crc_encode_cstr(tb_crc_mode_t mode, tb_uint32_t crc, tb_char_t const* cstr)
{
    // check
    tb_assert_and_check_return_val(cstr, 0);

    // ok?
    return tb_crc_encode(mode, crc, (tb_byte_t const*)cstr, tb_strlen(cstr));
}

