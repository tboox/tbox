/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        file.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../stream.h"
#include "../../asio/asio.h"
#include "../../string/string.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the file cache maxn
#define TB_BASIC_STREAM_FILE_CACHE_MAXN             TB_FILE_DIRECT_CSIZE

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the file stream type
typedef struct __tb_basic_stream_file_t
{
    // the base
    tb_basic_stream_t       base;

    // the file handle
    tb_handle_t         file;

    // the file handle is referenced? need not exit it
    tb_size_t           bref;

    // the file mode
    tb_size_t           mode;

}tb_basic_stream_file_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_basic_stream_file_t* tb_basic_stream_file_cast(tb_basic_stream_t* stream)
{
    tb_assert_and_check_return_val(stream && stream->type == TB_STREAM_TYPE_FILE, tb_null);
    return (tb_basic_stream_file_t*)stream;
}
static tb_bool_t tb_basic_stream_file_open(tb_basic_stream_t* stream)
{
    // check
    tb_basic_stream_file_t* fstream = tb_basic_stream_file_cast(stream);
    tb_assert_and_check_return_val(fstream && !fstream->file, tb_false);

    // opened?
    tb_check_return_val(!fstream->file, tb_true);

    // url
    tb_char_t const* url = tb_url_get(&fstream->base.url);
    tb_assert_and_check_return_val(url, tb_false);

    // open file
    fstream->file = tb_file_init(url, fstream->mode);
    
    // open file failed?
    if (!fstream->file)
    {
        // save state
        fstream->base.state = tb_file_info(url, tb_null)? TB_STATE_FILE_OPEN_FAILED : TB_STATE_FILE_NOT_EXISTS;
        return tb_false;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_basic_stream_file_clos(tb_basic_stream_t* stream)
{
    // check
    tb_basic_stream_file_t* fstream = tb_basic_stream_file_cast(stream);
    tb_assert_and_check_return_val(fstream, tb_false);

    // exit file
    if (!fstream->bref)
    {
        if (fstream->file && !tb_file_exit(fstream->file)) return tb_false;
        fstream->file = tb_null;
    }

    // ok
    return tb_true;
}
static tb_long_t tb_basic_stream_file_read(tb_basic_stream_t* stream, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_basic_stream_file_t* fstream = tb_basic_stream_file_cast(stream);
    tb_assert_and_check_return_val(fstream && fstream->file, -1);

    // check
    tb_check_return_val(data, -1);
    tb_check_return_val(size, 0);

    // read 
    return tb_file_read(fstream->file, data, size);
}
static tb_long_t tb_basic_stream_file_writ(tb_basic_stream_t* stream, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_basic_stream_file_t* fstream = tb_basic_stream_file_cast(stream);
    tb_assert_and_check_return_val(fstream && fstream->file && data, -1);

    // check
    tb_check_return_val(size, 0);

    // writ
    return tb_file_writ(fstream->file, data, size);
}
static tb_bool_t tb_basic_stream_file_sync(tb_basic_stream_t* stream, tb_bool_t bclosing)
{
    // check
    tb_basic_stream_file_t* fstream = tb_basic_stream_file_cast(stream);
    tb_assert_and_check_return_val(fstream && fstream->file, tb_false);

    // sync
    return tb_file_sync(fstream->file);
}
static tb_bool_t tb_basic_stream_file_seek(tb_basic_stream_t* stream, tb_hize_t offset)
{
    // check
    tb_basic_stream_file_t* fstream = tb_basic_stream_file_cast(stream);
    tb_assert_and_check_return_val(fstream && fstream->file, tb_false);

    // seek
    return (tb_file_seek(fstream->file, offset, TB_FILE_SEEK_BEG) == offset)? tb_true : tb_false;
}
static tb_long_t tb_basic_stream_file_wait(tb_basic_stream_t* stream, tb_size_t wait, tb_long_t timeout)
{
    // check
    tb_basic_stream_file_t* fstream = tb_basic_stream_file_cast(stream);
    tb_assert_and_check_return_val(fstream && fstream->file, -1);

    // wait 
    tb_long_t aioe = 0;
    if (!tb_basic_stream_beof(stream))
    {
        if (wait & TB_BASIC_STREAM_WAIT_READ) aioe |= TB_BASIC_STREAM_WAIT_READ;
        if (wait & TB_BASIC_STREAM_WAIT_WRIT) aioe |= TB_BASIC_STREAM_WAIT_WRIT;
    }

    // ok?
    return aioe;
}
static tb_bool_t tb_basic_stream_file_ctrl(tb_basic_stream_t* stream, tb_size_t ctrl, tb_va_list_t args)
{
    // check
    tb_basic_stream_file_t* fstream = tb_basic_stream_file_cast(stream);
    tb_assert_and_check_return_val(fstream, tb_false);

    // ctrl
    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_SIZE:
        {
            tb_hong_t* psize = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
            tb_assert_and_check_return_val(psize, tb_false);
            *psize = fstream->file? tb_file_size(fstream->file) : 0;
            return tb_true;
        }
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // the poffset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);

            // get offset
            *poffset = fstream->base.offset;
            return tb_true;
        }
    case TB_STREAM_CTRL_FILE_SET_MODE:
        {
            fstream->mode = (tb_size_t)tb_va_arg(args, tb_size_t);
            return tb_true;
        }
    case TB_STREAM_CTRL_FILE_GET_MODE:
        {
            tb_size_t* pmode = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(pmode, tb_false);
            *pmode = fstream->mode;
            return tb_true;
        }
    case TB_STREAM_CTRL_FILE_SET_HANDLE:
        {
            tb_handle_t handle = (tb_handle_t)tb_va_arg(args, tb_handle_t);
            fstream->file = handle;
            fstream->bref = handle? 1 : 0;
            return tb_true;
        }
    case TB_STREAM_CTRL_FILE_GET_HANDLE:
        {
            tb_handle_t* phandle = (tb_handle_t*)tb_va_arg(args, tb_handle_t*);
            tb_assert_and_check_return_val(phandle, tb_false);
            *phandle = fstream->file;
            return tb_true;
        }
    default:
        break;
    }
    return tb_false;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_basic_stream_t* tb_basic_stream_init_file()
{
    // done
    tb_bool_t               ok = tb_false;
    tb_basic_stream_file_t* stream = tb_null;
    do
    {
        // make stream
        stream = (tb_basic_stream_file_t*)tb_malloc0(sizeof(tb_basic_stream_file_t));
        tb_assert_and_check_break(stream);

        // init base
        if (!tb_basic_stream_init((tb_basic_stream_t*)stream, TB_STREAM_TYPE_FILE, TB_BASIC_STREAM_FILE_CACHE_MAXN)) break;

        // init stream
        stream->base.open      = tb_basic_stream_file_open;
        stream->base.clos      = tb_basic_stream_file_clos;
        stream->base.read      = tb_basic_stream_file_read;
        stream->base.writ      = tb_basic_stream_file_writ;
        stream->base.sync      = tb_basic_stream_file_sync;
        stream->base.seek      = tb_basic_stream_file_seek;
        stream->base.wait      = tb_basic_stream_file_wait;
        stream->base.ctrl      = tb_basic_stream_file_ctrl;
        stream->file           = tb_null;
        stream->mode           = TB_FILE_MODE_RO | TB_FILE_MODE_BINARY;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream) tb_basic_stream_exit((tb_basic_stream_t*)stream);
        stream = tb_null;
    }

    // ok
    return (tb_basic_stream_t*)stream;
}
tb_basic_stream_t* tb_basic_stream_init_from_file(tb_char_t const* path, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(path, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_basic_stream_t*  stream = tb_null;
    do
    {
        // init stream
        stream = tb_basic_stream_init_file();
        tb_assert_and_check_break(stream);

        // set path
        if (!tb_basic_stream_ctrl(stream, TB_STREAM_CTRL_SET_URL, path)) break;
        
        // set mode
        if (mode) if (!tb_basic_stream_ctrl(stream, TB_STREAM_CTRL_FILE_SET_MODE, mode)) break;
    
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream) tb_basic_stream_exit(stream);
        stream = tb_null;
    }

    // ok
    return stream;
}
