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
 * @file        atomic32.h
 *
 */
#ifndef TB_PLATFORM_WINDOWS_ATOMIC32_H
#define TB_PLATFORM_WINDOWS_ATOMIC32_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#if !defined(tb_atomic32_fetch_and_set_explicit) && !TB_CPU_BIT64
#   define tb_atomic32_fetch_and_set_explicit(a, v, mo)                 tb_atomic32_fetch_and_set_explicit_windows(a, v, mo)
#endif

#if !defined(tb_atomic32_compare_and_swap_explicit)
#   define tb_atomic32_compare_and_swap_explicit(a, p, v, succ, fail)   tb_atomic32_compare_and_swap_explicit_windows(a, p, v, succ, fail)
#endif

#if !defined(tb_atomic32_fetch_and_cmpset_explicit)
#   define tb_atomic32_fetch_and_cmpset_explicit(a, p, v, succ, fail)   tb_atomic32_fetch_and_cmpset_explicit_windows(a, p, v, succ, fail)
#endif

#if !defined(tb_atomic32_fetch_and_add_explicit) && !TB_CPU_BIT64
#   define tb_atomic32_fetch_and_add_explicit(a, v, mo)                 tb_atomic32_fetch_and_add_explicit_windows(a, v, mo)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_set_explicit_windows(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
    return (tb_int32_t)InterlockedExchange((LONG __tb_volatile__*)a, v);
}
static __tb_inline__ tb_bool_t tb_atomic32_compare_and_swap_explicit_windows(tb_atomic32_t* a, tb_int32_t* p, tb_int32_t v, tb_int_t succ, tb_int_t fail)
{
    tb_assert(a && p);
    tb_int32_t e = *p;
    *p = (tb_int32_t)InterlockedCompareExchange((LONG __tb_volatile__*)a, v, e);
    return *p == e;
}
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_cmpset_explicit_windows(tb_atomic32_t* a, tb_int32_t p, tb_int32_t v, tb_int_t succ, tb_int_t fail)
{
    tb_assert(a);
    return (tb_int32_t)InterlockedCompareExchange((LONG __tb_volatile__*)a, v, p);
}
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_add_explicit_windows(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
    return (tb_int32_t)InterlockedExchangeAdd((LONG __tb_volatile__*)a, v);
}

#endif
