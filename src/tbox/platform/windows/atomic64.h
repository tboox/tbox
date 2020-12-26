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
 * @file        atomic64.h
 *
 */
#ifndef TB_PLATFORM_WINDOWS_ATOMIC64_H
#define TB_PLATFORM_WINDOWS_ATOMIC64_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define tb_atomic64_compare_and_swap_explicit(a, p, v, succ, fail)          tb_atomic64_compare_and_swap_explicit_windows(a, p, v, succ, fail)
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD64
#   define tb_atomic64_fetch_and_add_explicit(a, v, mo)                     tb_atomic64_fetch_and_add_explicit_windows(a, v, mo)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */

// _InterlockedCompareExchange64XX
tb_bool_t tb_atomic64_compare_and_swap_explicit_generic(tb_atomic64_t* a, tb_int64_t* p, tb_int64_t v, tb_int_t succ, tb_int_t fail);
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE64
__int64 _InterlockedCompareExchange64(__int64 __tb_volatile__* Destination, __int64 Exchange, __int64 Comperand);
#    pragma intrinsic(_InterlockedCompareExchange64)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE64_NF
__int64 _InterlockedCompareExchange64_nf(__int64 __tb_volatile__* Destination, __int64 Exchange, __int64 Comperand);
#    pragma intrinsic(_InterlockedCompareExchange64_nf)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE64_ACQ
__int64 _InterlockedCompareExchange64_acq(__int64 __tb_volatile__* Destination, __int64 Exchange, __int64 Comperand);
#    pragma intrinsic(_InterlockedCompareExchange64_acq)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE64_REL
__int64 _InterlockedCompareExchange64_rel(__int64 __tb_volatile__* Destination, __int64 Exchange, __int64 Comperand);
#    pragma intrinsic(_InterlockedCompareExchange64_rel)
#endif

// _InterlockedExchangeAdd64XX
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD64
__int64 _InterlockedExchangeAdd64(__int64 volatile* Destination, __int64 Value);
#    pragma intrinsic(_InterlockedExchangeAdd64)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD64_NF
__int64 _InterlockedExchangeAdd64_nf(__int64 volatile* Destination, __int64 Value);
#    pragma intrinsic(_InterlockedExchangeAdd64_nf)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD64_ACQ
__int64 _InterlockedExchangeAdd64_acq(__int64 volatile* Destination, __int64 Value);
#    pragma intrinsic(_InterlockedExchangeAdd64_acq)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD64_REL
__int64 _InterlockedExchangeAdd64_rel(__int64 volatile* Destination, __int64 Value);
#    pragma intrinsic(_InterlockedExchangeAdd64_rel)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */

static __tb_inline__ tb_bool_t tb_atomic64_compare_and_swap_explicit_windows(tb_atomic64_t* a, tb_int64_t* p, tb_int64_t v, tb_int_t succ, tb_int_t fail)
{
    // check
    tb_assert(a && p);

#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE64_NF
    if (succ == TB_ATOMIC_RELAXED && succ == fail)
    {
        tb_int64_t e = *p;
        *p = (tb_int64_t)_InterlockedCompareExchange64_nf((__int64 __tb_volatile__*)a, v, e);
        return *p == e;
    }
#endif

#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE64_ACQ
    if (succ == TB_ATOMIC_ACQUIRE && succ == fail)
    {
        tb_int64_t e = *p;
        *p = (tb_int64_t)_InterlockedCompareExchange64_acq((__int64 __tb_volatile__*)a, v, e);
        return *p == e;
    }
#endif

#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE64_REL
    if (succ == TB_ATOMIC_RELEASE && succ == fail)
    {
        tb_int64_t e = *p;
        *p = (tb_int64_t)_InterlockedCompareExchange64_rel((__int64 __tb_volatile__*)a, v, e);
        return *p == e;
    }
#endif

    // try to use _InterlockedCompareExchange64 if exists
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE64
    {
        tb_int64_t e = *p;
        *p = (tb_int64_t)_InterlockedCompareExchange64((__int64 __tb_volatile__*)a, v, e);
        return *p == e;
    }
#endif

    // using the generic implementation
    return tb_atomic64_compare_and_swap_explicit_generic(a, p, v, succ, fail);
}
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD64
static __tb_inline__ tb_int64_t tb_atomic64_fetch_and_add_explicit_windows(tb_atomic64_t* a, tb_int64_t v, tb_int_t mo)
{
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD64_NF)
    if (mo == TB_ATOMIC_RELAXED) return (tb_int64_t)_InterlockedExchangeAdd64_nf((__int64 __tb_volatile__*)a, v);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD64_ACQ)
    if (mo == TB_ATOMIC_ACQUIRE) return (tb_int64_t)_InterlockedExchangeAdd64_acq((__int64 __tb_volatile__*)a, v);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD64_REL)
    if (mo == TB_ATOMIC_RELEASE) return (tb_int64_t)_InterlockedExchangeAdd64_rel((__int64 __tb_volatile__*)a, v);
#endif
    return (tb_int64_t)_InterlockedExchangeAdd64((__int64 __tb_volatile__*)a, v);
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__


#endif
