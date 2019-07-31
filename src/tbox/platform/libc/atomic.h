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
#ifndef TB_PLATFORM_COMPILER_LIBC_ATOMIC_H
#define TB_PLATFORM_COMPILER_LIBC_ATOMIC_H


/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <stdatomic.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#define tb_atomic_init(a, v)                        atomic_init(a, v)
#define tb_atomic_get(a)                            atomic_load(a)
#define tb_atomic_set(a, v)                         atomic_store(a, v)
#define tb_atomic_compare_and_set(a, p, v)          atomic_compare_exchange_strong(a, p, v)
#define tb_atomic_compare_and_set_weak(a, p, v)     atomic_compare_exchange_weak(a, p, v)
#define tb_atomic_fetch_and_set(a, v)               atomic_exchange(a, v)
#define tb_atomic_fetch_and_add(a, v)               atomic_fetch_add(a, v)
#define tb_atomic_fetch_and_sub(a, v)               atomic_fetch_sub(a, v)
#define tb_atomic_fetch_and_or(a, v)                atomic_fetch_or(a, v)
#define tb_atomic_fetch_and_and(a, v)               atomic_fetch_and(a, v)
#define tb_atomic_fetch_and_xor(a, v)               atomic_fetch_xor(a, v)


#endif
