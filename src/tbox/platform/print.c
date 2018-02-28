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
 * @file        print.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "print.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/print.c"
#elif defined(TB_CONFIG_OS_ANDROID)
#   include "android/print.c"
#elif defined(TB_CONFIG_OS_IOS)
#   include "mach/ios/print.c"
#elif defined(TB_CONFIG_LIBC_HAVE_FPUTS)
#   include "libc/print.c"
#else
tb_void_t tb_print(tb_char_t const* string)
{
    tb_trace_noimpl();
}
tb_void_t tb_printl(tb_char_t const* string)
{
    tb_trace_noimpl();
}
tb_void_t tb_print_sync()
{
    tb_trace_noimpl();
}
#endif
