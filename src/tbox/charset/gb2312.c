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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        gb2312.c
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../stream/stream.h"
#include "gb2312.g"

/* //////////////////////////////////////////////////////////////////////////////////////
 * helper
 */
static tb_uint32_t tb_charset_gb2312_from_ucs4(tb_uint32_t ch)
{
    // is ascii?
    if (ch <= 0x7f) return ch;

    // find the gb2312 character
    tb_long_t left = 0;
    tb_long_t right = (g_charset_ucs4_to_gb2312_table_size / sizeof(g_charset_ucs4_to_gb2312_table_data[0])) - 1;
    while (left <= right)
    {
        // the middle character
        tb_long_t       mid = (left + right) >> 1;
        tb_uint16_t     mid_ucs4 = g_charset_ucs4_to_gb2312_table_data[mid][0];

        // find it?
        if (mid_ucs4 == ch)
            return g_charset_ucs4_to_gb2312_table_data[mid][1];

        if (ch > mid_ucs4) left = mid + 1;
        else right = mid - 1;
    }

    return 0;
}
static tb_uint32_t tb_charset_gb2312_to_ucs4(tb_uint32_t ch)
{
    // is ascii?
    if (ch <= 0x7f) return ch;

    // is gb2312?
    if (ch >= 0xa1a1 && ch <= 0xf7fe)
        return g_charset_gb2312_to_ucs4_table_data[ch - 0xa1a1];
    else return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_long_t tb_charset_gb2312_get(tb_static_stream_ref_t sstream, tb_bool_t be, tb_uint32_t* ch);
tb_long_t tb_charset_gb2312_get(tb_static_stream_ref_t sstream, tb_bool_t be, tb_uint32_t* ch)
{   
    // init
    tb_byte_t const*    p = tb_static_stream_pos(sstream);
    tb_size_t           n = tb_static_stream_left(sstream);

    if (*p <= 0x7f) 
    {
        // not enough? break it
        tb_check_return_val(n, -1);

        // get character
        *ch = tb_static_stream_read_u8(sstream);
    }
    else
    {
        // not enough? break it
        tb_check_return_val(n > 1, -1);

        // get character
        *ch = tb_charset_gb2312_to_ucs4(be? tb_static_stream_read_u16_be(sstream) : tb_static_stream_read_u16_le(sstream));
    }

    // ok
    return 1;
}

tb_long_t tb_charset_gb2312_set(tb_static_stream_ref_t sstream, tb_bool_t be, tb_uint32_t ch);
tb_long_t tb_charset_gb2312_set(tb_static_stream_ref_t sstream, tb_bool_t be, tb_uint32_t ch)
{
    // init
    tb_size_t n = tb_static_stream_left(sstream);

    // character
    ch = tb_charset_gb2312_from_ucs4(ch);
    if (ch <= 0x7f) 
    {
        // not enough? break it
        tb_check_return_val(n, -1);

        // set character
        tb_static_stream_writ_u8(sstream, ch & 0xff);
    }
    else
    {
        // not enough? break it
        tb_check_return_val(n > 1, 0);

        // set character
        if (be) tb_static_stream_writ_u16_be(sstream, ch & 0xffff);
        else tb_static_stream_writ_u16_le(sstream, ch & 0xffff);
    }

    // ok
    return 1;
}
