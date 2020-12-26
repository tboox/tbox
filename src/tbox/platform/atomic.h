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
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_ATOMIC_H
#define TB_PLATFORM_ATOMIC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#if __tb_has_feature__(c_atomic) && !defined(__STDC_NO_ATOMICS__)
#   include "libc/atomic.h"
#elif defined(TB_COMPILER_IS_GCC) && defined(__ATOMIC_SEQ_CST)
#   include "compiler/gcc/atomic.h"
#elif defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/atomic.h"
#endif
#include "arch/atomic.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the atomic flag initialize value
#ifndef TB_ATOMIC_FLAG_INIT
#   define TB_ATOMIC_FLAG_INIT      {0}
#endif

/// no barriers or synchronization.
#ifndef TB_ATOMIC_RELAXED
#   define TB_ATOMIC_RELAXED        (1)
#endif

/// data dependency only for both barrier and synchronization with another thread.
#ifndef TB_ATOMIC_CONSUME
#   define TB_ATOMIC_CONSUME        (2)
#endif

/// barrier to hoisting of code and synchronizes with release (or stronger) semantic stores from another thread.
#ifndef TB_ATOMIC_ACQUIRE
#   define TB_ATOMIC_ACQUIRE        (3)
#endif

/// barrier to sinking of code and synchronizes with acquire (or stronger) semantic loads from another thread.
#ifndef TB_ATOMIC_RELEASE
#   define TB_ATOMIC_RELEASE        (4)
#endif

/// full barrier in both directions and synchronizes with acquire loads and release stores in another thread.
#ifndef TB_ATOMIC_ACQ_REL
#   define TB_ATOMIC_ACQ_REL        (5)
#endif

/// full barrier in both directions and synchronizes with acquire loads and release stores in all threads.
#ifndef TB_ATOMIC_SEQ_CST
#   define TB_ATOMIC_SEQ_CST        (6)
#endif

/// memory barrier (full barrier)
#ifndef tb_memory_barrier
#   define tb_memory_barrier()
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "atomic32.h"
#include "atomic64.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/*! initializes the default-constructed atomic object obj with the value desired.
 *
 * the function is not atomic: concurrent access from another thread, even through an atomic operation, is a data race.
 */
#if TB_CPU_BIT64
#   define tb_atomic_init(a, v)                            tb_atomic64_init(a, (tb_int64_t)(v))
#else
#   define tb_atomic_init(a, v)                            tb_atomic32_init(a, (tb_int32_t)(v))
#endif

/*! atomically compares the contents of memory pointed to by obj with the contents of memory pointed to by expected,
 * and if those are bitwise equal, replaces the former with desired (performs read-modify-write operation).
 *
 * otherwise, loads the actual contents of memory pointed to by obj into *p (performs load operation).
 *
 * @param a     pointer to the atomic object to test and modify
 * @param p     pointer to the value expected to be found in the atomic object
 * @param v     the value to store in the atomic object if it is as expected
 *
 * @return the result of the comparison: true if *a was equal to *p, false otherwise.
 * - succ	    the memory synchronization ordering for the read-modify-write operation if the comparison succeeds. All values are permitted.
 * - fail	    the memory synchronization ordering for the load operation if the comparison fails. Cannot be memory_order_release or memory_order_acq_rel and cannot specify stronger ordering than succ
 *
 * @code
 *   tb_atomic_init(&a, 1);
 *
 *   tb_long_t expected = 1;
 *   if (tb_atomic_compare_and_swap(&a, &expected, 2)) {
 *      // *a = 2
 *   } else {
 *      // expected = *a
 *   }
 * @endcode
 */
#if TB_CPU_BIT64
#   define tb_atomic_compare_and_swap_explicit(a, p, v, succ, fail) \
                                                        tb_atomic64_compare_and_swap_explicit(a, (tb_int64_t*)(p), (tb_int64_t)(v), succ, fail)
#   define tb_atomic_compare_and_swap(a, p, v)          tb_atomic64_compare_and_swap(a, (tb_int64_t*)(p), (tb_int64_t)(v))
#else
#   define tb_atomic_compare_and_swap_explicit(a, p, v, succ, fail) \
                                                        tb_atomic32_compare_and_swap_explicit(a, (tb_int32_t*)(p), (tb_int32_t)(v), succ, fail)
#   define tb_atomic_compare_and_swap(a, p, v)          tb_atomic32_compare_and_swap(a, (tb_int32_t*)(p), (tb_int32_t)(v))
#endif

/*! like tb_atomic_compare_and_swap(), but it's allowed to fail spuriously, that is, act as if *obj != *p even if they are equal.
 *
 * when a compare-and-swap is in a loop, the weak version will yield better performance on some platforms.
 * when a weak compare-and-swap would require a loop and a strong one would not, the strong one is preferable.
 *
 * @param a     pointer to the atomic object to test and modify
 * @param p     pointer to the value expected to be found in the atomic object
 * @param v     the value to store in the atomic object if it is as expected
 *
 * @return the result of the comparison: true if *a was equal to *p, false otherwise.
 * - succ	    the memory synchronization ordering for the read-modify-write operation if the comparison succeeds. All values are permitted.
 * - fail	    the memory synchronization ordering for the load operation if the comparison fails. Cannot be memory_order_release or memory_order_acq_rel and cannot specify stronger ordering than succ
 */
#if TB_CPU_BIT64
#   define tb_atomic_compare_and_swap_weak_explicit(a, p, v, succ, fail) \
                                                        tb_atomic64_compare_and_swap_weak_explicit(a, (tb_int64_t*)(p), (tb_int64_t)(v), succ, fail)
#   define tb_atomic_compare_and_swap_weak(a, p, v)     tb_atomic64_compare_and_swap_weak(a, (tb_int64_t*)(p), (tb_int64_t)(v))
#else
#   define tb_atomic_compare_and_swap_weak_explicit(a, p, v, succ, fail) \
                                                        tb_atomic32_compare_and_swap_weak_explicit(a, (tb_int32_t*)(p), (tb_int32_t)(v), succ, fail)
#   define tb_atomic_compare_and_swap_weak(a, p, v)     tb_atomic32_compare_and_swap_weak(a, (tb_int32_t*)(p), (tb_int32_t)(v))
#endif

/// fetch the atomic value and compare and set value
#if TB_CPU_BIT64
#   define tb_atomic_fetch_and_cmpset_explicit(a, p, v, succ, fail) \
                                                        (tb_long_t)tb_atomic64_fetch_and_cmpset_explicit(a, (tb_int64_t)(p), (tb_int64_t)(v), succ, fail)
#   define tb_atomic_fetch_and_cmpset(a, p, v)          (tb_long_t)tb_atomic64_fetch_and_cmpset(a, (tb_int64_t)(p), (tb_int64_t)(v))
#else
#   define tb_atomic_fetch_and_cmpset_explicit(a, p, v, succ, fail) \
                                                        (tb_long_t)tb_atomic32_fetch_and_cmpset_explicit(a, (tb_int32_t)(p), (tb_int32_t)(v), succ, fail)
#   define tb_atomic_fetch_and_cmpset(a, p, v)          (tb_long_t)tb_atomic32_fetch_and_cmpset(a, (tb_int32_t)(p), (tb_long_t)(v))
#endif

/// fetch the atomic value and set value
#if TB_CPU_BIT64
#   define tb_atomic_fetch_and_set_explicit(a, v, mo)   (tb_long_t)tb_atomic64_fetch_and_set_explicit(a, (tb_int64_t)(v), mo)
#   define tb_atomic_fetch_and_set(a, v)                (tb_long_t)tb_atomic64_fetch_and_set(a, (tb_int64_t)(v))
#else
#   define tb_atomic_fetch_and_set_explicit(a, v, mo)   (tb_long_t)tb_atomic32_fetch_and_set_explicit(a, (tb_int32_t)(v), mo)
#   define tb_atomic_fetch_and_set(a, v)                (tb_long_t)tb_atomic32_fetch_and_set(a, (tb_int32_t)(v))
#endif

/// fetch the atomic value and compute add value
#if TB_CPU_BIT64
#   define tb_atomic_fetch_and_add_explicit(a, v, mo)   (tb_long_t)tb_atomic64_fetch_and_add_explicit(a, (tb_int64_t)(v), mo)
#   define tb_atomic_fetch_and_add(a, v)                (tb_long_t)tb_atomic64_fetch_and_add(a, (tb_int64_t)(v))
#else
#   define tb_atomic_fetch_and_add_explicit(a, v, mo)   (tb_long_t)tb_atomic32_fetch_and_add_explicit(a, (tb_int32_t)(v), mo)
#   define tb_atomic_fetch_and_add(a, v)                (tb_long_t)tb_atomic32_fetch_and_add(a, (tb_int32_t)(v))
#endif

/// fetch the atomic value and compute sub value
#if TB_CPU_BIT64
#   define tb_atomic_fetch_and_sub_explicit(a, v, mo)   (tb_long_t)tb_atomic64_fetch_and_sub_explicit(a, (tb_int64_t)(v), mo)
#   define tb_atomic_fetch_and_sub(a, v)                (tb_long_t)tb_atomic64_fetch_and_sub(a, (tb_int64_t)(v))
#else
#   define tb_atomic_fetch_and_sub_explicit(a, v, mo)   (tb_long_t)tb_atomic32_fetch_and_sub_explicit(a, (tb_int32_t)(v), mo)
#   define tb_atomic_fetch_and_sub(a, v)                (tb_long_t)tb_atomic32_fetch_and_sub(a, (tb_int32_t)(v))
#endif

/// fetch the atomic value and compute or value
#if TB_CPU_BIT64
#   define tb_atomic_fetch_and_or_explicit(a, v, mo)    (tb_long_t)tb_atomic64_fetch_and_or_explicit(a, (tb_int64_t)(v), mo)
#   define tb_atomic_fetch_and_or(a, v)                 (tb_long_t)tb_atomic64_fetch_and_or(a, (tb_int64_t)(v))
#else
#   define tb_atomic_fetch_and_or_explicit(a, v, mo)    (tb_long_t)tb_atomic32_fetch_and_or_explicit(a, (tb_int32_t)(v), mo)
#   define tb_atomic_fetch_and_or(a, v)                 (tb_long_t)tb_atomic32_fetch_and_or(a, (tb_int32_t)(v))
#endif

/// fetch the atomic value and compute xor operation
#if TB_CPU_BIT64
#   define tb_atomic_fetch_and_xor_explicit(a, v, mo)   (tb_long_t)tb_atomic64_fetch_and_xor_explicit(a, (tb_int64_t)(v), mo)
#   define tb_atomic_fetch_and_xor(a, v)                (tb_long_t)tb_atomic64_fetch_and_xor(a, (tb_int64_t)(v))
#else
#   define tb_atomic_fetch_and_xor_explicit(a, v, mo)   (tb_long_t)tb_atomic32_fetch_and_xor_explicit(a, (tb_int32_t)(v), mo)
#   define tb_atomic_fetch_and_xor(a, v)                (tb_long_t)tb_atomic32_fetch_and_xor(a, (tb_int32_t)(v))
#endif

/// fetch the atomic value and compute and operation
#if TB_CPU_BIT64
#   define tb_atomic_fetch_and_and_explicit(a, v, mo)   (tb_long_t)tb_atomic64_fetch_and_and_explicit(a, (tb_int64_t)(v), mo)
#   define tb_atomic_fetch_and_and(a, v)                (tb_long_t)tb_atomic64_fetch_and_and(a, (tb_int64_t)(v))
#else
#   define tb_atomic_fetch_and_and_explicit(a, v, mo)   (tb_long_t)tb_atomic32_fetch_and_and_explicit(a, (tb_int32_t)(v), mo)
#   define tb_atomic_fetch_and_and(a, v)                (tb_long_t)tb_atomic32_fetch_and_and(a, (tb_int32_t)(v))
#endif

/// get the atomic value
#if TB_CPU_BIT64
#   define tb_atomic_get_explicit(a, mo)                (tb_long_t)tb_atomic64_get_explicit(a, mo)
#   define tb_atomic_get(a)                             (tb_long_t)tb_atomic64_get(a)
#else
#   define tb_atomic_get_explicit(a, mo)                (tb_long_t)tb_atomic32_get_explicit(a, mo)
#   define tb_atomic_get(a)                             (tb_long_t)tb_atomic32_get(a)
#endif

/// set the atomic value
#if TB_CPU_BIT64
#   define tb_atomic_set_explicit(a, v, mo)             tb_atomic64_set_explicit(a, (tb_int64_t)(v), mo)
#   define tb_atomic_set(a, v)                          tb_atomic64_set(a, (tb_int64_t)(v))
#else
#   define tb_atomic_set_explicit(a, v, mo)             tb_atomic32_set_explicit(a, (tb_int32_t)(v), mo)
#   define tb_atomic_set(a, v)                          tb_atomic32_set(a, (tb_int32_t)(v))
#endif

/// sets an atomic_flag to true and returns the old value
#ifndef tb_atomic_flag_test_and_set_explicit
#   define tb_atomic_flag_test_and_set_explicit_generic_impl
#   define tb_atomic_flag_test_and_set_explicit(a, mo)  tb_atomic_flag_test_and_set_explicit_generic(a, mo)
#endif
#ifndef tb_atomic_flag_test_and_set
#   define tb_atomic_flag_test_and_set(a)               tb_atomic_flag_test_and_set_explicit(a, TB_ATOMIC_SEQ_CST)
#endif

/// returns the test result of an atomic_flag
#ifndef tb_atomic_flag_test_explicit
#   define tb_atomic_flag_test_explicit_generic_impl
#   define tb_atomic_flag_test_explicit(a, mo)          tb_atomic_flag_test_explicit_generic(a, mo)
#endif
#ifndef tb_atomic_flag_test
#   define tb_atomic_flag_test(a)                       tb_atomic_flag_test_explicit(a, TB_ATOMIC_SEQ_CST)
#endif

/// sets an atomic_flag to false
#ifndef tb_atomic_flag_clear_explicit
#   define tb_atomic_flag_clear_explicit_generic_impl
#   define tb_atomic_flag_clear_explicit(a, mo)         tb_atomic_flag_clear_explicit_generic(a, mo)
#endif
#ifndef tb_atomic_flag_clear
#   define tb_atomic_flag_clear(a)                      tb_atomic_flag_clear_explicit(a, TB_ATOMIC_SEQ_CST)
#endif

/// get value of an atomic_flag directly (non-atomic)
#ifndef tb_atomic_flag_test_noatomic
#   define tb_atomic_flag_test_noatomic(a)              ((a)->__val)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#ifdef TB_CONFIG_API_HAVE_DEPRECATED
#   include "deprecated/atomic.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */
#ifdef tb_atomic_flag_test_and_set_explicit_generic_impl
static __tb_inline__ tb_bool_t tb_atomic_flag_test_and_set_explicit_generic(tb_atomic_flag_t* a, tb_int_t mo)
{
    tb_assert(a);
    tb_assert_static(sizeof(tb_atomic_flag_t) == sizeof(tb_atomic32_t));
    return (tb_bool_t)tb_atomic32_fetch_and_set_explicit((tb_atomic32_t*)a, 1, mo);
}
#endif

#ifdef tb_atomic_flag_test_explicit_generic_impl
static __tb_inline__ tb_bool_t tb_atomic_flag_test_explicit_generic(tb_atomic_flag_t* a, tb_int_t mo)
{
    tb_assert(a);
    tb_assert_static(sizeof(tb_atomic_flag_t) == sizeof(tb_atomic32_t));
    return (tb_bool_t)tb_atomic32_get_explicit((tb_atomic32_t*)a, mo);
}
#endif

#ifdef tb_atomic_flag_clear_explicit_generic_impl
static __tb_inline__ tb_void_t tb_atomic_flag_clear_explicit_generic(tb_atomic_flag_t* a, tb_int_t mo)
{
    tb_assert(a);
    tb_assert_static(sizeof(tb_atomic_flag_t) == sizeof(tb_atomic32_t));
    tb_atomic32_set_explicit((tb_atomic32_t*)a, 0, mo);
}
#endif

#endif
