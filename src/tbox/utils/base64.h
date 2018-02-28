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
 * @file        base.h
 * @ingroup     utils
 *
 */
#ifndef TB_UTILS_BASE64_H
#define TB_UTILS_BASE64_H

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

/*! encode base64
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
tb_size_t           tb_base64_encode(tb_byte_t const* ib, tb_size_t in, tb_char_t* ob, tb_size_t on);

/*! encode base64
 *
 * @param ib        the input data
 * @param in        the input size
 * @param ob        the output data
 * @param on        the output size
 *
 * @return          the real size
 */
tb_size_t           tb_base64_decode(tb_char_t const* ib, tb_size_t in, tb_byte_t* ob, tb_size_t on);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

