/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        charset.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../impl/charset.h"
#include "../../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_size_t tb_charset_conv_impl_char_cp_bytes(tb_byte_t const* idata, tb_size_t isize, tb_uint_t cp)
{
    // check
    tb_assert(idata && isize);

    // @note CharNextExA do not support utf8/utf16
    if (cp == CP_UTF8)
    {
        tb_byte_t ch = idata[0];
        if (!(ch & 0x80))
            return 1;
        else if ((ch & 0xe0) == 0xc0)
            return isize >= 2? 2 : 0;
        else if ((ch & 0xf0) == 0xe0)
            return isize >= 3? 3 : 0;
        else if ((ch & 0xf8) == 0xf0)
            return isize >= 4? 4 : 0;
        else if ((ch & 0xfc) == 0xf8)
            return isize >= 5? 5 : 0;
        else if ((ch & 0xfe) == 0xfc)
            return isize >= 6? 6 : 0;
        else
            return 0;
    }
    else if (cp == 1200 || cp == 1201) // utf16le or utf16be
    {
        // not enough?
        tb_check_return_val(isize > 1, 0);

        // get the first character
        tb_uint32_t c = cp == 1200? tb_bits_get_u16_le(idata) : tb_bits_get_u16_be(idata);
        if (c >= 0xd800 && c <= 0xdbff)
        {
            // not enough?
            tb_check_return_val(isize > 3, 0);

            // the next character
            tb_byte_t const* p = idata + 2;
            tb_uint32_t c2 = cp == 1200? tb_bits_get_u16_le(p) : tb_bits_get_u16_be(p);
            return (c2 >= 0xdc00 && c2 <= 0xdfff)? 4 : 2;
        }
        else return 2;
    }
    else if (cp == 12000 || cp == 12001) // utf32le or utf32be
        return isize >= 4? 4 : 0;

    // copy input buffer for patching '\0'
    tb_byte_t ibuffer[5];
    tb_size_t ineed = tb_min(isize, 4);
    if (ineed == 4)
    {
        ibuffer[0] = idata[0];
        ibuffer[1] = idata[1];
        ibuffer[2] = idata[2];
        ibuffer[3] = idata[3];
    }
    else tb_memcpy(ibuffer, idata, ineed);
    ibuffer[ineed] = 0;

    // get next character position
    tb_byte_t const* next = (tb_byte_t const*)CharNextExA(cp, (tb_char_t const*)ibuffer, 0);
    tb_check_return_val(next && next > ibuffer, 0);
    tb_assert_and_check_return_val(next <= ibuffer + ineed, 0);

    // get the current character bytes
    return next - ibuffer;
}
static tb_size_t tb_charset_conv_impl_char_cp_to_utf8(tb_byte_t const* idata, tb_size_t isize, tb_byte_t* odata, tb_size_t osize, tb_uint_t cp)
{
    // check
    tb_assert(idata && isize && odata && osize);

    // convert to wchar buffer first
    tb_wchar_t u16data[8];
    tb_int_t   u16size = MultiByteToWideChar(cp, 0, (tb_char_t const*)idata, (tb_int_t)isize, u16data, tb_arrayn(u16data));
    tb_assert_static(sizeof(tb_wchar_t) == 2);
    tb_check_return_val(u16size > 0, 0);

    // convert to the given charset
    tb_int_t oreal = WideCharToMultiByte(CP_UTF8, 0, u16data, u16size, (tb_char_t*)odata, (tb_int_t)osize, tb_null, tb_null);
    return oreal > 0? oreal : 0;
}
static tb_size_t tb_charset_conv_impl_char_cp_to_utf16le(tb_byte_t const* idata, tb_size_t isize, tb_byte_t* odata, tb_size_t osize, tb_uint_t cp)
{
    // check
    tb_assert(idata && isize && odata && osize);

    // convert to the given charset
    tb_int_t oreal = MultiByteToWideChar(cp, 0, (tb_char_t const*)idata, (tb_int_t)isize, (tb_wchar_t*)odata, (tb_int_t)osize / sizeof(tb_wchar_t));
    return oreal > 0? oreal * sizeof(tb_wchar_t) : 0;
}
static tb_size_t tb_charset_conv_impl_char_utf8_to_cp(tb_byte_t const* idata, tb_size_t isize, tb_byte_t* odata, tb_size_t osize, tb_uint_t cp)
{
    // check
    tb_assert(idata && isize && odata && osize);

    // convert to wchar buffer first
    tb_wchar_t u16data[8];
    tb_int_t   u16size = MultiByteToWideChar(CP_UTF8, 0, (tb_char_t const*)idata, (tb_int_t)isize, u16data, tb_arrayn(u16data));
    tb_assert_static(sizeof(tb_wchar_t) == 2);
    tb_check_return_val(u16size > 0, 0);

    // convert to the given charset
    tb_int_t oreal = WideCharToMultiByte(cp, 0, u16data, u16size, (tb_char_t*)odata, (tb_int_t)osize, tb_null, tb_null);
    return oreal > 0? oreal : 0;
}
static tb_size_t tb_charset_conv_impl_char_utf16le_to_cp(tb_byte_t const* idata, tb_size_t isize, tb_byte_t* odata, tb_size_t osize, tb_uint_t cp)
{
    // check
    tb_assert(idata && isize && odata && osize);

    // convert to the given charset
    tb_int_t oreal = WideCharToMultiByte(cp, 0, (tb_wchar_t const*)idata, (tb_int_t)isize / sizeof(tb_wchar_t), (tb_char_t*)odata, (tb_int_t)osize, tb_null, tb_null);
    return oreal > 0? oreal : 0;
}
static tb_long_t tb_charset_conv_impl_cp_to_utf8(tb_static_stream_ref_t fst, tb_static_stream_ref_t tst, tb_uint_t cp)
{
    // get input data
    tb_byte_t const* idata = tb_static_stream_pos(fst);
    tb_size_t        isize = tb_static_stream_left(fst);
    tb_check_return_val(isize, 0);
    tb_assert_and_check_return_val(idata, -1);

    // get output data
    tb_byte_t*       odata = (tb_byte_t*)tb_static_stream_pos(tst);
    tb_size_t        osize = tb_static_stream_left(tst);
    tb_assert_and_check_return_val(odata && osize, -1);

    // convert stream by characters
    tb_size_t ireal = 0;
    tb_size_t oreal = 0;
    while (isize && osize)
    {
        // get the current character bytes
        ireal = tb_charset_conv_impl_char_cp_bytes(idata, isize, cp);
        tb_check_break(ireal);

        // convert the current character
        oreal = tb_charset_conv_impl_char_cp_to_utf8(idata, ireal, odata, osize, cp);
        tb_check_break(oreal);

        // next
        idata += ireal;
        isize -= ireal;
        odata += oreal;
        osize -= oreal;
    }

    // update stream
    ireal = tb_static_stream_left(fst) - isize;
    oreal = tb_static_stream_left(tst) - osize;
    if (!tb_static_stream_skip(fst, ireal)) return -1;
    if (!tb_static_stream_skip(tst, oreal)) return -1;
    return oreal;
}
static tb_long_t tb_charset_conv_impl_cp_to_utf16le(tb_static_stream_ref_t fst, tb_static_stream_ref_t tst, tb_uint_t cp)
{
    // get input data
    tb_byte_t const* idata = tb_static_stream_pos(fst);
    tb_size_t        isize = tb_static_stream_left(fst);
    tb_check_return_val(isize, 0);
    tb_assert_and_check_return_val(idata, -1);

    // get output data
    tb_byte_t*       odata = (tb_byte_t*)tb_static_stream_pos(tst);
    tb_size_t        osize = tb_static_stream_left(tst);
    tb_assert_and_check_return_val(odata && osize, -1);

    // convert stream by characters
    tb_size_t ireal = 0;
    tb_size_t oreal = 0;
    while (isize && osize)
    {
        // get the current character bytes
        ireal = tb_charset_conv_impl_char_cp_bytes(idata, isize, cp);
        tb_check_break(ireal);

        // convert the current character
        oreal = tb_charset_conv_impl_char_cp_to_utf16le(idata, ireal, odata, osize, cp);
        tb_check_break(oreal);

        // next
        idata += ireal;
        isize -= ireal;
        odata += oreal;
        osize -= oreal;
    }

    // update stream
    ireal = tb_static_stream_left(fst) - isize;
    oreal = tb_static_stream_left(tst) - osize;
    if (!tb_static_stream_skip(fst, ireal)) return -1;
    if (!tb_static_stream_skip(tst, oreal)) return -1;
    return oreal;
}
static tb_long_t tb_charset_conv_impl_utf8_to_cp(tb_static_stream_ref_t fst, tb_static_stream_ref_t tst, tb_uint_t cp)
{
    // get input data
    tb_byte_t const* idata = tb_static_stream_pos(fst);
    tb_size_t        isize = tb_static_stream_left(fst);
    tb_check_return_val(isize, 0);
    tb_assert_and_check_return_val(idata, -1);

    // get output data
    tb_byte_t*       odata = (tb_byte_t*)tb_static_stream_pos(tst);
    tb_size_t        osize = tb_static_stream_left(tst);
    tb_assert_and_check_return_val(odata && osize, -1);

    // convert stream by characters
    tb_size_t ireal = 0;
    tb_size_t oreal = 0;
    while (isize && osize)
    {
        // get the current character bytes
        ireal = tb_charset_conv_impl_char_cp_bytes(idata, isize, CP_UTF8);
        tb_check_break(ireal);

        // convert the current character
        oreal = tb_charset_conv_impl_char_utf8_to_cp(idata, ireal, odata, osize, cp);
        tb_check_break(oreal);

        // next
        idata += ireal;
        isize -= ireal;
        odata += oreal;
        osize -= oreal;
    }

    // update stream
    ireal = tb_static_stream_left(fst) - isize;
    oreal = tb_static_stream_left(tst) - osize;
    if (!tb_static_stream_skip(fst, ireal)) return -1;
    if (!tb_static_stream_skip(tst, oreal)) return -1;
    return oreal;
}
static tb_long_t tb_charset_conv_impl_utf16le_to_cp(tb_static_stream_ref_t fst, tb_static_stream_ref_t tst, tb_uint_t cp)
{
    // get input data
    tb_byte_t const* idata = tb_static_stream_pos(fst);
    tb_size_t        isize = tb_static_stream_left(fst);
    tb_check_return_val(isize, 0);
    tb_assert_and_check_return_val(idata, -1);

    // get output data
    tb_byte_t*       odata = (tb_byte_t*)tb_static_stream_pos(tst);
    tb_size_t        osize = tb_static_stream_left(tst);
    tb_assert_and_check_return_val(odata && osize, -1);

    // convert stream by characters
    tb_size_t ireal = 0;
    tb_size_t oreal = 0;
    while (isize && osize)
    {
        // get the current character bytes
        ireal = tb_charset_conv_impl_char_cp_bytes(idata, isize, 1200); // utf16le
        tb_check_break(ireal);

        // convert the current character
        oreal = tb_charset_conv_impl_char_utf16le_to_cp(idata, ireal, odata, osize, cp);
        tb_check_break(oreal);

        // next
        idata += ireal;
        isize -= ireal;
        odata += oreal;
        osize -= oreal;
    }

    // update stream
    ireal = tb_static_stream_left(fst) - isize;
    oreal = tb_static_stream_left(tst) - osize;
    if (!tb_static_stream_skip(fst, ireal)) return -1;
    if (!tb_static_stream_skip(tst, oreal)) return -1;
    return oreal;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_long_t tb_charset_conv_impl(tb_size_t ftype, tb_size_t ttype, tb_static_stream_ref_t fst, tb_static_stream_ref_t tst)
{
    if (ttype == TB_CHARSET_TYPE_UTF8)
    {
        switch (ftype)
        {
        case TB_CHARSET_TYPE_ANSI: return tb_charset_conv_impl_cp_to_utf8(fst, tst, GetACP());
        default: break;
        }
    }
    else if (ftype == TB_CHARSET_TYPE_UTF8)
    {
        switch (ttype)
        {
        case TB_CHARSET_TYPE_ANSI: return tb_charset_conv_impl_utf8_to_cp(fst, tst, GetACP());
        case TB_CHARSET_TYPE_COCP: return tb_charset_conv_impl_utf8_to_cp(fst, tst, GetConsoleOutputCP());
        default: break;
        }
    }
    else if (ttype == (TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_LE))
    {
        switch (ftype)
        {
        case TB_CHARSET_TYPE_ANSI: return tb_charset_conv_impl_cp_to_utf16le(fst, tst, GetACP());
        default: break;
        }
    }
    else if (ftype == (TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_LE))
    {
        switch (ttype)
        {
        case TB_CHARSET_TYPE_ANSI: return tb_charset_conv_impl_utf16le_to_cp(fst, tst, GetACP());
        case TB_CHARSET_TYPE_COCP: return tb_charset_conv_impl_utf16le_to_cp(fst, tst, GetConsoleOutputCP());
        default: break;
        }
    }
    return -1;
}
