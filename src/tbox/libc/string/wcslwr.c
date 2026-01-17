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
 * @file        wcslwr.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "../misc/wctype.h"
#include "../stdlib/setlocale.h"
#ifdef TB_CONFIG_LIBC_HAVE_WCSLWR
#   include <wchar.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_wchar_t* tb_wcslwr(tb_wchar_t* s)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

    // set local locale
    tb_setlocale();

#ifdef TB_CONFIG_LIBC_HAVE_WCSLWR
    // convert it
    wcslwr(s);
#else
    tb_wchar_t* p = s;
    while (*p)
    {
        *p = tb_towlower(*p);
        p++;
    }
#endif

    // set default locale
    tb_resetlocale();

    // ok
    return s;
}
