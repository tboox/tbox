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
        }
        else return tb_null;
    }

    // atow
    return tb_atow(full, path, maxn) != (tb_size_t)-1? full : tb_null;
}

#endif
