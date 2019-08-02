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
 * @file        atomic64.h
 *
 */
#ifndef TB_PLATFORM_WINDOWS_ATOMIC64_H
#define TB_PLATFORM_WINDOWS_ATOMIC64_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "interface/kernel32.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifndef tb_atomic64_compare_and_swap_explicit
#   define tb_atomic64_compare_and_swap_explicit(a, p, v, succ, fail)      tb_atomic64_compare_and_swap_explicit_windows(a, p, v, succ, fail)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
tb_bool_t tb_atomic64_compare_and_swap_explicit_generic(tb_atomic64_t* a, tb_int64_t* p, tb_int64_t v, tb_int_t succ, tb_int_t fail);

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */

static __tb_inline__ tb_bool_t tb_atomic64_compare_and_swap_explicit_windows(tb_atomic64_t* a, tb_int64_t* p, tb_int64_t v, tb_int_t succ, tb_int_t fail)
{
    // check
    tb_assert(a && p);

    // try to use InterlockedCompareExchange64 if exists
    tb_kernel32_InterlockedCompareExchange64_t pInterlockedCompareExchange64 = tb_kernel32()->InterlockedCompareExchange64;
    if (pInterlockedCompareExchange64) 
    {
        tb_int64_t e = *p;
        *p = (tb_int64_t)pInterlockedCompareExchange64((LONGLONG __tb_volatile__*)a, v, e);
        return *p == e;
    }

    // using the generic implementation
    return tb_atomic64_compare_and_swap_explicit_generic(a, p, v, succ, fail);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__


#endif
