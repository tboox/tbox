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
 * @author      yangjz
 * @file        kmp.h
 * @ingroup     algorithm
 *
 */
#ifndef TB_ALGORITHM_KMP_H
#define TB_ALGORITHM_KMP_H

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

/*! the KMP data init, O(strlen(pattern))
 *
 * @param pattern   the pattern string
 * @return          the KMP matcher
 */
tb_pointer_t        tb_kmp_matcher_init(const tb_char_t *pattern);

/*! the insert sorter for all, O(strlen(text))
 *
 * @param text      the text
 * @param pattern   the pattern string
 * @param matcher   the KMP matcher
 * @return          the index
 */
tb_size_t           tb_kmp_match(const tb_char_t *text, const tb_char_t *pattern, tb_pointer_t matcher);

/*! the KMP data exit
 *
 * @param matcher   the KMP matcher
 */
tb_void_t           tb_kmp_matcher_exit(tb_pointer_t matcher);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__
#endif
