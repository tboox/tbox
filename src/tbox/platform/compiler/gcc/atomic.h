/*!The Treasure Box Library
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2009 - 2018, TBOOX Open Source Group.
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

#define tb_atomic_fetch_and_set(a, v)       tb_atomic_fetch_and_set_sync(a, v)
#define tb_atomic_fetch_and_pset(a, p, v)   tb_atomic_fetch_and_pset_sync(a, p, v)

#define tb_atomic_fetch_and_add(a, v)       tb_atomic_fetch_and_add_sync(a, v)
#define tb_atomic_fetch_and_sub(a, v)       tb_atomic_fetch_and_sub_sync(a, v)
#define tb_atomic_fetch_and_or(a, v)        tb_atomic_fetch_and_or_sync(a, v)
#define tb_atomic_fetch_and_and(a, v)       tb_atomic_fetch_and_and_sync(a, v)

#define tb_atomic_add_and_fetch(a, v)       tb_atomic_add_and_fetch_sync(a, v)
#define tb_atomic_sub_and_fetch(a, v)       tb_atomic_sub_and_fetch_sync(a, v)
#define tb_atomic_or_and_fetch(a, v)        tb_atomic_or_and_fetch_sync(a, v)
#define tb_atomic_and_and_fetch(a, v)       tb_atomic_and_and_fetch_sync(a, v)

// FIXME: ios armv6: no defined refernece?
#if !(defined(TB_CONFIG_OS_IOS) && TB_ARCH_ARM_VERSION < 7)
#   define tb_atomic_fetch_and_xor(a, v)    tb_atomic_fetch_and_xor_sync(a, v)
#   define tb_atomic_xor_and_fetch(a, v)    tb_atomic_xor_and_fetch_sync(a, v)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_long_t tb_atomic_fetch_and_set_sync(tb_atomic_t* a, tb_long_t v)
{
    return __sync_lock_test_and_set(a, v);
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_pset_sync(tb_atomic_t* a, tb_long_t p, tb_long_t v)
{
    return __sync_val_compare_and_swap(a, p, v);
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_add_sync(tb_atomic_t* a, tb_long_t v)
{
    return __sync_fetch_and_add(a, v);
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_sub_sync(tb_atomic_t* a, tb_long_t v)
{
    return __sync_fetch_and_sub(a, v);
}
#if !(defined(TB_CONFIG_OS_IOS) && (TB_ARCH_ARM_VERSION < 7))
static __tb_inline__ tb_long_t tb_atomic_fetch_and_xor_sync(tb_atomic_t* a, tb_long_t v)
{
    return __sync_fetch_and_xor(a, v);
}
#endif
static __tb_inline__ tb_long_t tb_atomic_fetch_and_and_sync(tb_atomic_t* a, tb_long_t v)
{
    return __sync_fetch_and_and(a, v);
}
static __tb_inline__ tb_long_t tb_atomic_fetch_and_or_sync(tb_atomic_t* a, tb_long_t v)
{
    return __sync_fetch_and_or(a, v);
}
static __tb_inline__ tb_long_t tb_atomic_add_and_fetch_sync(tb_atomic_t* a, tb_long_t v)
{
    return __sync_add_and_fetch(a, v);
}
static __tb_inline__ tb_long_t tb_atomic_sub_and_fetch_sync(tb_atomic_t* a, tb_long_t v)
{
    return __sync_sub_and_fetch(a, v);
}
#if !(defined(TB_CONFIG_OS_IOS) && (TB_ARCH_ARM_VERSION < 7))
static __tb_inline__ tb_long_t tb_atomic_xor_and_fetch_sync(tb_atomic_t* a, tb_long_t v)
{
    return __sync_xor_and_fetch(a, v);
}
#endif
static __tb_inline__ tb_long_t tb_atomic_and_and_fetch_sync(tb_atomic_t* a, tb_long_t v)
{
    return __sync_and_and_fetch(a, v);
}
static __tb_inline__ tb_long_t tb_atomic_or_and_fetch_sync(tb_atomic_t* a, tb_long_t v)
{
    return __sync_or_and_fetch(a, v);
}

#endif
