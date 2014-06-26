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
 * trace
 */
#define TB_TRACE_MODULE_NAME                "async_stream_file"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the file cache maxn
#define TB_ASYNC_STREAM_FILE_CACHE_MAXN             TB_FILE_DIRECT_CSIZE

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the file stream type
typedef struct __tb_async_stream_file_impl_t
{
    // the file handle
    tb_handle_t                         file;

    // the aico
    tb_handle_t                         aico;

    // the file handle is referenced? need not exit it
    tb_bool_t                           bref;

    // the file mode
    tb_size_t                           mode;

    // the file offset
    tb_atomic64_t                       offset;

    // is closing
    tb_bool_t                           bclosing;

    // the func
    union
    {
        tb_async_stream_read_func_t     read;
        tb_async_stream_writ_func_t     writ;
        tb_async_stream_sync_func_t     sync;
        tb_async_stream_task_func_t     task;
        tb_async_stream_clos_func_t     clos;

    }                                   func;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_stream_file_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_async_stream_file_impl_t* tb_async_stream_file_impl_cast(tb_async_stream_ref_t stream)
{
    // check
    tb_assert_and_check_return_val(stream && tb_async_stream_type(stream) == TB_STREAM_TYPE_FILE, tb_null);

    // ok?
    return (tb_async_stream_file_impl_t*)stream;
}
static tb_void_t tb_async_stream_file_impl_clos_clear(tb_async_stream_file_impl_t* impl)
{
    // check
    tb_assert_and_check_return(impl);

    // exit it
    if (!impl->bref && impl->file) tb_file_exit(impl->file);
    impl->file = tb_null;
    impl->bref = tb_false;

    // clear the offset
    tb_atomic64_set0(&impl->offset);

    // clear aico
    impl->aico = tb_null;

    // clear base
    tb_async_stream_clear((tb_async_stream_ref_t)impl);
}
static tb_void_t tb_async_stream_file_impl_clos_func(tb_handle_t aico, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast((tb_async_stream_ref_t)priv);
    tb_assert_and_check_return(impl && impl->func.clos);

    // trace
    tb_trace_d("clos: notify: ..");

    // clear it
    tb_async_stream_file_impl_clos_clear(impl);

    /* done clos func
     *
     * note: cannot use this stream after closing, the stream may be exited in the closing func
     */
    impl->func.clos((tb_async_stream_ref_t)impl, TB_STATE_OK, impl->priv);

    // trace
    tb_trace_d("clos: notify: ok");
}
static tb_bool_t tb_async_stream_file_impl_clos_try(tb_async_stream_ref_t stream)
{   
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // no aico? closed 
    if (!impl->aico)
    {
        // clear it
        tb_async_stream_file_impl_clos_clear(impl);

        // ok
        return tb_true;
    }

    // failed
    return tb_false;
}
static tb_bool_t tb_async_stream_file_impl_open_try(tb_async_stream_ref_t stream)
{
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && !impl->aico, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // init file
        if (!impl->file)
        {
            // the url
            tb_char_t const* url = tb_url_get(tb_async_stream_url(stream));
            tb_assert_and_check_break(url);

            // open file
            impl->file = tb_file_init(url, impl->mode | TB_FILE_MODE_ASIO);
            impl->bref = tb_false;
            tb_check_break(impl->file);
        }

        // the aicp
        tb_aicp_ref_t aicp = tb_async_stream_aicp(stream);
        tb_assert_and_check_break(aicp);

        // addo file
        impl->aico = tb_aico_init_file(aicp, impl->file);
        tb_assert_and_check_break(impl->aico);

        // killed?
        tb_check_break(!tb_async_stream_is_killed(stream));

        // init offset
        tb_atomic64_set0(&impl->offset);

        // open done
        tb_async_stream_open_done(stream);

        // ok
        ok = tb_true;

    } while (0);

    // failed? clear it
    if (!ok) tb_async_stream_file_impl_clos_clear(impl);

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_file_impl_open(tb_async_stream_ref_t stream, tb_async_stream_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && func, tb_false);

    // done
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    do
    {
        // try opening it
        if (!tb_async_stream_file_impl_open_try(stream))
        {
            // killed?
            if (tb_async_stream_is_killed(stream))
            {
                // save state
                state = TB_STATE_KILLED;
                break;
            }

            // open file failed?
            if (!impl->file)
            {
                // the url
                tb_char_t const* url = tb_url_get(tb_async_stream_url(stream));
                tb_assert_and_check_break(url);

                // trace
                tb_trace_e("open %s: failed", url);

                // save state
                state = tb_file_info(url, tb_null)? TB_STATE_FILE_OPEN_FAILED : TB_STATE_FILE_NOT_EXISTS;
            }
            break;
        }

        // ok
        state = TB_STATE_OK;

    } while (0);

    // open done
    return tb_async_stream_open_func(stream, state, func, priv);
}
static tb_bool_t tb_async_stream_file_impl_clos(tb_async_stream_ref_t stream, tb_async_stream_clos_func_t func, tb_cpointer_t priv)
{   
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && func, tb_false);

    // trace
    tb_trace_d("clos: ..");

    // init clos
    impl->func.clos  = func;
    impl->priv       = priv;

    /* exit aico
     *
     * note: cannot use this stream after exiting, the stream may be exited after calling clos func
     */
    if (impl->aico) tb_aico_exit(impl->aico, tb_async_stream_file_impl_clos_func, impl);
    // done func directly
    else tb_async_stream_file_impl_clos_func(tb_null, impl);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_impl_read_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_READ, tb_false);

    // the stream
    tb_async_stream_file_impl_t* impl = (tb_async_stream_file_impl_t*)aice->priv;
    tb_assert_and_check_return_val(impl && impl->func.read, tb_false);

    // done state
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        tb_atomic64_fetch_and_add(&impl->offset, aice->u.read.real);
        state = TB_STATE_OK;
        break;
        // closed
    case TB_STATE_CLOSED:
        state = TB_STATE_CLOSED;
        break;
        // killed
    case TB_STATE_KILLED:
        state = TB_STATE_KILLED;
        break;
    default:
        tb_trace_d("read: unknown state: %s", tb_state_cstr(aice->state));
        break;
    }
 
    // done func
    if (impl->func.read((tb_async_stream_ref_t)impl, state, aice->u.read.data, aice->u.read.real, aice->u.read.size, impl->priv))
    {
        // continue?
        if (aice->state == TB_STATE_OK)
        {
            // continue to post read
            tb_aico_read(aice->aico, (tb_hize_t)tb_atomic64_get(&impl->offset), aice->u.read.data, aice->u.read.size, tb_async_stream_file_impl_read_func, (tb_async_stream_ref_t)impl);
        }
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_impl_read(tb_async_stream_ref_t stream, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_async_stream_read_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->file && impl->aico && data && size && func, tb_false);

    // save func and priv
    impl->priv       = priv;
    impl->func.read  = func;

    // post read
    return tb_aico_read_after(impl->aico, delay, (tb_hize_t)tb_atomic64_get(&impl->offset), data, size, tb_async_stream_file_impl_read_func, stream);
}
static tb_bool_t tb_async_stream_file_impl_writ_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_WRIT, tb_false);
 
    // the stream
    tb_async_stream_file_impl_t* impl = (tb_async_stream_file_impl_t*)aice->priv;
    tb_assert_and_check_return_val(impl && impl->func.writ, tb_false);

    // done state
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        tb_assert_and_check_break(aice->u.writ.data && aice->u.writ.real <= aice->u.writ.size);
        tb_atomic64_fetch_and_add(&impl->offset, aice->u.writ.real);
        state = TB_STATE_OK;
        break;
        // closed
    case TB_STATE_CLOSED:
        state = TB_STATE_CLOSED;
        break;
        // killed
    case TB_STATE_KILLED:
        state = TB_STATE_KILLED;
        break;
    default:
        tb_trace_d("writ: unknown state: %s", tb_state_cstr(aice->state));
        break;
    }

    // done func
    if (impl->func.writ((tb_async_stream_ref_t)impl, state, aice->u.writ.data, aice->u.writ.real, aice->u.writ.size, impl->priv))
    {
        // continue?
        if (aice->state == TB_STATE_OK && aice->u.writ.real < aice->u.writ.size)
        {
            // continue to post writ
            tb_aico_writ(aice->aico, (tb_hize_t)tb_atomic64_get(&impl->offset), aice->u.writ.data + aice->u.writ.real, aice->u.writ.size - aice->u.writ.real, tb_async_stream_file_impl_writ_func, (tb_async_stream_ref_t)impl);
        }
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_impl_writ(tb_async_stream_ref_t stream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->file && impl->aico && data && size && func, tb_false);

    // save func and priv
    impl->priv       = priv;
    impl->func.writ  = func;

    // post writ
    return tb_aico_writ_after(impl->aico, delay, (tb_hize_t)tb_atomic64_get(&impl->offset), data, size, tb_async_stream_file_impl_writ_func, stream);
}
static tb_bool_t tb_async_stream_file_impl_seek(tb_async_stream_ref_t stream, tb_hize_t offset, tb_async_stream_seek_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && func, tb_false);

    // done
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    do
    {
        // check
        tb_assert_and_check_break(impl->file);

        // update offset
        tb_atomic64_set(&impl->offset, offset);

        // ok
        state = TB_STATE_OK;

    } while (0);

    // done func
    func(stream, state, offset, priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_impl_sync_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_FSYNC, tb_false);

    // the stream
    tb_async_stream_file_impl_t* impl = (tb_async_stream_file_impl_t*)aice->priv;
    tb_assert_and_check_return_val(impl && impl->func.sync, tb_false);

    // done func
    impl->func.sync((tb_async_stream_ref_t)impl, aice->state == TB_STATE_OK? TB_STATE_OK : TB_STATE_UNKNOWN_ERROR, impl->bclosing, impl->priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_impl_sync(tb_async_stream_ref_t stream, tb_bool_t bclosing, tb_async_stream_sync_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->file && impl->aico && func, tb_false);

    // save func and priv
    impl->priv       = priv;
    impl->func.sync  = func;
    impl->bclosing   = bclosing;

    // post sync
    return tb_aico_fsync(impl->aico, tb_async_stream_file_impl_sync_func, stream);
}
static tb_bool_t tb_async_stream_file_impl_task_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the stream
    tb_async_stream_file_impl_t* impl = (tb_async_stream_file_impl_t*)aice->priv;
    tb_assert_and_check_return_val(impl && impl->func.task, tb_false);

    // done func
    tb_bool_t ok = impl->func.task((tb_async_stream_ref_t)impl, aice->state, impl->priv);

    // ok and continue?
    if (ok && aice->state == TB_STATE_OK)
    {
        // post task
        tb_aico_task_run(aice->aico, aice->u.runtask.delay, tb_async_stream_file_impl_task_func, impl);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_impl_task(tb_async_stream_ref_t stream, tb_size_t delay, tb_async_stream_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->file && impl->aico && func, tb_false);

    // save func and priv
    impl->priv       = priv;
    impl->func.task  = func;

    // post task
    return tb_aico_task_run(impl->aico, delay, tb_async_stream_file_impl_task_func, stream);
}
static tb_void_t tb_async_stream_file_impl_kill(tb_async_stream_ref_t stream)
{   
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    tb_assert_and_check_return(impl);

    // trace
    tb_trace_d("kill: %s: ..", tb_url_get(tb_async_stream_url(stream)));

    // kill it
    if (impl->aico) tb_aico_kill(impl->aico);
}
static tb_bool_t tb_async_stream_file_impl_exit(tb_async_stream_ref_t stream)
{   
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // aico has been not closed already?
    tb_assert_and_check_return_val(!impl->aico, tb_false);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_impl_ctrl(tb_async_stream_ref_t stream, tb_size_t ctrl, tb_va_list_t args)
{
    // check
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // ctrl
    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_SIZE:
        {
            // check
            tb_assert_and_check_return_val(tb_async_stream_is_opened(stream) && impl->file, tb_false);

            // get size
            tb_hong_t* psize = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
            tb_assert_and_check_return_val(psize, tb_false);
            *psize = tb_file_size(impl->file);
            return tb_true;
        }
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // check
            tb_assert_and_check_return_val(tb_async_stream_is_opened(stream) && impl->file, tb_false);

            // get offset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);
            *poffset = (tb_hize_t)tb_atomic64_get(&impl->offset);
            return tb_true;
        }
    case TB_STREAM_CTRL_FILE_SET_MODE:
        {
            // check
            tb_assert_and_check_return_val(tb_async_stream_is_closed(stream), tb_false);

            // set mode
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
            // check
            tb_assert_and_check_return_val(tb_async_stream_is_closed(stream), tb_false);

            // exit file first if exists
            if (!impl->bref && impl->file) tb_file_exit(impl->file);

            // set handle
            tb_handle_t handle = (tb_handle_t)tb_va_arg(args, tb_handle_t);
            impl->file = handle;
            impl->bref = handle? tb_true : tb_false;
            return tb_true;
        }
    case TB_STREAM_CTRL_FILE_GET_HANDLE:
        {
            // get handle
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
tb_async_stream_ref_t tb_async_stream_init_file(tb_aicp_ref_t aicp)
{
    // init stream
    tb_async_stream_ref_t stream = tb_async_stream_init(    aicp
                                                        ,   TB_STREAM_TYPE_FILE
                                                        ,   sizeof(tb_async_stream_file_impl_t)
                                                        ,   TB_ASYNC_STREAM_FILE_CACHE_MAXN
                                                        ,   TB_ASYNC_STREAM_FILE_CACHE_MAXN
                                                        ,   tb_async_stream_file_impl_open_try
                                                        ,   tb_async_stream_file_impl_clos_try
                                                        ,   tb_async_stream_file_impl_open
                                                        ,   tb_async_stream_file_impl_clos
                                                        ,   tb_async_stream_file_impl_exit
                                                        ,   tb_async_stream_file_impl_kill
                                                        ,   tb_async_stream_file_impl_ctrl
                                                        ,   tb_async_stream_file_impl_read
                                                        ,   tb_async_stream_file_impl_writ
                                                        ,   tb_async_stream_file_impl_seek
                                                        ,   tb_async_stream_file_impl_sync
                                                        ,   tb_async_stream_file_impl_task);
    tb_assert_and_check_return_val(stream, tb_null);

    // init the stream impl
    tb_async_stream_file_impl_t* impl = tb_async_stream_file_impl_cast(stream);
    if (impl)
    {
        // init mode
        impl->mode = TB_FILE_MODE_RO | TB_FILE_MODE_BINARY;
    }

    // ok?
    return stream;
}
tb_async_stream_ref_t tb_async_stream_init_from_file(tb_aicp_ref_t aicp, tb_char_t const* path, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(path, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    tb_async_stream_ref_t   stream = tb_null;
    do
    {
        // init stream
        stream = tb_async_stream_init_file(aicp);
        tb_assert_and_check_break(stream);

        // set path
        if (!tb_async_stream_ctrl(stream, TB_STREAM_CTRL_SET_URL, path)) break;
        
        // set mode
        if (stream) if (!tb_async_stream_ctrl(stream, TB_STREAM_CTRL_FILE_SET_MODE, mode)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (stream) tb_async_stream_exit(stream);
        stream = tb_null;
    }

    // ok
    return stream;
}
