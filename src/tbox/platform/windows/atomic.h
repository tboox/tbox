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
 * @file        atomic.h
 *
 */
#ifndef TB_PLATFORM_WINDOWS_ATOMIC_H
#define TB_PLATFORM_WINDOWS_ATOMIC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <windows.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#if defined(MemoryBarrier)
#   define tb_memory_barrier()                          MemoryBarrier()
#elif defined(_AMD64_)
#   define tb_memory_barrier()                          __faststorefence()
#elif defined(_IA64_)
#   define tb_memory_barrier()                          __mf()
#endif

#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8
#   define tb_atomic_flag_test_and_set_explicit(a, mo)  tb_atomic_flag_test_and_set_explicit_windows(a, mo)
#   define tb_atomic_flag_test_and_set(a)               tb_atomic_flag_test_and_set_explicit(a, TB_ATOMIC_SEQ_CST)
#   define tb_atomic_flag_test_explicit(a, mo)          tb_atomic_flag_test_explicit_windows(a, mo)
#   define tb_atomic_flag_test(a)                       tb_atomic_flag_test_explicit(a, TB_ATOMIC_SEQ_CST)
#   define tb_atomic_flag_clear_explicit(a, mo)         tb_atomic_flag_clear_explicit_windows(a, mo)
#   define tb_atomic_flag_clear(a)                      tb_atomic_flag_clear_explicit(a, TB_ATOMIC_SEQ_CST)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * declarations
 */
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8
CHAR _InterlockedExchange8(CHAR __tb_volatile__* Destination, CHAR Exchange);
CHAR _InterlockedOr8(CHAR __tb_volatile__* Destination, CHAR Value);
#   pragma intrinsic(_InterlockedExchange8)
#   pragma intrinsic(_InterlockedOr8)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */
#ifdef TB_CONFIG_WINDOWS_HAVE__INTERLOCKEDEXCHANGE8
static __tb_inline__ tb_bool_t tb_atomic_flag_test_and_set_explicit_windows(tb_atomic_flag_t* a, tb_int_t mo)
{
    tb_assert(a);
    tb_assert_static(sizeof(tb_atomic_flag_t) == sizeof(tb_char_t));
    return (tb_bool_t)_InterlockedExchange8((CHAR __tb_volatile__*)a, 1);
}
static __tb_inline__ tb_bool_t tb_atomic_flag_test_explicit_windows(tb_atomic_flag_t* a, tb_int_t mo)
{
    tb_assert(a);
    tb_assert_static(sizeof(tb_atomic_flag_t) == sizeof(tb_char_t));
    return (tb_bool_t)_InterlockedOr8((CHAR __tb_volatile__*)a, 0);
}
static __tb_inline__ tb_void_t tb_atomic_flag_clear_explicit_windows(tb_atomic_flag_t* a, tb_int_t mo)
{
    tb_assert(a);
    tb_assert_static(sizeof(tb_atomic_flag_t) == sizeof(tb_char_t));
    _InterlockedExchange8((CHAR __tb_volatile__*)a, 0);
}
#endif


#endif
