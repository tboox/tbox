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

#   define tb_atomic_init(a, v)                         tb_atomic_set_explicit_gcc(a, v, __ATOMIC_RELAXED)
#   define tb_atomic_get(a)                             tb_atomic_get_explicit_gcc(a, __ATOMIC_SEQ_CST)
#   define tb_atomic_set(a, v)                          tb_atomic_set_explicit_gcc(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic_compare_and_set(a, p, v)           tb_atomic_compare_and_set_explicit_gcc(a, p, v, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#   define tb_atomic_compare_and_set_weak(a, p, v)      tb_atomic_compare_and_set_weak_explicit_gcc(a, p, v, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)
#   define tb_atomic_fetch_and_set(a, v)                tb_atomic_fetch_and_set_explicit_gcc(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic_fetch_and_add(a, v)                __atomic_fetch_add(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic_fetch_and_sub(a, v)                __atomic_fetch_sub(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic_fetch_and_or(a, v)                 __atomic_fetch_or(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic_fetch_and_and(a, v)                __atomic_fetch_and(a, v, __ATOMIC_SEQ_CST)
#   define tb_atomic_fetch_and_xor(a, v)                __atomic_fetch_xor(a, v, __ATOMIC_SEQ_CST)
#   define tb_memory_barrier()                          __atomic_thread_fence(__ATOMIC_SEQ_CST)

#else

#   define tb_atomic_compare_and_set(a, p, v)           tb_atomic_compare_and_set_gcc(a, p, v)
#   define tb_atomic_fetch_and_cmpset(a, p, v)          __sync_val_compare_and_swap(a, p, v)

#   define tb_atomic_fetch_and_add(a, v)                __sync_fetch_and_add(a, v)
#   define tb_atomic_fetch_and_sub(a, v)                __sync_fetch_and_sub(a, v)
#   define tb_atomic_fetch_and_or(a, v)                 __sync_fetch_and_or(a, v)
#   define tb_atomic_fetch_and_and(a, v)                __sync_fetch_and_and(a, v)

// FIXME: ios armv6: no defined refernece?
#   if !(defined(TB_CONFIG_OS_IOS) && TB_ARCH_ARM_VERSION < 7)
#       define tb_atomic_fetch_and_xor(a, v)            __sync_fetch_and_xor(a, v)
#   endif

#   define tb_memory_barrier()                          __sync_synchronize()
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */
#ifdef __ATOMIC_SEQ_CST
static __tb_inline__ tb_long_t tb_atomic_get_explicit_gcc(tb_atomic_t* a, tb_size_t mo)
{
    tb_assert(a);
    tb_long_t t;
    __atomic_load(a, &t, mo);
    return t;
}
static __tb_inline__ tb_void_t tb_atomic_set_explicit_gcc(tb_atomic_t* a, tb_long_t v, tb_size_t mo)
{
    tb_assert(a);
    __atomic_store(a, &v, mo);
}
static __tb_inline__ tb_bool_t tb_atomic_compare_and_set_explicit_gcc(tb_atomic_t* a, tb_long_t* p, tb_long_t v, tb_size_t succ, tb_size_t fail)
{
    tb_assert(a);
    return __atomic_compare_exchange(a, p, &v, 0, succ, fail);	
}
static __tb_inline__ tb_bool_t tb_atomic_compare_and_set_weak_explicit_gcc(tb_atomic_t* a, tb_long_t* p, tb_long_t v, tb_size_t succ, tb_size_t fail)
{
    tb_assert(a);
    return __atomic_compare_exchange(a, p, &v, 1, succ, fail);	
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_set_explicit_gcc(tb_atomic_t* a, tb_long_t v, tb_size_t mo)
{
    tb_assert(a);
    tb_long_t o;
    __atomic_exchange(a, &v, &o, mo);	
    return o;
}
#else
static __tb_inline__ tb_bool_t tb_atomic_compare_and_set_gcc(tb_atomic_t* a, tb_long_t* p, tb_long_t v)
{
    tb_assert(a && p);
    tb_long_t e = *p;
    *p = __sync_val_compare_and_swap(a, e, v);
    return *p == e;
}
#endif

#endif
