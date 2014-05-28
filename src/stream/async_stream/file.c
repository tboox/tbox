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
#include "../stream.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the file cache maxn
#define TB_ASYNC_STREAM_FILE_CACHE_MAXN             TB_FILE_DIRECT_CSIZE

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the file stream type
typedef struct __tb_async_stream_file_t
{
    // the base
    tb_async_stream_t                   base;

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

}tb_async_stream_file_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_async_stream_file_t* tb_async_stream_file_cast(tb_handle_t stream)
{
    tb_async_stream_t* astream = (tb_async_stream_t*)stream;
    tb_assert_and_check_return_val(astream && astream->base.type == TB_STREAM_TYPE_FILE, tb_null);
    return (tb_async_stream_file_t*)astream;
}
static tb_void_t tb_async_stream_file_clos_clear(tb_async_stream_file_t* fstream)
{
    // check
    tb_assert_and_check_return(fstream);

    // exit it
    if (!fstream->bref && fstream->file) tb_file_exit(fstream->file);
    fstream->file = tb_null;
    fstream->bref = tb_false;

    // clear the offset
    tb_atomic64_set0(&fstream->offset);

    // clear aico
    fstream->aico = tb_null;

    // clear base
    tb_async_stream_clear(&fstream->base);
}
static tb_void_t tb_async_stream_file_clos_func(tb_handle_t aico, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast((tb_handle_t)priv);
    tb_assert_and_check_return(fstream && fstream->func.clos);

    // trace
    tb_trace_d("clos: notify: ..");

    // clear it
    tb_async_stream_file_clos_clear(fstream);

    /* done clos func
     *
     * note: cannot use this stream after closing, the stream may be exited in the closing func
     */
    fstream->func.clos(&fstream->base, TB_STATE_OK, fstream->priv);

    // trace
    tb_trace_d("clos: notify: ok");
}
static tb_bool_t tb_async_stream_file_clos_try(tb_handle_t astream)
{   
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast(astream);
    tb_assert_and_check_return_val(fstream, tb_false);

    // no aico? closed 
    if (!fstream->aico)
    {
        // clear it
        tb_async_stream_file_clos_clear(fstream);

        // ok
        return tb_true;
    }

    // failed
    return tb_false;
}
static tb_bool_t tb_async_stream_file_open_try(tb_handle_t astream)
{
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast(astream);
    tb_assert_and_check_return_val(fstream && fstream->base.aicp, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // init file
        if (!fstream->file)
        {
            // the url
            tb_char_t const* url = tb_url_get(&fstream->base.base.url);
            tb_assert_and_check_break(url);

            // open file
            fstream->file = tb_file_init(url, fstream->mode | TB_FILE_MODE_AICP);
            fstream->bref = tb_false;
            tb_check_break(fstream->file);
        }

        // addo file
        fstream->aico = tb_aico_init_file(fstream->base.aicp, fstream->file);
        tb_assert_and_check_break(fstream->aico);

        // init offset
        tb_atomic64_set0(&fstream->offset);

        // open done
        tb_async_stream_open_done(astream);

        // ok
        ok = tb_true;

    } while (0);

    // failed? clear it
    if (!ok) tb_async_stream_file_clos_clear(fstream);

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_file_open(tb_handle_t astream, tb_async_stream_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast(astream);
    tb_assert_and_check_return_val(fstream && fstream->base.aicp && func, tb_false);

    // done
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    do
    {
        // try opening it
        if (!tb_async_stream_file_open_try(astream))
        {
            // open file failed?
            if (!fstream->file)
            {
                // the url
                tb_char_t const* url = tb_url_get(&fstream->base.base.url);
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
    return tb_async_stream_open_func(astream, state, func, priv);
}
static tb_bool_t tb_async_stream_file_clos(tb_handle_t astream, tb_async_stream_clos_func_t func, tb_cpointer_t priv)
{   
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast(astream);
    tb_assert_and_check_return_val(fstream && func, tb_false);

    // trace
    tb_trace_d("clos: ..");

    // init clos
    fstream->func.clos  = func;
    fstream->priv       = priv;

    /* exit aico
     *
     * note: cannot use this stream after exiting, the stream may be exited after calling clos func
     */
    if (fstream->aico) tb_aico_exit(fstream->aico, tb_async_stream_file_clos_func, fstream);
    // done func directly
    else tb_async_stream_file_clos_func(tb_null, fstream);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_read_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_READ, tb_false);

    // the stream
    tb_async_stream_file_t* fstream = (tb_async_stream_file_t*)aice->priv;
    tb_assert_and_check_return_val(fstream && fstream->func.read, tb_false);

    // done state
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        tb_atomic64_fetch_and_add(&fstream->offset, aice->u.read.real);
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
    if (fstream->func.read((tb_async_stream_t*)fstream, state, aice->u.read.data, aice->u.read.real, aice->u.read.size, fstream->priv))
    {
        // continue?
        if (aice->state == TB_STATE_OK)
        {
            // continue to post read
            tb_aico_read(aice->aico, (tb_hize_t)tb_atomic64_get(&fstream->offset), aice->u.read.data, aice->u.read.size, tb_async_stream_file_read_func, (tb_async_stream_t*)fstream);
        }
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_read(tb_handle_t astream, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_async_stream_read_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast(astream);
    tb_assert_and_check_return_val(fstream && fstream->file && fstream->aico && data && size && func, tb_false);

    // save func and priv
    fstream->priv       = priv;
    fstream->func.read  = func;

    // post read
    return tb_aico_read_after(fstream->aico, delay, (tb_hize_t)tb_atomic64_get(&fstream->offset), data, size, tb_async_stream_file_read_func, astream);
}
static tb_bool_t tb_async_stream_file_writ_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_WRIT, tb_false);
 
    // the stream
    tb_async_stream_file_t* fstream = (tb_async_stream_file_t*)aice->priv;
    tb_assert_and_check_return_val(fstream && fstream->func.writ, tb_false);

    // done state
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        tb_assert_and_check_break(aice->u.writ.data && aice->u.writ.real <= aice->u.writ.size);
        tb_atomic64_fetch_and_add(&fstream->offset, aice->u.writ.real);
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
    if (fstream->func.writ((tb_async_stream_t*)fstream, state, aice->u.writ.data, aice->u.writ.real, aice->u.writ.size, fstream->priv))
    {
        // continue?
        if (aice->state == TB_STATE_OK && aice->u.writ.real < aice->u.writ.size)
        {
            // continue to post writ
            tb_aico_writ(aice->aico, (tb_hize_t)tb_atomic64_get(&fstream->offset), aice->u.writ.data + aice->u.writ.real, aice->u.writ.size - aice->u.writ.real, tb_async_stream_file_writ_func, (tb_async_stream_t*)fstream);
        }
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_writ(tb_handle_t astream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast(astream);
    tb_assert_and_check_return_val(fstream && fstream->file && fstream->aico && data && size && func, tb_false);

    // save func and priv
    fstream->priv       = priv;
    fstream->func.writ  = func;

    // post writ
    return tb_aico_writ_after(fstream->aico, delay, (tb_hize_t)tb_atomic64_get(&fstream->offset), data, size, tb_async_stream_file_writ_func, astream);
}
static tb_bool_t tb_async_stream_file_seek(tb_handle_t astream, tb_hize_t offset, tb_async_stream_seek_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast(astream);
    tb_assert_and_check_return_val(fstream && func, tb_false);

    // done
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    do
    {
        // check
        tb_assert_and_check_break(fstream->file);

        // update offset
        tb_atomic64_set(&fstream->offset, offset);

        // ok
        state = TB_STATE_OK;

    } while (0);

    // done func
    func(astream, state, offset, priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_sync_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_FSYNC, tb_false);

    // the stream
    tb_async_stream_file_t* fstream = (tb_async_stream_file_t*)aice->priv;
    tb_assert_and_check_return_val(fstream && fstream->func.sync, tb_false);

    // done func
    fstream->func.sync((tb_async_stream_t*)fstream, aice->state == TB_STATE_OK? TB_STATE_OK : TB_STATE_UNKNOWN_ERROR, fstream->bclosing, fstream->priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_sync(tb_handle_t astream, tb_bool_t bclosing, tb_async_stream_sync_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast(astream);
    tb_assert_and_check_return_val(fstream && fstream->file && fstream->aico && func, tb_false);

    // save func and priv
    fstream->priv       = priv;
    fstream->func.sync  = func;
    fstream->bclosing   = bclosing;

    // post sync
    return tb_aico_fsync(fstream->aico, tb_async_stream_file_sync_func, astream);
}
static tb_bool_t tb_async_stream_file_task_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the stream
    tb_async_stream_file_t* fstream = (tb_async_stream_file_t*)aice->priv;
    tb_assert_and_check_return_val(fstream && fstream->func.task, tb_false);

    // done func
    tb_bool_t ok = fstream->func.task((tb_async_stream_t*)fstream, aice->state, fstream->priv);

    // ok and continue?
    if (ok && aice->state == TB_STATE_OK)
    {
        // post task
        tb_aico_task_run(aice->aico, aice->u.runtask.delay, tb_async_stream_file_task_func, fstream);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_task(tb_handle_t astream, tb_size_t delay, tb_async_stream_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast(astream);
    tb_assert_and_check_return_val(fstream && fstream->file && fstream->aico && func, tb_false);

    // save func and priv
    fstream->priv       = priv;
    fstream->func.task  = func;

    // post task
    return tb_aico_task_run(fstream->aico, delay, tb_async_stream_file_task_func, astream);
}
static tb_void_t tb_async_stream_file_kill(tb_handle_t astream)
{   
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast(astream);
    tb_assert_and_check_return(fstream);

    // trace
    tb_trace_d("kill: ..");

    // kill it
    if (fstream->aico) tb_aico_kill(fstream->aico);
}
static tb_bool_t tb_async_stream_file_exit(tb_handle_t astream)
{   
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast(astream);
    tb_assert_and_check_return_val(fstream, tb_false);

    // aico has been not closed already?
    tb_assert_and_check_return_val(!fstream->aico, tb_false);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_file_ctrl(tb_handle_t astream, tb_size_t ctrl, tb_va_list_t args)
{
    // check
    tb_async_stream_file_t* fstream = tb_async_stream_file_cast(astream);
    tb_assert_and_check_return_val(fstream, tb_false);

    // ctrl
    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_SIZE:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_opened(astream) && fstream->file, tb_false);

            // get size
            tb_hong_t* psize = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
            tb_assert_and_check_return_val(psize, tb_false);
            *psize = tb_file_size(fstream->file);
            return tb_true;
        }
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_opened(astream) && fstream->file, tb_false);

            // get offset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);
            *poffset = (tb_hize_t)tb_atomic64_get(&fstream->offset);
            return tb_true;
        }
    case TB_STREAM_CTRL_FILE_SET_MODE:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_closed(astream), tb_false);

            // set mode
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
            // check
            tb_assert_and_check_return_val(tb_stream_is_closed(astream), tb_false);

            // exit file first if exists
            if (!fstream->bref && fstream->file) tb_file_exit(fstream->file);

            // set handle
            tb_handle_t handle = (tb_handle_t)tb_va_arg(args, tb_handle_t);
            fstream->file = handle;
            fstream->bref = handle? tb_true : tb_false;
            return tb_true;
        }
    case TB_STREAM_CTRL_FILE_GET_HANDLE:
        {
            // get handle
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
tb_async_stream_t* tb_async_stream_init_file(tb_aicp_t* aicp)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_async_stream_file_t* fstream = tb_null;
    do
    {
        // make stream
        fstream = (tb_async_stream_file_t*)tb_malloc0(sizeof(tb_async_stream_file_t));
        tb_assert_and_check_break(fstream);

        // init stream
        if (!tb_async_stream_init((tb_async_stream_t*)fstream, aicp, TB_STREAM_TYPE_FILE, TB_ASYNC_STREAM_FILE_CACHE_MAXN, TB_ASYNC_STREAM_FILE_CACHE_MAXN)) break;
        fstream->base.open      = tb_async_stream_file_open;
        fstream->base.clos      = tb_async_stream_file_clos;
        fstream->base.read      = tb_async_stream_file_read;
        fstream->base.writ      = tb_async_stream_file_writ;
        fstream->base.seek      = tb_async_stream_file_seek;
        fstream->base.sync      = tb_async_stream_file_sync;
        fstream->base.task      = tb_async_stream_file_task;
        fstream->base.exit      = tb_async_stream_file_exit;
        fstream->base.base.kill = tb_async_stream_file_kill;
        fstream->base.base.ctrl = tb_async_stream_file_ctrl;
        fstream->base.open_try  = tb_async_stream_file_open_try;
        fstream->base.clos_try  = tb_async_stream_file_clos_try;

        fstream->mode           = TB_FILE_MODE_RO | TB_FILE_MODE_BINARY;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (fstream) tb_async_stream_exit((tb_async_stream_t*)fstream);
        fstream = tb_null;
    }

    // ok
    return (tb_async_stream_t*)fstream;
}
tb_async_stream_t* tb_async_stream_init_from_file(tb_aicp_t* aicp, tb_char_t const* path, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(path, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_async_stream_t*  fstream = tb_null;
    do
    {
        // init stream
        fstream = tb_async_stream_init_file(aicp);
        tb_assert_and_check_break(fstream);

        // set path
        if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_SET_URL, path)) break;
        
        // set mode
        if (mode) if (!tb_stream_ctrl(fstream, TB_STREAM_CTRL_FILE_SET_MODE, mode)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (fstream) tb_async_stream_exit(fstream);
        fstream = tb_null;
    }

    // ok
    return fstream;
}
