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
 * @file        ssl.c
 * @ingroup     asio
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "aicp_ssl"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "ssl.h"
#include "aico.h"
#include "aicp.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the aicp ssl open type
typedef struct __tb_aicp_ssl_open_t
{
    // the func
    tb_aicp_ssl_open_func_t     func;

    // the priv 
    tb_cpointer_t               priv;

}tb_aicp_ssl_open_t;

// the aicp ssl clos type
typedef struct __tb_aicp_ssl_clos_t
{
    // the func
    tb_aicp_ssl_clos_func_t     func;

    // the priv 
    tb_cpointer_t               priv;

}tb_aicp_ssl_clos_t;

// the aicp ssl read type
typedef struct __tb_aicp_ssl_read_t
{
    // the func
    tb_aicp_ssl_read_func_t     func;

    // the data
    tb_byte_t*                  data;

    // the size
    tb_size_t                   size;

    // the priv 
    tb_cpointer_t               priv;

    // the delay
    tb_size_t                   delay;

}tb_aicp_ssl_read_t;

// the aicp ssl writ type
typedef struct __tb_aicp_ssl_writ_t
{
    // the func
    tb_aicp_ssl_writ_func_t     func;

    // the data
    tb_byte_t const*            data;

    // the size
    tb_size_t                   size;

    // the priv 
    tb_cpointer_t               priv;

}tb_aicp_ssl_writ_t;

// the aicp ssl task type
typedef struct __tb_aicp_ssl_task_t
{
    // the func
    tb_aicp_ssl_task_func_t     func;

    // the priv 
    tb_cpointer_t               priv;

}tb_aicp_ssl_task_t;

/// the aicp ssl close opening type
typedef struct __tb_aicp_ssl_clos_opening_t
{
    /// the func
    tb_aicp_ssl_open_func_t     func;

    /// the priv
    tb_cpointer_t               priv;

    /// the open state
    tb_size_t                   state;

}tb_aicp_ssl_clos_opening_t;

// the aicp ssl type
typedef struct __tb_aicp_ssl_t
{
    // the ssl 
    tb_handle_t                 ssl;

    // the aicp
    tb_aicp_t*                  aicp;

    // the aico
    tb_handle_t                 aico;

    // the func
    union
    {
        tb_aicp_ssl_open_t      open;
        tb_aicp_ssl_read_t      read;
        tb_aicp_ssl_writ_t      writ;
        tb_aicp_ssl_task_t      task;
        tb_aicp_ssl_clos_t      clos;

    }                           func;

    // the open and func
    union
    {
        tb_aicp_ssl_read_t      read;
        tb_aicp_ssl_writ_t      writ;

    }                           open_and;

    // the clos opening
    tb_aicp_ssl_clos_opening_t  clos_opening;

    // the post
    struct 
    {
        // the post func
        tb_bool_t               (*func)(tb_aice_t const* aice);

        // the post delay
        tb_size_t               delay;

        // the real size
        tb_long_t               real;

        // the read or writ data
        tb_byte_t*              data;

        // the read or writ size
        tb_size_t               size;

        // post read?
        tb_bool_t               read;

        // have post?
        tb_bool_t               post;

    }                           post;

    // the timeout
    tb_long_t                   timeout;

    /* the state
     *
     * TB_STATE_CLOSED
     * TB_STATE_OPENED
     * TB_STATE_OPENING
     * TB_STATE_KILLING
     */
    tb_atomic_t                 state;

    // the read data
    tb_scoped_buffer_t          read_data;

    // the writ data
    tb_scoped_buffer_t          writ_data;

}tb_aicp_ssl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_long_t tb_aicp_ssl_fill_read(tb_aicp_ssl_t* ssl, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(ssl, -1);

    // done
    tb_long_t real = -1;
    do
    {
        // check
        tb_assert_and_check_break(ssl->aico);
        tb_assert_and_check_break(data && size && ssl->post.real >= 0);

        // save real
        tb_size_t read_real = ssl->post.real;

        // clear real
        ssl->post.real = -1;

        // check
        tb_assert_and_check_break(read_real <= size);

        // the data and size
        tb_byte_t*  read_data = tb_scoped_buffer_data(&ssl->read_data);
        tb_size_t   read_size = tb_scoped_buffer_size(&ssl->read_data);
        tb_assert_and_check_break(read_data && read_size && size <= read_size);

        // copy data
        tb_memcpy(data, read_data, read_real);

        // trace
        tb_trace_d("[aico:%p]: read: fill: %lu: ok", ssl->aico, read_real);

        // read ok
        real = read_real;

    } while (0);

    // ok?
    return real;
}
static tb_long_t tb_aicp_ssl_fill_writ(tb_aicp_ssl_t* ssl, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(ssl, -1);

    // done
    tb_long_t real = -1;
    do
    {
        // check
        tb_assert_and_check_break(ssl->aico);
        tb_assert_and_check_break(size && ssl->post.real >= 0);

        // save real
        tb_size_t writ_real = ssl->post.real;

        // clear real
        ssl->post.real = -1;

        // check
        tb_assert_and_check_break(writ_real <= size);

        // trace
        tb_trace_d("[aico:%p]: writ: try: %lu: ok", ssl->aico, writ_real);

        // writ ok
        real = writ_real;

    } while (0);

    // ok?
    return real;
}
static tb_bool_t tb_aicp_ssl_done_post(tb_aicp_ssl_t* ssl)
{
    // check
    tb_assert_and_check_return_val(ssl, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // check
        tb_assert_and_check_break(ssl->post.post && ssl->post.data && ssl->post.size && ssl->post.func);

        // check
        tb_assert_and_check_break(ssl->aico);

        // post read?
        if (ssl->post.read)
        {
            // trace
            tb_trace_d("[aico:%p]: post: read: %lu: ..", ssl->aico, ssl->post.size);

            // post read
            if (!tb_aico_recv_after(ssl->aico, ssl->post.delay, ssl->post.data, ssl->post.size, ssl->post.func, ssl)) break;
        }
        // post writ?
        else
        {
            // trace
            tb_trace_d("[aico:%p]: post: writ: %lu: ..", ssl->aico, ssl->post.size);

            // post writ
            if (!tb_aico_send_after(ssl->aico, ssl->post.delay, ssl->post.data, ssl->post.size, ssl->post.func, ssl)) break;
        }

        // delay only for first
        ssl->post.delay = 0;

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}
static tb_void_t tb_aicp_ssl_clos_clear(tb_aicp_ssl_t* ssl)
{
    // check
    tb_assert_and_check_return(ssl);

    // close ssl
    if (ssl->ssl && ssl->aico) 
    {       
        // init bio sock, need some blocking time for closing
        tb_ssl_set_bio_sock(ssl->ssl, tb_aico_handle(ssl->aico));

        // close it
        tb_ssl_clos(ssl->ssl);
    }

    // clear data
    tb_scoped_buffer_clear(&ssl->read_data);
    tb_scoped_buffer_clear(&ssl->writ_data);

    // clear real
    ssl->post.real = 0;
    ssl->post.real = 0;

    // closed
    tb_atomic_set(&ssl->state, TB_STATE_CLOSED);
}
static tb_void_t tb_aicp_ssl_clos_opening(tb_handle_t handle, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return(ssl);

    // trace
    tb_trace_d("[aico:%p]: clos: opening: state: %s", ssl->aico, tb_state_cstr(ssl->clos_opening.state));

    // done func
    if (ssl->clos_opening.func) ssl->clos_opening.func(ssl, ssl->clos_opening.state, ssl->clos_opening.priv);
}
static tb_bool_t tb_aicp_ssl_open_func(tb_aicp_ssl_t* ssl, tb_size_t state, tb_aicp_ssl_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(ssl, tb_false);

    // ok?
    tb_bool_t ok = tb_true;
    if (state == TB_STATE_OK || !ssl->aico) 
    {
        // opened
        tb_atomic_set(&ssl->state, TB_STATE_OPENED);

        // done func
        if (func) ok = func(ssl, state, priv);
    }
    // failed? 
    else 
    {
        // init func and state
        ssl->clos_opening.func   = func;
        ssl->clos_opening.priv   = priv;
        ssl->clos_opening.state  = state;

        // close it
        tb_aicp_ssl_clos(ssl, tb_aicp_ssl_clos_opening, tb_null);
    }

    // ok?
    return ok;
}
static tb_bool_t tb_aicp_ssl_open_done(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && (aice->code == TB_AICE_CODE_RECV || aice->code == TB_AICE_CODE_SEND), tb_false);

    // the ssl
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)aice->priv;
    tb_assert_and_check_return_val(ssl && ssl->func.open.func, tb_false);

    // the real
    tb_size_t real = aice->code == TB_AICE_CODE_RECV? aice->u.recv.real : aice->u.send.real;

    // trace
    tb_trace_d("[aico:%p]: open: done: real: %lu, state: %s", ssl->aico, real, tb_state_cstr(aice->state));

    // done
    tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
    do
    {
        // clear post
        ssl->post.post  = tb_false;
        ssl->post.data  = tb_null;
        ssl->post.size  = 0;
        
        // failed or closed?
        if (aice->state != TB_STATE_OK)
        {
            state = aice->state;
            break;
        }

        // save the real size
        ssl->post.real = real;

        // trace
        tb_trace_d("[aico:%p]: open: done: try: ..", ssl->aico);
    
        // try opening it
        tb_long_t ok = tb_ssl_open_try(ssl->ssl);

        // trace
        tb_trace_d("[aico:%p]: open: done: try: %ld", ssl->aico, ok);
    
        // ok?
        if (ok > 0)
        {
            // done func
            tb_aicp_ssl_open_func(ssl, TB_STATE_OK, ssl->func.open.func, ssl->func.open.priv);
        }
        // failed?
        else if (ok < 0)
        {
            // save state
            state = tb_ssl_state(ssl->ssl);
            break;
        }
        // have post? continue it
        else if (ssl->post.post)
        {
            // post it
            if (!tb_aicp_ssl_done_post(ssl))
            {
                // trace
                tb_trace_e("[aico:%p]: open: done: post failed!", ssl->aico);
                break;
            }
        }
        else
        {
            // trace
            tb_trace_d("[aico:%p]: open: done: no post!", ssl->aico);
        }

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed?
    if (state != TB_STATE_OK)
    {
        // done func
        tb_aicp_ssl_open_func(ssl, state, ssl->func.open.func, ssl->func.open.priv);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_aicp_ssl_read_done(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && (aice->code == TB_AICE_CODE_RECV || aice->code == TB_AICE_CODE_SEND), tb_false);

    // the ssl
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)aice->priv;
    tb_assert_and_check_return_val(ssl && ssl->func.read.func, tb_false);

    // the real
    tb_size_t real = aice->code == TB_AICE_CODE_RECV? aice->u.recv.real : aice->u.send.real;

    // trace
    tb_trace_d("[aico:%p]: read: done: real: %lu, state: %s", ssl->aico, real, tb_state_cstr(aice->state));

    // done
    tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
    do
    {
        // clear post
        ssl->post.post  = tb_false;
        ssl->post.data  = tb_null;
        ssl->post.size  = 0;

        // failed or closed?
        if (aice->state != TB_STATE_OK)
        {
            state = aice->state;
            break;
        }

        // save the real size
        ssl->post.real = real;

        // trace
        tb_trace_d("[aico:%p]: read: done: try: %lu: ..", ssl->aico, ssl->func.read.size);
    
        // try reading it
        tb_long_t real = tb_ssl_read(ssl->ssl, ssl->func.read.data, ssl->func.read.size);

        // trace
        tb_trace_d("[aico:%p]: read: done: try: %lu: %ld", ssl->aico, ssl->func.read.size, real);
    
        // ok?
        if (real > 0)
        {
            // done func
            ssl->func.read.func(ssl, TB_STATE_OK, ssl->func.read.data, real, ssl->func.read.size, ssl->func.read.priv);
        }
        // failed?
        else if (real < 0)
        {
            // save state
            state = tb_ssl_state(ssl->ssl);
            break;
        }
        // have post? continue it
        else if (ssl->post.post)
        {
            // post it
            if (!tb_aicp_ssl_done_post(ssl))
            {
                // trace
                tb_trace_e("[aico:%p]: read: done: post failed!", ssl->aico);
                break;
            }
        }
        else
        {
            // trace
            tb_trace_d("[aico:%p]: read: done: no post!", ssl->aico);
    
            // done func
            ssl->func.read.func(ssl, TB_STATE_OK, ssl->func.read.data, 0, ssl->func.read.size, ssl->func.read.priv);
        }

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed?
    if (state != TB_STATE_OK)
    {
        // done func
        ssl->func.read.func(ssl, state, ssl->func.read.data, 0, ssl->func.read.size, ssl->func.read.priv);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_aicp_ssl_writ_done(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && (aice->code == TB_AICE_CODE_RECV || aice->code == TB_AICE_CODE_SEND), tb_false);

    // the ssl
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)aice->priv;
    tb_assert_and_check_return_val(ssl && ssl->func.writ.func, tb_false);

    // the real
    tb_size_t real = aice->code == TB_AICE_CODE_RECV? aice->u.recv.real : aice->u.send.real;

    // trace
    tb_trace_d("[aico:%p]: writ: done: real: %lu, state: %s", ssl->aico, real, tb_state_cstr(aice->state));

    // done
    tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
    do
    {
        // clear post
        ssl->post.post  = tb_false;
        ssl->post.data  = tb_null;
        ssl->post.size  = 0;

        // failed or closed?
        if (aice->state != TB_STATE_OK)
        {
            state = aice->state;
            break;
        }

        // save the real size
        ssl->post.real = real;

        // trace
        tb_trace_d("[aico:%p]: writ: done: try: %lu: ..", ssl->aico, ssl->func.writ.size);

        // try writing it
        tb_long_t real = tb_ssl_writ(ssl->ssl, ssl->func.writ.data, ssl->func.writ.size);

        // trace
        tb_trace_d("[aico:%p]: writ: done: try: %lu: %ld", ssl->aico, ssl->func.writ.size, real);
    
        // ok?
        if (real > 0)
        {
            // done func
            ssl->func.writ.func(ssl, TB_STATE_OK, ssl->func.writ.data, real, ssl->func.writ.size, ssl->func.writ.priv);
        }
        // failed?
        else if (real < 0)
        {
            // save state
            state = tb_ssl_state(ssl->ssl);
            break;
        }
        // have post? continue it
        else if (ssl->post.post)
        {
            // post it
            if (!tb_aicp_ssl_done_post(ssl))
            {
                // trace
                tb_trace_e("[aico:%p]: writ: done: post failed!", ssl->aico);
                break;
            }
        }
        else
        {
            // trace
            tb_trace_d("[aico:%p]: writ: done: no post!", ssl->aico);
    
            // done func
            ssl->func.writ.func(ssl, TB_STATE_OK, ssl->func.writ.data, 0, ssl->func.writ.size, ssl->func.writ.priv);
        }

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed?
    if (state != TB_STATE_OK)
    {
        // done func
        ssl->func.writ.func(ssl, state, ssl->func.writ.data, 0, ssl->func.writ.size, ssl->func.writ.priv);
    }

    // ok
    return tb_true;
}
static tb_long_t tb_aicp_ssl_read_func(tb_cpointer_t priv, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)priv;
    tb_assert_and_check_return_val(ssl && ssl->post.func && !ssl->post.post, -1);

    // done
    tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
    do
    {
        // fill to read it?
        if (ssl->post.real >= 0) return tb_aicp_ssl_fill_read(ssl, data, size);

        // resize data
        if (tb_scoped_buffer_size(&ssl->read_data) < size)
            tb_scoped_buffer_resize(&ssl->read_data, size);

        // the data and size
        tb_byte_t*  read_data = tb_scoped_buffer_data(&ssl->read_data);
        tb_size_t   read_size = tb_scoped_buffer_size(&ssl->read_data);
        tb_assert_and_check_break(read_data && read_size && size <= read_size);

        // post read
        ssl->post.post = tb_true;
        ssl->post.read = tb_true;
        ssl->post.data = read_data;
        ssl->post.size = size;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // read failed or continue?
    return state != TB_STATE_OK? -1 : 0;
}
static tb_long_t tb_aicp_ssl_writ_func(tb_cpointer_t priv, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)priv;
    tb_assert_and_check_return_val(ssl && ssl->post.func && !ssl->post.post, -1);

    // done
    tb_size_t state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;
    do
    {
        // fill to writ it?
        if (ssl->post.real >= 0) return tb_aicp_ssl_fill_writ(ssl, data, size);

        // save data
        tb_scoped_buffer_memncpy(&ssl->writ_data, data, size);

        // the data and size
        tb_byte_t*  writ_data = tb_scoped_buffer_data(&ssl->writ_data);
        tb_size_t   writ_size = tb_scoped_buffer_size(&ssl->writ_data);
        tb_assert_and_check_break(writ_data && writ_size && size == writ_size);

        // post writ
        ssl->post.post = tb_true;
        ssl->post.read = tb_false;
        ssl->post.data = writ_data;
        ssl->post.size = writ_size;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // ok?
    return state != TB_STATE_OK? -1 : 0;
}
static tb_bool_t tb_aicp_ssl_open_and_read(tb_handle_t ssl, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_aicp_ssl_read_t* read = (tb_aicp_ssl_read_t*)priv;
    tb_assert_and_check_return_val(ssl && read && read->func, tb_false);

    // done
    tb_bool_t ok = tb_true;
    do
    {
        // check
        tb_check_break(state == TB_STATE_OK);

        // clear state
        state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;

        // post read
        if (!tb_aicp_ssl_read(ssl, read->data, read->size, read->func, read->priv)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed?
    if (state != TB_STATE_OK)
    {
        // done func
        ok = read->func(ssl, state, read->data, 0, read->size, read->priv);
    }

    // ok?
    return ok;
}
static tb_bool_t tb_aicp_ssl_open_and_writ(tb_handle_t ssl, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_aicp_ssl_writ_t* writ = (tb_aicp_ssl_writ_t*)priv;
    tb_assert_and_check_return_val(ssl && writ && writ->func, tb_false);

    // done
    tb_bool_t ok = tb_true;
    do
    {
        // check
        tb_check_break(state == TB_STATE_OK);

        // clear state
        state = TB_STATE_SOCK_SSL_UNKNOWN_ERROR;

        // post writ
        if (!tb_aicp_ssl_writ(ssl, writ->data, writ->size, writ->func, writ->priv)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed?
    if (state != TB_STATE_OK)
    {
        // done func
        ok = writ->func(ssl, state, writ->data, 0, writ->size, writ->priv);
    }

    // ok?
    return ok;
}
static tb_bool_t tb_aicp_ssl_done_task(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the ssl
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)aice->priv;
    tb_assert_and_check_return_val(ssl && ssl->func.task.func, tb_false);

    // trace
    tb_trace_d("[aico:%p]: task: done: state: %s", ssl->aico, tb_state_cstr(aice->state));

    // done func
    ssl->func.task.func(ssl, aice->state, ssl->post.delay, ssl->func.task.priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_aicp_ssl_done_clos(tb_aice_t const* aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RUNTASK, tb_false);

    // the ssl
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)aice->priv;
    tb_assert_and_check_return_val(ssl && ssl->func.clos.func, tb_false);

    // trace
    tb_trace_d("[aico:%p]: clos: notify: ..", ssl->aico);

    // clear ssl
    tb_aicp_ssl_clos_clear(ssl);

    // done func
    ssl->func.clos.func(ssl, TB_STATE_OK, ssl->func.clos.priv);

    // trace
    tb_trace_d("[aico:%p]: clos: notify: ok", ssl->aico);

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_aicp_ssl_init(tb_aicp_t* aicp, tb_bool_t bserver)
{
    // check
    tb_assert_and_check_return_val(aicp, tb_null);

    // done
    tb_bool_t       ok = tb_false;
    tb_aicp_ssl_t*  ssl = tb_null;
    do
    {
        // make ssl
        ssl = tb_malloc0(sizeof(tb_aicp_ssl_t));
        tb_assert_and_check_break(ssl);

        // init state
        ssl->state = TB_STATE_CLOSED;

        // init aicp
        ssl->aicp = aicp;

        // init ssl
        ssl->ssl = tb_ssl_init(bserver);
        tb_assert_and_check_break(ssl->ssl);

        // init read data
        if (!tb_scoped_buffer_init(&ssl->read_data)) break;

        // init writ data
        if (!tb_scoped_buffer_init(&ssl->writ_data)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (ssl) tb_aicp_ssl_exit(ssl);
        ssl = tb_null;
    }

    // ok?
    return ssl;
}
tb_void_t tb_aicp_ssl_kill(tb_handle_t handle)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return(ssl);

    // kill it
    tb_size_t state = tb_atomic_fetch_and_set(&ssl->state, TB_STATE_KILLING);
    tb_check_return(state != TB_STATE_KILLING);

    // trace
    tb_trace_d("[aico:%p]: kill: ..", ssl->aico);

    // kill aico
    if (ssl->aico) tb_aico_kill(ssl->aico);
}
tb_bool_t tb_aicp_ssl_exit(tb_handle_t handle)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return_val(ssl, tb_false);

    // trace
    tb_trace_d("[aico:%p]: exit: ..", ssl->aico);

    // try closing it
    tb_size_t tryn = 30;
    tb_bool_t ok = tb_false;
    while (!(ok = tb_aicp_ssl_clos_try(ssl)) && tryn--)
    {
        // wait some time
        tb_msleep(200);
    }

    // close failed?
    if (!ok)
    {
        // trace
        tb_trace_e("[aico:%p]: exit: failed!", ssl->aico);
        return tb_false;
    }

    // exit ssl
    if (ssl->ssl) tb_ssl_exit(ssl->ssl);
    ssl->ssl = tb_null;

    // exit data
    tb_scoped_buffer_exit(&ssl->read_data);
    tb_scoped_buffer_exit(&ssl->writ_data);

    // trace
    tb_trace_d("[aico:%p]: exit: ok", ssl->aico);
    
    // exit it
    tb_free(ssl);

    // ok
    return tb_true;
}
tb_void_t tb_aicp_ssl_set_aico(tb_handle_t handle, tb_handle_t aico)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return(ssl);

    // save aico
    ssl->aico = aico;
}
tb_void_t tb_aicp_ssl_set_timeout(tb_handle_t handle, tb_long_t timeout)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return(ssl);

    // save timeout
    ssl->timeout = timeout;
}
tb_bool_t tb_aicp_ssl_open(tb_handle_t handle, tb_aicp_ssl_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return_val(ssl && func, tb_false);

    // done
    do
    {
        // set opening
        tb_size_t state = tb_atomic_fetch_and_pset(&ssl->state, TB_STATE_CLOSED, TB_STATE_OPENING);

        // opened? done func directly
        if (state == TB_STATE_OPENED) 
        {
            func(ssl, TB_STATE_OK, priv);
            break;
        }

        // must be closed
        tb_assert_and_check_return_val(state == TB_STATE_CLOSED, tb_false);

        // check
        tb_assert_and_check_return_val(ssl->aicp && ssl->ssl && ssl->aico, tb_false);

        // killed?
        if (TB_STATE_KILLING == tb_atomic_get(&ssl->state))
        {
            // done func
            tb_aicp_ssl_open_func(ssl, TB_STATE_KILLED, func, priv);
            break;
        }

        // init timeout
        if (ssl->timeout)
        {
            tb_aico_timeout_set(ssl->aico, TB_AICO_TIMEOUT_RECV, ssl->timeout);
            tb_aico_timeout_set(ssl->aico, TB_AICO_TIMEOUT_SEND, ssl->timeout);
        }

        // save func
        ssl->func.open.func = func;
        ssl->func.open.priv = priv;

        // init post
        ssl->post.func  = tb_aicp_ssl_open_done;
        ssl->post.delay = 0;
        ssl->post.post  = tb_false;
        ssl->post.data  = tb_null;
        ssl->post.size  = 0;
        ssl->post.real  = -1;

        // init post func
        tb_ssl_set_bio_func(ssl->ssl, tb_aicp_ssl_read_func, tb_aicp_ssl_writ_func, tb_null, ssl);

        // try opening it
        tb_long_t r = tb_ssl_open_try(ssl->ssl);

        // ok
        if (r > 0)
        {
            // done func
            tb_aicp_ssl_open_func(ssl, TB_STATE_OK, func, priv);
        }
        // failed?
        else if (r < 0)
        {
            // done func
            tb_aicp_ssl_open_func(ssl, tb_ssl_state(ssl->ssl), func, priv);
        }
        // have post? continue it
        else if (ssl->post.post)
        {
            // post it
            if (!tb_aicp_ssl_done_post(ssl))
            {
                // trace
                tb_trace_e("[aico:%p]: open: post failed!", ssl->aico);
        
                // done func
                tb_aicp_ssl_open_func(ssl, TB_STATE_SOCK_SSL_UNKNOWN_ERROR, func, priv);
            }
        }
        else
        {
            // trace
            tb_trace_e("[aico:%p]: open: no post!", ssl->aico);
    
            // done func
            tb_aicp_ssl_open_func(ssl, TB_STATE_SOCK_SSL_UNKNOWN_ERROR, func, priv);
        }

    } while (0);

    // post or done func ok
    return tb_true;
}
tb_bool_t tb_aicp_ssl_clos(tb_handle_t handle, tb_aicp_ssl_clos_func_t func, tb_cpointer_t priv)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return_val(ssl && func, tb_false);

    // trace
    tb_trace_d("[aico:%p]: clos: ..", ssl->aico);

    // try closing ok?
    if (tb_aicp_ssl_clos_try(ssl))
    {
        // done func
        func(ssl, TB_STATE_OK, priv);
        return tb_true;
    }

    // init func
    ssl->func.clos.func = func;
    ssl->func.clos.priv = priv;

    // clos aico
    if (ssl->aico && tb_aico_task_run(ssl->aico, 0, tb_aicp_ssl_done_clos, ssl));
    else
    {
        // clear ssl
        tb_aicp_ssl_clos_clear(ssl);

        // done func
        ssl->func.clos.func(ssl, TB_STATE_OK, ssl->func.clos.priv);
    }

    // ok
    return tb_true;
}
tb_bool_t tb_aicp_ssl_clos_try(tb_handle_t handle)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return_val(ssl, tb_false);

    // trace
    tb_trace_d("[aico:%p]: clos: try: ..", ssl->aico);

    // done
    tb_bool_t ok = tb_true;
    do
    {
        // closed? ok
        if (TB_STATE_CLOSED == tb_atomic_get(&ssl->state)) break;

        // no aico? ok
        if (!ssl->aico) break;

        // failed
        ok = tb_false;

    } while (0);

    // ok? closed
    if (ok) tb_atomic_set(&ssl->state, TB_STATE_CLOSED);

    // trace
    tb_trace_d("[aico:%p]: clos: try: %s", ssl->aico, ok? "ok" : "no");

    // ok?
    return ok;
}
tb_bool_t tb_aicp_ssl_read(tb_handle_t handle, tb_byte_t* data, tb_size_t size, tb_aicp_ssl_read_func_t func, tb_cpointer_t priv)
{
    return tb_aicp_ssl_read_after(handle, 0, data, size, func, priv);
}
tb_bool_t tb_aicp_ssl_writ(tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicp_ssl_writ_func_t func, tb_cpointer_t priv)
{
    return tb_aicp_ssl_writ_after(handle, 0, data, size, func, priv);
}
tb_bool_t tb_aicp_ssl_read_after(tb_handle_t handle, tb_size_t delay, tb_byte_t* data, tb_size_t size, tb_aicp_ssl_read_func_t func, tb_cpointer_t priv)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return_val(ssl && data && size && func, tb_false);

    // trace
    tb_trace_d("[aico:%p]: read: %lu, after: %lu", ssl->aico, size, delay);
            
    // opened?
    tb_assert_and_check_return_val(TB_STATE_OPENED == tb_atomic_get(&ssl->state), tb_false);

    // check
    tb_assert_and_check_return_val(ssl->aicp && ssl->ssl && ssl->aico, tb_false);
 
    // done
    do
    {
        // save func
        ssl->func.read.func     = func;
        ssl->func.read.priv     = priv;
        ssl->func.read.data     = data;
        ssl->func.read.size     = size;

        // init post
        ssl->post.func  = tb_aicp_ssl_read_done;
        ssl->post.delay = delay;
        ssl->post.post  = tb_false;
        ssl->post.data  = tb_null;
        ssl->post.size  = 0;
        ssl->post.real = -1;

        // init post func
        tb_ssl_set_bio_func(ssl->ssl, tb_aicp_ssl_read_func, tb_aicp_ssl_writ_func, tb_null, ssl);

        // try reading it
        tb_long_t real = tb_ssl_read(ssl->ssl, data, size);

        // ok
        if (real > 0)
        {
            // done func
            func(ssl, TB_STATE_OK, data, real, size, priv);
        }
        // failed?
        else if (real < 0)
        {
            // done func
            func(ssl, tb_ssl_state(ssl->ssl), data, 0, size, priv);
        }
        // have post? continue it
        else if (ssl->post.post)
        {
            // post it
            if (!tb_aicp_ssl_done_post(ssl))
            {
                // trace
                tb_trace_e("[aico:%p]: read: post failed!", ssl->aico);
        
                // done func
                func(ssl, TB_STATE_SOCK_SSL_UNKNOWN_ERROR, data, 0, size, priv);
            }
        }
        else
        {
            // trace
            tb_trace_e("[aico:%p]: read: no post!", ssl->aico);
    
            // done func
            func(ssl, TB_STATE_SOCK_SSL_UNKNOWN_ERROR, data, 0, size, priv);
        }

    } while (0);

    // post or done func ok
    return tb_true;
}
tb_bool_t tb_aicp_ssl_writ_after(tb_handle_t handle, tb_size_t delay, tb_byte_t const* data, tb_size_t size, tb_aicp_ssl_writ_func_t func, tb_cpointer_t priv)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return_val(ssl && data && size && func, tb_false);

    // trace
    tb_trace_d("[aico:%p]: writ: %lu, after: %lu", ssl->aico, size, delay);

    // opened?
    tb_assert_and_check_return_val(TB_STATE_OPENED == tb_atomic_get(&ssl->state), tb_false);

    // check
    tb_assert_and_check_return_val(ssl->aicp && ssl->ssl && ssl->aico, tb_false);

    // done
    do
    {
        // save func
        ssl->func.writ.func     = func;
        ssl->func.writ.priv     = priv;
        ssl->func.writ.data     = data;
        ssl->func.writ.size     = size;

        // init post
        ssl->post.func  = tb_aicp_ssl_writ_done;
        ssl->post.delay = delay;
        ssl->post.post  = tb_false;
        ssl->post.data  = tb_null;
        ssl->post.size  = 0;
        ssl->post.real  = -1;

        // init post func
        tb_ssl_set_bio_func(ssl->ssl, tb_aicp_ssl_read_func, tb_aicp_ssl_writ_func, tb_null, ssl);

        // try writing it
        tb_long_t real = tb_ssl_writ(ssl->ssl, data, size);

        // ok
        if (real > 0)
        {
            // done func
            func(ssl, TB_STATE_OK, data, real, size, priv);
        }
        // failed?
        else if (real < 0)
        {
            // done func
            func(ssl, tb_ssl_state(ssl->ssl), data, 0, size, priv);
        }
        // have post? continue it
        else if (ssl->post.post)
        {
            // post it
            if (!tb_aicp_ssl_done_post(ssl))
            {
                // trace
                tb_trace_e("[aico:%p]: writ: post failed!", ssl->aico);
        
                // done func
                func(ssl, TB_STATE_SOCK_SSL_UNKNOWN_ERROR, data, 0, size, priv);
            }
        }
        else
        {
            // trace
            tb_trace_e("[aico:%p]: writ: no post!", ssl->aico);
    
            // done func
            func(ssl, TB_STATE_SOCK_SSL_UNKNOWN_ERROR, data, 0, size, priv);
        }

    } while (0);

    // post or done func ok
    return tb_true;
}
tb_bool_t tb_aicp_ssl_task(tb_handle_t handle, tb_size_t delay, tb_aicp_ssl_task_func_t func, tb_cpointer_t priv)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return_val(ssl && ssl->aico && func, tb_false);

    // save func
    ssl->func.task.func     = func;
    ssl->func.task.priv     = priv;
    ssl->post.delay         = delay;

    // run task
    return tb_aico_task_run(ssl->aico, delay, tb_aicp_ssl_done_task, ssl);
}
tb_bool_t tb_aicp_ssl_open_read(tb_handle_t handle, tb_byte_t* data, tb_size_t size, tb_aicp_ssl_read_func_t func, tb_cpointer_t priv)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return_val(ssl && data && size && func, tb_false);

    // not opened? open it first
    if (TB_STATE_CLOSED == tb_atomic_get(&ssl->state))
    {
        ssl->open_and.read.func = func;
        ssl->open_and.read.data = data;
        ssl->open_and.read.size = size;
        ssl->open_and.read.priv = priv;
        return tb_aicp_ssl_open(handle, tb_aicp_ssl_open_and_read, &ssl->open_and.read);
    }

    // read it
    return tb_aicp_ssl_read(handle, data, size, func, priv);
}
tb_bool_t tb_aicp_ssl_open_writ(tb_handle_t handle, tb_byte_t const* data, tb_size_t size, tb_aicp_ssl_writ_func_t func, tb_cpointer_t priv)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return_val(ssl && data && size && func, tb_false);

    // not opened? open it first
    if (TB_STATE_CLOSED == tb_atomic_get(&ssl->state))
    {
        ssl->open_and.writ.func = func;
        ssl->open_and.writ.data = data;
        ssl->open_and.writ.size = size;
        ssl->open_and.writ.priv = priv;
        return tb_aicp_ssl_open(handle, tb_aicp_ssl_open_and_writ, &ssl->open_and.writ);
    }

    // writ it
    return tb_aicp_ssl_writ(handle, data, size, func, priv);
}
tb_aicp_t* tb_aicp_ssl_aicp(tb_handle_t handle)
{
    // check
    tb_aicp_ssl_t* ssl = (tb_aicp_ssl_t*)handle;
    tb_assert_and_check_return_val(ssl, tb_null);

    // the aicp
    return ssl->aicp;
}

