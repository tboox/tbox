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
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_IMPL_ATOMIC64_H
#define TB_PLATFORM_IMPL_ATOMIC64_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "atomic32.h"
#if __tb_has_feature__(c_atomic) && !defined(__STDC_NO_ATOMICS__)
#   include "libc/atomic64.h"
#elif defined(TB_COMPILER_IS_GCC) \
    && (defined(__ATOMIC_SEQ_CST) || defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8))
#   include "compiler/gcc/atomic64.h"
#elif defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/atomic64.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifndef tb_atomic64_init
#   define tb_atomic64_init(a, v)                           do { *(a) = (v); } while (0)
#endif

#ifndef tb_atomic64_compare_and_swap_explicit
#   define tb_atomic64_compare_and_swap_explicit(a, p, v, succ, fail) \
                                                            tb_atomic64_compare_and_swap_explicit_generic(a, p, v, succ, fail)
#endif
#ifndef tb_atomic64_compare_and_swap
#   define tb_atomic64_compare_and_swap(a, p, v)            tb_atomic64_compare_and_swap_explicit(a, p, v, TB_ATOMIC_SEQ_CST, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic64_compare_and_swap_weak_explicit
#   define tb_atomic64_compare_and_swap_weak_explicit(a, p, v, succ, fail) \
                                                            tb_atomic64_compare_and_swap_explicit(a, p, v, succ, fail)
#endif
#ifndef tb_atomic64_compare_and_swap_weak
#   define tb_atomic64_compare_and_swap_weak(a, p, v)       tb_atomic64_compare_and_swap_weak_explicit(a, p, v, TB_ATOMIC_SEQ_CST, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic64_fetch_and_cmpset_explicit
#   define tb_atomic64_fetch_and_cmpset_explicit(a, p, v, succ, fail) \
                                                            tb_atomic64_fetch_and_cmpset_explicit_generic(a, p, v, succ, fail)
#endif
#ifndef tb_atomic64_fetch_and_cmpset
#   define tb_atomic64_fetch_and_cmpset(a, p, v)            tb_atomic64_fetch_and_cmpset_explicit(a, p, v, TB_ATOMIC_SEQ_CST, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic64_fetch_and_set_explicit
#   define tb_atomic64_fetch_and_set_explicit(a, v, mo)     tb_atomic64_fetch_and_set_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic64_fetch_and_set
#   define tb_atomic64_fetch_and_set(a, v)                  tb_atomic64_fetch_and_set_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic64_fetch_and_add_explicit
#   define tb_atomic64_fetch_and_add_explicit(a, v, mo)     tb_atomic64_fetch_and_add_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic64_fetch_and_add
#   define tb_atomic64_fetch_and_add(a, v)                  tb_atomic64_fetch_and_add_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic64_fetch_and_sub_explicit
#   define tb_atomic64_fetch_and_sub_explicit(a, v, mo)     tb_atomic64_fetch_and_add_explicit(a, -(v), mo)
#endif
#ifndef tb_atomic64_fetch_and_sub
#   define tb_atomic64_fetch_and_sub(a, v)                  tb_atomic64_fetch_and_sub_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic64_fetch_and_or_explicit
#   define tb_atomic64_fetch_and_or_explicit(a, v, mo)      tb_atomic64_fetch_and_or_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic64_fetch_and_or
#   define tb_atomic64_fetch_and_or(a, v)                   tb_atomic64_fetch_and_or_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic64_fetch_and_xor_explicit
#   define tb_atomic64_fetch_and_xor_explicit(a, v, mo)     tb_atomic64_fetch_and_xor_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic64_fetch_and_xor
#   define tb_atomic64_fetch_and_xor(a, v)                  tb_atomic64_fetch_and_xor_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic64_fetch_and_and_explicit
#   define tb_atomic64_fetch_and_and_explicit(a, v, mo)     tb_atomic64_fetch_and_and_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic64_fetch_and_and
#   define tb_atomic64_fetch_and_and(a, v)                  tb_atomic64_fetch_and_and_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic64_get_explicit
#   define tb_atomic64_get_explicit(a, mo)                  tb_atomic64_fetch_and_cmpset_explicit(a, 0, 0, mo, mo)
#endif
#ifndef tb_atomic64_get
#   define tb_atomic64_get(a)                               tb_atomic64_get_explicit(a, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic64_set_explicit
#   define tb_atomic64_set_explicit(a, v, mo)               tb_atomic64_fetch_and_set_explicit(a, v, mo)
#endif
#ifndef tb_atomic64_set
#   define tb_atomic64_set(a, v)                            tb_atomic64_set_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifdef TB_CONFIG_API_HAVE_DEPRECATED
#   include "deprecated/atomic64.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* compare and set the 64bits value
 *
 * @param a     pointer to the atomic object to test and modify
 * @param p     pointer to the value expected to be found in the atomic object
 * @param v     the value to store in the atomic object if it is as expected
 * @param succ  the memory order if be successful
 * @param fail  the memory order if be failed
 *
 * @return      the result of the comparison: true if *a was equal to *p, false otherwise.
 */
tb_bool_t       tb_atomic64_compare_and_swap_explicit_generic(tb_atomic64_t* a, tb_int64_t* p, tb_int64_t v, tb_int_t succ, tb_int_t fail);


/* //////////////////////////////////////////////////////////////////////////////////////
 * inline interfaces
 */
static __tb_inline__ tb_int64_t tb_atomic64_fetch_and_cmpset_explicit_generic(tb_atomic64_t* a, tb_int64_t p, tb_int64_t v, tb_int_t succ, tb_int_t fail)
{
    tb_atomic64_compare_and_swap_explicit(a, &p, v, succ, fail);
    return p;
}
static __tb_inline__ tb_int64_t tb_atomic64_fetch_and_set_explicit_generic(tb_atomic64_t* a, tb_int64_t v, tb_int_t mo)
{
    tb_int64_t o;
    do { o = *a; } while (!tb_atomic64_compare_and_swap_weak_explicit(a, &o, v, mo, mo));
    return o;
}
static __tb_inline__ tb_int64_t tb_atomic64_fetch_and_add_explicit_generic(tb_atomic64_t* a, tb_int64_t v, tb_int_t mo)
{
    tb_int64_t o; do { o = *a; } while (!tb_atomic64_compare_and_swap_weak_explicit(a, &o, o + v, mo, mo));
    return o;
}
static __tb_inline__ tb_int64_t tb_atomic64_fetch_and_xor_explicit_generic(tb_atomic64_t* a, tb_int64_t v, tb_int_t mo)
{
    tb_int64_t o; do { o = *a; } while (!tb_atomic64_compare_and_swap_weak_explicit(a, &o, o ^ v, mo, mo));
    return o;
}
static __tb_inline__ tb_int64_t tb_atomic64_fetch_and_and_explicit_generic(tb_atomic64_t* a, tb_int64_t v, tb_int_t mo)
{
    tb_int64_t o; do { o = *a; } while (!tb_atomic64_compare_and_swap_weak_explicit(a, &o, o & v, mo, mo));
    return o;
}
static __tb_inline__ tb_int64_t tb_atomic64_fetch_and_or_explicit_generic(tb_atomic64_t* a, tb_int64_t v, tb_int_t mo)
{
    tb_int64_t o; do { o = *a; } while (!tb_atomic64_compare_and_swap_weak_explicit(a, &o, o | v, mo, mo));
    return o;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
