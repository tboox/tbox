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
 * @file        wcsncpy.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#ifdef TB_CONFIG_LIBC_HAVE_WCSNCPY
#   include <wchar.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces 
 */
#ifdef TB_CONFIG_LIBC_HAVE_WCSNCPY
tb_wchar_t* tb_wcsncpy(tb_wchar_t* s1, tb_wchar_t const* s2, tb_size_t n)
{
    tb_assert_and_check_return_val(s1 && s2, tb_null);
    return wcsncpy(s1, s2, n);
}
#else
tb_wchar_t* tb_wcsncpy(tb_wchar_t* s1, tb_wchar_t const* s2, tb_size_t n)
{
    // check
    tb_assert_and_check_return_val(s1 && s2, s1);

    // no size or same? 
    tb_check_return_val(n && s1 != s2, s1);

    // copy
#if 0
    tb_wchar_t* s = s1;
    while (n) 
    {
        if (*s = *s2) s2++;
        ++s;
        --n;
    }
    return s1;
#else
    tb_size_t sn = tb_wcslen(s2);
    tb_size_t cn = tb_min(sn, n);
    tb_size_t fn = sn < n? n - sn : 0;
    tb_memcpy(s1, s2, cn * sizeof(tb_wchar_t));
    if (fn) tb_memset(s1 + cn, 0, fn * sizeof(tb_wchar_t));
    return s1;
#endif
}
#endif
