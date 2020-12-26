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
#ifndef TB_PLATFORM_COMPILER_GCC_ATOMIC_H
#define TB_PLATFORM_COMPILER_GCC_ATOMIC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef __ATOMIC_SEQ_CST

#   define TB_ATOMIC_RELAXED                            __ATOMIC_RELAXED
#   define TB_ATOMIC_CONSUME                            __ATOMIC_CONSUME
#   define TB_ATOMIC_ACQUIRE                            __ATOMIC_ACQUIRE
#   define TB_ATOMIC_RELEASE                            __ATOMIC_RELEASE
#   define TB_ATOMIC_ACQ_REL                            __ATOMIC_ACQ_REL
#   define TB_ATOMIC_SEQ_CST                            __ATOMIC_SEQ_CST

#   define tb_memory_barrier()                          __atomic_thread_fence(__ATOMIC_SEQ_CST)

#   define tb_atomic_flag_test_and_set_explicit(a, mo)  __atomic_test_and_set(a, mo)
#   define tb_atomic_flag_test_and_set(a)               __atomic_test_and_set(a, __ATOMIC_SEQ_CST)
#   define tb_atomic_flag_test_explicit(a, mo)          tb_atomic_flag_test_explicit_gcc(a, mo)
#   define tb_atomic_flag_test(a)                       tb_atomic_flag_test_explicit(a, __ATOMIC_SEQ_CST)
#   define tb_atomic_flag_clear_explicit(a, mo)         __atomic_clear(a, mo)
#   define tb_atomic_flag_clear(a)                      __atomic_clear(a, __ATOMIC_SEQ_CST)
#else
#   define tb_memory_barrier()                          __sync_synchronize()
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */
#ifdef __ATOMIC_SEQ_CST
static __tb_inline__ tb_bool_t tb_atomic_flag_test_explicit_gcc(tb_atomic_flag_t* a, tb_int_t mo)
{
    tb_assert(a);
    tb_assert_static(sizeof(tb_atomic_flag_t) == sizeof(unsigned char));
    unsigned char t;
    __atomic_load((__tb_volatile__ unsigned char*)a, &t, mo);
    return (tb_bool_t)t;
}
#endif

#endif
