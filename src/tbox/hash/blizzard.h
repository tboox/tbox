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
 * @file        blizzard.h
 * @ingroup     hash
 *
 */
#ifndef TB_HASH_BLIZZARD_H
#define TB_HASH_BLIZZARD_H

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

/*! make blizzard hash
 *
 * Blizzard One-Way Hash algorithm from MPQ
 *
 * @param data      the data
 * @param size      the size
 * @param seed      uses this seed if be non-zero
 *
 * @return          the blizzard value
 */
tb_size_t           tb_blizzard_make(tb_byte_t const* data, tb_size_t size, tb_size_t seed);

/*! make blizzard hash from c-string
 *
 * @param cstr      the c-string
 * @param seed      uses this seed if be non-zero
 *
 * @return          the blizzard value
 */
tb_size_t           tb_blizzard_make_from_cstr(tb_char_t const* cstr, tb_size_t seed);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
