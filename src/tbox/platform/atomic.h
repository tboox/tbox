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
#elif defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/atomic.h"
#elif defined(TB_COMPILER_IS_GCC) \
        && defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4) && __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
#   include "compiler/gcc/atomic.h"
#endif
#include "arch/atomic.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

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

/*! initializes the default-constructed atomic object obj with the value desired. 
 *
 * the function is not atomic: concurrent access from another thread, even through an atomic operation, is a data race.
 */
#ifndef tb_atomic_init
#   define tb_atomic_init(a, v)                         do { *(a) = (v); } while (0)
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
#ifndef tb_atomic_compare_and_swap_explicit
#   define tb_atomic_compare_and_swap_explicit(a, p, v, succ, fail) \
                                                        tb_atomic_compare_and_swap_explicit_generic(a, p, v, succ, fail)
#endif
#ifndef tb_atomic_compare_and_swap
#   define tb_atomic_compare_and_swap(a, p, v)          tb_atomic_compare_and_swap_explicit(a, p, v, TB_ATOMIC_SEQ_CST, TB_ATOMIC_SEQ_CST)
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
#ifndef tb_atomic_compare_and_swap_weak_explicit
#   define tb_atomic_compare_and_swap_weak_explicit(a, p, v, succ, fail) \
                                                        tb_atomic_compare_and_swap_explicit(a, p, v, succ, fail)
#endif
#ifndef tb_atomic_compare_and_swap_weak
#   define tb_atomic_compare_and_swap_weak(a, p, v)     tb_atomic_compare_and_swap_weak_explicit(a, p, v, TB_ATOMIC_SEQ_CST, TB_ATOMIC_SEQ_CST)
#endif

/// fetch the atomic value and compare and set value
#ifndef tb_atomic_fetch_and_cmpset_explicit
#   define tb_atomic_fetch_and_cmpset_explicit(a, p, v, succ, fail) \
                                                        tb_atomic_fetch_and_cmpset_explicit_generic(a, p, v, succ, fail)
#endif
#ifndef tb_atomic_fetch_and_cmpset
#   define tb_atomic_fetch_and_cmpset(a, p, v)          tb_atomic_fetch_and_cmpset_explicit(a, p, v, TB_ATOMIC_SEQ_CST, TB_ATOMIC_SEQ_CST)
#endif

/// fetch the atomic value and set value
#ifndef tb_atomic_fetch_and_set_explicit
#   define tb_atomic_fetch_and_set_explicit(a, v, mo)   tb_atomic_fetch_and_set_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic_fetch_and_set
#   define tb_atomic_fetch_and_set(a, v)                tb_atomic_fetch_and_set_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

/// fetch the atomic value and compute add value
#ifndef tb_atomic_fetch_and_add_explicit
#   define tb_atomic_fetch_and_add_explicit(a, v, mo)   tb_atomic_fetch_and_add_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic_fetch_and_add
#   define tb_atomic_fetch_and_add(a, v)                tb_atomic_fetch_and_add_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

/// fetch the atomic value and compute sub value
#ifndef tb_atomic_fetch_and_sub_explicit
#   define tb_atomic_fetch_and_sub_explicit(a, v, mo)   tb_atomic_fetch_and_add_explicit(a, -(v), mo)
#endif
#ifndef tb_atomic_fetch_and_sub
#   define tb_atomic_fetch_and_sub(a, v)                tb_atomic_fetch_and_sub_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

/// fetch the atomic value and compute or value
#ifndef tb_atomic_fetch_and_or_explicit
#   define tb_atomic_fetch_and_or_explicit(a, v, mo)    tb_atomic_fetch_and_or_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic_fetch_and_or
#   define tb_atomic_fetch_and_or(a, v)                 tb_atomic_fetch_and_or_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

/// fetch the atomic value and compute xor operation
#ifndef tb_atomic_fetch_and_xor_explicit
#   define tb_atomic_fetch_and_xor_explicit(a, v, mo)   tb_atomic_fetch_and_xor_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic_fetch_and_xor
#   define tb_atomic_fetch_and_xor(a, v)                tb_atomic_fetch_and_xor_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

/// fetch the atomic value and compute and operation
#ifndef tb_atomic_fetch_and_and_explicit
#   define tb_atomic_fetch_and_and_explicit(a, v, mo)   tb_atomic_fetch_and_and_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic_fetch_and_and
#   define tb_atomic_fetch_and_and(a, v)                tb_atomic_fetch_and_and_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

/// get the atomic value
#ifndef tb_atomic_get_explicit
#   define tb_atomic_get_explicit(a, mo)                tb_atomic_fetch_and_cmpset_explicit(a, 0, 0, mo, mo)
#endif
#ifndef tb_atomic_get
#   define tb_atomic_get(a)                             tb_atomic_get_explicit(a, TB_ATOMIC_SEQ_CST)
#endif

/// set the atomic value
#ifndef tb_atomic_set_explicit
#   define tb_atomic_set_explicit(a, v, mo)             tb_atomic_fetch_and_set_explicit(a, v, mo)
#endif
#ifndef tb_atomic_set
#   define tb_atomic_set(a, v)                          tb_atomic_set_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifdef TB_CONFIG_API_HAVE_DEPRECATED
#   include "deprecated/atomic.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_bool_t tb_atomic_compare_and_swap_explicit_generic(tb_atomic_t* a, tb_long_t* p, tb_long_t v, tb_size_t succ, tb_size_t fail)
{
    // FIXME
    // no safe

    tb_atomic_t o = *a;
    if (o == *p) 
    {
        *a = v;
        return tb_true;
    }
    else
    {
        *p = o;
        return tb_false;
    }
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_cmpset_explicit_generic(tb_atomic_t* a, tb_long_t p, tb_long_t v, tb_size_t succ, tb_size_t fail)
{
    tb_atomic_compare_and_swap_explicit(a, &p, v, succ, fail);
    return p;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_set_explicit_generic(tb_atomic_t* a, tb_long_t v, tb_size_t mo)
{
    tb_long_t o;
    do { o = *a; } while (!tb_atomic_compare_and_swap_weak_explicit(a, &o, v, mo, mo));
    return o;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_add_explicit_generic(tb_atomic_t* a, tb_long_t v, tb_size_t mo)
{
    tb_long_t o; do { o = *a; } while (!tb_atomic_compare_and_swap_weak_explicit(a, &o, o + v, mo, mo));
    return o;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_xor_explicit_generic(tb_atomic_t* a, tb_long_t v, tb_size_t mo)
{
    tb_long_t o; do { o = *a; } while (!tb_atomic_compare_and_swap_weak_explicit(a, &o, o ^ v, mo, mo));
    return o;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_and_explicit_generic(tb_atomic_t* a, tb_long_t v, tb_size_t mo)
{
    tb_long_t o; do { o = *a; } while (!tb_atomic_compare_and_swap_weak_explicit(a, &o, o & v, mo, mo));
    return o;
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_or_explicit_generic(tb_atomic_t* a, tb_long_t v, tb_size_t mo)
{
    tb_long_t o; do { o = *a; } while (!tb_atomic_compare_and_swap_weak_explicit(a, &o, o | v, mo, mo));
    return o;
}


#endif
