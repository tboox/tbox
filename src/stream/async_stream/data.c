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
typedef struct __tb_async_stream_impl_read_t
{
    // the func
    tb_async_stream_read_func_t         func;

    // the size
    tb_size_t                           size;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_stream_impl_read_t;

// the data writ type
typedef struct __tb_async_stream_impl_writ_t
{
    // the func
    tb_async_stream_writ_func_t         func;

    // the data
    tb_byte_t const*                    data;

    // the size
    tb_size_t                           size;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_stream_impl_writ_t;

// the data task type
typedef struct __tb_async_stream_impl_task_t
{
    // the func
    tb_async_stream_task_func_t         func;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_stream_impl_task_t;

// the data clos type
typedef struct __tb_async_stream_impl_clos_t
{
    // the func
    tb_async_stream_clos_func_t         func;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_stream_impl_clos_t;

// the data stream type
typedef struct __tb_async_stream_impl_t
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
        tb_async_stream_impl_read_t     read;
        tb_async_stream_impl_writ_t     writ;
        tb_async_stream_impl_task_t     task;
        tb_async_stream_impl_clos_t     clos;

    }                                   func;

}tb_async_stream_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static __tb_inline__ tb_async_stream_impl_t* tb_async_stream_impl_cast(tb_async_stream_t* stream)
{
    tb_assert_and_check_return_val(stream && stream->type == TB_STREAM_TYPE_DATA, tb_null);
    return (tb_async_stream_impl_t*)stream;
}
static tb_void_t tb_async_stream_impl_clos_clear(tb_async_stream_impl_t* impl)
{
    // check
    tb_assert_and_check_return(impl);

    // clear aico
    impl->aico = tb_null;

    // clear head
    impl->head = tb_null;

    // clear offset
    tb_atomic64_set0(&impl->offset);

    // clear base
    tb_async_stream_clear(&impl->base);
}
static tb_void_t tb_async_stream_impl_clos_func(tb_handle_t aico, tb_cpointer_t priv)
{
    // check
    tb_async_stream_impl_t* impl = tb_async_stream_impl_cast((tb_handle_t)priv);
    tb_assert_and_check_return(impl && impl->func.clos.func);

    // trace
    tb_trace_d("clos: notify: ..");

    // clear it
    tb_async_stream_impl_clos_clear(impl);

    /* done clos func
     *
     * note: cannot use this stream after closing, the stream may be exited in the closing func
     */
    impl->func.clos.func(&impl->base, TB_STATE_OK, impl->func.clos.priv);

    // trace
    tb_trace_d("clos: notify: ok");
}
static tb_bool_t tb_async_stream_impl_clos_try(tb_async_stream_t* stream)
{   
    // check
    tb_async_stream_impl_t* impl = tb_async_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // no aico? closed 
    if (!impl->aico)
    {
        // clear it
        tb_async_stream_impl_clos_clear(impl);

        // ok
        return tb_true;
    }

    // failed
    return tb_false;
}
static tb_bool_t tb_async_stream_impl_clos(tb_async_stream_t* stream, tb_async_stream_clos_func_t func, tb_cpointer_t priv)
{   
    // check
    tb_async_stream_impl_t* impl = tb_async_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl && func, tb_false);

    // trace
    tb_trace_d("clos: ..");

    // init func
    impl->func.clos.func = func;
    impl->func.clos.priv = priv;

    // exit it
    if (impl->aico) tb_aico_exit(impl->aico, tb_async_stream_impl_clos_func, impl);
    // done func directly
    else tb_async_stream_impl_clos_func(tb_null, impl);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_impl_open_try(tb_async_stream_t* stream)
{
    // check
    tb_async_stream_impl_t* impl = tb_async_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // check
        tb_assert_and_check_break(impl->data && impl->size);
        
        // init aico
        if (!impl->aico) impl->aico = tb_aico_init_task(impl->base.aicp, tb_false);
        tb_assert_and_check_break(impl->aico);

        // killed?
        tb_check_break(!tb_async_stream_is_killed(stream));

        // init head
        impl->head = impl->data;

        // init offset
        tb_atomic64_set0(&impl->offset);

        // open done
        tb_async_stream_open_done(stream);

        // ok
        ok = tb_true;

    } while (0);

    // failed? clear it
    if (!ok) tb_async_stream_impl_clos_clear(impl);

    // ok?
    return ok;
}
static tb_bool_t tb_async_stream_impl_open(tb_async_stream_t* stream, tb_async_stream_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_impl_t* impl = tb_async_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl && func, tb_false);

    // try opening it
    tb_size_t state = tb_async_stream_impl_open_try(stream)? TB_STATE_OK : TB_STATE_UNKNOWN_ERROR;

    // killed?
    if (state != TB_STATE_OK && tb_async_stream_is_killed(stream))
        state = TB_STATE_KILLED;

    // done func
    return tb_async_stream_open_func(stream, state, func, priv);
}
static tb_bool_t tb_async_stream_impl_read_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the stream
    tb_async_stream_impl_t* impl = (tb_async_stream_impl_t*)aice->priv;
    tb_assert_and_check_return_val(impl && impl->func.read.func, tb_false);

    // done state
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        {
            // check
            tb_assert_and_check_break(impl->data && impl->head);
            
            // the left
            tb_size_t left = impl->data + impl->size - impl->head;

            // the real
            tb_size_t real = impl->func.read.size;
            if (!real || real > left) real = left;

            // no data? closed
            if (!real) 
            {
                state = TB_STATE_CLOSED;
                break;
            }

            // save data
            tb_byte_t* data = impl->head;

            // save head
            impl->head += real;

            // save offset
            tb_atomic64_set(&impl->offset, impl->head - impl->data);

            // ok
            state = TB_STATE_OK;

            // done func
            if (impl->func.read.func((tb_async_stream_t*)impl, state, data, real, impl->func.read.size, impl->func.read.priv))
            {
                // continue to post read
                tb_aico_task_run(aice->aico, 0, tb_async_stream_impl_read_func, (tb_async_stream_t*)impl);
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
    if (state != TB_STATE_OK) impl->func.read.func((tb_async_stream_t*)impl, state, tb_null, 0, impl->func.read.size, impl->func.read.priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_impl_read(tb_async_stream_t* stream, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_async_stream_read_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_impl_t* impl = tb_async_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->aico && func, tb_false);

    // save func and priv
    impl->func.read.priv     = priv;
    impl->func.read.func     = func;
    impl->func.read.size     = size;

    // post read
    return tb_aico_task_run(impl->aico, delay, tb_async_stream_impl_read_func, stream);
}
static tb_bool_t tb_async_stream_impl_writ_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the stream
    tb_async_stream_impl_t* impl = (tb_async_stream_impl_t*)aice->priv;
    tb_assert_and_check_return_val(impl && impl->func.writ.func, tb_false);

    // done state
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    switch (aice->state)
    {
        // ok
    case TB_STATE_OK:
        {
            // check
            tb_assert_and_check_break(impl->data && impl->head);
            tb_assert_and_check_break(impl->func.writ.data && impl->func.writ.size);
    
            // the left
            tb_size_t left = impl->data + impl->size - impl->head;

            // the real
            tb_size_t real = impl->func.writ.size;
            if (real > left) real = left;

            // no data? closed
            if (!real) 
            {
                state = TB_STATE_CLOSED;
                break;
            }

            // save data
            tb_memcpy(impl->head, impl->func.writ.data, real);

            // save head
            impl->head += real;

            // save offset
            tb_atomic64_set(&impl->offset, impl->head - impl->data);

            // ok
            state = TB_STATE_OK;

            // done func
            if (impl->func.writ.func((tb_async_stream_t*)impl, state, impl->func.writ.data, real, impl->func.writ.size, impl->func.writ.priv))
            {
                // not finished?
                if (real < impl->func.writ.size)
                {
                    // update data and size
                    impl->func.writ.data += real;
                    impl->func.writ.size -= real;

                    // continue to post writ
                    tb_aico_task_run(aice->aico, 0, tb_async_stream_impl_writ_func, (tb_async_stream_t*)impl);
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
    if (state != TB_STATE_OK) impl->func.writ.func((tb_async_stream_t*)impl, state, impl->func.writ.data, 0, impl->func.writ.size, impl->func.writ.priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_impl_writ(tb_async_stream_t* stream, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_async_stream_writ_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_impl_t* impl = tb_async_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl && data && size && func, tb_false);

    // save func and priv
    impl->func.writ.priv     = priv;
    impl->func.writ.func     = func;
    impl->func.writ.data     = data;
    impl->func.writ.size     = size;

    // post writ
    return tb_aico_task_run(impl->aico, delay, tb_async_stream_impl_writ_func, stream);
}
static tb_bool_t tb_async_stream_impl_seek(tb_async_stream_t* stream, tb_hize_t offset, tb_async_stream_seek_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_impl_t* impl = tb_async_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl && func, tb_false);

    // done
    tb_size_t state = TB_STATE_UNKNOWN_ERROR;
    do
    {
        // check
        tb_assert_and_check_break(impl->data && offset <= impl->size);

        // seek 
        impl->head = impl->data + offset;

        // save offset
        tb_atomic64_set(&impl->offset, offset);

        // ok
        state = TB_STATE_OK;

    } while (0);

    // done func
    func(stream, state, offset, priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_impl_task_func(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the stream
    tb_async_stream_impl_t* impl = (tb_async_stream_impl_t*)aice->priv;
    tb_assert_and_check_return_val(impl && impl->func.task.func, tb_false);

    // done func
    tb_bool_t ok = impl->func.task.func((tb_async_stream_t*)impl, aice->state == TB_STATE_OK? TB_STATE_OK : TB_STATE_UNKNOWN_ERROR, impl->func.task.priv);

    // ok and continue?
    if (ok && aice->state == TB_STATE_OK)
    {
        // post task
        tb_aico_task_run(aice->aico, aice->u.runtask.delay, tb_async_stream_impl_task_func, impl);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_impl_task(tb_async_stream_t* stream, tb_size_t delay, tb_async_stream_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_stream_impl_t* impl = tb_async_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl && impl->aico && func, tb_false);

    // save func and priv
    impl->func.task.priv     = priv;
    impl->func.task.func     = func;

    // post task
    return tb_aico_task_run(impl->aico, delay, tb_async_stream_impl_task_func, stream);
}
static tb_void_t tb_async_stream_impl_kill(tb_async_stream_t* stream)
{   
    // check
    tb_async_stream_impl_t* impl = tb_async_stream_impl_cast(stream);
    tb_assert_and_check_return(impl);

    // kill the task aico
    if (impl->aico) tb_aico_kill(impl->aico);
}
static tb_bool_t tb_async_stream_impl_exit(tb_async_stream_t* stream)
{   
    // check
    tb_async_stream_impl_t* impl = tb_async_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // aico has been not closed already?
    tb_assert_and_check_return_val(!impl->aico, tb_false);

    // clear head
    impl->head = tb_null;

    // clear offset
    tb_atomic64_set0(&impl->offset);

    // exit data
    if (impl->data && !impl->bref) tb_free(impl->data);
    impl->data = tb_null;
    impl->size = 0;

    // ok
    return tb_true;
}
static tb_bool_t tb_async_stream_impl_ctrl(tb_async_stream_t* stream, tb_size_t ctrl, tb_va_list_t args)
{ 
    // check
    tb_async_stream_impl_t* impl = tb_async_stream_impl_cast(stream);
    tb_assert_and_check_return_val(impl, tb_false);

    // ctrl
    switch (ctrl)
    {
    case TB_STREAM_CTRL_GET_SIZE:
        {
            // the psize
            tb_hong_t* psize = (tb_hong_t*)tb_va_arg(args, tb_hong_t*);
            tb_assert_and_check_return_val(psize, tb_false);

            // get size
            *psize = impl->size;
            return tb_true;
        }
    case TB_STREAM_CTRL_GET_OFFSET:
        {
            // check
            tb_assert_and_check_return_val(impl->data && impl->size, tb_false);
            tb_assert_and_check_return_val(tb_async_stream_is_opened(stream), tb_false);

            // the poffset
            tb_hize_t* poffset = (tb_hize_t*)tb_va_arg(args, tb_hize_t*);
            tb_assert_and_check_return_val(poffset, tb_false);

            // get offset
            *poffset = tb_atomic64_get(&impl->offset);
            return tb_true;
        }
    case TB_STREAM_CTRL_DATA_SET_DATA:
        {
            // exit data first if exists
            if (impl->data && !impl->bref) tb_free(impl->data);

            // save data
            impl->data = (tb_byte_t*)tb_va_arg(args, tb_byte_t*);
            impl->size = (tb_size_t)tb_va_arg(args, tb_size_t);
            impl->head = tb_null;
            impl->bref = tb_true;

            // clear offset
            tb_atomic64_set0(&impl->offset);

            // check
            tb_assert_and_check_return_val(impl->data && impl->size, tb_false);
            return tb_true;
        }
    case TB_STREAM_CTRL_SET_URL:
        {
            // check
            tb_assert_and_check_return_val(tb_async_stream_is_closed(stream), tb_false);

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
            tb_byte_t*  data = (tb_byte_t*)tb_malloc(maxn); 
            tb_assert_and_check_return_val(data, tb_false);

            // decode base64 data
            tb_size_t   size = tb_base64_decode(base64_data, base64_size, data, maxn);
            tb_assert_and_check_return_val(size, tb_false);

            // exit data first if exists
            if (impl->data && !impl->bref) tb_free(impl->data);

            // save data
            impl->data = data;
            impl->size = size;
            impl->bref = tb_false;
            impl->head = tb_null;

            // clear offset
            tb_atomic64_set0(&impl->offset);

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
    // done
    tb_bool_t                   ok = tb_false;
    tb_async_stream_impl_t*     impl = tb_null; 
    do
    {
        // make stream
        impl = (tb_async_stream_impl_t*)tb_malloc0(sizeof(tb_async_stream_impl_t));
        tb_assert_and_check_break(impl);

        // init stream
        if (!tb_async_stream_init((tb_async_stream_t*)impl, aicp, TB_STREAM_TYPE_DATA, 0, 0)) break;
        impl->base.open      = tb_async_stream_impl_open;
        impl->base.clos      = tb_async_stream_impl_clos;
        impl->base.read      = tb_async_stream_impl_read;
        impl->base.writ      = tb_async_stream_impl_writ;
        impl->base.seek      = tb_async_stream_impl_seek;
        impl->base.task      = tb_async_stream_impl_task;
        impl->base.exit      = tb_async_stream_impl_exit;
        impl->base.kill      = tb_async_stream_impl_kill;
        impl->base.ctrl      = tb_async_stream_impl_ctrl;
        impl->base.open_try  = tb_async_stream_impl_open_try;
        impl->base.clos_try  = tb_async_stream_impl_clos_try;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_async_stream_exit((tb_async_stream_t*)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_async_stream_t*)impl;
}
tb_async_stream_t* tb_async_stream_init_from_data(tb_aicp_t* aicp, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(data && size, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_async_stream_t*  impl = tb_null;
    do
    {
        // init stream
        impl = tb_async_stream_init_data(aicp);
        tb_assert_and_check_break(impl);

        // set data
        if (!tb_async_stream_ctrl(impl, TB_STREAM_CTRL_DATA_SET_DATA, data, size)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_async_stream_exit(impl);
        impl = tb_null;
    }

    // ok?
    return impl;
}
