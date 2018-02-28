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
 * @file        strcmp.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_ASSEMBLER_IS_GAS
#   define TB_LIBC_STRING_IMPL_STRCMP
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_ASSEMBLER_IS_GAS
static tb_long_t tb_strcmp_impl(tb_char_t const* s1, tb_char_t const* s2)
{
    tb_assert_and_check_return_val(s1 && s2, 0);
    if (s1 == s2) return 0;

    __tb_register__ tb_long_t r = 0;
    __tb_asm__ __tb_volatile__
    (
        "1:\n"
        "ldrb   r2, [%1], #1\n"
        "ldrb   r3, [%2], #1\n"
        "cmp    r2, #1\n"
        "cmpcs  r2, r3\n"       // r2 == r3? if r2 >= 1
        "beq    1b\n"
        "sub    %0, r2, r3"     // r = r2 - r3 if r2 != r3

        : "=r"(r)
        : "r"(s1), "r"(s2)
        : "memory"
    );

    return r;
}
#endif
