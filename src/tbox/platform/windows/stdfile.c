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
#include "../../stream/stream.h"
#include "interface/interface.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the stream cache maxn
#define TB_STDFILE_STREAM_CACHE_MAXN             (4096)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the stdfile type
typedef struct __tb_stdfile_t
{
    // the file type
    tb_size_t           type;

    // the input stream 
    tb_stream_ref_t     istream;

    // the output stream
    tb_stream_ref_t     ostream;

}tb_stdfile_t;

// the stdfile stream type
typedef struct __tb_stdfile_stream_t
{
    // the stream type
    tb_size_t           type;

    // the std file handle
    HANDLE              fp;

    // is console mode?
    tb_bool_t           is_console;

}tb_stdfile_stream_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * stream implementation
 */
static tb_bool_t tb_stdfile_stream_open(tb_stream_ref_t self)
{
    // check
    tb_stdfile_stream_t* stream = (tb_stdfile_stream_t*)self;
    tb_assert_and_check_return_val(stream && stream->fp, tb_false);

    // get console mode
    DWORD real = 0;
    if (GetConsoleMode(stream->fp, &real))
        stream->is_console = tb_true;
    else stream->is_console = tb_false;

    // ok
    return tb_true;
}
static tb_bool_t tb_stdfile_stream_clos(tb_stream_ref_t self)
{
    return tb_true;
}
static tb_long_t tb_stdfile_stream_read(tb_stream_ref_t self, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_stdfile_stream_t* stream = (tb_stdfile_stream_t*)self;
    tb_assert_and_check_return_val(stream && stream->fp && data, -1);
    tb_assert_and_check_return_val(stream->type == TB_STDFILE_TYPE_STDIN, -1);
    tb_check_return_val(size, 0);

    // read data from console or file (redirected)
    tb_long_t read = -1;
    if (stream->is_console)
    {
        // get wide characters size
        DWORD wsize = size / sizeof(tb_wchar_t);
        tb_check_return_val(wsize, 0);

        // read wide characters from console
        DWORD wreal = 0;
        if (ReadConsoleW(stream->fp, (tb_wchar_t*)data, wsize, &wreal, tb_null)) 
            read = wreal * sizeof(tb_wchar_t);
    }
    else
    {
        DWORD real = 0;
        if (ReadFile(stream->fp, data, (DWORD)size, &real, tb_null)) 
            read = (tb_long_t)real;
    }
    return read;
}
static tb_long_t tb_stdfile_stream_writ(tb_stream_ref_t self, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_stdfile_stream_t* stream = (tb_stdfile_stream_t*)self;
    tb_assert_and_check_return_val(stream && stream->fp && data, -1);
    tb_assert_and_check_return_val(stream->type != TB_STDFILE_TYPE_STDIN, -1);
    tb_check_return_val(size, 0);

    // write data to console or file (redirected)
    tb_long_t writ = -1;
    if (stream->is_console)
    {
        // get wide characters size
        DWORD wsize = size / sizeof(tb_wchar_t);
        tb_check_return_val(wsize, 0);

        // write wide characters to console
        DWORD wreal = 0;
        if (WriteConsoleW(stream->fp, (tb_wchar_t const*)data, wsize, &wreal, tb_null)) 
            writ = wreal * sizeof(tb_wchar_t);
    }
    else
    {
        DWORD real = 0;
        if (WriteFile(stream->fp, data, (DWORD)size, &real, tb_null)) 
            writ = (tb_long_t)real;
    }
    return writ;
}
static tb_bool_t tb_stdfile_stream_sync(tb_stream_ref_t self, tb_bool_t bclosing)
{
    // check
    tb_stdfile_stream_t* stream = (tb_stdfile_stream_t*)self;
    tb_assert_and_check_return_val(stream && stream->fp, tb_false);
    tb_assert_and_check_return_val(stream->type != TB_STDFILE_TYPE_STDIN, -1);

    // only for the redirected file
    tb_check_return_val(!stream->is_console, tb_true);

    // flush buffers
    return FlushFileBuffers(stream->fp)? tb_true : tb_false;
}
static tb_long_t tb_stdfile_stream_wait(tb_stream_ref_t self, tb_size_t wait, tb_long_t timeout)
{
    return wait;
}
static tb_bool_t tb_stdfile_stream_ctrl(tb_stream_ref_t self, tb_size_t ctrl, tb_va_list_t args)
{
    return tb_false;
}
static tb_stream_ref_t tb_stdfile_stream_init(tb_size_t type, HANDLE fp)
{
    tb_stdfile_stream_t* stream = (tb_stdfile_stream_t*)tb_stream_init(     TB_STREAM_TYPE_NONE
                                                                        ,   sizeof(tb_stdfile_stream_t)
                                                                        ,   TB_STDFILE_STREAM_CACHE_MAXN
                                                                        ,   tb_stdfile_stream_open
                                                                        ,   tb_stdfile_stream_clos
                                                                        ,   tb_null
                                                                        ,   tb_stdfile_stream_ctrl
                                                                        ,   tb_stdfile_stream_wait
                                                                        ,   tb_stdfile_stream_read
                                                                        ,   tb_stdfile_stream_writ
                                                                        ,   tb_null
                                                                        ,   tb_stdfile_stream_sync
                                                                        ,   tb_null);
    tb_assert_and_check_return_val(stream, tb_null);

    stream->fp          = fp;
    stream->type        = type;
    stream->is_console  = tb_true;
    return (tb_stream_ref_t)stream;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_stdfile_ref_t tb_stdfile_init(tb_size_t type)
{
    // check
    tb_assert_and_check_return_val(type, tb_null);

    // get std handle
    HANDLE fp = tb_null;
    switch (type)
    {
    case TB_STDFILE_TYPE_STDIN:  fp = GetStdHandle(STD_INPUT_HANDLE); break;
    case TB_STDFILE_TYPE_STDOUT: fp = GetStdHandle(STD_OUTPUT_HANDLE); break;
    case TB_STDFILE_TYPE_STDERR: fp = GetStdHandle(STD_ERROR_HANDLE); break;
    }
    tb_assert_and_check_return_val(fp, tb_null);

    // create standard device file
    tb_bool_t ok = tb_false;
    tb_stdfile_t* file = tb_null;
    do
    {
        // make file
        file = tb_malloc0_type(tb_stdfile_t);
        tb_assert_and_check_break(file);

        // init type
        file->type = type;

        // init input/output stream
        if (type == TB_STDFILE_TYPE_STDIN)
        {
            file->istream = tb_stdfile_stream_init(type, fp);
            tb_assert_and_check_break(file->istream);
        }
        else
        {
            file->ostream = tb_stdfile_stream_init(type, fp);
            tb_assert_and_check_break(file->ostream);
        }

        // ok
        ok = tb_true;

    } while (0);

    // failed? exit file
    if (!ok) 
    {
        if (file) tb_stdfile_exit((tb_stdfile_ref_t)file);
        file = tb_null;
    }
    return (tb_stdfile_ref_t)file;
}
tb_void_t tb_stdfile_exit(tb_stdfile_ref_t self)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return(stdfile);

    // exit istream
    if (stdfile->istream) tb_stream_exit(stdfile->istream);
    stdfile->istream = tb_null;

    // exit ostream
    if (stdfile->ostream) tb_stream_exit(stdfile->ostream);
    stdfile->ostream = tb_null;

    // free it
    tb_free(stdfile);
}
tb_size_t tb_stdfile_type(tb_stdfile_ref_t self)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile, TB_STDFILE_TYPE_NONE);

    return stdfile->type;
}
tb_bool_t tb_stdfile_flush(tb_stdfile_ref_t self)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_stdfile_read(tb_stdfile_ref_t self, tb_byte_t* data, tb_size_t size)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_stdfile_writ(tb_stdfile_ref_t self, tb_byte_t const* data, tb_size_t size)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_stdfile_getc(tb_stdfile_ref_t self, tb_char_t* pch)
{
    return tb_stdfile_read(self, (tb_byte_t*)pch, 1);
}
tb_bool_t tb_stdfile_putc(tb_stdfile_ref_t self, tb_char_t ch)
{
    return tb_stdfile_writ(self, (tb_byte_t const*)&ch, 1);
}
tb_bool_t tb_stdfile_gets(tb_stdfile_ref_t self, tb_char_t* str, tb_size_t num)
{
    // check
    tb_assert_and_check_return_val(num && str, tb_false);

    tb_size_t i;
    tb_char_t ch;
    for (i = 0; i < num - 1; i++)
    {
        if (tb_stdfile_getc(self, &ch))
        {
            if (ch == '\n')
            {
                str[i++] = '\n';
                break;
            }
            else str[i] = ch;
        }
        else return tb_false;
    }
    if (i < num) str[i] = '\0';
    return i < num;
}
tb_bool_t tb_stdfile_puts(tb_stdfile_ref_t self, tb_char_t const* str)
{
    // check
    tb_assert_and_check_return_val(str, tb_false);

    // write string to stdout/stderr
    tb_size_t len = tb_strlen(str);
    return len? tb_stdfile_writ(self, (tb_byte_t const*)str, tb_strlen(str)) : tb_true;
}

