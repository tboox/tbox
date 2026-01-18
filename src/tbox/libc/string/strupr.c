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
 * @file        strupr.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "../misc/ctype.h"
#ifdef TB_CONFIG_LIBC_HAVE_STRUPR
#   include <string.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_char_t* tb_strupr(tb_char_t* s)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

#ifdef TB_CONFIG_LIBC_HAVE_STRUPR
    // convert it
    strupr(s);

    // ok
    return s;
#else
    tb_char_t* p = s;
    while (*p)
    {
        *p = tb_toupper(*p);
        p++;
    }
    return s;
#endif
}
