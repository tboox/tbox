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
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_IMPL_ATOMIC32_H
#define TB_PLATFORM_IMPL_ATOMIC32_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#if __tb_has_feature__(c_atomic) && !defined(__STDC_NO_ATOMICS__)
#   include "libc/atomic32.h"
#elif defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/atomic32.h"
#elif defined(TB_COMPILER_IS_GCC) \
        && (defined(__ATOMIC_SEQ_CST) || defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4))
#   include "compiler/gcc/atomic32.h"
#endif
#include "arch/atomic32.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifndef tb_atomic32_init
#   define tb_atomic32_init(a, v)                           do { *(a) = (v); } while (0)
#endif

#ifndef tb_atomic32_compare_and_swap_explicit
#   define tb_atomic32_compare_and_swap_explicit(a, p, v, succ, fail) \
                                                            tb_atomic32_compare_and_swap_explicit_generic(a, p, v, succ, fail)
#endif
#ifndef tb_atomic32_compare_and_swap
#   define tb_atomic32_compare_and_swap(a, p, v)            tb_atomic32_compare_and_swap_explicit(a, p, v, TB_ATOMIC_SEQ_CST, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic32_compare_and_swap_weak_explicit
#   define tb_atomic32_compare_and_swap_weak_explicit(a, p, v, succ, fail) \
                                                            tb_atomic32_compare_and_swap_explicit(a, p, v, succ, fail)
#endif
#ifndef tb_atomic32_compare_and_swap_weak
#   define tb_atomic32_compare_and_swap_weak(a, p, v)       tb_atomic32_compare_and_swap_weak_explicit(a, p, v, TB_ATOMIC_SEQ_CST, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic32_fetch_and_cmpset_explicit
#   define tb_atomic32_fetch_and_cmpset_explicit(a, p, v, succ, fail) \
                                                            tb_atomic32_fetch_and_cmpset_explicit_generic(a, p, v, succ, fail)
#endif
#ifndef tb_atomic32_fetch_and_cmpset
#   define tb_atomic32_fetch_and_cmpset(a, p, v)            tb_atomic32_fetch_and_cmpset_explicit(a, p, v, TB_ATOMIC_SEQ_CST, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic32_fetch_and_set_explicit
#   define tb_atomic32_fetch_and_set_explicit(a, v, mo)     tb_atomic32_fetch_and_set_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic32_fetch_and_set
#   define tb_atomic32_fetch_and_set(a, v)                  tb_atomic32_fetch_and_set_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic32_fetch_and_add_explicit
#   define tb_atomic32_fetch_and_add_explicit(a, v, mo)     tb_atomic32_fetch_and_add_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic32_fetch_and_add
#   define tb_atomic32_fetch_and_add(a, v)                  tb_atomic32_fetch_and_add_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic32_fetch_and_sub_explicit
#   define tb_atomic32_fetch_and_sub_explicit(a, v, mo)     tb_atomic32_fetch_and_add_explicit(a, -(v), mo)
#endif
#ifndef tb_atomic32_fetch_and_sub
#   define tb_atomic32_fetch_and_sub(a, v)                  tb_atomic32_fetch_and_sub_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic32_fetch_and_or_explicit
#   define tb_atomic32_fetch_and_or_explicit(a, v, mo)      tb_atomic32_fetch_and_or_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic32_fetch_and_or
#   define tb_atomic32_fetch_and_or(a, v)                   tb_atomic32_fetch_and_or_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic32_fetch_and_xor_explicit
#   define tb_atomic32_fetch_and_xor_explicit(a, v, mo)     tb_atomic32_fetch_and_xor_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic32_fetch_and_xor
#   define tb_atomic32_fetch_and_xor(a, v)                  tb_atomic32_fetch_and_xor_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic32_fetch_and_and_explicit
#   define tb_atomic32_fetch_and_and_explicit(a, v, mo)     tb_atomic32_fetch_and_and_explicit_generic(a, v, mo)
#endif
#ifndef tb_atomic32_fetch_and_and
#   define tb_atomic32_fetch_and_and(a, v)                  tb_atomic32_fetch_and_and_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic32_get_explicit
#   define tb_atomic32_get_explicit(a, mo)                  tb_atomic32_fetch_and_cmpset_explicit(a, 0, 0, mo, mo)
#endif
#ifndef tb_atomic32_get
#   define tb_atomic32_get(a)                               tb_atomic32_get_explicit(a, TB_ATOMIC_SEQ_CST)
#endif

#ifndef tb_atomic32_set_explicit
#   define tb_atomic32_set_explicit(a, v, mo)               tb_atomic32_fetch_and_set_explicit(a, v, mo)
#endif
#ifndef tb_atomic32_set
#   define tb_atomic32_set(a, v)                            tb_atomic32_set_explicit(a, v, TB_ATOMIC_SEQ_CST)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_bool_t tb_atomic32_compare_and_swap_explicit_generic(tb_atomic32_t* a, tb_int32_t* p, tb_int32_t v, tb_int_t succ, tb_int_t fail)
{
    // FIXME
    // no safe

    tb_atomic32_t o = *a;
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
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_cmpset_explicit_generic(tb_atomic32_t* a, tb_int32_t p, tb_int32_t v, tb_int_t succ, tb_int_t fail)
{
    tb_atomic32_compare_and_swap_explicit(a, &p, v, succ, fail);
    return p;
}
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_set_explicit_generic(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
    tb_int32_t o;
    do { o = *a; } while (!tb_atomic32_compare_and_swap_weak_explicit(a, &o, v, mo, mo));
    return o;
}
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_add_explicit_generic(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
    tb_int32_t o; do { o = *a; } while (!tb_atomic32_compare_and_swap_weak_explicit(a, &o, o + v, mo, mo));
    return o;
}
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_xor_explicit_generic(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
    tb_int32_t o; do { o = *a; } while (!tb_atomic32_compare_and_swap_weak_explicit(a, &o, o ^ v, mo, mo));
    return o;
}
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_and_explicit_generic(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
    tb_int32_t o; do { o = *a; } while (!tb_atomic32_compare_and_swap_weak_explicit(a, &o, o & v, mo, mo));
    return o;
}
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_or_explicit_generic(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
    tb_int32_t o; do { o = *a; } while (!tb_atomic32_compare_and_swap_weak_explicit(a, &o, o | v, mo, mo));
    return o;
}


#endif
