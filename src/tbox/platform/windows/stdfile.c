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
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../mutex.h"
#include "../stdfile.h"
#include "../../stream/stream.h"
#include "../../charset/charset.h"
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

    // the input filter stream
    tb_stream_ref_t     ifstream;

    // the output stream
    tb_stream_ref_t     ostream;

    // the output filter stream
    tb_stream_ref_t     ofstream;

    // the output cache
    tb_buffer_t         ocache;

    // the mutex lock
    tb_mutex_ref_t      mutex;

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
        DWORD wsize = (DWORD)(size / sizeof(tb_wchar_t));
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
        DWORD wsize = (DWORD)(size / sizeof(tb_wchar_t));
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
static tb_stream_ref_t tb_stdfile_stream_init(tb_size_t type, HANDLE fp, tb_bool_t is_console)
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
    stream->is_console  = is_console;
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
    tb_check_return_val(fp, tb_null);

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

        // init mutex
        file->mutex = tb_mutex_init();
        tb_assert_and_check_break(file->mutex);

        // check wchar
        tb_assert_static(sizeof(tb_wchar_t) == 2);

        // get console mode
        DWORD       real = 0;
        tb_bool_t   is_console = tb_true;
        if (!GetConsoleMode(fp, &real))
            is_console = tb_false;

        // init encoding for console mode
#if TB_CONFIG_FORCE_UTF8
        tb_size_t encoding = TB_CHARSET_TYPE_UTF8;
#else
        tb_size_t encoding = TB_CHARSET_TYPE_ANSI;
#endif

        // init input/output stream
        if (type == TB_STDFILE_TYPE_STDIN)
        {
            // init stream
            file->istream = tb_stdfile_stream_init(type, fp, is_console);
            tb_assert_and_check_break(file->istream);

            if (is_console)
            {
                file->ifstream = tb_stream_init_filter_from_charset(file->istream, TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_LE, encoding);
                tb_assert_and_check_break(file->ifstream);
            }
            else file->ifstream = file->istream;

            // open stream
            if (!tb_stream_open(file->ifstream)) break;
        }
        else
        {
            // init stream
            file->ostream = tb_stdfile_stream_init(type, fp, is_console);
            tb_assert_and_check_break(file->ostream);

            if (is_console)
            {
                file->ofstream = tb_stream_init_filter_from_charset(file->ostream, encoding, TB_CHARSET_TYPE_UTF16 | TB_CHARSET_TYPE_LE);
                tb_assert_and_check_break(file->ofstream);
            }
            else
            {
                // redirect (console output cp)
                file->ofstream = tb_stream_init_filter_from_charset(file->ostream, encoding, TB_CHARSET_TYPE_COCP);
                tb_assert_and_check_break(file->ofstream);
            }

            // open stream
            if (!tb_stream_open(file->ofstream)) break;
        }

        // init output cache
        if (!tb_buffer_init(&file->ocache)) break;

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

    // flush the left data first
    if (stdfile->type != TB_STDFILE_TYPE_STDIN)
        tb_stdfile_flush(self);

    // enter mutex
    if (stdfile->mutex) tb_mutex_enter(stdfile->mutex);

    // exit ifstream
    if (stdfile->ifstream != stdfile->istream) tb_stream_exit(stdfile->ifstream);
    stdfile->ifstream = tb_null;

    // exit ofstream
    if (stdfile->ofstream != stdfile->ostream) tb_stream_exit(stdfile->ofstream);
    stdfile->ofstream = tb_null;

    // exit istream
    if (stdfile->istream) tb_stream_exit(stdfile->istream);
    stdfile->istream = tb_null;

    // exit ostream
    if (stdfile->ostream) tb_stream_exit(stdfile->ostream);
    stdfile->ostream = tb_null;

    // exit ocache
    tb_buffer_exit(&stdfile->ocache);

    // leave mutex
    if (stdfile->mutex) tb_mutex_leave(stdfile->mutex);

    // exit mutex
    if (stdfile->mutex) tb_mutex_exit(stdfile->mutex);
    stdfile->mutex = tb_null;

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
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->ofstream && stdfile->mutex, tb_false);
    tb_assert_and_check_return_val(stdfile->type != TB_STDFILE_TYPE_STDIN, tb_false);

    // enter mutex
    if (!tb_mutex_enter(stdfile->mutex)) return tb_false;

    // write data
    tb_bool_t ok = tb_false;
    do
    {
        // write cached data first
        tb_byte_t const* odata = tb_buffer_data(&stdfile->ocache);
        tb_size_t        osize = tb_buffer_size(&stdfile->ocache);
        if (odata && osize)
        {
            if (!tb_stream_bwrit(stdfile->ofstream, odata, osize)) break;
            tb_buffer_clear(&stdfile->ocache);
        }

        // flush stream
        ok = tb_stream_sync(stdfile->ofstream, tb_false);

    } while (0);

    // leave mutex
    tb_mutex_leave(stdfile->mutex);
    return ok;
}
tb_bool_t tb_stdfile_read(tb_stdfile_ref_t self, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->ifstream && stdfile->mutex && data, tb_false);
    tb_assert_and_check_return_val(stdfile->type == TB_STDFILE_TYPE_STDIN, tb_false);
    tb_check_return_val(size, tb_true);

    tb_bool_t ok = tb_false;
    if (tb_mutex_enter(stdfile->mutex))
    {
        ok = tb_stream_bread(stdfile->ifstream, data, size);
        tb_mutex_leave(stdfile->mutex);
    }
    return ok;
}
tb_bool_t tb_stdfile_writ(tb_stdfile_ref_t self, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->ofstream && stdfile->mutex && data, tb_false);
    tb_assert_and_check_return_val(stdfile->type != TB_STDFILE_TYPE_STDIN, tb_false);
    tb_check_return_val(size, tb_true);

    // enter mutex
    if (!tb_mutex_enter(stdfile->mutex)) return tb_false;

    // write data
    tb_bool_t ok = tb_false;
    do
    {
        // write cached data first
        tb_byte_t const* odata = tb_buffer_data(&stdfile->ocache);
        tb_size_t        osize = tb_buffer_size(&stdfile->ocache);
        if (odata && osize)
        {
            if (!tb_stream_bwrit(stdfile->ofstream, odata, osize)) break;
            tb_buffer_clear(&stdfile->ocache);
        }

        // write data by lines
        tb_char_t const* p = (tb_char_t const*)data;
        tb_char_t const* e = p + size;
        tb_char_t const* lf = tb_null;
        while (p < e)
        {
            lf = tb_strnchr(p, e - p, '\n');
            if (lf)
            {
                if (lf > p && lf[-1] == '\r')
                {
                    if (!tb_stream_bwrit(stdfile->ofstream, (tb_byte_t const*)p, lf + 1 - p)) break;
                }
                else
                {
                    if (lf > p && !tb_stream_bwrit(stdfile->ofstream, (tb_byte_t const*)p, lf - p)) break;
                    if (!tb_stream_bwrit(stdfile->ofstream, (tb_byte_t const*)"\r\n", 2)) break;
                }

                // next line
                p = lf + 1;

                // flush data
                if (!tb_stream_sync(stdfile->ofstream, tb_false)) break;
            }
            else
            {
                // cache the left data
                tb_buffer_memncat(&stdfile->ocache, (tb_byte_t const*)p, e - p);
                p = e;
                break;
            }
        }
        tb_check_break(p == e);

        // ok
        ok = tb_true;

    } while (0);

    // leave mutex
    tb_mutex_leave(stdfile->mutex);
    return ok;
}
tb_bool_t tb_stdfile_peek(tb_stdfile_ref_t self, tb_char_t* pch)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->ifstream && pch, tb_false);
    tb_assert_and_check_return_val(stdfile->type == TB_STDFILE_TYPE_STDIN, tb_false);

    tb_bool_t ok = tb_false;
    if (tb_mutex_enter(stdfile->mutex))
    {
        tb_byte_t* data = tb_null;
        if (tb_stream_need(stdfile->ifstream, &data, 1) && data)
        {
            *pch = (tb_char_t)*data;
            ok = tb_true;
        }
        tb_mutex_leave(stdfile->mutex);
    }
    return ok;
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

