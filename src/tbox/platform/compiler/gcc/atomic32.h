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
#ifndef TB_PLATFORM_COMPILER_GCC_ATOMIC32_H
#define TB_PLATFORM_COMPILER_GCC_ATOMIC32_H


/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifdef __ATOMIC_SEQ_CST

#   define tb_atomic32_init(a, v)                           tb_atomic32_set_explicit_gcc(a, v, __ATOMIC_RELAXED)
#   define tb_atomic32_get(a)                               tb_atomic32_get_explicit_gcc(a, __ATOMIC_SEQ_CST)
#   define tb_atomic32_get_explicit(a, mo)                  tb_atomic32_get_explicit_gcc(a, mo)
#   define tb_atomic32_set(a, v)                            tb_atomic32_set_explicit_gcc(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic32_set_explicit(a, v, mo)               tb_atomic32_set_explicit_gcc(a, v, mo)
#   define tb_atomic32_compare_and_swap(a, p, v)            tb_atomic32_compare_and_swap_explicit_gcc(a, p, v, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#   define tb_atomic32_compare_and_swap_explicit(a, p, v, succ, fail) \
                                                            tb_atomic32_compare_and_swap_explicit_gcc(a, p, v, succ, fail)
#   define tb_atomic32_compare_and_swap_weak(a, p, v)       tb_atomic32_compare_and_swap_weak_explicit_gcc(a, p, v, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#   define tb_atomic32_compare_and_swap_weak_explicit(a, p, v, succ, fail) \
                                                            tb_atomic32_compare_and_swap_weak_explicit_gcc(a, p, v, succ, fail)
#   define tb_atomic32_fetch_and_set(a, v)                  tb_atomic32_fetch_and_set_explicit_gcc(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic32_fetch_and_set_explicit(a, v, mo)     tb_atomic32_fetch_and_set_explicit_gcc(a, v, mo)
#   define tb_atomic32_fetch_and_add(a, v)                  __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic32_fetch_and_add_explicit(a, v, mo)     __atomic_fetch_add(a, v, mo)
#   define tb_atomic32_fetch_and_sub(a, v)                  __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic32_fetch_and_sub_explicit(a, v, mo)     __atomic_fetch_sub(a, v, mo)
#   define tb_atomic32_fetch_and_or(a, v)                   __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic32_fetch_and_or_explicit(a, v, mo)      __atomic_fetch_or(a, v, mo)
#   define tb_atomic32_fetch_and_and(a, v)                  __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic32_fetch_and_and_explicit(a, v, mo)     __atomic_fetch_and(a, v, mo)
#   define tb_atomic32_fetch_and_xor(a, v)                  __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic32_fetch_and_xor_explicit(a, v, mo)     __atomic_fetch_xor(a, v, mo)

#else

#   define tb_atomic32_compare_and_swap_explicit(a, p, v, succ, fail) \
                                                            tb_atomic32_compare_and_swap_gcc(a, p, v)
#   define tb_atomic32_fetch_and_cmpset_explicit(a, p, v, succ, fail) \
                                                            __sync_val_compare_and_swap(a, p, v)
#   define tb_atomic32_fetch_and_add_explicit(a, v, mo)     __sync_fetch_and_add(a, v)
#   define tb_atomic32_fetch_and_sub_explicit(a, v, mo)     __sync_fetch_and_sub(a, v)
#   define tb_atomic32_fetch_and_or_explicit(a, v, mo)      __sync_fetch_and_or(a, v)
#   define tb_atomic32_fetch_and_and_explicit(a, v, mo)     __sync_fetch_and_and(a, v)

// FIXME: ios armv6: no defined refernece?
#   if !(defined(TB_CONFIG_OS_IOS) && TB_ARCH_ARM_VERSION < 7)
#       define tb_atomic32_fetch_and_xor_explicit(a, v, mo) __sync_fetch_and_xor(a, v)
#   endif

#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */
#ifdef __ATOMIC_SEQ_CST
static __tb_inline__ tb_int32_t tb_atomic32_get_explicit_gcc(tb_atomic32_t* a, tb_int_t mo)
{
    tb_assert(a);
    tb_int32_t t;
    __atomic_load(a, &t, mo);
    return t;
}
static __tb_inline__ tb_void_t tb_atomic32_set_explicit_gcc(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
    tb_assert(a);
    __atomic_store(a, &v, mo);
}
static __tb_inline__ tb_bool_t tb_atomic32_compare_and_swap_explicit_gcc(tb_atomic32_t* a, tb_int32_t* p, tb_int32_t v, tb_int_t succ, tb_int_t fail)
{
    tb_assert(a);
    return __atomic_compare_exchange(a, p, &v, 0, succ, fail);
}
static __tb_inline__ tb_bool_t tb_atomic32_compare_and_swap_weak_explicit_gcc(tb_atomic32_t* a, tb_int32_t* p, tb_int32_t v, tb_int_t succ, tb_int_t fail)
{
    tb_assert(a);
    return __atomic_compare_exchange(a, p, &v, 1, succ, fail);
}
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_set_explicit_gcc(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
    tb_assert(a);
    tb_int32_t o;
    __atomic_exchange(a, &v, &o, mo);
    return o;
}
#else
static __tb_inline__ tb_bool_t tb_atomic32_compare_and_swap_gcc(tb_atomic32_t* a, tb_int32_t* p, tb_int32_t v)
{
    tb_assert(a && p);
    tb_int32_t e = *p;
    *p = __sync_val_compare_and_swap(a, e, v);
    return *p == e;
}
#endif

#endif
