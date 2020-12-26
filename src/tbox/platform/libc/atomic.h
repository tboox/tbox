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
#ifndef TB_PLATFORM_COMPILER_LIBC_ATOMIC_H
#define TB_PLATFORM_COMPILER_LIBC_ATOMIC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <stdatomic.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#define TB_ATOMIC_RELAXED                               memory_order_relaxed
#define TB_ATOMIC_CONSUME                               memory_order_consume
#define TB_ATOMIC_ACQUIRE                               memory_order_acquire
#define TB_ATOMIC_RELEASE                               memory_order_release
#define TB_ATOMIC_ACQ_REL                               memory_order_acq_rel
#define TB_ATOMIC_SEQ_CST                               memory_order_seq_cst

#define tb_memory_barrier()                             atomic_thread_fence(memory_order_seq_cst)

#define TB_ATOMIC_FLAG_INIT                             ATOMIC_FLAG_INIT
#define tb_atomic_flag_test_and_set_explicit(a, mo)     atomic_flag_test_and_set_explicit(a, mo)
#define tb_atomic_flag_test_and_set(a)                  atomic_flag_test_and_set(a)
#ifdef atomic_flag_test_explicit
#   define tb_atomic_flag_test_explicit(a, mo)          atomic_flag_test_explicit(a, mo)
#else
#   define tb_atomic_flag_test_explicit(a, mo)          tb_atomic_flag_test_explicit_libc(a, mo)
#endif
#define tb_atomic_flag_test(a)                          tb_atomic_flag_test_explicit(a, memory_order_seq_cst)
#define tb_atomic_flag_test_noatomic(a)                 tb_atomic_flag_test_noatomic_libc(a)
#define tb_atomic_flag_clear_explicit(a, mo)            atomic_flag_clear_explicit(a, mo)
#define tb_atomic_flag_clear(a)                         atomic_flag_clear(a)

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */
static __tb_inline__ tb_bool_t tb_atomic_flag_test_explicit_libc(tb_atomic_flag_t* a, tb_int_t mo)
{
    tb_assert(a);
    tb_assert_static(sizeof(tb_atomic_flag_t) == sizeof(unsigned char));
    return (tb_bool_t)atomic_load_explicit((__tb_volatile__ _Atomic unsigned char*)a, mo);
}
static __tb_inline__ tb_bool_t tb_atomic_flag_test_noatomic_libc(tb_atomic_flag_t* a)
{
    tb_assert(a);
    tb_assert_static(sizeof(tb_atomic_flag_t) == sizeof(unsigned char));
    return (tb_bool_t)*((__tb_volatile__ unsigned char*)a);
}

#endif
