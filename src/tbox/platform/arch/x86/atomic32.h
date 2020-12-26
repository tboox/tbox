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
#ifndef TB_PLATFORM_ARCH_x86_ATOMIC32_H
#define TB_PLATFORM_ARCH_x86_ATOMIC32_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_ASSEMBLER_IS_GAS

#ifndef tb_atomic32_fetch_and_set_explicit
#   define tb_atomic32_fetch_and_set_explicit(a, v, mo)         tb_atomic32_fetch_and_set_explicit_x86(a, v, mo)
#endif

#ifndef tb_atomic32_compare_and_swap_explicit
#   define tb_atomic32_compare_and_swap_explicit(a, p, v, succ, fail) \
                                                                tb_atomic32_compare_and_swap_explicit_x86(a, p, v, succ, fail)
#endif

#ifndef tb_atomic32_fetch_and_add_explicit
#   define tb_atomic32_fetch_and_add_explicit(a, v, mo)         tb_atomic32_fetch_and_add_explicit_x86(a, v, mo)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_set_explicit_x86(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
    // check
    tb_assert(a);

    __tb_asm__ __tb_volatile__
    (
        "lock xchgl %0, %1\n"   //!< xchgl v, [a]
        : "+r" (v)
        : "m" (*a)
        : "memory"
    );

    return v;
}

static __tb_inline__ tb_bool_t tb_atomic32_compare_and_swap_explicit_x86(tb_atomic32_t* a, tb_int32_t* p, tb_int32_t v, tb_int_t succ, tb_int_t fail)
{
    // check
    tb_assert(a && p);

    /*
     * cmpxchgl v, [a]:
     *
     * if (eax == [a])
     * {
     *      zf = 1;
     *      [a] = v;
     * }
     * else
     * {
     *      zf = 0;
     *      eax = [a];
     * }
     *
     */
    tb_int32_t o;
    tb_int32_t e = *p;
    __tb_asm__ __tb_volatile__
    (
        "lock cmpxchgl  %3, %1  \n"     //!< cmpxchgq v, [a]
        : "=a" (o)
        : "m" (*a), "a" (e), "r" (v)
        : "cc", "memory"                //!< "cc" means that flags were changed.
    );
    *p = o;
    return o == e;
}

static __tb_inline__ tb_int32_t tb_atomic32_fetch_and_add_explicit_x86(tb_atomic32_t* a, tb_int32_t v, tb_int_t mo)
{
    // check
    tb_assert(a);

    /*
     * xaddl v, [a]:
     *
     * o = [a]
     * [a] += v;
     * v = o;
     *
     * cf, ef, of, sf, zf, pf... maybe changed
     */
    __tb_asm__ __tb_volatile__
    (
        "lock xaddl %0, %1 \n"          //!< xaddl v, [a]
        : "+r" (v)
        : "m" (*a)
        : "cc", "memory"
    );

    return v;
}


#endif // TB_ASSEMBLER_IS_GAS


#endif
