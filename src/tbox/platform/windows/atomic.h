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
 * @file        atomic.h
 *
 */
#ifndef TB_PLATFORM_WINDOWS_ATOMIC_H
#define TB_PLATFORM_WINDOWS_ATOMIC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// no barriers or synchronization.
#define TB_ATOMIC_RELAXED        (1)

// data dependency only for both barrier and synchronization with another thread.
#define TB_ATOMIC_CONSUME        (2)

// barrier to hoisting of code and synchronizes with release (or stronger) semantic stores from another thread.
#define TB_ATOMIC_ACQUIRE        (3)

// barrier to sinking of code and synchronizes with acquire (or stronger) semantic loads from another thread.
#define TB_ATOMIC_RELEASE        (4)

// full barrier in both directions and synchronizes with acquire loads and release stores in another thread.
#define TB_ATOMIC_ACQ_REL        (5)

/// full barrier in both directions and synchronizes with acquire loads and release stores in all threads.
#define TB_ATOMIC_SEQ_CST        (6)

// memory barrier
#if defined(MemoryBarrier)
#   define tb_memory_barrier()                          MemoryBarrier()
#elif defined(_AMD64_)
#   define tb_memory_barrier()                          __faststorefence()
#elif defined(_IA64_)
#   define tb_memory_barrier()                          __mf()
#endif

#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8) && defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDOR8)
#   define tb_atomic_flag_test_and_set_explicit(a, mo)  tb_atomic_flag_test_and_set_explicit_windows(a, mo)
#   define tb_atomic_flag_test_and_set(a)               tb_atomic_flag_test_and_set_explicit(a, TB_ATOMIC_SEQ_CST)
#   define tb_atomic_flag_test_explicit(a, mo)          tb_atomic_flag_test_explicit_windows(a, mo)
#   define tb_atomic_flag_test(a)                       tb_atomic_flag_test_explicit(a, TB_ATOMIC_SEQ_CST)
#   define tb_atomic_flag_clear_explicit(a, mo)         tb_atomic_flag_clear_explicit_windows(a, mo)
#   define tb_atomic_flag_clear(a)                      tb_atomic_flag_clear_explicit(a, TB_ATOMIC_SEQ_CST)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * declarations
 */

// _InterlockedExchange8XX
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8
CHAR _InterlockedExchange8(CHAR __tb_volatile__* Destination, CHAR Exchange);
#    pragma intrinsic(_InterlockedExchange8)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8_NF
CHAR _InterlockedExchange8_nf(CHAR __tb_volatile__* Destination, CHAR Exchange);
#    pragma intrinsic(_InterlockedExchange8_nf)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8_ACQ
CHAR _InterlockedExchange8_acq(CHAR __tb_volatile__* Destination, CHAR Exchange);
#    pragma intrinsic(_InterlockedExchange8_acq)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8_REL
CHAR _InterlockedExchange8_rel(CHAR __tb_volatile__* Destination, CHAR Exchange);
#    pragma intrinsic(_InterlockedExchange8_rel)
#endif

// _InterlockedOr8XX
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDOR8
CHAR _InterlockedOr8(CHAR __tb_volatile__* Destination, CHAR Value);
#    pragma intrinsic(_InterlockedOr8)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDOR8_NF
CHAR _InterlockedOr8_nf(CHAR __tb_volatile__* Destination, CHAR Value);
#    pragma intrinsic(_InterlockedOr8_nf)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDOR8_ACQ
CHAR _InterlockedOr8_acq(CHAR __tb_volatile__* Destination, CHAR Value);
#    pragma intrinsic(_InterlockedOr8_acq)
#endif
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDOR8_REL
CHAR _InterlockedOr8_rel(CHAR __tb_volatile__* Destination, CHAR Value);
#    pragma intrinsic(_InterlockedOr8_rel)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8) && defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDOR8)
static __tb_inline__ tb_bool_t tb_atomic_flag_test_and_set_explicit_windows(tb_atomic_flag_t* a, tb_int_t mo)
{
    tb_assert(a);
    tb_assert_static(sizeof(tb_atomic_flag_t) == sizeof(tb_char_t));
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8_NF)
    if (mo == TB_ATOMIC_RELAXED) return (tb_bool_t)_InterlockedExchange8_nf((CHAR __tb_volatile__*)a, 1);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8_ACQ)
    if (mo == TB_ATOMIC_ACQUIRE) return (tb_bool_t)_InterlockedExchange8_acq((CHAR __tb_volatile__*)a, 1);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8_REL)
    if (mo == TB_ATOMIC_RELEASE) return (tb_bool_t)_InterlockedExchange8_rel((CHAR __tb_volatile__*)a, 1);
#endif
    return (tb_bool_t)_InterlockedExchange8((CHAR __tb_volatile__*)a, 1);
}
static __tb_inline__ tb_void_t tb_atomic_flag_clear_explicit_windows(tb_atomic_flag_t* a, tb_int_t mo)
{
    tb_assert(a);
    tb_assert_static(sizeof(tb_atomic_flag_t) == sizeof(tb_char_t));
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8_NF)
    if (mo == TB_ATOMIC_RELAXED)
    {
        _InterlockedExchange8_nf((CHAR __tb_volatile__*)a, 0);
        return ;
    }
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8_ACQ)
    if (mo == TB_ATOMIC_ACQUIRE)
    {
        _InterlockedExchange8_acq((CHAR __tb_volatile__*)a, 0);
        return ;
    }
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8_REL)
    if (mo == TB_ATOMIC_RELEASE)
    {
        _InterlockedExchange8_rel((CHAR __tb_volatile__*)a, 0);
        return ;
    }
#endif
    _InterlockedExchange8((CHAR __tb_volatile__*)a, 0);
}
static __tb_inline__ tb_bool_t tb_atomic_flag_test_explicit_windows(tb_atomic_flag_t* a, tb_int_t mo)
{
    tb_assert(a);
    tb_assert_static(sizeof(tb_atomic_flag_t) == sizeof(tb_char_t));
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDOR8_NF)
    if (mo == TB_ATOMIC_RELAXED) return (tb_bool_t)_InterlockedOr8_nf((CHAR __tb_volatile__*)a, 0);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDOR8_ACQ)
    if (mo == TB_ATOMIC_ACQUIRE) return (tb_bool_t)_InterlockedOr8_acq((CHAR __tb_volatile__*)a, 0);
#endif
#if defined(TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDOR8_REL)
    if (mo == TB_ATOMIC_RELEASE) return (tb_bool_t)_InterlockedOr8_rel((CHAR __tb_volatile__*)a, 0);
#endif
    return (tb_bool_t)_InterlockedOr8((CHAR __tb_volatile__*)a, 0);
}
#endif


#endif
