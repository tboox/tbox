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
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        stdfile.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "platform_stdfile"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stdfile.h"
#include "../utils/utils.h"
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/interface/interface.h"
#elif defined(TB_CONFIG_POSIX_HAVE_SELECT)
#   include <unistd.h>
#   include <sys/select.h>
#   include <errno.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * instance implementation
 */
static tb_handle_t tb_stdfile_instance_init(tb_cpointer_t* ppriv)
{
    tb_size_t* ptype = (tb_size_t*)ppriv;
    return ptype? (tb_handle_t)tb_stdfile_init(*ptype) : tb_null;
}
static tb_void_t tb_stdfile_instance_exit(tb_handle_t stdfile, tb_cpointer_t priv)
{
    tb_stdfile_exit((tb_stdfile_ref_t)stdfile);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_stdfile_ref_t tb_stdfile_input()
{
    return (tb_stdfile_ref_t)tb_singleton_instance(TB_SINGLETON_TYPE_STDFILE_STDIN, tb_stdfile_instance_init, tb_stdfile_instance_exit, tb_null, tb_u2p(TB_STDFILE_TYPE_STDIN));
}
tb_stdfile_ref_t tb_stdfile_output()
{
    return (tb_stdfile_ref_t)tb_singleton_instance(TB_SINGLETON_TYPE_STDFILE_STDOUT, tb_stdfile_instance_init, tb_stdfile_instance_exit, tb_null, tb_u2p(TB_STDFILE_TYPE_STDOUT));
}
tb_stdfile_ref_t tb_stdfile_error()
{
    return (tb_stdfile_ref_t)tb_singleton_instance(TB_SINGLETON_TYPE_STDFILE_STDERR, tb_stdfile_instance_init, tb_stdfile_instance_exit, tb_null, tb_u2p(TB_STDFILE_TYPE_STDERR));
}

#if defined(TB_CONFIG_OS_WINDOWS) && defined(TB_CONFIG_MODULE_HAVE_CHARSET)
#   include "windows/stdfile.c"
#elif defined(TB_CONFIG_LIBC_HAVE_FREAD) && defined(TB_CONFIG_LIBC_HAVE_FWRITE)
#   include "libc/stdfile.c"
#else
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
tb_bool_t tb_stdfile_peek(tb_stdfile_ref_t file, tb_char_t* pch)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_stdfile_getc(tb_stdfile_ref_t file, tb_char_t* pch)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_stdfile_putc(tb_stdfile_ref_t file, tb_char_t ch)
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
#endif

#if defined(TB_CONFIG_OS_WINDOWS)
tb_bool_t tb_stdfile_readable(tb_stdfile_ref_t self)
{
#ifdef TB_CONFIG_MODULE_HAVE_CHARSET
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->type == TB_STDFILE_TYPE_STDIN, tb_false);
#endif

    return WaitForSingleObject(GetStdHandle(STD_INPUT_HANDLE), 0) == WAIT_OBJECT_0;
}
#elif defined(TB_CONFIG_POSIX_HAVE_SELECT)
tb_bool_t tb_stdfile_readable(tb_stdfile_ref_t self)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->fp, tb_false);
    tb_assert_and_check_return_val(stdfile->type == TB_STDFILE_TYPE_STDIN, tb_false);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    return select(1, &readfds, tb_null, tb_null, &timeout) > 0;
}
#else
tb_bool_t tb_stdfile_readable(tb_stdfile_ref_t self)
{
    tb_trace_noimpl();
    return tb_false;
}
#endif
