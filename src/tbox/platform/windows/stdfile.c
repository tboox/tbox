/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * Copyright (C) 2009 - 2019, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        stdfile.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../stdfile.h"
#include "interface/interface.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_stdfile_ref_t tb_stdfile_init(tb_size_t type)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_void_t tb_stdfile_exit(tb_stdfile_ref_t stdfile)
{
    tb_trace_noimpl();
}
tb_size_t tb_stdfile_type(tb_stdfile_ref_t stdfile)
{
    tb_trace_noimpl();
    return 0;
}
tb_bool_t tb_stdfile_flush(tb_stdfile_ref_t stdfile)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_stdfile_read(tb_stdfile_ref_t file, tb_byte_t* data, tb_size_t size)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_stdfile_writ(tb_stdfile_ref_t file, tb_byte_t const* data, tb_size_t size)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_stdfile_getc(tb_stdfile_ref_t file, tb_int_t* pch)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_stdfile_putc(tb_stdfile_ref_t file, tb_int_t ch)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_stdfile_gets(tb_stdfile_ref_t file, tb_char_t* str, tb_size_t num)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_stdfile_puts(tb_stdfile_ref_t file, tb_char_t const* str)
{
    tb_trace_noimpl();
    return tb_false;
}
