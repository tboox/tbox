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

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the file cache maxn
#define TB_STREAM_FILE_CACHE_MAXN             TB_FILE_DIRECT_CSIZE

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the file stream type
typedef struct __tb_stream_file_impl_t
{
    // the file handle
    tb_handle_t         file;

    // the file handle is referenced? need not exit it
    tb_size_t           bref;

    // the file mode
    tb_size_t           mode;

}tb_stream_file_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_stream_file_impl_t* tb_stream_file_impl_cast(tb_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream && tb_stream_type(stream) == TB_STREAM_TYPE_FILE, tb_null);

    // ok?
    return (tb_stream_file_impl_t*)stream;
}
static tb_bool_t tb_stream_file_impl_open(tb_stream_ref_t stream)
{
    // check
    tb_stream_file_impl_t* impl = tb_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && !impl->file, tb_false);

    // opened?
    tb_check_return_val(!impl->file, tb_true);

    // url
    tb_char_t const* url = tb_url_get(tb_stream_url(stream));
    tb_assert_and_check_return_val(url, tb_false);

    // open file
    impl->file = tb_file_init(url, impl->mode);
    
    // open file failed?
    if (!impl->file)
    {
        // save state
        tb_stream_state_set(stream, tb_file_info(url, tb_null)? TB_STATE_FILE_OPEN_FAILED : TB_STATE_FILE_NOT_EXISTS);
        return tb_false;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_stream_file_impl_clos(tb_stream_ref_t stream)
{
    // check
    tb_stream_file_impl_t* impl = tb_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // exit file
    if (!impl->bref)
    {
        if (impl->file && !tb_file_exit(impl->file)) return tb_false;
        impl->file = tb_null;
    }

    // ok
    return tb_true;
}
static tb_long_t tb_stream_file_impl_read(tb_stream_ref_t stream, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_stream_file_impl_t* impl = tb_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->file, -1);

    // check
    tb_check_return_val(data, -1);
    tb_check_return_val(size, 0);

    // read 
    return tb_file_read(impl->file, data, size);
}
static tb_long_t tb_stream_file_impl_writ(tb_stream_ref_t stream, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_stream_file_impl_t* impl = tb_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->file && data, -1);

    // check
    tb_check_return_val(size, 0);

    // writ
    return tb_file_writ(impl->file, data, size);
}
static tb_bool_t tb_stream_file_impl_sync(tb_stream_ref_t stream, tb_bool_t bclosing)
{
    // check
    tb_stream_file_impl_t* impl = tb_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->file, tb_false);

    // sync
    return tb_file_sync(impl->file);
}
static tb_bool_t tb_stream_file_impl_seek(tb_stream_ref_t stream, tb_hize_t offset)
{
    // check
    tb_stream_file_impl_t* impl = tb_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->file, tb_false);

    // seek
    return (tb_file_seek(impl->file, offset, TB_FILE_SEEK_BEG) == offset)? tb_true : tb_false;
}
static tb_long_t tb_stream_file_impl_wait(tb_stream_ref_t stream, tb_size_t wait, tb_long_t timeout)
{
    // check
    tb_stream_file_impl_t* impl = tb_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->file, -1);

    // wait 
    tb_long_t aioe = 0;
    if (!tb_stream_beof(stream))
    {
        if (wait & TB_STREAM_WAIT_READ) aioe |= TB_STREAM_WAIT_READ;
        if (wait & TB_STREAM_WAIT_WRIT) aioe |= TB_STREAM_WAIT_WRIT;
    }

    // ok?
    return aioe;
}
static tb_bool_t tb_stream_file_impl_ctrl(tb_stream_ref_t stream, tb_size_t ctrl, tb_va_list_t args)
{
    // check
    tb_stream_file_impl_t*  impl = tb_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // ctrl
    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_SIZE:
        {
            tb_hong_t* psize = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
            tb_assert_and_check_return_val(psize, tb_false);
            *psize = impl->file? tb_file_size(impl->file) : 0;
            return tb_true;
        }
    case TB_STREAM_CTRL_FILE_SET_MODE:
        {
            impl->mode = (tb_size_t)tb_va_arg(args, tb_size_t);
            return tb_true;
        }
    case TB_STREAM_CTRL_FILE_GET_MODE:
        {
            tb_size_t* pmode = (tb_size_t*)tb_va_arg(args, tb_size_t*);
            tb_assert_and_check_return_val(pmode, tb_false);
            *pmode = impl->mode;
            return tb_true;
        }
    case TB_STREAM_CTRL_FILE_SET_HANDLE:
        {
            tb_handle_t handle = (tb_handle_t)tb_va_arg(args, tb_handle_t);
            impl->file = handle;
            impl->bref = handle? 1 : 0;
            return tb_true;
        }
    case TB_STREAM_CTRL_FILE_GET_HANDLE:
        {
            tb_handle_t* phandle = (tb_handle_t*)tb_va_arg(args, tb_handle_t*);
            tb_assert_and_check_return_val(phandle, tb_false);
            *phandle = impl->file;
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
tb_stream_ref_t tb_stream_init_file()
{
    // init stream
    tb_stream_ref_t stream = tb_stream_init(    TB_STREAM_TYPE_FILE
                                            ,   sizeof(tb_stream_file_impl_t)
                                            ,   TB_STREAM_FILE_CACHE_MAXN
                                            ,   tb_stream_file_impl_open
                                            ,   tb_stream_file_impl_clos
                                            ,   tb_null
                                            ,   tb_stream_file_impl_ctrl
                                            ,   tb_stream_file_impl_wait
                                            ,   tb_stream_file_impl_read
                                            ,   tb_stream_file_impl_writ
                                            ,   tb_stream_file_impl_seek
                                            ,   tb_stream_file_impl_sync
                                            ,   tb_null);
    tb_assert_and_check_return_val(stream, tb_null);

    // init the stream impl
    tb_stream_file_impl_t* impl = tb_stream_file_impl_cast(stream);
    if (impl)
    {
        // init mode
        impl->mode = TB_FILE_MODE_RO | TB_FILE_MODE_BINARY;
    }

    // ok?
    return (tb_stream_ref_t)stream;
}
tb_stream_ref_t tb_stream_init_from_file(tb_char_t const* path, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(path, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_stream_ref_t     stream = tb_null;
    do
    {
        // init stream
        stream = tb_stream_init_file();
        tb_assert_and_check_break(stream);

        // set path
        if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_SET_URL, path)) break;
        
        // set mode
        if (mode) if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_FILE_SET_MODE, mode)) break;
    
        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream) tb_stream_exit(stream);
        stream = tb_null;
    }

    // ok
    return stream;
}
