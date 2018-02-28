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
 * @file        iso8859.c
 * @ingroup     charset
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../stream/stream.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_long_t tb_charset_iso8859_get(tb_static_stream_ref_t sstream, tb_bool_t be, tb_uint32_t* ch);
tb_long_t tb_charset_iso8859_get(tb_static_stream_ref_t sstream, tb_bool_t be, tb_uint32_t* ch)
{
    tb_byte_t b = tb_static_stream_read_u8(sstream);
    if (b < 0xa0) *ch = b;
    else
    {
        // @note: need lookup characters table
        tb_trace_d("iso8859: unknown character: %x", b);
        *ch = 0;
    }
    return 1;
}

tb_long_t tb_charset_iso8859_set(tb_static_stream_ref_t sstream, tb_bool_t be, tb_uint32_t ch);
tb_long_t tb_charset_iso8859_set(tb_static_stream_ref_t sstream, tb_bool_t be, tb_uint32_t ch)
{
    if (ch <= 0xa0) tb_static_stream_writ_u8(sstream, (tb_uint8_t)ch);
    else 
    {
        // @note: need lookup characters table
        tb_trace_d("iso8859: unknown character: %x", ch);
    }
    return 1;
}

