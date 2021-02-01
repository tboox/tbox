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
 * @author      CodeHz
 * @file        unixaddr.c
 * @ingroup     network
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "unixaddr"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "unixaddr.h"
#include "../libc/libc.h"
#include "../math/math.h"
#include "../utils/utils.h"
#include "../string/string.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_void_t tb_unixaddr_clear(tb_unixaddr_ref_t unixaddr)
{
    // check
    tb_assert_and_check_return(unixaddr);

    // clear it
    tb_memset(unixaddr->path, 0, sizeof(unixaddr->path));
}
tb_bool_t tb_unixaddr_is_abstract(tb_unixaddr_ref_t unixaddr)
{
    // check
    tb_assert_and_check_return_val(unixaddr, tb_false);

    // is abstract?
    return unixaddr->is_abstract;
}
tb_bool_t tb_unixaddr_is_equal(tb_unixaddr_ref_t unixaddr, tb_unixaddr_ref_t other)
{
    // check
    tb_assert_and_check_return_val(unixaddr && other, tb_false);

    // is equal?
    return tb_strcmp(unixaddr->path, other->path) == 0;
}
tb_char_t const* tb_unixaddr_cstr(tb_unixaddr_ref_t unixaddr, tb_char_t* data, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(unixaddr && data && maxn >= TB_UNIXADDR_CSTR_MAXN, tb_null);

    // make it
    tb_long_t size = tb_snprintf(data, maxn - 1, "%s", unixaddr->path);
    if (size >= 0) data[size] = '\0';

    // ok
    return data;
}
tb_bool_t tb_unixaddr_cstr_set(tb_unixaddr_ref_t unixaddr, tb_char_t const* cstr, tb_bool_t is_abstract)
{
    // check
    tb_assert_and_check_return_val(cstr, tb_false);

    // set is_abstract
    unixaddr->is_abstract = is_abstract;

    // copy and report
    return tb_strlcpy(unixaddr->path, cstr, TB_UNIXADDR_CSTR_MAXN) < TB_UNIXADDR_CSTR_MAXN;
}