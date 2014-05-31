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
 * @file        utf8.c
 * @ingroup     charset
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../stream/stream.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

/*
 * 0x00000000 - 0x0000007f:  0xxxxxxx
 * 0x00000080 - 0x000007ff:  110xxxxx 10xxxxxx
 * 0x00000800 - 0x0000ffff:  1110xxxx 10xxxxxx 10xxxxxx
 * 0x00010000 - 0x001fffff:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 * 0x00200000 - 0x03ffffff:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 * 0x04000000 - 0x7fffffff:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 */
tb_long_t tb_charset_utf8_get(tb_static_stream_t* sstream, tb_bool_t be, tb_uint32_t* ch);
tb_long_t tb_charset_utf8_get(tb_static_stream_t* sstream, tb_bool_t be, tb_uint32_t* ch)
{
    // init
    tb_byte_t const*    p = tb_static_stream_pos(sstream);
    tb_byte_t const*    q = p;
    tb_size_t           n = tb_static_stream_left(sstream);

    // 0x00000000 - 0x0000007f
    if (!(*p & 0x80))
    {
        // not enough? break it
        tb_check_return_val(n, -1);

        // get character
        *ch = *p++;
    }
    // 0x00000080 - 0x000007ff
    else if ((*p & 0xe0) == 0xc0)
    {
        // not enough? break it
        tb_check_return_val(n > 1, -1);

        // get character
        *ch = ((((tb_uint32_t)(p[0] & 0x1f)) << 6) | (p[1] & 0x3f));
        p += 2;
    }
    // 0x00000800 - 0x0000ffff
    else if ((*p & 0xf0) == 0xe0)
    {
        // not enough? break it
        tb_check_return_val(n > 2, -1);

        // get character
        *ch = ((((tb_uint32_t)(p[0] & 0x0f)) << 12) | (((tb_uint32_t)(p[1] & 0x3f)) << 6) | (p[2] & 0x3f));
        p += 3;
    }
    // 0x00010000 - 0x001fffff
    else if ((*p & 0xf8) == 0xf0)
    {
        // not enough? break it
        tb_check_return_val(n > 3, -1);

        // get character
        *ch = ((((tb_uint32_t)(p[0] & 0x07)) << 18) | (((tb_uint32_t)(p[1] & 0x3f)) << 12) | (((tb_uint32_t)(p[2] & 0x3f)) << 6) | (p[3] & 0x3f));
        p += 4;
    }
    // 0x00200000 - 0x03ffffff
    else if ((*p & 0xfc) == 0xf8)
    {
        // not enough? break it
        tb_check_return_val(n > 4, -1);

        // get character
        *ch = ((((tb_uint32_t)(p[0] & 0x03)) << 24) | (((tb_uint32_t)(p[1] & 0x3f)) << 18) | (((tb_uint32_t)(p[2] & 0x3f)) << 12) | (((tb_uint32_t)(p[3] & 0x3f)) << 6) | (p[4] & 0x3f));
        p += 5;
    }
    // 0x04000000 - 0x7fffffff
    else if ((*p & 0xfe) == 0xfc)
    {
        // not enough? break it
        tb_check_return_val(n > 5, -1);

        // get character
        *ch = ((((tb_uint32_t)(p[0] & 0x01)) << 30) | (((tb_uint32_t)(p[1] & 0x3f)) << 24) | (((tb_uint32_t)(p[2] & 0x3f)) << 18) | (((tb_uint32_t)(p[3] & 0x3f)) << 12) | (((tb_uint32_t)(p[4] & 0x3f)) << 6) | (p[5] & 0x3f));
        p += 6;
    }
    else
    {
        // invalid character
        tb_trace_d("invalid utf8 character: %x", *p);

        // skip it
        tb_static_stream_skip(sstream, 1);

        // no character
        return 0;
    }

    // next
    if (p > q) tb_static_stream_skip(sstream, p - q);

    // ok?
    return p > q? 1 : 0;
}

tb_long_t tb_charset_utf8_set(tb_static_stream_t* sstream, tb_bool_t be, tb_uint32_t ch);
tb_long_t tb_charset_utf8_set(tb_static_stream_t* sstream, tb_bool_t be, tb_uint32_t ch)
{
    // init
    tb_byte_t*  p = (tb_byte_t*)tb_static_stream_pos(sstream);
    tb_byte_t*  q = p;
    tb_size_t   n = tb_static_stream_left(sstream);

    // 0x00000000 - 0x0000007f
    if (ch <= 0x0000007f) 
    {
        // not enough? break it
        tb_check_return_val(n, -1);

        // set character
        *p++ = ch;
    }
    // 0x00000080 - 0x000007ff
    else if (ch <= 0x000007ff) 
    {
        // not enough? break it
        tb_check_return_val(n > 1, -1);

        // set character
        // 110xxxxx 10xxxxxx
        //      xxx xxxxxxxx
        *p++ = ((ch >> 6) & 0x1f) | 0xc0;
        *p++ = (ch & 0x3f) | 0x80;
    }
    // 0x00000800 - 0x0000ffff
    else if (ch <= 0x0000ffff) 
    {
        // not enough? break it
        tb_check_return_val(n > 2, -1);

        // set character
        // 1110xxxx 10xxxxxx 10xxxxxx
        //          xxxxxxxx xxxxxxxx
        *p++ = ((ch >> 12) & 0x0f) | 0xe0;
        *p++ = ((ch >> 6) & 0x3f) | 0x80;
        *p++ = (ch & 0x3f) | 0x80;
    }
    // 0x00010000 - 0x001fffff
    else if (ch <= 0x001fffff) 
    {
        // not enough? break it
        tb_check_return_val(n > 3, -1);

        // set character
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        //             xxxxx xxxxxxxx xxxxxxxx
        *p++ = ((ch >> 18) & 0x07) | 0xf0;
        *p++ = ((ch >> 12) & 0x3f) | 0x80;
        *p++ = ((ch >> 6) & 0x3f) | 0x80;
        *p++ = (ch & 0x3f) | 0x80;
    }
    // 0x00200000 - 0x03ffffff
    else if (ch <= 0x03ffffff) 
    {
        // not enough? break it
        tb_check_return_val(n > 4, -1);

        // set character
        // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        //                xx xxxxxxxx xxxxxxxx xxxxxxxx
        *p++ = ((ch >> 24) & 0x03) | 0xf8;
        *p++ = ((ch >> 18) & 0x3f) | 0x80;
        *p++ = ((ch >> 12) & 0x3f) | 0x80;
        *p++ = ((ch >> 6) & 0x3f) | 0x80;
        *p++ = (ch & 0x3f) | 0x80;
    }
    // 0x04000000 - 0x7fffffff
    else if (ch <= 0x7fffffff) 
    {
        // not enough? break it
        tb_check_return_val(n > 5, -1);

        // set character
        // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        //                    xxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
        *p++ = ((ch >> 30) & 0x01) | 0xfc;
        *p++ = ((ch >> 24) & 0x3f) | 0x80;
        *p++ = ((ch >> 18) & 0x3f) | 0x80;
        *p++ = ((ch >> 12) & 0x3f) | 0x80;
        *p++ = ((ch >> 6) & 0x3f) | 0x80;
        *p++ = (ch & 0x3f) | 0x80;
    }

    // next
    if (p > q) tb_static_stream_skip(sstream, p - q);

    // ok?
    return p > q? 1 : 0;
}

