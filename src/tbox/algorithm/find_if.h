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
 * @file        find_if.h
 * @ingroup     algorithm
 *
 */
#ifndef TB_ALGORITHM_FIND_IF_H
#define TB_ALGORITHM_FIND_IF_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "predicate.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! find item if pred(item, value)
 *
 * @param iterator  the iterator
 * @param head      the iterator head
 * @param tail      the iterator tail
 * @param pred      the predicate
 * @param value     the value of the predicate
 *
 * @return          the iterator itor, return tb_iterator_tail(iterator) if not found
 */
tb_size_t           tb_find_if(tb_iterator_ref_t iterator, tb_size_t head, tb_size_t tail, tb_predicate_ref_t pred, tb_cpointer_t value);

/*! find item for all if pred(item, value)
 *
 * @param iterator  the iterator
 * @param pred      the predicate
 * @param value     the value of the predicate
 *
 * @return          the iterator itor, return tb_iterator_tail(iterator) if not found
 */
tb_size_t           tb_find_all_if(tb_iterator_ref_t iterator, tb_predicate_ref_t pred, tb_cpointer_t value);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__
#endif
