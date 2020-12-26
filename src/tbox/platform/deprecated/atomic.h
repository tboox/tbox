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
#ifndef TB_PLATFORM_DEPRECATED_ATOMIC_H
#define TB_PLATFORM_DEPRECATED_ATOMIC_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../atomic.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifndef tb_atomic_set0
#   define tb_atomic_set0(a)                  tb_atomic_set(a, 0)
#endif

#ifndef tb_atomic_fetch_and_pset
#   define tb_atomic_fetch_and_pset(a, p, v)  tb_atomic_fetch_and_cmpset(a, p, v)
#endif

#ifndef tb_atomic_fetch_and_set0
#   define tb_atomic_fetch_and_set0(a)        tb_atomic_fetch_and_set(a, 0)
#endif

#ifndef tb_atomic_fetch_and_inc
#   define tb_atomic_fetch_and_inc(a)         tb_atomic_fetch_and_add(a, 1)
#endif

#ifndef tb_atomic_fetch_and_dec
#   define tb_atomic_fetch_and_dec(a)         tb_atomic_fetch_and_add(a, -1)
#endif

#ifndef tb_atomic_add_and_fetch
#   define tb_atomic_add_and_fetch(a, v)      (tb_atomic_fetch_and_add(a, v) + (v))
#endif

#ifndef tb_atomic_inc_and_fetch
#   define tb_atomic_inc_and_fetch(a)         tb_atomic_add_and_fetch(a, 1)
#endif

#ifndef tb_atomic_dec_and_fetch
#   define tb_atomic_dec_and_fetch(a)         tb_atomic_add_and_fetch(a, -1)
#endif

#ifndef tb_atomic_sub_and_fetch
#   define tb_atomic_sub_and_fetch(a, v)      tb_atomic_add_and_fetch(a, -(v))
#endif

#ifndef tb_atomic_or_and_fetch
#   define tb_atomic_or_and_fetch(a, v)       (tb_atomic_fetch_and_or(a, v) | (v))
#endif

#ifndef tb_atomic_xor_and_fetch
#   define tb_atomic_xor_and_fetch(a, v)      (tb_atomic_fetch_and_xor(a, v) ^ (v))
#endif

#ifndef tb_atomic_and_and_fetch
#   define tb_atomic_and_and_fetch(a, v)      (tb_atomic_fetch_and_and(a, v) & (v))
#endif

#endif
