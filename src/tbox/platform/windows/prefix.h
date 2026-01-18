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
 * @file        prefix.h
 *
 */
#ifndef TB_PLATFORM_WINDOWS_PREFIX_H
#define TB_PLATFORM_WINDOWS_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../path.h"
#include "../../libc/libc.h"
#include "../../utils/utils.h"
#include "../../network/ipaddr.h"
#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

// FILETIME => tb_time_t
static __tb_inline__ tb_time_t tb_filetime_to_time(FILETIME const* pft)
{
    ULARGE_INTEGER  ui = {{0}};
    ui.LowPart      = pft->dwLowDateTime;
    ui.HighPart     = pft->dwHighDateTime;
    return (tb_time_t)((LONGLONG)(ui.QuadPart - 116444736000000000ull) / 10000000ul);
}

// tb_time_t => FILETIME
static __tb_inline__ tb_void_t tb_time_to_filetime(tb_time_t tm, FILETIME* pft)
{
    ULARGE_INTEGER ui = {{0}};
    ui.QuadPart = (tm * 10000000ul) + 116444736000000000ull;
    pft->dwLowDateTime = ui.LowPart;
    pft->dwHighDateTime = ui.HighPart;
}

// convert wide string to multibyte string with known length (avoid wcslen)
static __tb_inline__ tb_size_t tb_wtoa_n(tb_wchar_t const* wstr, tb_size_t wlen, tb_char_t* mstr, tb_size_t maxn)
{
    tb_assert_and_check_return_val(wstr && mstr && maxn > 0, 0);
    if (!wlen)
    {
        mstr[0] = '\0';
        return 0;
    }

    tb_int_t size = WideCharToMultiByte(CP_UTF8, 0, (LPCWCH)wstr, (tb_int_t)wlen, mstr, (tb_int_t)maxn - 1, tb_null, tb_null);
    if (size > 0 && size < (tb_int_t)maxn)
    {
        mstr[size] = '\0';
        return (tb_size_t)size;
    }
    return 0;
}

// convert multibyte string to wide string with known length (avoid strlen)
static __tb_inline__ tb_size_t tb_atow_n(tb_char_t const* mstr, tb_size_t mlen, tb_wchar_t* wstr, tb_size_t maxn)
{
    tb_assert_and_check_return_val(mstr && wstr && maxn > 0, 0);
    if (!mlen)
    {
        wstr[0] = L'\0';
        return 0;
    }

    tb_int_t size = MultiByteToWideChar(CP_UTF8, 0, mstr, (tb_int_t)mlen, (LPWSTR)wstr, (tb_int_t)maxn - 1);
    if (size > 0 && size < (tb_int_t)maxn)
    {
        wstr[size] = L'\0';
        return (tb_size_t)size;
    }
    return 0;
}

// get absolute path for wchar
static __tb_inline__ tb_wchar_t const* tb_path_absolute_w(tb_char_t const* path, tb_wchar_t* full, tb_size_t maxn)
{
    // get absolute path
    tb_char_t data[TB_PATH_MAXN] = {0};
    path = tb_path_absolute(path, data, TB_PATH_MAXN);
    tb_check_return_val(path, tb_null);

    /* we need deal with files with a name longer than 259 characters
     * @see https://stackoverflow.com/questions/5188527/how-to-deal-with-files-with-a-name-longer-than-259-characters
     */
    tb_size_t size = tb_strlen(path);
    if (size >= MAX_PATH)
    {
        tb_char_t* e = data + size - 1;
        if (e + 5 < data + sizeof(data))
        {
            e[5] = '\0';
            while (e >= data)
            {
                e[4] = *e;
                e--;
            }
            data[0] = '\\';
            data[1] = '\\';
            data[2] = '?';
            data[3] = '\\';
            path = data;
            size += 4; // add "\\?\" prefix length
        }
        else return tb_null;
    }

    // use tb_atow_n with known length to avoid tb_atow's internal strlen
    return tb_atow_n(path, size, full, maxn)? full : tb_null;
}

#endif
