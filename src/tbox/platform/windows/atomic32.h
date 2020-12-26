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

#define tb_atomic32_fetch_and_set_explicit(a, v, mo)                 tb_atomic32_fetch_and_set_explicit_windows(a, v, mo)
#define tb_atomic32_compare_and_swap_explicit(a, p, v, succ, fail)   tb_atomic32_compare_and_swap_explicit_windows(a, p, v, succ, fail)
#define tb_atomic32_fetch_and_cmpset_explicit(a, p, v, succ, fail)   tb_atomic32_fetch_and_cmpset_explicit_windows(a, p, v, succ, fail)
#define tb_atomic32_fetch_and_add_explicit(a, v, mo)                 tb_atomic32_fetch_and_add_explicit_windows(a, v, mo)

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */

// _InterlockedExchangeXX
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE
LONG _InterlockedExchange(LONG volatile* Destination, LONG Value);
#    pragma intrinsic(_InterlockedExchange)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE_NF
LONG _InterlockedExchange_nf(LONG volatile* Destination, LONG Value);
#    pragma intrinsic(_InterlockedExchange_nf)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE_ACQ
LONG _InterlockedExchange_acq(LONG volatile* Destination, LONG Value);
#    pragma intrinsic(_InterlockedExchange_acq)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE_REL
LONG _InterlockedExchange_rel(LONG volatile* Destination, LONG Value);
#    pragma intrinsic(_InterlockedExchange_rel)
#endif

// _InterlockedCompareExchangeXX
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE
LONG _InterlockedCompareExchange(LONG volatile* Destination, LONG Exchange, LONG Comperand);
#    pragma intrinsic(_InterlockedCompareExchange)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE_nf
LONG _InterlockedCompareExchange_nf(LONG volatile* Destination, LONG Exchange, LONG Comperand);
#    pragma intrinsic(_InterlockedCompareExchange_nf)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE_acq
LONG _InterlockedCompareExchange_acq(LONG volatile* Destination, LONG Exchange, LONG Comperand);
#    pragma intrinsic(_InterlockedCompareExchange_acq)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE_rel
LONG _InterlockedCompareExchange_rel(LONG volatile* Destination, LONG Exchange, LONG Comperand);
#    pragma intrinsic(_InterlockedCompareExchange_rel)
#endif

// _InterlockedExchangeAddXX
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD
LONG _InterlockedExchangeAdd(LONG volatile* Destination, LONG Value);
#    pragma intrinsic(_InterlockedExchangeAdd)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD_NF
LONG _InterlockedExchangeAdd_nf(LONG volatile* Destination, LONG Value);
#    pragma intrinsic(_InterlockedExchangeAdd_nf)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD_ACQ
LONG _InterlockedExchangeAdd_acq(LONG volatile* Destination, LONG Value);
#    pragma intrinsic(_InterlockedExchangeAdd_acq)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD_REL
LONG _InterlockedExchangeAdd_rel(LONG volatile* Destination, LONG Value);
#    pragma intrinsic(_InterlockedExchangeAdd_rel)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_set_explicit_windows(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE_NF)
    if (mo == TB_ATOMIC_RELAXED) return (tb_int32_t)_InterlockedExchange_nf((LONG __tb_volatile__*)a, v);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE_ACQ)
    if (mo == TB_ATOMIC_ACQUIRE) return (tb_int32_t)_InterlockedExchange_acq((LONG __tb_volatile__*)a, v);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE_REL)
    if (mo == TB_ATOMIC_RELEASE) return (tb_int32_t)_InterlockedExchange_rel((LONG __tb_volatile__*)a, v);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE)
    return (tb_int32_t)_InterlockedExchange((LONG __tb_volatile__*)a, v);
#else
    return (tb_int32_t)InterlockedExchange((LONG __tb_volatile__*)a, v);
#endif
}
static __tb_inline__ tb_bool_t tb_atomic32_compare_and_swap_explicit_windows(tb_atomic32_t* a, tb_int32_t* p, tb_int32_t v, tb_int_t succ, tb_int_t fail)
{
    tb_assert(a && p);
    tb_int32_t e = *p;
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE_NF)
    if (succ == TB_ATOMIC_RELAXED && succ == fail)
    {
        *p = (tb_int32_t)_InterlockedCompareExchange_nf((LONG __tb_volatile__*)a, v, e);
        return *p == e;
    }
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE_ACQ)
    if (succ == TB_ATOMIC_ACQUIRE && succ == fail)
    {
        *p = (tb_int32_t)_InterlockedCompareExchange_acq((LONG __tb_volatile__*)a, v, e);
        return *p == e;
    }
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE_REL)
    if (succ == TB_ATOMIC_RELEASE && succ == fail)
    {
        *p = (tb_int32_t)_InterlockedCompareExchange_rel((LONG __tb_volatile__*)a, v, e);
        return *p == e;
    }
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE)
    *p = (tb_int32_t)_InterlockedCompareExchange((LONG __tb_volatile__*)a, v, e);
#else
    *p = (tb_int32_t)InterlockedCompareExchange((LONG __tb_volatile__*)a, v, e);
#endif
    return *p == e;
}
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_cmpset_explicit_windows(tb_atomic32_t* a, tb_int32_t p, tb_int32_t v, tb_int_t succ, tb_int_t fail)
{
    tb_assert(a);
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE_NF)
    if (succ == TB_ATOMIC_RELAXED && succ == fail) return (tb_int32_t)_InterlockedCompareExchange_nf((LONG __tb_volatile__*)a, v, p);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE_ACQ)
    if (succ == TB_ATOMIC_ACQUIRE && succ == fail) return (tb_int32_t)_InterlockedCompareExchange_acq((LONG __tb_volatile__*)a, v, p);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE_REL)
    if (succ == TB_ATOMIC_RELEASE && succ == fail) return (tb_int32_t)_InterlockedCompareExchange_rel((LONG __tb_volatile__*)a, v, p);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDCOMPAREEXCHANGE)
    return (tb_int32_t)_InterlockedCompareExchange((LONG __tb_volatile__*)a, v, p);
#else
    return (tb_int32_t)InterlockedCompareExchange((LONG __tb_volatile__*)a, v, p);
#endif
}
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_add_explicit_windows(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD_NF)
    if (mo == TB_ATOMIC_RELAXED) return (tb_int32_t)_InterlockedExchangeAdd_nf((LONG __tb_volatile__*)a, v);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD_ACQ)
    if (mo == TB_ATOMIC_ACQUIRE) return (tb_int32_t)_InterlockedExchangeAdd_acq((LONG __tb_volatile__*)a, v);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD_REL)
    if (mo == TB_ATOMIC_RELEASE) return (tb_int32_t)_InterlockedExchangeAdd_rel((LONG __tb_volatile__*)a, v);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGEADD)
    return (tb_int32_t)_InterlockedExchangeAdd((LONG __tb_volatile__*)a, v);
#else
    return (tb_int32_t)InterlockedExchangeAdd((LONG __tb_volatile__*)a, v);
#endif
}

#endif
