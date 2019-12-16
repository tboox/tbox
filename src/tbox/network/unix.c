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
 * @file        ipv4.c 
 * @ingroup     network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "ipv4"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "unix.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../string/string.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_void_t tb_unixaddr_clear(tb_unixaddr_ref_t unix)
{
    // check
    tb_assert_and_check_return(unix);

    // clear it
    tb_memset(unix->str, 0, sizeof(unix->str));
}
tb_bool_t tb_unixaddr_is_equal(tb_unixaddr_ref_t unix, tb_unixaddr_ref_t other)
{
    // check
    tb_assert_and_check_return_val(unix && other, tb_false);

    // is equal?
    return tb_strcmp(unix->str, other->str) == 0;
}
tb_char_t const* tb_unixaddr_cstr(tb_unixaddr_ref_t unix, tb_char_t* data, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(unix && data && maxn >=TB_UNIXADDR_CSTR_MAXN, tb_null);

    // make it
    tb_long_t size = tb_snprintf(data, maxn - 1, "%s", unix->str);
    if (size >= 0) data[size] = '\0';

    // ok
    return data;
}
tb_bool_t tb_unixaddr_cstr_set(tb_unixaddr_ref_t unix, tb_char_t const* cstr)
{
    // check
    tb_assert_and_check_return_val(cstr, tb_false);
    tb_size_t len = tb_strlen(cstr);
    tb_assert_and_check_return_val(len <TB_UNIXADDR_CSTR_MAXN, tb_false);

    // copy
    tb_memcpy(unix->str, cstr, len + 1);

    // ok
    return tb_true;
}