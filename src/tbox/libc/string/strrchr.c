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
 * @file        strrchr.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#ifdef TB_CONFIG_LIBC_HAVE_STRRCHR
#   include <string.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_LIBC_HAVE_STRRCHR
tb_char_t* tb_strrchr(tb_char_t const* s, tb_char_t c)
{
    tb_assert(s);
    return (tb_char_t*)strrchr(s, c);
}
#else
tb_char_t* tb_strrchr(tb_char_t const* s, tb_char_t c)
{
    tb_assert_and_check_return_val(s, tb_null);
    return tb_strnrchr(s, tb_strlen(s), c);
}
#endif
