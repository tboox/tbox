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
static __tb_inline__ tb_time_t tb_filetime_to_time(FILETIME ft)
{
    ULARGE_INTEGER  ui = {{0}};  
    ui.LowPart      = ft.dwLowDateTime;  
    ui.HighPart     = ft.dwHighDateTime;  
    return (tb_time_t)((LONGLONG)(ui.QuadPart - 116444736000000000ull) / 10000000ul);  
}

// the path full for wchar
static __tb_inline__ tb_wchar_t const* tb_path_absolute_w(tb_char_t const* path, tb_wchar_t* full, tb_size_t maxn)
{
    // the path full
    tb_char_t full_a[TB_PATH_MAXN] = {0};
    if (!tb_path_absolute(path, full_a, TB_PATH_MAXN)) return tb_null;

    // atow
    return tb_atow(full, full_a, maxn) != -1? full : tb_null;
}

#endif
