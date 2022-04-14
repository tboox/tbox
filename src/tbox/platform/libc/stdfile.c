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
 * includes
 */
#include "prefix.h"
#include "../stdfile.h"
#include <stdio.h>
#include <stdlib.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the stdfile type
typedef struct __tb_stdfile_t
{
    // the file type
    tb_size_t   type;

    // the file pointer
    FILE*       fp;

}tb_stdfile_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_stdfile_ref_t tb_stdfile_init(tb_size_t type)
{
    // check
    tb_assert_and_check_return_val(type, tb_null);

    // get std pointer
    FILE* fp = tb_null;
    switch (type)
    {
    case TB_STDFILE_TYPE_STDIN:  fp = stdin; break;
    case TB_STDFILE_TYPE_STDOUT: fp = stdout; break;
    case TB_STDFILE_TYPE_STDERR: fp = stderr; break;
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

        // init file
        file->fp    = fp;
        file->type  = type;

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
    tb_assert_and_check_return_val(stdfile && stdfile->fp, tb_false);
    tb_assert_and_check_return_val(stdfile->type != TB_STDFILE_TYPE_STDIN, tb_false);

    return !fflush(stdfile->fp)? tb_true : tb_false;
}
tb_bool_t tb_stdfile_read(tb_stdfile_ref_t self, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->fp && data, tb_false);
    tb_assert_and_check_return_val(stdfile->type == TB_STDFILE_TYPE_STDIN, tb_false);

    // read data from stdin
    return fread(data, size, 1, stdfile->fp) == 1;
}
tb_bool_t tb_stdfile_writ(tb_stdfile_ref_t self, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->fp && data, tb_false);
    tb_assert_and_check_return_val(stdfile->type != TB_STDFILE_TYPE_STDIN, tb_false);

    // write data to stdout/stderr
    return fwrite(data, size, 1, stdfile->fp) == 1;
}
#if defined(TB_CONFIG_LIBC_HAVE_FGETC) && defined(TB_CONFIG_LIBC_HAVE_UNGETC)
tb_bool_t tb_stdfile_peek(tb_stdfile_ref_t self, tb_char_t* pch)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->fp && pch, tb_false);
    tb_assert_and_check_return_val(stdfile->type == TB_STDFILE_TYPE_STDIN, tb_false);

    // read character from stdin
    tb_int_t ch = fgetc(stdfile->fp);
    tb_check_return_val(ch != EOF, tb_false);

    // unread character from stdin
    ungetc(ch, stdfile->fp);

    // save result
    *pch = (tb_char_t)ch;
    return tb_true;
}
#else
tb_bool_t tb_stdfile_peek(tb_stdfile_ref_t self, tb_char_t* pch)
{
    tb_trace_noimpl();
    return tb_false;
}
#endif
#ifdef TB_CONFIG_LIBC_HAVE_FGETC
tb_bool_t tb_stdfile_getc(tb_stdfile_ref_t self, tb_char_t* pch)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->fp && pch, tb_false);
    tb_assert_and_check_return_val(stdfile->type == TB_STDFILE_TYPE_STDIN, tb_false);

    // read character from stdin
    tb_int_t ch = fgetc(stdfile->fp);
    tb_check_return_val(ch != EOF, tb_false);

    // save result
    *pch = (tb_char_t)ch;
    return tb_true;
}
#else
tb_bool_t tb_stdfile_getc(tb_stdfile_ref_t self, tb_char_t* pch)
{
    return tb_stdfile_read(self, (tb_byte_t*)pch, 1);
}
#endif
#ifdef TB_CONFIG_LIBC_HAVE_FPUTC
tb_bool_t tb_stdfile_putc(tb_stdfile_ref_t self, tb_char_t ch)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->fp, tb_false);
    tb_assert_and_check_return_val(stdfile->type != TB_STDFILE_TYPE_STDIN, tb_false);

    // write character to stdout/stderr
    return fputc((tb_int_t)ch, stdfile->fp) == ch;
}
#else
tb_bool_t tb_stdfile_putc(tb_stdfile_ref_t self, tb_char_t ch)
{
    return tb_stdfile_writ(self, (tb_byte_t const*)&ch, 1);
}
#endif
#ifdef TB_CONFIG_LIBC_HAVE_FGETS
tb_bool_t tb_stdfile_gets(tb_stdfile_ref_t self, tb_char_t* str, tb_size_t num)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->fp && str && num, tb_false);
    tb_assert_and_check_return_val(stdfile->type == TB_STDFILE_TYPE_STDIN, tb_false);

    // read string from stdin
    return fgets(str, (tb_int_t)num, stdfile->fp) == str;
}
#else
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
#endif
#ifdef TB_CONFIG_LIBC_HAVE_FPUTS
tb_bool_t tb_stdfile_puts(tb_stdfile_ref_t self, tb_char_t const* str)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && stdfile->fp && str, tb_false);
    tb_assert_and_check_return_val(stdfile->type != TB_STDFILE_TYPE_STDIN, tb_false);

    // write string to stdout/stderr
    return fputs(str, stdfile->fp) >= 0;
}
#else
tb_bool_t tb_stdfile_puts(tb_stdfile_ref_t self, tb_char_t const* str)
{
    // check
    tb_assert_and_check_return_val(str, tb_false);

    // write string to stdout/stderr
    tb_size_t len = tb_strlen(str);
    return len? tb_stdfile_writ(self, (tb_byte_t const*)str, tb_strlen(str)) : tb_true;
}
#endif

