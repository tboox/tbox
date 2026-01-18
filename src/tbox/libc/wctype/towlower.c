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
 * @file        towlower.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../libc.h"
#ifdef TB_CONFIG_OS_WINDOWS
#   include <windows.h>
#endif
#ifdef TB_CONFIG_LIBC_HAVE_TOWLOWER
#   include <wctype.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_wchar_t tb_towlower(tb_wchar_t c)
{
#ifdef TB_CONFIG_OS_WINDOWS
    return (tb_wchar_t)(tb_size_t)CharLowerW((LPWSTR)(tb_size_t)c);
#elif defined(TB_CONFIG_LIBC_HAVE_TOWLOWER)
    return (tb_wchar_t)towlower((wint_t)c);
#else
    return tb_tolower(c);
#endif
}
