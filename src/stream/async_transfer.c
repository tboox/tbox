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
 * @file        async_transfer.c
 * @ingroup     stream
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "async_transfer"
#define TB_TRACE_MODULE_DEBUG               (1)
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stream.h"
#include "async_transfer.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the async transfer open type
typedef struct __tb_async_transfer_open_t
{
    // the func
    tb_async_transfer_open_func_t       func;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_transfer_open_t;

// the async transfer ctrl type
typedef struct __tb_async_transfer_ctrl_t
{
    // the func
    tb_async_transfer_ctrl_func_t       func;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_transfer_ctrl_t;

// the async transfer clos type
typedef struct __tb_async_transfer_clos_t
{
    // the func
    tb_async_transfer_clos_func_t       func;

    // the priv
    tb_cpointer_t                       priv;

}tb_async_transfer_clos_t;

// the async transfer done type
typedef struct __tb_async_transfer_done_t
{
    // the func
    tb_async_transfer_done_func_t       func;

    // the priv
    tb_cpointer_t                       priv;

    // the base_time time
    tb_hong_t                           base_time;

    // the base_time time for 1s
    tb_hong_t                           base_time1s;

    // the saved_size size
    tb_hize_t                           saved_size;

    // the saved_size size for 1s
    tb_size_t                           saved_size1s;
 
    // the closed size
    tb_hong_t                           closed_size;

    // the closed state
    tb_size_t                           closed_state;

    // the closed offset 
    tb_hize_t                           closed_offset;

    // the current rate
    tb_size_t                           current_rate;

}tb_async_transfer_done_t;

// the async transfer close opening type
typedef struct __tb_async_transfer_clos_opening_t
{
    // the func
    tb_async_transfer_open_func_t       func;

    // the priv
    tb_cpointer_t                       priv;

    // the open state
    tb_size_t                           state;

}tb_async_transfer_clos_opening_t;

// the async transfer type
typedef struct __tb_async_transfer_t
{
    // the aicp
    tb_aicp_t*                          aicp;

    // the istream
    tb_async_stream_t*                  istream;

    // the ostream
    tb_async_stream_t*                  ostream;

    // the istream is owner?
    tb_uint8_t                          iowner      : 1;

    // the ostream is owner?
    tb_uint8_t                          oowner      : 1;

    // auto closing it?
    tb_uint8_t                          autoclosing : 1;

    /* state
     *
     * TB_STATE_CLOSED
     * TB_STATE_OPENED
     * TB_STATE_OPENING
     * TB_STATE_KILLING
     */
    tb_atomic_t                         state;

    /* pause state
     *
     * TB_STATE_OK
     * TB_STATE_PAUSED
     * TB_STATE_PAUSING
     */
    tb_atomic_t                         state_pause;

    // the limited rate
    tb_atomic_t                         limited_rate;

    // the ctrl
    tb_async_transfer_ctrl_t            ctrl;

    // the open
    tb_async_transfer_open_t            open;

    // the clos
    tb_async_transfer_clos_t            clos;

    // the done
    tb_async_transfer_done_t            done;

    // the clos opening
    tb_async_transfer_clos_opening_t    clos_opening;

}tb_async_transfer_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_async_transfer_clos_func(tb_async_transfer_t* transfer, tb_size_t state)
{
    // check
    tb_assert_and_check_return(transfer && transfer->clos.func);
         
    // trace
    tb_trace_d("closed");

    // closed
    tb_atomic_set(&transfer->state, TB_STATE_CLOSED);

    // clear pause state
    tb_atomic_set(&transfer->state_pause, TB_STATE_OK);

    // done func
    transfer->clos.func(state, transfer->clos.priv);
}
static tb_void_t tb_async_transfer_clos_opening_func(tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)priv;
    tb_assert_and_check_return(transfer && transfer->clos_opening.func);

    // trace
    tb_trace_d("clos: opening");
 
    // done
    transfer->clos_opening.func(transfer->clos_opening.state, 0, 0, transfer->clos_opening.priv);
}
static tb_bool_t tb_async_transfer_open_func(tb_async_transfer_t* transfer, tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_async_transfer_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(transfer, tb_false);

    // ok?
    tb_bool_t ok = tb_true;
    if (state == TB_STATE_OK) 
    {
        // opened
        tb_atomic_set(&transfer->state, TB_STATE_OPENED);

        // done func
        if (func) ok = func(state, offset, size, priv);
    }
    // failed? 
    else 
    {
        // init func and state
        transfer->clos_opening.func   = func;
        transfer->clos_opening.priv   = priv;
        transfer->clos_opening.state  = state;

        // close it
        ok = tb_async_transfer_clos(transfer, tb_async_transfer_clos_opening_func, transfer);
    }

    // ok?
    return ok;
}
static tb_void_t tb_async_transfer_done_clos_func(tb_size_t state, tb_cpointer_t priv);
static tb_bool_t tb_async_transfer_done_func(tb_async_transfer_t* transfer, tb_size_t state)
{
    // check
    tb_assert_and_check_return_val(transfer && transfer->istream && transfer->done.func, tb_false);

    // open failed? closed?
    if (TB_STATE_CLOSED == tb_atomic_get(&transfer->state))
    {
        // done func 
        return transfer->done.func(state, 0, 0, 0, 0, transfer->done.priv);
    }

    // trace
    tb_trace_d("done: %llu bytes, rate: %lu bytes/s, state: %s", tb_stream_offset(transfer->istream), transfer->done.current_rate, tb_state_cstr(state));

    // auto closing it?
    if (transfer->autoclosing)
    {
        // killed or failed or closed? close it
        if ((state != TB_STATE_OK && state != TB_STATE_PAUSED) || (TB_STATE_KILLING == tb_atomic_get(&transfer->state))) 
        {
            // save the closed state
            transfer->done.closed_state    = (TB_STATE_KILLING == tb_atomic_get(&transfer->state))? TB_STATE_KILLED : state;
            transfer->done.closed_size     = tb_stream_size(transfer->istream);
            transfer->done.closed_offset   = tb_stream_offset(transfer->istream);
            return tb_async_transfer_clos(transfer, tb_async_transfer_done_clos_func, transfer);
        }
    }

    // done
    return transfer->done.func(state, tb_stream_offset(transfer->istream), tb_stream_size(transfer->istream), transfer->done.saved_size, transfer->done.current_rate, transfer->done.priv);
}
static tb_void_t tb_async_transfer_done_clos_func(tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)priv;
    tb_assert_and_check_return(transfer && transfer->done.func);

    // trace
    tb_trace_d("done: closed");
 
    // done
    transfer->done.func(transfer->done.closed_state, transfer->done.closed_offset, transfer->done.closed_size, transfer->done.saved_size, transfer->done.current_rate, transfer->done.priv);
}
static tb_bool_t tb_async_transfer_open_done_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_cpointer_t priv)
{
    // the transfer
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)priv;
    tb_assert_and_check_return_val(transfer && transfer->done.func, tb_false);

    // trace
    tb_trace_d("open_done: offset: %llu, size: %lld, state: %s", offset, size, tb_state_cstr(state));

    // done
    tb_bool_t ok = tb_true;
    do
    {
        // ok? 
        tb_check_break(state == TB_STATE_OK);

        // reset state
        state = TB_STATE_UNKNOWN_ERROR;
        
        // killed?
        if (TB_STATE_KILLING == tb_atomic_get(&transfer->state))
        {
            state = TB_STATE_KILLED;
            break;
        }

        // done it
        if (!tb_async_transfer_done(transfer, transfer->done.func, transfer->done.priv)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed? 
    if (state != TB_STATE_OK) 
    {   
        // done func for closing it
        ok = tb_async_transfer_done_func(transfer, state);
    }

    // ok?
    return ok;
}
static tb_bool_t tb_async_transfer_istream_read_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv);
static tb_bool_t tb_async_transfer_ostream_writ_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)priv;
    tb_assert_and_check_return_val(astream && transfer && transfer->aicp && transfer->istream, tb_false);

    // trace
    tb_trace_d("writ: real: %lu, size: %lu, state: %s", real, size, tb_state_cstr(state));

    // the time
    tb_hong_t time = tb_aicp_time(transfer->aicp);

    // done
    tb_bool_t bwrit = tb_false;
    do
    {
        // ok?
        tb_check_break(state == TB_STATE_OK);
            
        // reset state
        state = TB_STATE_UNKNOWN_ERROR;

        // done func at first once
        if (!transfer->done.saved_size && !tb_async_transfer_done_func(transfer, TB_STATE_OK)) break;

        // update saved size
        transfer->done.saved_size += real;
    
        // < 1s?
        tb_size_t delay = 0;
        tb_size_t limited_rate = tb_atomic_get(&transfer->limited_rate);
        if (time < transfer->done.base_time1s + 1000)
        {
            // save size for 1s
            transfer->done.saved_size1s += real;

            // save current rate if < 1s from base_time
            if (time < transfer->done.base_time + 1000) transfer->done.current_rate = transfer->done.saved_size1s;
                    
            // compute the delay for limit rate
            if (limited_rate) delay = transfer->done.saved_size1s >= limited_rate? transfer->done.base_time1s + 1000 - time : 0;
        }
        else
        {
            // save current rate
            transfer->done.current_rate = transfer->done.saved_size1s;

            // update base_time1s
            transfer->done.base_time1s = time;

            // reset size
            transfer->done.saved_size1s = 0;

            // reset delay
            delay = 0;

            // done func
            if (!tb_async_transfer_done_func(transfer, TB_STATE_OK)) break;
        }

        // killed?
        if (TB_STATE_KILLING == tb_atomic_get(&transfer->state))
        {
            state = TB_STATE_KILLED;
            break;
        }

        // not finished? continue to writ
        tb_size_t state_pause = TB_STATE_OK;
        if (real < size) bwrit = tb_true;
        // pausing or paused?
        else if (   (TB_STATE_PAUSED == (state_pause = tb_atomic_fetch_and_pset(&transfer->state_pause, TB_STATE_PAUSING, TB_STATE_PAUSED)))
                ||  (state_pause == TB_STATE_PAUSING))
        {
            // done func
            if (!tb_async_transfer_done_func(transfer, TB_STATE_PAUSED)) break;
        }
        // continue?
        else 
        {
            // trace
            tb_trace_d("delay: %lu ms", delay);

            // continue to read it
            if (!tb_async_stream_read_after(transfer->istream, delay, limited_rate, tb_async_transfer_istream_read_func, (tb_pointer_t)transfer)) break;
        }

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed? 
    if (state != TB_STATE_OK) 
    {
        // compute the total rate
        transfer->done.current_rate = (transfer->done.saved_size && (time > transfer->done.base_time))? (tb_size_t)((transfer->done.saved_size * 1000) / (time - transfer->done.base_time)) : (tb_size_t)transfer->done.saved_size;

        // done func
        tb_async_transfer_done_func(transfer, state);

        // break;
        bwrit = tb_false;
    }

    // continue to writ or break it
    return bwrit;
}
static tb_bool_t tb_async_transfer_ostream_sync_func(tb_async_stream_t* astream, tb_size_t state, tb_bool_t bclosing, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)priv;
    tb_assert_and_check_return_val(astream && transfer && transfer->aicp && transfer->istream, tb_false);

    // trace
    tb_trace_d("sync: state: %s", tb_state_cstr(state));

    // the time
    tb_hong_t time = tb_aicp_time(transfer->aicp);

    // compute the total rate
    transfer->done.current_rate = (transfer->done.saved_size && (time > transfer->done.base_time))? (tb_size_t)((transfer->done.saved_size * 1000) / (time - transfer->done.base_time)) : (tb_size_t)transfer->done.saved_size;

    // done func
    return tb_async_transfer_done_func(transfer, state == TB_STATE_OK? TB_STATE_CLOSED : state);
}
static tb_bool_t tb_async_transfer_istream_read_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)priv;
    tb_assert_and_check_return_val(astream && transfer && transfer->aicp && transfer->ostream, tb_false);

    // trace
    tb_trace_d("read: size: %lu, state: %s", real, tb_state_cstr(state));

    // done
    tb_bool_t bread = tb_false;
    do
    {
        // ok?
        tb_check_break(state == TB_STATE_OK);

        // reset state
        state = TB_STATE_UNKNOWN_ERROR;

        // killed?
        if (TB_STATE_KILLING == tb_atomic_get(&transfer->state))
        {
            state = TB_STATE_KILLED;
            break;
        }

        // check
        tb_assert_and_check_break(data);

        // no data? continue it
        if (!real)
        {
            bread = tb_true;
            state = TB_STATE_OK;
            break;
        }

        // writ it
        if (!tb_async_stream_writ(transfer->ostream, data, real, tb_async_transfer_ostream_writ_func, transfer)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // closed or failed?
    if (state != TB_STATE_OK) 
    {
        // sync it if closed
        tb_bool_t bend = tb_true;
        if (state == TB_STATE_CLOSED)
            bend = tb_async_stream_sync(transfer->ostream, tb_true, tb_async_transfer_ostream_sync_func, transfer)? tb_false : tb_true;

        // end? 
        if (bend)
        {
            // the time
            tb_hong_t time = tb_aicp_time(transfer->aicp);

            // compute the total rate
            transfer->done.current_rate = (transfer->done.saved_size && (time > transfer->done.base_time))? (tb_size_t)((transfer->done.saved_size * 1000) / (time - transfer->done.base_time)) : (tb_size_t)transfer->done.saved_size;

            // done func
            tb_async_transfer_done_func(transfer, state);
        }

        // break
        bread = tb_false;
    }

    // continue to read or break it
    return bread;
}
static tb_bool_t tb_async_transfer_ostream_open_func(tb_async_stream_t* astream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)priv;
    tb_assert_and_check_return_val(astream && transfer && transfer->open.func, tb_false);

    // trace
    tb_trace_d("open: ostream: %s, state: %s", tb_url_get(&astream->base.url), tb_state_cstr(state));

    // done
    tb_bool_t ok = tb_true;
    do
    {
        // ok?
        tb_check_break(state == TB_STATE_OK);

        // reset state
        state = TB_STATE_UNKNOWN_ERROR;

        // check
        tb_assert_and_check_break(transfer->istream);
 
        // killed?
        if (TB_STATE_KILLING == tb_atomic_get(&transfer->state))
        {
            state = TB_STATE_KILLED;
            break;
        }

        // done func
        ok = tb_async_transfer_open_func(transfer, TB_STATE_OK, tb_stream_offset(transfer->istream), tb_stream_size(transfer->istream), transfer->open.func, transfer->open.priv);

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed?
    if (state != TB_STATE_OK) 
    {
        // done func
        ok = tb_async_transfer_open_func(transfer, state, 0, 0, transfer->open.func, transfer->open.priv);
    }

    // ok
    return ok;
}
static tb_bool_t tb_async_transfer_istream_open_func(tb_async_stream_t* astream, tb_size_t state, tb_hize_t offset, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)priv;
    tb_assert_and_check_return_val(astream && transfer && transfer->open.func, tb_false);

    // trace
    tb_trace_d("open: istream: %s, offset: %llu, state: %s", tb_url_get(&astream->base.url), offset, tb_state_cstr(state));

    // done
    tb_bool_t ok = tb_true;
    do
    {
        // ok?
        tb_check_break(state == TB_STATE_OK);

        // reset state
        state = TB_STATE_UNKNOWN_ERROR;
            
        // killed?
        if (TB_STATE_KILLING == tb_atomic_get(&transfer->state))
        {
            state = TB_STATE_KILLED;
            break;
        }

        // open it
        if (!tb_async_stream_open(transfer->ostream, tb_async_transfer_ostream_open_func, transfer)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed?
    if (state != TB_STATE_OK) 
    {
        // done func
        ok = tb_async_transfer_open_func(transfer, state, 0, 0, transfer->open.func, transfer->open.priv);
    }

    // ok?
    return ok;
}
static tb_void_t tb_async_transfer_ostream_clos_func(tb_async_stream_t* astream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)priv;
    tb_assert_and_check_return(astream && transfer);

    // trace
    tb_trace_d("clos: ostream: %s, state: %s", tb_url_get(&astream->base.url), tb_state_cstr(state));

    // done func
    tb_async_transfer_clos_func(transfer, state);
}
static tb_void_t tb_async_transfer_istream_clos_func(tb_async_stream_t* astream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)priv;
    tb_assert_and_check_return(astream && transfer);

    // trace
    tb_trace_d("clos: istream: %s, state: %s", tb_url_get(&astream->base.url), tb_state_cstr(state));

    // done
    do
    {
        // ok?
        tb_check_break(state == TB_STATE_OK);

        // reset state
        state = TB_STATE_UNKNOWN_ERROR;
           
        // clos it
        if (!tb_async_stream_clos(transfer->ostream, tb_async_transfer_ostream_clos_func, transfer)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed?
    if (state != TB_STATE_OK) 
    {
        // trace
        tb_trace_e("clos: failed: %s", tb_state_cstr(state));

        // done func
        tb_async_transfer_clos_func(transfer, state);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_async_transfer_init(tb_aicp_t* aicp, tb_bool_t autoclosing)
{
    // using the default aicp
    if (!aicp) aicp = tb_aicp();
    tb_assert_and_check_return_val(aicp, tb_null);

    // make transfer
    tb_async_transfer_t* transfer = tb_malloc0(sizeof(tb_async_transfer_t));
    tb_assert_and_check_return_val(transfer, tb_null);

    // init state
    transfer->state         = TB_STATE_CLOSED;
    transfer->state_pause   = TB_STATE_OK;
    transfer->autoclosing   = autoclosing? 1 : 0;

    // init aicp
    transfer->aicp          = aicp;

    // ok?
    return (tb_handle_t)transfer;
}
tb_bool_t tb_async_transfer_init_istream(tb_handle_t handle, tb_async_stream_t* stream)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer, tb_false);

    // muse be closed
    tb_assert_and_check_return_val(TB_STATE_CLOSED == tb_atomic_get(&transfer->state), tb_false);

    // exit the previous stream first
    if (transfer->istream && transfer->istream != stream)
    {
        if (transfer->iowner) tb_async_stream_exit(transfer->istream);
        transfer->istream = tb_null;
    }

    // init stream
    transfer->istream   = stream;
    transfer->iowner    = 0;

    // ok
    return tb_true;
}
tb_bool_t tb_async_transfer_init_istream_from_url(tb_handle_t handle, tb_char_t const* url)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && transfer->aicp && url, tb_false);

    // muse be closed
    tb_assert_and_check_return_val(TB_STATE_CLOSED == tb_atomic_get(&transfer->state), tb_false);

    // check stream type
    if (transfer->istream)
    {
        // probe protocol
        tb_size_t protocol = tb_url_protocol_probe(url);
        tb_assert_static(TB_URL_PROTOCOL_FILE == TB_STREAM_TYPE_FILE);
        tb_assert_static(TB_URL_PROTOCOL_HTTP == TB_STREAM_TYPE_HTTP);
        tb_assert_static(TB_URL_PROTOCOL_SOCK == TB_STREAM_TYPE_SOCK);
        tb_assert_static(TB_URL_PROTOCOL_DATA == TB_STREAM_TYPE_DATA);

        // protocol => type
        tb_size_t type = protocol;
        if (!type || type > TB_STREAM_TYPE_DATA)
        {
            tb_trace_e("unknown stream for url: %s", url);
            return tb_false;
        }

        // exit the previous stream first if be different stream type
        if (tb_stream_type(transfer->istream) != type)
        {
            if (transfer->iowner) tb_async_stream_exit(transfer->istream);
            transfer->istream = tb_null;
        }
    }

    // using the previous stream?
    if (transfer->istream)
    {
        // ctrl stream
        if (!tb_stream_ctrl(transfer->istream, TB_STREAM_CTRL_SET_URL, url)) return tb_false;
    }
    else 
    {
        // init stream
        transfer->istream = tb_async_stream_init_from_url(transfer->aicp, url);
        tb_assert_and_check_return_val(transfer->istream, tb_false);

        // init owner
        transfer->iowner = 1;
    }

    // ok
    return tb_true;
}
tb_bool_t tb_async_transfer_init_istream_from_data(tb_handle_t handle, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && transfer->aicp && data && size, tb_false);

    // muse be closed
    tb_assert_and_check_return_val(TB_STATE_CLOSED == tb_atomic_get(&transfer->state), tb_false);

    // exit the previous stream first if be not data stream
    if (transfer->istream && tb_stream_type(transfer->istream) != TB_STREAM_TYPE_DATA)
    {
        if (transfer->iowner) tb_async_stream_exit(transfer->istream);
        transfer->istream = tb_null;
    }

    // using the previous stream?
    if (transfer->istream)
    {
        // ctrl stream
        if (!tb_stream_ctrl(transfer->istream, TB_STREAM_CTRL_DATA_SET_DATA, data, size)) return tb_false;
    }
    else 
    {
        // init stream
        transfer->istream = tb_async_stream_init_from_data(transfer->aicp, (tb_byte_t*)data, size);
        tb_assert_and_check_return_val(transfer->istream, tb_false);

        // init owner
        transfer->iowner = 1;
    }

    // ok
    return tb_true;
}
tb_bool_t tb_async_transfer_init_ostream(tb_handle_t handle, tb_async_stream_t* stream)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer, tb_false);

    // muse be closed
    tb_assert_and_check_return_val(TB_STATE_CLOSED == tb_atomic_get(&transfer->state), tb_false);

    // exit the previous stream first
    if (transfer->ostream && transfer->ostream != stream)
    {
        if (transfer->oowner) tb_async_stream_exit(transfer->ostream);
        transfer->ostream = tb_null;
    }

    // init stream
    transfer->ostream   = stream;
    transfer->oowner    = 0;

    // ok
    return tb_true;
}
tb_bool_t tb_async_transfer_init_ostream_from_url(tb_handle_t handle, tb_char_t const* url)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && transfer->aicp && url, tb_false);

    // muse be closed
    tb_assert_and_check_return_val(TB_STATE_CLOSED == tb_atomic_get(&transfer->state), tb_false);

    // check stream type
    if (transfer->ostream)
    {
        // probe protocol
        tb_size_t protocol = tb_url_protocol_probe(url);
        tb_assert_static(TB_URL_PROTOCOL_FILE == TB_STREAM_TYPE_FILE);
        tb_assert_static(TB_URL_PROTOCOL_HTTP == TB_STREAM_TYPE_HTTP);
        tb_assert_static(TB_URL_PROTOCOL_SOCK == TB_STREAM_TYPE_SOCK);
        tb_assert_static(TB_URL_PROTOCOL_DATA == TB_STREAM_TYPE_DATA);

        // protocol => type
        tb_size_t type = protocol;
        if (!type || type > TB_STREAM_TYPE_DATA)
        {
            tb_trace_e("unknown stream for url: %s", url);
            return tb_false;
        }

        // exit the previous stream first if be different stream type
        if (tb_stream_type(transfer->ostream) != type)
        {
            if (transfer->oowner) tb_async_stream_exit(transfer->ostream);
            transfer->ostream = tb_null;
        }
    }

    // using the previous stream?
    if (transfer->ostream)
    {
        // ctrl stream
        if (!tb_stream_ctrl(transfer->ostream, TB_STREAM_CTRL_SET_URL, url)) return tb_false;
    }
    else 
    {
        // init stream
        transfer->ostream = tb_async_stream_init_from_url(transfer->aicp, url);
        tb_assert_and_check_return_val(transfer->ostream, tb_false);

        // ctrl stream for file
        if (tb_stream_type(transfer->ostream) == TB_STREAM_TYPE_FILE) 
        {
            // ctrl mode
            if (!tb_stream_ctrl(transfer->ostream, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC)) return tb_false;
        }

        // init owner
        transfer->oowner = 1;
    }

    // ok
    return tb_true;
}
tb_bool_t tb_async_transfer_init_ostream_from_data(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && transfer->aicp && data && size, tb_false);

    // muse be closed
    tb_assert_and_check_return_val(TB_STATE_CLOSED == tb_atomic_get(&transfer->state), tb_false);

    // exit the previous stream first if be not data stream
    if (transfer->ostream && tb_stream_type(transfer->ostream) != TB_STREAM_TYPE_DATA)
    {
        if (transfer->oowner) tb_async_stream_exit(transfer->ostream);
        transfer->ostream = tb_null;
    }

    // using the previous stream?
    if (transfer->ostream)
    {
        // ctrl stream
        if (!tb_stream_ctrl(transfer->ostream, TB_STREAM_CTRL_DATA_SET_DATA, data, size)) return tb_false;
    }
    else 
    {
        // init stream
        transfer->ostream = tb_async_stream_init_from_data(transfer->aicp, data, size);
        tb_assert_and_check_return_val(transfer->ostream, tb_false);

        // init owner
        transfer->oowner = 1;
    }

    // ok
    return tb_true;
}
tb_bool_t tb_async_transfer_ctrl_istream(tb_handle_t handle, tb_size_t ctrl, ...)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && transfer->istream, tb_false);

    // init args
    tb_va_list_t args;
    tb_va_start(args, ctrl);

    // ctrl it
    tb_bool_t ok = tb_stream_ctrl_with_args(transfer->istream, ctrl, args);

    // exit args
    tb_va_end(args);

    // ok?
    return ok;
}
tb_bool_t tb_async_transfer_ctrl_ostream(tb_handle_t handle, tb_size_t ctrl, ...)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && transfer->ostream, tb_false);

    // init args
    tb_va_list_t args;
    tb_va_start(args, ctrl);

    // ctrl it
    tb_bool_t ok = tb_stream_ctrl_with_args(transfer->ostream, ctrl, args);

    // exit args
    tb_va_end(args);

    // ok?
    return ok;
}
tb_bool_t tb_async_transfer_open(tb_handle_t handle, tb_hize_t offset, tb_async_transfer_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && transfer->aicp && func, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // set opening
        tb_size_t state = tb_atomic_fetch_and_pset(&transfer->state, TB_STATE_CLOSED, TB_STATE_OPENING);

        // opened? done func directly
        if (state == TB_STATE_OPENED)
        {
            // check
            tb_assert_and_check_break(transfer->istream && transfer->ostream);

            // done func
            func(TB_STATE_OK, tb_stream_offset(transfer->istream), tb_stream_size(transfer->istream), priv);

            // ok
            ok = tb_true;
            break;
        }

        // must be closed
        tb_assert_and_check_break(state == TB_STATE_CLOSED);

        // clear pause state
        tb_atomic_set(&transfer->state_pause, TB_STATE_OK);

        // init func
        transfer->open.func = func;
        transfer->open.priv = priv;

        // check
        tb_assert_and_check_break(transfer->istream);
        tb_assert_and_check_break(transfer->ostream);

        // init some rate info
        transfer->done.base_time      = tb_aicp_time(transfer->aicp);
        transfer->done.base_time1s    = transfer->done.base_time;
        transfer->done.saved_size     = 0;
        transfer->done.saved_size1s   = 0;
        transfer->done.current_rate   = 0;

        // ctrl stream
        if (transfer->ctrl.func && !transfer->ctrl.func(transfer->istream, transfer->ostream, transfer->ctrl.priv)) break;

        // open and seek istream
        if (!tb_async_stream_open_seek(transfer->istream, offset, tb_async_transfer_istream_open_func, transfer)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed? restore state
    if (!ok) tb_atomic_set(&transfer->state, TB_STATE_CLOSED);

    // ok?
    return ok;
}
tb_bool_t tb_async_transfer_clos(tb_handle_t handle, tb_async_transfer_clos_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && func, tb_false);
    
    // try closing ok?
    if (tb_async_transfer_clos_try(transfer))
    {
        // done func directly
        func(TB_STATE_OK, priv);
        return tb_true;
    }

    // init func
    transfer->clos.func = func;
    transfer->clos.priv = priv;

    // clos istream
    if (transfer->istream) return tb_async_stream_clos(transfer->istream, tb_async_transfer_istream_clos_func, transfer);
    // clos ostream
    else if (transfer->ostream) return tb_async_stream_clos(transfer->ostream, tb_async_transfer_ostream_clos_func, transfer);
    // done func directly
    else tb_async_transfer_clos_func(transfer, TB_STATE_OK);

    // ok
    return tb_true;
}
tb_bool_t tb_async_transfer_clos_try(tb_handle_t handle)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer, tb_false);

    // trace
    tb_trace_d("clos: try: ..");
       
    // done
    tb_bool_t ok = tb_false;
    do
    {
        // closed?
        if (TB_STATE_CLOSED == tb_atomic_get(&transfer->state))
        {
            ok = tb_true;
            break;
        }

        // try closing istream
        if (transfer->istream && !tb_async_stream_clos_try(transfer->istream)) break;

        // try closing ostream
        if (transfer->ostream && !tb_async_stream_clos_try(transfer->ostream)) break;

        // closed
        tb_atomic_set(&transfer->state, TB_STATE_CLOSED);

        // clear pause state
        tb_atomic_set(&transfer->state_pause, TB_STATE_OK);

        // ok
        ok = tb_true;
        
    } while (0);

    // trace
    tb_trace_d("clos: try: %s", ok? "ok" : "no");
         
    // ok?
    return ok;
}
tb_bool_t tb_async_transfer_ctrl(tb_handle_t handle, tb_async_transfer_ctrl_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && func, tb_false);
    
    // check state
    tb_assert_and_check_return_val(TB_STATE_CLOSED == tb_atomic_get(&transfer->state), tb_false);

    // init func
    transfer->ctrl.func = func;
    transfer->ctrl.priv = priv;

    // ok
    return tb_true;
}
tb_bool_t tb_async_transfer_done(tb_handle_t handle, tb_async_transfer_done_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && func, tb_false);
    
    // check state
    tb_assert_and_check_return_val(TB_STATE_OPENED == tb_atomic_get(&transfer->state), tb_false);

    // check stream
    tb_assert_and_check_return_val(transfer->istream && transfer->ostream, tb_false);

    // init func
    transfer->done.func = func;
    transfer->done.priv = priv;

    // read it
    return tb_async_stream_read(transfer->istream, (tb_size_t)tb_atomic_get(&transfer->limited_rate), tb_async_transfer_istream_read_func, transfer);
}
tb_bool_t tb_async_transfer_open_done(tb_handle_t handle, tb_hize_t offset, tb_async_transfer_done_func_t func, tb_cpointer_t priv)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && func, tb_false);

    // no opened? open it first
    if (TB_STATE_CLOSED == tb_atomic_get(&transfer->state))
    {
        transfer->done.func = func;
        transfer->done.priv = priv;
        return tb_async_transfer_open(transfer, offset, tb_async_transfer_open_done_func, transfer);
    }

    // done it
    return tb_async_transfer_done(transfer, func, priv);
}
tb_void_t tb_async_transfer_kill(tb_handle_t handle)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return(transfer);

    // kill it
    tb_size_t state = tb_atomic_fetch_and_set(&transfer->state, TB_STATE_KILLING);
    tb_check_return(state != TB_STATE_KILLING);

    // trace
    tb_trace_d("kill: ..");

    // kill istream
    if (transfer->istream) tb_stream_kill(transfer->istream);

    // kill ostream
    if (transfer->ostream) tb_stream_kill(transfer->ostream);
}
tb_bool_t tb_async_transfer_exit(tb_handle_t handle)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer, tb_false);

    // trace
    tb_trace_d("exit: ..");

    // kill it first
    tb_async_transfer_kill(handle);

    // try closing it
    tb_size_t tryn = 30;
    tb_bool_t ok = tb_false;
    while (!(ok = tb_async_transfer_clos_try(transfer)) && tryn--)
    {
        // wait some time
        tb_msleep(200);
    }

    // close failed?
    if (!ok)
    {
        // trace
        tb_trace_e("exit: failed!");
        return tb_false;
    }

    // exit istream
    if (transfer->istream && transfer->iowner) tb_async_stream_exit(transfer->istream);
    transfer->istream = tb_null;

    // exit ostream
    if (transfer->ostream && transfer->oowner) tb_async_stream_exit(transfer->ostream);
    transfer->ostream = tb_null;

    // exit transfer
    tb_free(transfer);

    // trace
    tb_trace_d("exit: ok");

    // ok
    return tb_true;
}
tb_void_t tb_async_transfer_pause(tb_handle_t handle)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return(transfer);

    // pause it
    tb_atomic_pset(&transfer->state_pause, TB_STATE_OK, TB_STATE_PAUSING);
}
tb_bool_t tb_async_transfer_resume(tb_handle_t handle)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && transfer->aicp, tb_false);

    // done
    tb_bool_t ok = tb_false;
    tb_size_t state_pause = TB_STATE_OK;
    do
    {
        // must be opened?
        tb_check_break(TB_STATE_OPENED == tb_atomic_get(&transfer->state));

        // resume it
        tb_size_t state_pause = tb_atomic_fetch_and_set(&transfer->state_pause, TB_STATE_OK);

        // pausing or ok? return ok directly
        tb_check_return_val(state_pause == TB_STATE_PAUSED, tb_true);

        // check
        tb_assert_and_check_break(transfer->istream);
        tb_assert_and_check_break(transfer->ostream);

        // init some rate info
        transfer->done.base_time      = tb_aicp_time(transfer->aicp);
        transfer->done.base_time1s    = transfer->done.base_time;
        transfer->done.saved_size1s   = 0;
        transfer->done.current_rate   = 0;

        // read it
        if (!tb_async_stream_read(transfer->istream, (tb_size_t)tb_atomic_get(&transfer->limited_rate), tb_async_transfer_istream_read_func, transfer)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed? restore state
    if (!ok && state_pause != TB_STATE_OK) tb_atomic_pset(&transfer->state_pause, TB_STATE_OK, state_pause);

    // ok?
    return ok;
}
tb_void_t tb_async_transfer_limitrate(tb_handle_t handle, tb_size_t rate)
{
    // check
    tb_async_transfer_t* transfer = (tb_async_transfer_t*)handle;
    tb_assert_and_check_return(transfer);

    // set the limited rate
    tb_atomic_set(&transfer->limited_rate, rate);
}

