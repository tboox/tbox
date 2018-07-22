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
 * @author      yangjz1125
 * @file        merge.c
 * @ingroup     algorithm
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "merge.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_void_t tb_merge(tb_iterator_ref_t iterator1, tb_size_t head1, tb_size_t tail1,
                   tb_iterator_ref_t iterator2, tb_size_t head2, tb_size_t tail2,
                   tb_iterator_ref_t output, tb_size_t head,
                   tb_bool_t (*comp)(tb_cpointer_t,tb_cpointer_t))
{
    tb_size_t now1 = head1, now2 = head2;
    while(now1 != tail1 && now2 != tail2)
    {
        tb_pointer_t a = tb_iterator_item(iterator1, now1), b = tb_iterator_item(iterator2, now2);
        if(comp(a,b))
        {
            tb_iterator_copy(output, head, a);
        }
        else 
        {
            tb_iterator_copy(output, head, b);
        }
        now1 = tb_iterator_next(iterator1, now1);
        now2 = tb_iterator_next(iterator2, now2);
    }
    while(now1 != tail1)
    {
        tb_iterator_copy(output, head, tb_iterator_item(iterator1, now1));
        now1 = tb_iterator_next(iterator1, now1);
    }
    while(now2 != tail2)
    {
        tb_iterator_copy(output, head, tb_iterator_item(iterator2, now2));
        now1 = tb_iterator_next(iterator2, now2);
    }
}

tb_void_t tb_merge_all(tb_iterator_ref_t iterator1, tb_iterator_ref_t iterator2,
                       tb_iterator_ref_t output, tb_size_t head,  tb_bool_t (*comp)(tb_cpointer_t,tb_cpointer_t))
{
    tb_merge(iterator1, tb_iterator_head(iterator1), tb_iterator_tail(iterator1),
             iterator2, tb_iterator_head(iterator2), tb_iterator_tail(iterator2),
             output, head, comp
             );
}
