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
 * @file        insert_sort.h
 * @ingroup     algorithm
 *
 */
#ifndef TB_ALGORITHM_MERGE_H
#define TB_ALGORITHM_MERGE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! merge two things, O(n)
 *
 * @param iterator1  the iterator
 * @param head1      the iterator head
 * @param tail1      the iterator tail
 * @param iterator2  the iterator
 * @param head2      the iterator head
 * @param tail2      the iterator tail
 * @param comp       the comparer
 */
tb_void_t           tb_merge(tb_iterator_ref_t iterator1, tb_size_t head1, tb_size_t tail1,
                             tb_iterator_ref_t iterator2, tb_size_t head2, tb_size_t tail2,
                             tb_iterator_ref_t output, tb_size_t head,
                             tb_bool_t (*comp)(tb_cpointer_t,tb_cpointer_t));


/*! merge two things for all, O(n)
 *
 * @param iterator1  the iterator
 * @param head1      the iterator head
 * @param tail1      the iterator tail
 * @param iterator2  the iterator
 * @param head2      the iterator head
 * @param tail2      the iterator tail
 * @param comp       the comparer
 */
tb_void_t           tb_merge_all(tb_iterator_ref_t iterator1, tb_iterator_ref_t iterator2, 
                                 tb_iterator_ref_t output, tb_size_t head, tb_bool_t (*comp)(tb_cpointer_t,tb_cpointer_t));

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__
#endif
