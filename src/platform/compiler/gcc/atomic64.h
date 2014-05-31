/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        atomic64.h
 *
 */
#ifndef TB_PLATFORM_COMPILER_GCC_ATOMIC64_H
#define TB_PLATFORM_COMPILER_GCC_ATOMIC64_H


/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#define tb_atomic64_get(a)                      tb_atomic64_get_sync(a)
#define tb_atomic64_set(a, v)                   tb_atomic64_set_sync(a, v)
#define tb_atomic64_set0(a)                     tb_atomic64_set0_sync(a)
#define tb_atomic64_pset(a, p, v)               tb_atomic64_pset_sync(a, p, v)
#define tb_atomic64_fetch_and_set0(a)           tb_atomic64_fetch_and_set0_sync(a)
#define tb_atomic64_fetch_and_set(a, v)         tb_atomic64_fetch_and_set_sync(a, v)
#define tb_atomic64_fetch_and_pset(a, p, v)     tb_atomic64_fetch_and_pset_sync(a, p, v)

#define tb_atomic64_fetch_and_inc(a)            tb_atomic64_fetch_and_inc_sync(a)
#define tb_atomic64_fetch_and_dec(a)            tb_atomic64_fetch_and_dec_sync(a)
#define tb_atomic64_fetch_and_add(a, v)         tb_atomic64_fetch_and_add_sync(a, v)
#define tb_atomic64_fetch_and_sub(a, v)         tb_atomic64_fetch_and_sub_sync(a, v)
#define tb_atomic64_fetch_and_or(a, v)          tb_atomic64_fetch_and_or_sync(a, v)
#define tb_atomic64_fetch_and_and(a, v)         tb_atomic64_fetch_and_and_sync(a, v)

#define tb_atomic64_inc_and_fetch(a)            tb_atomic64_inc_and_fetch_sync(a)
#define tb_atomic64_dec_and_fetch(a)            tb_atomic64_dec_and_fetch_sync(a)
#define tb_atomic64_add_and_fetch(a, v)         tb_atomic64_add_and_fetch_sync(a, v)
#define tb_atomic64_sub_and_fetch(a, v)         tb_atomic64_sub_and_fetch_sync(a, v)
#define tb_atomic64_or_and_fetch(a, v)          tb_atomic64_or_and_fetch_sync(a, v)
#define tb_atomic64_and_and_fetch(a, v)         tb_atomic64_and_and_fetch_sync(a, v)

// FIXME: ios armv6: no defined refernece?
#if !(defined(TB_CONFIG_OS_IOS) && TB_ARCH_ARM_VERSION < 7)
#   define tb_atomic64_fetch_and_xor(a, v)      tb_atomic64_fetch_and_xor_sync(a, v)
#   define tb_atomic64_xor_and_fetch(a, v)      tb_atomic64_xor_and_fetch_sync(a, v)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * get & set
 */
static __tb_inline__ tb_hize_t tb_atomic64_get_sync(tb_atomic64_t* a)
{
    tb_assert(a);
    return __sync_val_compare_and_swap_8(a, 0, 0);
}
static __tb_inline__ tb_void_t tb_atomic64_set_sync(tb_atomic64_t* a, tb_hize_t v)
{
    tb_assert(a);
    __sync_lock_test_and_set_8(a, v);
}
static __tb_inline__ tb_void_t tb_atomic64_set0_sync(tb_atomic64_t* a)
{
    tb_assert(a);
     __sync_lock_test_and_set_8(a, 0);
}
static __tb_inline__ tb_void_t tb_atomic64_pset_sync(tb_atomic64_t* a, tb_hize_t p, tb_hize_t v)
{
    tb_assert(a);
    __sync_val_compare_and_swap_8(a, p, v);
}
static __tb_inline__ tb_hize_t tb_atomic64_fetch_and_set0_sync(tb_atomic64_t* a)
{
    tb_assert(a);
    return __sync_lock_test_and_set_8(a, 0);
}
static __tb_inline__ tb_hize_t tb_atomic64_fetch_and_set_sync(tb_atomic64_t* a, tb_hize_t v)
{
    tb_assert(a);
    return __sync_lock_test_and_set_8(a, v);
}
static __tb_inline__ tb_hize_t tb_atomic64_fetch_and_pset_sync(tb_atomic64_t* a, tb_hize_t p, tb_hize_t v)
{
    tb_assert(a);
    return __sync_val_compare_and_swap_8(a, p, v);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * fetch and ...
 */
static __tb_inline__ tb_hong_t tb_atomic64_fetch_and_inc_sync(tb_atomic64_t* a)
{
    tb_assert(a);
    return __sync_fetch_and_add_8(a, 1);
}
static __tb_inline__ tb_hong_t tb_atomic64_fetch_and_dec_sync(tb_atomic64_t* a)
{
    tb_assert(a);
    return __sync_fetch_and_sub_8(a, 1);
}
static __tb_inline__ tb_hong_t tb_atomic64_fetch_and_add_sync(tb_atomic64_t* a, tb_hong_t v)
{
    tb_assert(a);
    return __sync_fetch_and_add_8(a, v);
}
static __tb_inline__ tb_hong_t tb_atomic64_fetch_and_sub_sync(tb_atomic64_t* a, tb_hong_t v)
{
    tb_assert(a);
    return __sync_fetch_and_sub(a, v);
}

#if !(defined(TB_CONFIG_OS_IOS) && (TB_ARCH_ARM_VERSION < 7))
static __tb_inline__ tb_hize_t tb_atomic64_fetch_and_xor_sync(tb_atomic64_t* a, tb_hize_t v)
{
    tb_assert(a);
    return __sync_fetch_and_xor_8(a, v);
}
#endif
static __tb_inline__ tb_hize_t tb_atomic64_fetch_and_and_sync(tb_atomic64_t* a, tb_hize_t v)
{
    tb_assert(a);
    return __sync_fetch_and_and_8(a, v);
}
static __tb_inline__ tb_hize_t tb_atomic64_fetch_and_or_sync(tb_atomic64_t* a, tb_hize_t v)
{
    tb_assert(a);
    return __sync_fetch_and_or_8(a, v);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * ... and fetch
 */
static __tb_inline__ tb_hong_t tb_atomic64_inc_and_fetch_sync(tb_atomic64_t* a)
{
    tb_assert(a);
    return __sync_add_and_fetch_8(a, 1);
}
static __tb_inline__ tb_hong_t tb_atomic64_dec_and_fetch_sync(tb_atomic64_t* a)
{
    tb_assert(a);
    return __sync_sub_and_fetch_8(a, 1);
}
static __tb_inline__ tb_hong_t tb_atomic64_add_and_fetch_sync(tb_atomic64_t* a, tb_hong_t v)
{
    tb_assert(a);
    return __sync_add_and_fetch_8(a, v);
}
static __tb_inline__ tb_hong_t tb_atomic64_sub_and_fetch_sync(tb_atomic64_t* a, tb_hong_t v)
{
    tb_assert(a);
    return __sync_sub_and_fetch_8(a, v);
}

#if !(defined(TB_CONFIG_OS_IOS) && (TB_ARCH_ARM_VERSION < 7))
static __tb_inline__ tb_hize_t tb_atomic64_xor_and_fetch_sync(tb_atomic64_t* a, tb_hize_t v)
{
    tb_assert(a);
    return __sync_xor_and_fetch_8(a, v);
}
#endif

static __tb_inline__ tb_hize_t tb_atomic64_and_and_fetch_sync(tb_atomic64_t* a, tb_hize_t v)
{
    tb_assert(a);
    return __sync_and_and_fetch_8(a, v);
}
static __tb_inline__ tb_hize_t tb_atomic64_or_and_fetch_sync(tb_atomic64_t* a, tb_hize_t v)
{
    tb_assert(a);
    return __sync_or_and_fetch_8(a, v);
}

#endif
