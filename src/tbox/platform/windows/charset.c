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
 * Copyright (C) 2009 - 2019, TBOOX Open Source Group.
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

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_long_t tb_charset_conv_impl_ansi_to_utf8(tb_static_stream_ref_t fst, tb_static_stream_ref_t tst)
{
    // no data? 
    tb_check_return_val(tb_static_stream_left(fst), 0);

    // get input data
    tb_byte_t const* idata = tb_static_stream_pos(fst);
    tb_size_t        isize = tb_static_stream_left(fst);
    tb_assert_and_check_return_val(idata && isize, -1);

    // get output data
    tb_byte_t const* odata = tb_static_stream_pos(tst);
    tb_size_t        osize = tb_static_stream_left(tst);
    tb_assert_and_check_return_val(odata && osize, -1);

    // convert to wchar buffer first
    tb_wchar_t u16data[4096];
    tb_int_t   u16maxn = tb_min((osize >> 1) + 4, tb_arrayn(u16data) - 1);
    tb_int_t   u16size = MultiByteToWideChar(CP_ACP, 0, (tb_char_t const*)idata, isize, u16data, u16maxn);
    tb_assert_static(sizeof(tb_wchar_t) == 2);
    tb_check_return_val(u16size > 0, 0);

    // convert to the given charset
    tb_int_t oreal = WideCharToMultiByte(CP_UTF8, 0, u16data, u16size, (tb_char_t*)odata, (tb_int_t)osize, tb_null, tb_null);
    if (oreal > 0) 
    {
        // get the real converted input size
        tb_int_t ireal = WideCharToMultiByte(CP_ACP, 0, u16data, u16size, tb_null, 0, tb_null, tb_null);
        tb_assert_and_check_return_val(ireal > 0, -1);

        // update stream
        if (!tb_static_stream_skip(fst, ireal)) return -1;
        if (!tb_static_stream_skip(tst, oreal)) return -1;
        return oreal;
    }
    return 0;
}
static tb_long_t tb_charset_conv_impl_ansi_to_utf16le(tb_static_stream_ref_t fst, tb_static_stream_ref_t tst)
{
    // no data? 
    tb_check_return_val(tb_static_stream_left(fst), 0);

    // get input data
    tb_byte_t const* idata = tb_static_stream_pos(fst);
    tb_size_t        isize = tb_static_stream_left(fst);
    tb_assert_and_check_return_val(idata && isize, -1);

    // get output data
    tb_byte_t const* odata = tb_static_stream_pos(tst);
    tb_size_t        osize = tb_static_stream_left(tst);
    tb_assert_and_check_return_val(odata && osize, -1);

    // convert to the given charset
    tb_int_t oreal = MultiByteToWideChar(CP_ACP, 0, (tb_char_t const*)idata, isize, (tb_wchar_t*)odata, osize / sizeof(tb_wchar_t));
    if (oreal > 0) 
    {
        // get the real converted input size
        tb_int_t ireal = WideCharToMultiByte(CP_ACP, 0, (tb_wchar_t const*)odata, oreal, tb_null, 0, tb_null, tb_null);
        tb_assert_and_check_return_val(ireal > 0, -1);

        // update stream
        if (!tb_static_stream_skip(fst, ireal)) return -1;
        if (!tb_static_stream_skip(tst, oreal * sizeof(tb_wchar_t))) return -1;
        return oreal * sizeof(tb_wchar_t);
    }
    return 0;
}
static tb_long_t tb_charset_conv_impl_utf8_to_ansi(tb_static_stream_ref_t fst, tb_static_stream_ref_t tst)
{
    // no data? 
    tb_check_return_val(tb_static_stream_left(fst), 0);

    // get input data
    tb_byte_t const* idata = tb_static_stream_pos(fst);
    tb_size_t        isize = tb_static_stream_left(fst);
    tb_assert_and_check_return_val(idata && isize, -1);

    // get output data
    tb_byte_t const* odata = tb_static_stream_pos(tst);
    tb_size_t        osize = tb_static_stream_left(tst);
    tb_assert_and_check_return_val(odata && osize, -1);

    // convert to wchar buffer first
    tb_wchar_t u16data[4096];
    tb_int_t   u16maxn = tb_min((osize >> 1) + 4, tb_arrayn(u16data) - 1);
    tb_int_t   u16size = MultiByteToWideChar(CP_UTF8, 0, (tb_char_t const*)idata, isize, u16data, u16maxn);
    tb_assert_static(sizeof(tb_wchar_t) == 2);
    tb_check_return_val(u16size > 0, 0);

    // convert to the given charset
    tb_int_t oreal = WideCharToMultiByte(CP_ACP, 0, u16data, u16size, (tb_char_t*)odata, (tb_int_t)osize, tb_null, tb_null);
    if (oreal > 0) 
    {
        // get the real converted input size
        tb_int_t ireal = WideCharToMultiByte(CP_UTF8, 0, u16data, u16size, tb_null, 0, tb_null, tb_null);
        tb_assert_and_check_return_val(ireal > 0, -1);

        // update stream
        if (!tb_static_stream_skip(fst, ireal)) return -1;
        if (!tb_static_stream_skip(tst, oreal)) return -1;
        return oreal;
    }
    return 0;
}
static tb_long_t tb_charset_conv_impl_utf16le_to_ansi(tb_static_stream_ref_t fst, tb_static_stream_ref_t tst)
{
    // no data? 
    tb_check_return_val(tb_static_stream_left(fst), 0);

    // get input data
    tb_byte_t const* idata = tb_static_stream_pos(fst);
    tb_size_t        isize = tb_static_stream_left(fst);
    tb_assert_and_check_return_val(idata && isize, -1);

    // get output data
    tb_byte_t const* odata = tb_static_stream_pos(tst);
    tb_size_t        osize = tb_static_stream_left(tst);
    tb_assert_and_check_return_val(odata && osize, -1);

    // convert to the given charset
    tb_int_t oreal = WideCharToMultiByte(CP_ACP, 0, (tb_wchar_t const*)idata, isize / sizeof(tb_wchar_t), (tb_char_t*)odata, osize, tb_null, tb_null);
    if (oreal > 0) 
    {
        // get the real converted input size
        tb_int_t ireal = MultiByteToWideChar(CP_ACP, 0, (tb_char_t const*)odata, oreal, tb_null, 0);
        tb_assert_and_check_return_val(ireal > 0, -1);

        // update stream
        if (!tb_static_stream_skip(fst, ireal * sizeof(tb_wchar_t))) return -1;
        if (!tb_static_stream_skip(tst, oreal)) return -1;
        return oreal;
    }
    return 0;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_long_t tb_charset_conv_impl(tb_size_t ftype, tb_size_t ttype, tb_static_stream_ref_t fst, tb_static_stream_ref_t tst)
{
    if (ftype == TB_CHARSET_TYPE_ANSI && ttype == TB_CHARSET_TYPE_UTF8)
        return tb_charset_conv_impl_ansi_to_utf8(fst, tst);
    else if (ftype == TB_CHARSET_TYPE_ANSI && ttype == (TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_LE))
        return tb_charset_conv_impl_ansi_to_utf16le(fst, tst);
    else if (ftype == (TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_LE) && ttype == TB_CHARSET_TYPE_ANSI)
        return tb_charset_conv_impl_utf16le_to_ansi(fst, tst);
    else if (ftype == TB_CHARSET_TYPE_UTF8 && ttype == TB_CHARSET_TYPE_ANSI)
        return tb_charset_conv_impl_utf8_to_ansi(fst, tst);
    return -1;
}
