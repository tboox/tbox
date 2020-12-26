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
 *
 */
#ifndef TB_PLATFORM_ARCH_x64_ATOMIC64_H
#define TB_PLATFORM_ARCH_x64_ATOMIC64_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_ASSEMBLER_IS_GAS

#ifndef tb_atomic64_fetch_and_set_explicit
#   define tb_atomic64_fetch_and_set_explicit(a, v, mo)         tb_atomic64_fetch_and_set_explicit_x64(a, v, mo)
#endif

#ifndef tb_atomic64_compare_and_swap_explicit
#   define tb_atomic64_compare_and_swap_explicit(a, p, v, succ, fail) \
                                                                tb_atomic64_compare_and_swap_explicit_x64(a, p, v, succ, fail)
#endif

#ifndef tb_atomic64_fetch_and_add_explicit
#   define tb_atomic64_fetch_and_add_explicit(a, v, mo)         tb_atomic64_fetch_and_add_explicit_x64(a, v, mo)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_int64_t tb_atomic64_fetch_and_set_explicit_x64(tb_atomic64_t* a, tb_int64_t v, tb_int_t mo)
{
    // check
    tb_assert(a);

    __tb_asm__ __tb_volatile__
    (
        "lock xchgq %0, %1\n"   //!< xchgq v, [a]

        : "+r" (v)
        : "m" (*a)
        : "memory"
    );

    return v;
}
static __tb_inline__ tb_bool_t tb_atomic64_compare_and_swap_explicit_x64(tb_atomic64_t* a, tb_int64_t* p, tb_int64_t v, tb_int_t succ, tb_int_t fail)
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
    tb_int64_t o;
    tb_int64_t e = *p;
    __tb_asm__ __tb_volatile__
    (
        "lock cmpxchgq  %3, %1  \n"     //!< cmpxchgl v, [a]

        : "=a" (o)
        : "m" (*a), "a" (e), "r" (v)
        : "cc", "memory"                //!< "cc" means that flags were changed.
    );
    *p = o;
    return o == e;
}
static __tb_inline__ tb_int64_t tb_atomic64_fetch_and_add_explicit_x64(tb_atomic64_t* a, tb_int64_t v, tb_int_t mo)
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
        "lock xaddq %0, %1 \n"          //!< xaddq v, [a]

        : "+r" (v)
        : "m" (*a)
        : "cc", "memory"
    );

    return v;
}

#endif // TB_ASSEMBLER_IS_GAS

#endif
