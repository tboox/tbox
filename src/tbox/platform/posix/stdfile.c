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
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../stdfile.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the stdfile type
typedef struct __tb_stdfile_t
{
    // the file descriptor
    tb_int_t    fd;

    // the file type
    tb_size_t   type;

}tb_stdfile_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_stdfile_ref_t tb_stdfile_init(tb_size_t type)
{
    // check
    tb_assert_and_check_return_val(type, tb_null);

    // get std fd
    tb_int_t fd = -1;
    switch (type)
    {
    case TB_STDFILE_TYPE_STDIN:  fd = fileno(stdin); break;
    case TB_STDFILE_TYPE_STDOUT: fd = fileno(stdout); break;
    case TB_STDFILE_TYPE_STDERR: fd = fileno(stderr); break;
    }
    tb_assert_and_check_return_val(fd != -1, tb_null);

    // create standard device file
    tb_bool_t ok = tb_false;
    tb_stdfile_t* file = tb_null;
    do
    {
        // make file
        file = tb_malloc0_type(tb_stdfile_t);
        tb_assert_and_check_break(file);

        // init file
        file->fd    = fd;
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
    tb_assert_and_check_return_val(stdfile, tb_false);

#ifdef TB_CONFIG_POSIX_HAVE_FDATASYNC
    return !fdatasync(stdfile->fd)? tb_true : tb_false;
#else
    return !fsync(stdfile->fd)? tb_true : tb_false;
#endif
}
tb_long_t tb_stdfile_read(tb_stdfile_ref_t self, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && data, -1);

    // read data from stdin
    return read(stdfile->fd, data, size);
}
tb_long_t tb_stdfile_writ(tb_stdfile_ref_t self, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_stdfile_t* stdfile = (tb_stdfile_t*)self;
    tb_assert_and_check_return_val(stdfile && data, -1);

    // write data to stdout/stderr
    return write(stdfile->fd, data, size);
}
