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
 * @file        insert_sort.c
 * @ingroup     algorithm
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "kmp.h"
#include "../libc/libc.h"
#include "../memory/memory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_pointer_t tb_kmp_matcher_init(const tb_char_t *pattern)
{
    tb_size_t len = tb_strlen(pattern), *matcher = tb_nalloc_type(len + 1, tb_size_t);
    tb_size_t k = 0;
    matcher[0] = 0;
    for(tb_size_t q = 1; q < len; ++q)
    {
        while(k > 0 && pattern[k] != pattern[q])
        {
            k = matcher[k-1];
        }
        if(pattern[k] == pattern[q])
        {
            ++k;
        }
        matcher[q] = k;
    }
    return matcher;
}

tb_size_t tb_kmp_match(const tb_char_t *text, const tb_char_t *pattern, tb_pointer_t matcher){
    tb_size_t tlen = tb_strlen(text), plen = tb_strlen(pattern);
    tb_size_t q = 0;
    for(tb_size_t i = 0; i < tlen; ++i)
    {
        while(q > 0 && pattern[q] != text[i])
        {
            q = ((tb_size_t*)matcher)[q];
        }
        if(pattern[q] == text[i])
        {
            ++q;
        }
        if(q == plen)
        {
            return i - plen + 1;
        }
    }
    return tlen;
}

tb_void_t tb_kmp_matcher_exit(tb_pointer_t matcher){
    tb_free(matcher);
}