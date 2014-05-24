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
 * @data        data.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "async_stream_data"
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../utils/utils.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the data read type
typedef struct __tb_async_stream_data_read_t
{
    // the func
    tb_async_stream_read_func_t         func;

    // the size
    tb_size_t                           size;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_stream_data_read_t;

// the data writ type
typedef struct __tb_async_stream_data_writ_t
{
    // the func
    tb_async_stream_writ_func_t         func;

    // the data
    tb_byte_t const*                    data;

    // the size
    tb_size_t                           size;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_stream_data_writ_t;

// the data task type
typedef struct __tb_async_stream_data_task_t
{
    // the func
    tb_async_stream_task_func_t         func;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_stream_data_task_t;

// the data clos type
typedef struct __tb_async_stream_data_clos_t
{
    // the func
    tb_async_stream_clos_func_t         func;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_stream_data_clos_t;

// the data stream type
typedef struct __tb_async_stream_data_t
{
    // the base
    tb_async_stream_t                   base;

    // the aico for task
    tb_handle_t                         aico;

    // the data
    tb_byte_t*                          data;

    // the head
    tb_byte_t*                          head;

    // the size
    tb_size_t                           size;

    // the data is referenced?
    tb_bool_t                           bref;

    // the offset
    tb_atomic64_t                       offset;

    // the func
    union
    {
        tb_async_stream_data_read_t     read;
        tb_async_stream_data_writ_t     writ;
        tb_async_stream_data_task_t     task;
        tb_async_stream_data_clos_t     clos;

    }                                   func;

}tb_async_stream_data_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_async_stream_data_t* tb_async_stream_data_cast(tb_handle_t stream)
{
    tb_async_stream_t* astream = (tb_async_stream_t*)stream;
    tb_assert_and_check_return_val(astream && astream->base.type == TB_STREAM_TYPE_DATA, tb_null);
    return (tb_async_stream_data_t*)astream;
}
static tb_bool_t tb_async_stream_data_open(tb_handle_t astream, tb_async_stream_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_data_t* dstream = tb_async_stream_data_cast(astream);
    tb_assert_and_check_return_val(dstream, tb_false);

    // done
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    do
    {
        // check
        tb_assert_and_check_break(dstream->data && dstream->size);
        
        // init aico
        if (!dstream->aico) dstream->aico = tb_aico_init_task(dstream->base.aicp, tb_false);
        tb_assert_and_check_break(dstream->aico);

        // init head
        dstream->head = dstream->data;

        // init offset
        tb_atomic64_set0(&dstream->offset);

        // opened
        tb_atomic_set(&dstream->base.base.istate, TB_STATE_OPENED);

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed? restore state
    if (state != TB_STATE_OK) tb_atomic_set(&dstream->base.base.istate, TB_STATE_CLOSED);

    // done func
    if (func) func(astream, state, priv);

    // ok?
    return func? tb_true : ((state == TB_STATE_OK)? tb_true : tb_false);
}
static tb_void_t tb_async_stream_data_clos_func(tb_handle_t aico, tb_cpointer_t priv)
{
    // check
    tb_async_stream_data_t* dstream = tb_async_stream_data_cast((tb_handle_t)priv);
    tb_assert_and_check_return(dstream && dstream->func.clos.func);

    // trace
    tb_trace_d("clos: notify: ..");

    // clear aico
    dstream->aico = tb_null;

    // clear head
    dstream->head = tb_null;

    // clear offset
    tb_atomic64_set0(&dstream->offset);

    // clear base
    tb_async_stream_clear(&dstream->base);

    /* done clos func
     *
     * note: cannot use this stream after closing, the stream may be exited in the closing func
     */
    dstream->func.clos.func(&dstream->base, TB_STATE_OK, dstream->func.clos.priv);

    // trace
    tb_trace_d("clos: notify: ok");
}
static tb_bool_t tb_async_stream_data_clos(tb_handle_t astream, tb_async_stream_clos_func_t func, tb_cpointer_t priv)
{   
    // check
    tb_async_stream_data_t* dstream = tb_async_stream_data_cast(astream);
    tb_assert_and_check_return_val(dstream && dstream->aico && func, tb_false);

    // trace
    tb_trace_d("clos: ..");

    // init func
    dstream->func.clos.func = func;
    dstream->func.clos.priv = priv;

    // exit it
    tb_aico_exit(dstream->aico, tb_async_stream_data_clos_func, dstream);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_data_read_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the stream
    tb_async_stream_data_t* dstream = (tb_async_stream_data_t*)aice->priv;
    tb_assert_and_check_return_val(dstream && dstream->func.read.func, tb_false);

    // done state
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        {
            // check
            tb_assert_and_check_break(dstream->data && dstream->head);
            
            // the left
            tb_size_t left = dstream->data + dstream->size - dstream->head;

            // the real
            tb_size_t real = dstream->func.read.size;
            if (!real || real > left) real = left;

            // no data? closed
            if (!real) 
            {
                state = TB_STATE_CLOSED;
                break;
            }

            // save data
            tb_byte_t* data = dstream->head;

            // save head
            dstream->head += real;

            // save offset
            tb_atomic64_set(&dstream->offset, dstream->head - dstream->data);

            // ok
            state = TB_STATE_OK;

            // done func
            if (dstream->func.read.func((tb_async_stream_t*)dstream, state, data, real, dstream->func.read.size, dstream->func.read.priv))
            {
                // continue to post read
                tb_aico_task_run(aice->aico, 0, tb_async_stream_data_read_func, (tb_async_stream_t*)dstream);
            }
        }
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
    if (state != TB_STATE_OK) dstream->func.read.func((tb_async_stream_t*)dstream, state, tb_null, 0, dstream->func.read.size, dstream->func.read.priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_data_read(tb_handle_t astream, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_async_stream_read_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_data_t* dstream = tb_async_stream_data_cast(astream);
    tb_assert_and_check_return_val(dstream && dstream->aico && func, tb_false);

    // save func and priv
    dstream->func.read.priv     = priv;
    dstream->func.read.func     = func;
    dstream->func.read.size     = size;

    // post read
    return tb_aico_task_run(dstream->aico, delay, tb_async_stream_data_read_func, astream);
}
static tb_bool_t tb_async_stream_data_writ_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the stream
    tb_async_stream_data_t* dstream = (tb_async_stream_data_t*)aice->priv;
    tb_assert_and_check_return_val(dstream && dstream->func.writ.func, tb_false);

    // done state
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        {
            // check
            tb_assert_and_check_break(dstream->data && dstream->head);
            tb_assert_and_check_break(dstream->func.writ.data && dstream->func.writ.size);
    
            // the left
            tb_size_t left = dstream->data + dstream->size - dstream->head;

            // the real
            tb_size_t real = dstream->func.writ.size;
            if (real > left) real = left;

            // no data? closed
            if (!real) 
            {
                state = TB_STATE_CLOSED;
                break;
            }

            // save data
            tb_memcpy(dstream->head, dstream->func.writ.data, real);

            // save head
            dstream->head += real;

            // save offset
            tb_atomic64_set(&dstream->offset, dstream->head - dstream->data);

            // ok
            state = TB_STATE_OK;

            // done func
            if (dstream->func.writ.func((tb_async_stream_t*)dstream, state, dstream->func.writ.data, real, dstream->func.writ.size, dstream->func.writ.priv))
            {
                // not finished?
                if (real < dstream->func.writ.size)
                {
                    // update data and size
                    dstream->func.writ.data += real;
                    dstream->func.writ.size -= real;

                    // continue to post writ
                    tb_aico_task_run(aice->aico, 0, tb_async_stream_data_writ_func, (tb_async_stream_t*)dstream);
                }
            }
        }
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
    if (state != TB_STATE_OK) dstream->func.writ.func((tb_async_stream_t*)dstream, state, dstream->func.writ.data, 0, dstream->func.writ.size, dstream->func.writ.priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_data_writ(tb_handle_t astream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_data_t* dstream = tb_async_stream_data_cast(astream);
    tb_assert_and_check_return_val(dstream && data && size && func, tb_false);

    // save func and priv
    dstream->func.writ.priv     = priv;
    dstream->func.writ.func     = func;
    dstream->func.writ.data     = data;
    dstream->func.writ.size     = size;

    // post writ
    return tb_aico_task_run(dstream->aico, delay, tb_async_stream_data_writ_func, astream);
}
static tb_bool_t tb_async_stream_data_seek(tb_handle_t astream, tb_hize_t offset, tb_async_stream_seek_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_data_t* dstream = tb_async_stream_data_cast(astream);
    tb_assert_and_check_return_val(dstream && func, tb_false);

    // done
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    do
    {
        // check
        tb_assert_and_check_break(dstream->data && offset <= dstream->size);

        // seek 
        dstream->head = dstream->data + offset;

        // save offset
        tb_atomic64_set(&dstream->offset, offset);

        // ok
        state = TB_STATE_OK;

    } while (0);

    // done func
    func(astream, state, offset, priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_data_task_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the stream
    tb_async_stream_data_t* dstream = (tb_async_stream_data_t*)aice->priv;
    tb_assert_and_check_return_val(dstream && dstream->func.task.func, tb_false);

    // done func
    tb_bool_t ok = dstream->func.task.func((tb_async_stream_t*)dstream, aice->state == TB_STATE_OK? TB_STATE_OK : TB_STATE_UNKNOWN_ERROR, dstream->func.task.priv);

    // ok and continue?
    if (ok && aice->state == TB_STATE_OK)
    {
        // post task
        tb_aico_task_run(aice->aico, aice->u.runtask.delay, tb_async_stream_data_task_func, dstream);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_data_task(tb_handle_t astream, tb_size_t delay, tb_async_stream_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_data_t* dstream = tb_async_stream_data_cast(astream);
    tb_assert_and_check_return_val(dstream && dstream->aico && func, tb_false);

    // save func and priv
    dstream->func.task.priv     = priv;
    dstream->func.task.func     = func;

    // post task
    return tb_aico_task_run(dstream->aico, delay, tb_async_stream_data_task_func, astream);
}
static tb_void_t tb_async_stream_data_kill(tb_handle_t astream)
{   
    // check
    tb_async_stream_data_t* dstream = tb_async_stream_data_cast(astream);
    tb_assert_and_check_return(dstream);

    // kill the task aico
    if (dstream->aico) tb_aico_kill(dstream->aico);
}
static tb_bool_t tb_async_stream_data_exit(tb_handle_t astream)
{   
    // check
    tb_async_stream_data_t* dstream = tb_async_stream_data_cast(astream);
    tb_assert_and_check_return_val(dstream, tb_false);

    // aico has been not closed already?
    tb_assert_and_check_return_val(!dstream->aico, tb_false);

    // clear head
    dstream->head = tb_null;

    // clear offset
    tb_atomic64_set0(&dstream->offset);

    // exit data
    if (dstream->data && !dstream->bref) tb_free(dstream->data);
    dstream->data = tb_null;
    dstream->size = 0;

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_data_ctrl(tb_handle_t astream, tb_size_t ctrl, tb_va_list_t args)
{ 
    // check
    tb_async_stream_data_t* dstream = tb_async_stream_data_cast(astream);
    tb_assert_and_check_return_val(dstream, tb_false);

    // ctrl
    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_SIZE:
        {
            // the psize
            tb_hong_t* psize = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
            tb_assert_and_check_return_val(psize, tb_false);

            // get size
            *psize = dstream->size;
            return tb_true;
        }
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // check
            tb_assert_and_check_return_val(dstream->data && dstream->size, tb_false);
            tb_assert_and_check_return_val(tb_stream_is_opened(astream), tb_false);

            // the poffset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);

            // get offset
            *poffset = tb_atomic64_get(&dstream->offset);
            return tb_true;
        }
    case TB_STREAM_CTRL_DATA_SET_DATA:
        {
            // exit data first if exists
            if (dstream->data && !dstream->bref) tb_free(dstream->data);

            // save data
            dstream->data = (tb_byte_t*)tb_va_arg(args, tb_byte_t*);
            dstream->size = (tb_size_t)tb_va_arg(args, tb_size_t);
            dstream->head = tb_null;
            dstream->bref = tb_true;

            // clear offset
            tb_atomic64_set0(&dstream->offset);

            // check
            tb_assert_and_check_return_val(dstream->data && dstream->size, tb_false);
            return tb_true;
        }
    case TB_STREAM_CTRL_SET_URL:
        {
            // check
            tb_assert_and_check_return_val(tb_stream_is_closed(astream), tb_false);

            // set url
            tb_char_t const* url = (tb_char_t const*)tb_va_arg(args, tb_char_t const*);
            tb_assert_and_check_return_val(url, tb_false); 
            
            // the url size
            tb_size_t url_size = tb_strlen(url);
            tb_assert_and_check_return_val(url_size > 7, tb_false);

            // the base64 data and size
            tb_char_t const*    base64_data = url + 7;
            tb_size_t           base64_size = url_size - 7;

            // make data
            tb_size_t   maxn = base64_size;
            tb_byte_t*  data = tb_malloc(maxn); 
            tb_assert_and_check_return_val(data, tb_false);

            // decode base64 data
            tb_size_t   size = tb_base64_decode(base64_data, base64_size, data, maxn);
            tb_assert_and_check_return_val(size, tb_false);

            // exit data first if exists
            if (dstream->data && !dstream->bref) tb_free(dstream->data);

            // save data
            dstream->data = data;
            dstream->size = size;
            dstream->bref = tb_false;
            dstream->head = tb_null;

            // clear offset
            tb_atomic64_set0(&dstream->offset);

            // ok
            return tb_true;
        }
        break;
    default:
        break;
    }
    return tb_false;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_async_stream_t* tb_async_stream_init_data(tb_aicp_t* aicp)
{
    // check
    tb_assert_and_check_return_val(aicp, tb_null);

    // done
    tb_bool_t                   ok = tb_false;
    tb_async_stream_data_t*     dstream = tb_null; 
    do
    {
        // make stream
        dstream = (tb_async_stream_data_t*)tb_malloc0(sizeof(tb_async_stream_data_t));
        tb_assert_and_check_break(dstream);

        // init stream
        if (!tb_async_stream_init((tb_async_stream_t*)dstream, aicp, TB_STREAM_TYPE_DATA, 0, 0)) break;
        dstream->base.open      = tb_async_stream_data_open;
        dstream->base.read      = tb_async_stream_data_read;
        dstream->base.writ      = tb_async_stream_data_writ;
        dstream->base.seek      = tb_async_stream_data_seek;
        dstream->base.task      = tb_async_stream_data_task;
        dstream->base.clos      = tb_async_stream_data_clos;
        dstream->base.exit      = tb_async_stream_data_exit;
        dstream->base.base.kill = tb_async_stream_data_kill;
        dstream->base.base.ctrl = tb_async_stream_data_ctrl;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (dstream) tb_async_stream_exit((tb_async_stream_t*)dstream);
        dstream = tb_null;
    }

    // ok?
    return (tb_async_stream_t*)dstream;
}
tb_async_stream_t* tb_async_stream_init_from_data(tb_aicp_t* aicp, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(aicp && data && size, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_async_stream_t*  dstream = tb_null;
    do
    {
        // init stream
        dstream = tb_async_stream_init_data(aicp);
        tb_assert_and_check_break(dstream);

        // set data
        if (!tb_stream_ctrl(dstream, TB_STREAM_CTRL_DATA_SET_DATA, data, size)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (dstream) tb_async_stream_exit(dstream);
        dstream = tb_null;
    }

    // ok?
    return dstream;
}
