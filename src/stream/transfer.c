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
 * @file        transfer.c
 * @ingroup     stream
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "transfer"
#define TB_TRACE_MODULE_DEBUG               (1)
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "transfer.h"
#include "stream.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the transfer ctrl type
typedef struct __tb_transfer_ctrl_t
{
    // the func
    tb_transfer_ctrl_func_t     func;

    // the priv
    tb_cpointer_t               priv;

}tb_transfer_ctrl_t;

// the transfer open type
typedef struct __tb_transfer_open_t
{
    // the func
    tb_transfer_open_func_t     func;

    // the priv
    tb_cpointer_t               priv;

}tb_transfer_open_t;

// the transfer save type
typedef struct __tb_transfer_save_t
{
    // the func
    tb_transfer_save_func_t     func;

    // the priv
    tb_cpointer_t               priv;

    // the base time
    tb_hong_t                   base;

    // the base time for 1s
    tb_hong_t                   base1s;

    // the saved size
    tb_hize_t                   saved;

    // the saved size for 1s
    tb_size_t                   saved1s;
 
    // the closed size
    tb_hong_t                   csize;

    // the closed state
    tb_size_t                   cstate;

    // the closed offset 
    tb_hize_t                   coffset;

    // the current rate
    tb_size_t                   crate;

}tb_transfer_save_t;

// the transfer type
typedef struct __tb_transfer_t
{
    // the istream
    tb_handle_t                 istream;

    // the ostream
    tb_handle_t                 ostream;

    // the istream is owner?
    tb_uint8_t                  iowner : 1;

    // the ostream is owner?
    tb_uint8_t                  oowner : 1;

    // is stoped?
    tb_atomic_t                 stoped;

    // is opened?
    tb_atomic_t                 opened;

    // is paused?
    tb_atomic_t                 paused;

    // is pausing?
    tb_atomic_t                 pausing;

    // the seek offset
    tb_hize_t                   offset;

    // the limit rate
    tb_atomic_t                 lrate;

    // ctrl
    tb_transfer_ctrl_t          ctrl;

    // open
    tb_transfer_open_t          open;

    // save
    tb_transfer_save_t          save;

}tb_transfer_t;
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_MODULE_HAVE_ASIO
static tb_void_t tb_transfer_clos_func(tb_transfer_t* transfer)
{
    // check
    tb_assert_and_check_return(transfer && transfer->istream && transfer->save.func);
         
    // trace
    tb_trace_d("closed");

    // save func
    tb_transfer_save_t save = transfer->save;

    // clear opened
    tb_atomic_set0(&transfer->opened);

    // done
    save.func(save.cstate, save.coffset, save.csize, save.saved, save.crate, save.priv);
}
static tb_void_t tb_transfer_istream_clos_func(tb_async_stream_t* astream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)priv;
    tb_assert_and_check_return(astream && transfer);

    // trace
    tb_trace_d("clos: istream: %s, state: %s", tb_url_get(&astream->base.url), tb_state_cstr(state));

    // done func
    tb_transfer_clos_func(transfer);
}
static tb_void_t tb_transfer_ostream_clos_func(tb_async_stream_t* astream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)priv;
    tb_assert_and_check_return(astream && transfer);

    // trace
    tb_trace_d("clos: ostream: %s, state: %s", tb_url_get(&astream->base.url), tb_state_cstr(state));

    // done
    do
    {
        // ok?
        tb_check_break(state == TB_STATE_OK);

        // reset state
        state = TB_STATE_UNKNOWN_ERROR;
           
        // clos it
        if (!tb_async_stream_clos(transfer->istream, tb_transfer_istream_clos_func, transfer)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed?
    if (state != TB_STATE_OK) 
    {
        // trace
        tb_trace_e("clos: failed: %s", tb_state_cstr(state));

        // done func
        tb_transfer_clos_func(transfer);
    }
}
static tb_bool_t tb_transfer_istream_save_func(tb_transfer_t* transfer, tb_size_t state)
{
    // check
    tb_assert_and_check_return_val(transfer && transfer->istream && transfer->save.func, tb_false);

    // trace
    tb_trace_d("save: %llu, rate: %lu bytes/s, state: %s", transfer->save.crate, tb_stream_offset(transfer->istream), tb_state_cstr(state));

    // killed or failed or closed? close it
    if ((state != TB_STATE_OK && state != TB_STATE_PAUSED) || tb_atomic_get(&transfer->stoped)) 
    {
        // save the closed state
        transfer->save.cstate    = state;
        transfer->save.csize     = tb_stream_size(transfer->istream);
        transfer->save.coffset   = tb_stream_offset(transfer->istream);
        return tb_async_stream_clos(transfer->ostream, tb_transfer_ostream_clos_func, transfer);
    }

    // done
    return transfer->save.func(state, tb_stream_offset(transfer->istream), tb_stream_size(transfer->istream), transfer->save.saved, transfer->save.crate, transfer->save.priv);
}
static tb_bool_t tb_transfer_istream_read_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv);
static tb_bool_t tb_transfer_ostream_writ_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)priv;
    tb_assert_and_check_return_val(astream && transfer && transfer->istream, tb_false);

    // trace
    tb_trace_d("writ: real: %lu, size: %lu, state: %s", real, size, tb_state_cstr(state));

    // the time
    tb_hong_t time = tb_aicp_time(tb_async_stream_aicp(astream));

    // done
    tb_bool_t bwrit = tb_false;
    do
    {
        // ok?
        tb_check_break(state == TB_STATE_OK);
            
        // reset state
        state = TB_STATE_UNKNOWN_ERROR;

        // done func at first once
        if (!transfer->save.saved && !tb_transfer_istream_save_func(transfer, TB_STATE_OK)) break;

        // save size
        transfer->save.saved += real;
    
        // < 1s?
        tb_size_t delay = 0;
        tb_size_t lrate = tb_atomic_get(&transfer->lrate);
        if (time < transfer->save.base1s + 1000)
        {
            // save size for 1s
            transfer->save.saved1s += real;

            // save current rate if < 1s from base
            if (time < transfer->save.base + 1000) transfer->save.crate = transfer->save.saved1s;
                    
            // compute the delay for limit rate
            if (lrate) delay = transfer->save.saved1s >= lrate? transfer->save.base1s + 1000 - time : 0;
        }
        else
        {
            // save current rate
            transfer->save.crate = transfer->save.saved1s;

            // update base1s
            transfer->save.base1s = time;

            // reset size
            transfer->save.saved1s = 0;

            // reset delay
            delay = 0;

            // done func
            if (!tb_transfer_istream_save_func(transfer, TB_STATE_OK)) break;
        }

        // stoped?
        if (tb_atomic_get(&transfer->stoped))
        {
            state = TB_STATE_KILLED;
            break;
        }

        // not finished? continue to writ
        if (real < size) bwrit = tb_true;
        // pausing or paused?
        else if (tb_atomic_get(&transfer->pausing) || tb_atomic_get(&transfer->paused))
        {
            // paused
            tb_atomic_set(&transfer->paused, 1);
            
            // clear pausing
            tb_atomic_set0(&transfer->pausing);
    
            // done func
            if (!tb_transfer_istream_save_func(transfer, TB_STATE_OK)) break;
        }
        // continue?
        else 
        {
            // trace
            tb_trace_d("delay: %lu ms", delay);

            // continue to read it
            if (!tb_async_stream_read_after(transfer->istream, delay, lrate, tb_transfer_istream_read_func, (tb_pointer_t)transfer)) break;
        }

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed? 
    if (state != TB_STATE_OK) 
    {
        // compute the total rate
        transfer->save.crate = (transfer->save.saved && (time > transfer->save.base))? (tb_size_t)((transfer->save.saved * 1000) / (time - transfer->save.base)) : (tb_size_t)transfer->save.saved;

        // done func
        tb_transfer_istream_save_func(transfer, TB_STATE_OK);

        // break;
        bwrit = tb_false;
    }

    // continue to writ or break it
    return bwrit;
}
static tb_bool_t tb_transfer_ostream_sync_func(tb_async_stream_t* astream, tb_size_t state, tb_bool_t bclosing, tb_cpointer_t priv)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)priv;
    tb_assert_and_check_return_val(astream && transfer && transfer->istream, tb_false);

    // trace
    tb_trace_d("sync: state: %s", tb_state_cstr(state));

    // the time
    tb_hong_t time = tb_aicp_time(tb_async_stream_aicp(astream));

    // compute the total rate
    transfer->save.crate = (transfer->save.saved && (time > transfer->save.base))? (tb_size_t)((transfer->save.saved * 1000) / (time - transfer->save.base)) : (tb_size_t)transfer->save.saved;

    // done func
    return tb_transfer_istream_save_func(transfer, state == TB_STATE_OK? TB_STATE_CLOSED : state);
}
static tb_bool_t tb_transfer_istream_read_func(tb_async_stream_t* astream, tb_size_t state, tb_byte_t const* data, tb_size_t real, tb_size_t size, tb_cpointer_t priv)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)priv;
    tb_assert_and_check_return_val(astream && transfer && transfer->ostream, tb_false);

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

        // stoped?
        if (tb_atomic_get(&transfer->stoped))
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
        if (!tb_async_stream_writ(transfer->ostream, data, real, tb_transfer_ostream_writ_func, transfer)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // closed or failed?
    if (state != TB_STATE_OK) 
    {
        // sync it if closed
        tb_bool_t bend = tb_true;
        if (state == TB_STATE_CLOSED)
            bend = tb_async_stream_sync(transfer->ostream, tb_true, tb_transfer_ostream_sync_func, transfer)? tb_false : tb_true;

        // end? 
        if (bend)
        {
            // the time
            tb_hong_t time = tb_aicp_time(tb_async_stream_aicp(astream));

            // compute the total rate
            transfer->save.crate = (transfer->save.saved && (time > transfer->save.base))? (tb_size_t)((transfer->save.saved * 1000) / (time - transfer->save.base)) : (tb_size_t)transfer->save.saved;

            // done func
            tb_transfer_istream_save_func(transfer, state);
        }

        // break
        bread = tb_false;
    }

    // continue to read or break it
    return bread;
}
static tb_bool_t tb_transfer_ostream_open_func(tb_async_stream_t* astream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)priv;
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

        // stoped?
        if (tb_atomic_get(&transfer->stoped))
        {
            state = TB_STATE_KILLED;
            break;
        }

        // save opened
        tb_atomic_set(&transfer->opened, 1);

        // done func
        ok = transfer->open.func(TB_STATE_OK, tb_stream_offset(transfer->istream), tb_stream_size(transfer->istream), transfer->open.priv);

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed?
    if (state != TB_STATE_OK) 
    {
        // stoped
        tb_atomic_set(&transfer->stoped, 1);

        // done func
        ok = transfer->open.func(state, 0, 0, transfer->open.priv);
    }

    // ok
    return ok;
}
static tb_bool_t tb_transfer_istream_open_func(tb_async_stream_t* astream, tb_size_t state, tb_hize_t offset, tb_cpointer_t priv)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)priv;
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
            
        // stoped?
        if (tb_atomic_get(&transfer->stoped))
        {
            state = TB_STATE_KILLED;
            break;
        }

        // open it
        if (!tb_async_stream_open(transfer->ostream, tb_transfer_ostream_open_func, transfer)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed?
    if (state != TB_STATE_OK) 
    {
        // stoped
        tb_atomic_set(&transfer->stoped, 1);

        // done func
        ok = transfer->open.func(state, 0, 0, transfer->open.priv);
    }

    // ok?
    return ok;
}
static tb_bool_t tb_transfer_open_save_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_cpointer_t priv)
{
    // the transfer
    tb_transfer_t* transfer = (tb_transfer_t*)priv;
    tb_assert_and_check_return_val(transfer && transfer->save.func, tb_false);

    // trace
    tb_trace_d("open_save: offset: %llu, size: %lld, state: %s", offset, size, tb_state_cstr(state));

    // done
    tb_bool_t ok = tb_true;
    do
    {
        // ok? 
        tb_check_break(state == TB_STATE_OK);

        // reset state
        state = TB_STATE_UNKNOWN_ERROR;
        
        // stoped?
        if (tb_atomic_get(&transfer->stoped))
        {
            state = TB_STATE_KILLED;
            break;
        }

        // save it
        if (!tb_transfer_save(transfer, transfer->save.func, transfer->save.priv)) break;

        // ok
        state = TB_STATE_OK;

    } while (0);

    // failed? 
    if (state != TB_STATE_OK) 
    {   
        // stoped
        tb_atomic_set(&transfer->stoped, 1);

        // done func
        ok = tb_transfer_istream_save_func(transfer, state);
    }

    // ok?
    return ok;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_hong_t tb_transfer_save_gg(tb_basic_stream_t* istream, tb_basic_stream_t* ostream, tb_size_t lrate, tb_transfer_save_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(ostream && istream, -1); 

    // open it first if istream have been not opened
    if (tb_stream_is_closed(istream) && tb_stream_is_closed(istream)) return -1;
    
    // open it first if ostream have been not opened
    if (tb_stream_is_closed(ostream) && tb_stream_is_closed(ostream)) return -1;
                
    // done func
    if (func) func(TB_STATE_OK, tb_stream_offset(istream), tb_stream_size(istream), 0, 0, priv);

    // writ data
    tb_byte_t   data[TB_BASIC_STREAM_BLOCK_MAXN];
    tb_hize_t   writ = 0;
    tb_hize_t   left = tb_stream_left(istream);
    tb_hong_t   base = tb_cache_time_spak();
    tb_hong_t   base1s = base;
    tb_hong_t   time = 0;
    tb_size_t   crate = 0;
    tb_long_t   delay = 0;
    tb_hize_t   writ1s = 0;
    do
    {
        // the need
        tb_size_t need = lrate? tb_min(lrate, TB_BASIC_STREAM_BLOCK_MAXN) : TB_BASIC_STREAM_BLOCK_MAXN;

        // read data
        tb_long_t real = tb_basic_stream_read(istream, data, need);
        if (real > 0)
        {
            // writ data
            if (!tb_basic_stream_bwrit(ostream, data, real)) break;

            // save writ
            writ += real;

            // has func or limit rate?
            if (func || lrate) 
            {
                // the time
                time = tb_cache_time_spak();

                // < 1s?
                if (time < base1s + 1000)
                {
                    // save writ1s
                    writ1s += real;

                    // save current rate if < 1s from base
                    if (time < base + 1000) crate = writ1s;
                
                    // compute the delay for limit rate
                    if (lrate) delay = writ1s >= lrate? base1s + 1000 - time : 0;
                }
                else
                {
                    // save current rate
                    crate = writ1s;

                    // update base1s
                    base1s = time;

                    // reset writ1s
                    writ1s = 0;

                    // reset delay
                    delay = 0;

                    // done func
                    if (func) func(TB_STATE_OK, tb_stream_offset(istream), tb_stream_size(istream), writ, crate, priv);
                }

                // wait some time for limit rate
                if (delay) tb_msleep(delay);
            }
        }
        else if (!real) 
        {
            // wait
            tb_long_t wait = tb_basic_stream_wait(istream, TB_BASIC_STREAM_WAIT_READ, tb_stream_timeout(istream));
            tb_assert_and_check_break(wait >= 0);

            // timeout?
            tb_check_break(wait);

            // has writ?
            tb_assert_and_check_break(wait & TB_BASIC_STREAM_WAIT_READ);
        }
        else break;

        // is end?
        if (left && writ >= left) break;

    } while(1);

    // sync the ostream
    if (!tb_basic_stream_sync(ostream, tb_true)) return -1;

    // has func?
    if (func) 
    {
        // the time
        time = tb_cache_time_spak();

        // compute the total rate
        tb_size_t trate = (writ && (time > base))? (tb_size_t)((writ * 1000) / (time - base)) : writ;
    
        // done func
        func(TB_STATE_CLOSED, tb_stream_offset(istream), tb_stream_size(istream), writ, trate, priv);
    }

    // ok?
    return writ;
}
tb_hong_t tb_transfer_save_gu(tb_basic_stream_t* istream, tb_char_t const* ourl, tb_size_t lrate, tb_transfer_save_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(istream && ourl, -1);

    // done
    tb_hong_t       size = -1;
    tb_basic_stream_t*  ostream = tb_null;
    do
    {
        // init ostream
        ostream = tb_basic_stream_init_from_url(ourl);
        tb_assert_and_check_break(ostream);

        // ctrl file
        if (tb_stream_type(ostream) == TB_STREAM_TYPE_FILE) 
        {
            // ctrl mode
            if (!tb_stream_ctrl(ostream, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC)) break;
        }

        // save stream
        size = tb_transfer_save_gg(istream, ostream, lrate, func, priv);

    } while (0);

    // exit ostream
    if (ostream) tb_basic_stream_exit(ostream);
    ostream = tb_null;

    // ok?
    return size;
}
tb_hong_t tb_transfer_save_gd(tb_basic_stream_t* istream, tb_byte_t* odata, tb_size_t osize, tb_size_t lrate, tb_transfer_save_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(istream && odata && osize, -1);

    // done
    tb_hong_t       size = -1;
    tb_basic_stream_t*  ostream = tb_null;
    do
    {
        // init ostream
        ostream = tb_basic_stream_init_from_data(odata, osize);
        tb_assert_and_check_break(ostream);

        // save stream
        size = tb_transfer_save_gg(istream, ostream, lrate, func, priv);

    } while (0);

    // exit ostream
    if (ostream) tb_basic_stream_exit(ostream);
    ostream = tb_null;

    // ok?
    return size;
}
tb_hong_t tb_transfer_save_uu(tb_char_t const* iurl, tb_char_t const* ourl, tb_size_t lrate, tb_transfer_save_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(iurl && ourl, -1);

    // done
    tb_hong_t       size = -1;
    tb_basic_stream_t*  istream = tb_null;
    tb_basic_stream_t*  ostream = tb_null;
    do
    {
        // init istream
        istream = tb_basic_stream_init_from_url(iurl);
        tb_assert_and_check_break(istream);

        // init ostream
        ostream = tb_basic_stream_init_from_url(ourl);
        tb_assert_and_check_break(ostream);

        // ctrl file
        if (tb_stream_type(ostream) == TB_STREAM_TYPE_FILE) 
        {
            // ctrl mode
            if (!tb_stream_ctrl(ostream, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC)) break;
        }

        // open istream
        if (!tb_basic_stream_open(istream)) break;
        
        // open ostream
        if (!tb_basic_stream_open(ostream)) break;

        // save stream
        size = tb_transfer_save_gg(istream, ostream, lrate, func, priv);

    } while (0);

    // exit istream
    if (istream) tb_basic_stream_exit(istream);
    istream = tb_null;

    // exit ostream
    if (ostream) tb_basic_stream_exit(ostream);
    ostream = tb_null;

    // ok?
    return size;
}
tb_hong_t tb_transfer_save_ug(tb_char_t const* iurl, tb_basic_stream_t* ostream, tb_size_t lrate, tb_transfer_save_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(iurl && ostream, -1);

    // done
    tb_hong_t       size = -1;
    tb_basic_stream_t*  istream = tb_null;
    do
    {
        // init istream
        istream = tb_basic_stream_init_from_url(iurl);
        tb_assert_and_check_break(istream);

        // save stream
        size = tb_transfer_save_gg(istream, ostream, lrate, func, priv);

    } while (0);

    // exit istream
    if (istream) tb_basic_stream_exit(istream);
    istream = tb_null;

    // ok?
    return size;
}
tb_hong_t tb_transfer_save_ud(tb_char_t const* iurl, tb_byte_t* odata, tb_size_t osize, tb_size_t lrate, tb_transfer_save_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(iurl && odata && osize, -1);

    // done
    tb_hong_t       size = -1;
    tb_basic_stream_t*  istream = tb_null;
    tb_basic_stream_t*  ostream = tb_null;
    do
    {
        // init istream
        istream = tb_basic_stream_init_from_url(iurl);
        tb_assert_and_check_break(istream);

        // init ostream
        ostream = tb_basic_stream_init_from_data(odata, osize);
        tb_assert_and_check_break(ostream);

        // save stream
        size = tb_transfer_save_gg(istream, ostream, lrate, func, priv);

    } while (0);

    // exit istream
    if (istream) tb_basic_stream_exit(istream);
    istream = tb_null;

    // exit ostream
    if (ostream) tb_basic_stream_exit(ostream);
    ostream = tb_null;

    // ok?
    return size;
}
tb_hong_t tb_transfer_save_du(tb_byte_t const* idata, tb_size_t isize, tb_char_t const* ourl, tb_size_t lrate, tb_transfer_save_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(idata && isize && ourl, -1);

    // done
    tb_hong_t       size = -1;
    tb_basic_stream_t*  istream = tb_null;
    tb_basic_stream_t*  ostream = tb_null;
    do
    {
        // init istream
        istream = tb_basic_stream_init_from_data(idata, isize);
        tb_assert_and_check_break(istream);

        // init ostream
        ostream = tb_basic_stream_init_from_url(ourl);
        tb_assert_and_check_break(ostream);

        // ctrl file
        if (tb_stream_type(ostream) == TB_STREAM_TYPE_FILE) 
        {
            // ctrl mode
            if (!tb_stream_ctrl(ostream, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC)) break;
        }

        // save stream
        size = tb_transfer_save_gg(istream, ostream, lrate, func, priv);

    } while (0);

    // exit istream
    if (istream) tb_basic_stream_exit(istream);
    istream = tb_null;

    // exit ostream
    if (ostream) tb_basic_stream_exit(ostream);
    ostream = tb_null;

    // ok?
    return size;
}
tb_hong_t tb_transfer_save_dg(tb_byte_t const* idata, tb_size_t isize, tb_basic_stream_t* ostream, tb_size_t lrate, tb_transfer_save_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(idata && isize && ostream, -1);

    // done
    tb_hong_t       size = -1;
    tb_basic_stream_t*  istream = tb_null;
    do
    {
        // init istream
        istream = tb_basic_stream_init_from_data(idata, isize);
        tb_assert_and_check_break(istream);

        // save stream
        size = tb_transfer_save_gg(istream, ostream, lrate, func, priv);

    } while (0);

    // exit istream
    if (istream) tb_basic_stream_exit(istream);
    istream = tb_null;

    // ok?
    return size;
}
#ifdef TB_CONFIG_MODULE_HAVE_ASIO
tb_handle_t tb_transfer_init_aa(tb_async_stream_t* istream, tb_async_stream_t* ostream, tb_hize_t offset)
{
    // done
    tb_transfer_t* transfer = tb_null;
    do
    {
        // check
        tb_assert_and_check_break(istream && ostream);  

        // make transfer
        transfer = tb_malloc0(sizeof(tb_transfer_t));
        tb_assert_and_check_break(transfer);

        // init transfer
        transfer->istream   = istream;
        transfer->ostream   = ostream;
        transfer->iowner    = 0;
        transfer->oowner    = 0;
        transfer->stoped    = 1;
        transfer->offset    = offset;

    } while (0);

    // ok?
    return (tb_handle_t)transfer;
}
tb_handle_t tb_transfer_init_au(tb_async_stream_t* istream, tb_char_t const* ourl, tb_hize_t offset)
{
    // done
    tb_async_stream_t*  ostream = tb_null;
    tb_transfer_t*      transfer = tb_null;
    do
    {
        // check
        tb_assert_and_check_break(istream && ourl);

        // init ostream
        ostream = tb_async_stream_init_from_url(tb_async_stream_aicp(istream), ourl);
        tb_assert_and_check_break(ostream);

        // ctrl file
        if (tb_stream_type(ostream) == TB_STREAM_TYPE_FILE) 
        {
            // ctrl mode
            if (!tb_stream_ctrl(ostream, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC)) break;
        }

        // make transfer
        transfer = tb_malloc0(sizeof(tb_transfer_t));
        tb_assert_and_check_break(transfer);

        // init transfer
        transfer->istream   = istream;
        transfer->ostream   = ostream;
        transfer->iowner    = 0;
        transfer->oowner    = 1;
        transfer->stoped    = 1;
        transfer->offset    = offset;
        
        // ok
        ostream = tb_null;

    } while (0);

    // exit ostream
    if (ostream) tb_async_stream_exit(ostream);
    ostream = tb_null;

    // ok?
    return (tb_handle_t)transfer;
}
tb_handle_t tb_transfer_init_uu(tb_aicp_t* aicp, tb_char_t const* iurl, tb_char_t const* ourl, tb_hize_t offset)
{
    // done
    tb_async_stream_t*  istream = tb_null;
    tb_async_stream_t*  ostream = tb_null;
    tb_transfer_t*      transfer = tb_null;
    do
    {
        // check
        tb_assert_and_check_break(aicp && iurl && ourl);

        // init istream
        istream = tb_async_stream_init_from_url(aicp, iurl);
        tb_assert_and_check_break(istream);

        // init ostream
        ostream = tb_async_stream_init_from_url(aicp, ourl);
        tb_assert_and_check_break(ostream);

        // ctrl file
        if (tb_stream_type(ostream) == TB_STREAM_TYPE_FILE) 
        {
            // ctrl mode
            if (!tb_stream_ctrl(ostream, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC)) break;
        }

        // make transfer
        transfer = tb_malloc0(sizeof(tb_transfer_t));
        tb_assert_and_check_break(transfer);

        // init transfer
        transfer->istream   = istream;
        transfer->ostream   = ostream;
        transfer->iowner    = 1;
        transfer->oowner    = 1;
        transfer->stoped    = 1;
        transfer->offset    = offset;
            
        // ok
        istream = tb_null;
        ostream = tb_null;

    } while (0);

    // exit istream
    if (istream) tb_async_stream_exit(istream);
    istream = tb_null;

    // exit ostream
    if (ostream) tb_async_stream_exit(ostream);
    ostream = tb_null;

    // ok?
    return (tb_handle_t)transfer;
}
tb_handle_t tb_transfer_init_ua(tb_char_t const* iurl, tb_async_stream_t* ostream, tb_hize_t offset)
{
    // done
    tb_async_stream_t*  istream = tb_null;
    tb_transfer_t*  transfer = tb_null;
    do
    {
        // check
        tb_assert_and_check_break(iurl && ostream);

        // init istream
        istream = tb_async_stream_init_from_url(tb_async_stream_aicp(ostream), iurl);
        tb_assert_and_check_break(istream);

        // make transfer
        transfer = tb_malloc0(sizeof(tb_transfer_t));
        tb_assert_and_check_break(transfer);

        // init transfer
        transfer->istream   = istream;
        transfer->ostream   = ostream;
        transfer->iowner    = 1;
        transfer->oowner    = 0;
        transfer->stoped    = 1;
        transfer->offset    = offset;
            
        // ok
        istream = tb_null;

    } while (0);

    // exit istream
    if (istream) tb_async_stream_exit(istream);
    istream = tb_null;

    // ok?
    return (tb_handle_t)transfer;
}
tb_handle_t tb_transfer_init_du(tb_aicp_t* aicp, tb_byte_t const* idata, tb_size_t isize, tb_char_t const* ourl, tb_hize_t offset)
{
    // done
    tb_async_stream_t*  istream = tb_null;
    tb_async_stream_t*  ostream = tb_null;
    tb_transfer_t*      transfer = tb_null;
    do
    {
        // check
        tb_assert_and_check_break(aicp && idata && isize && ourl);

        // init istream
        istream = tb_async_stream_init_from_data(aicp, idata, isize);
        tb_assert_and_check_break(istream);

        // init ostream
        ostream = tb_async_stream_init_from_url(aicp, ourl);
        tb_assert_and_check_break(ostream);

        // ctrl file
        if (tb_stream_type(ostream) == TB_STREAM_TYPE_FILE) 
        {
            // ctrl mode
            if (!tb_stream_ctrl(ostream, TB_STREAM_CTRL_FILE_SET_MODE, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC)) break;
        }

        // make transfer
        transfer = tb_malloc0(sizeof(tb_transfer_t));
        tb_assert_and_check_break(transfer);

        // init transfer
        transfer->istream   = istream;
        transfer->ostream   = ostream;
        transfer->iowner    = 1;
        transfer->oowner    = 1;
        transfer->stoped    = 1;
        transfer->offset    = offset;
    
        // ok
        istream = tb_null;
        ostream = tb_null;

    } while (0);

    // exit istream
    if (istream) tb_async_stream_exit(istream);
    istream = tb_null;

    // exit ostream
    if (ostream) tb_async_stream_exit(ostream);
    ostream = tb_null;

    // ok?
    return (tb_handle_t)transfer;
}
tb_handle_t tb_transfer_init_da(tb_byte_t const* idata, tb_size_t isize, tb_async_stream_t* ostream, tb_hize_t offset)
{
    // done
    tb_async_stream_t*  istream = tb_null;
    tb_transfer_t*      transfer = tb_null;
    do
    {
        // check
        tb_assert_and_check_break(idata && isize && ostream);

        // init istream
        istream = tb_async_stream_init_from_data(tb_async_stream_aicp(ostream), idata, isize);
        tb_assert_and_check_break(istream);

        // make transfer
        transfer = tb_malloc0(sizeof(tb_transfer_t));
        tb_assert_and_check_break(transfer);

        // init transfer
        transfer->istream   = istream;
        transfer->ostream   = ostream;
        transfer->iowner    = 1;
        transfer->oowner    = 0;
        transfer->stoped    = 1;
        transfer->offset    = offset;
            
        // ok
        istream = tb_null;

    } while (0);

    // exit istream
    if (istream) tb_async_stream_exit(istream);
    istream = tb_null;

    // ok?
    return (tb_handle_t)transfer;
}
tb_bool_t tb_transfer_ctrl(tb_handle_t handle, tb_transfer_ctrl_func_t func, tb_cpointer_t priv)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer, tb_false);

    // init func
    transfer->ctrl.func = func;
    transfer->ctrl.priv = priv;

    // ok
    return tb_true;
}
tb_bool_t tb_transfer_open(tb_handle_t handle, tb_transfer_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && func, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // check
        tb_assert_and_check_break(tb_atomic_get(&transfer->stoped));
        tb_assert_and_check_break(!tb_atomic_get(&transfer->opened));

        // clear state
        tb_atomic_set0(&transfer->stoped);
        tb_atomic_set0(&transfer->paused);

        // init func
        transfer->open.func = func;
        transfer->open.priv = priv;

        // check
        tb_assert_and_check_break(transfer->istream);
        tb_assert_and_check_break(transfer->ostream);

        // init some rate info
        transfer->save.base      = tb_aicp_time(tb_async_stream_aicp(transfer->istream));
        transfer->save.base1s    = transfer->save.base;
        transfer->save.saved     = 0;
        transfer->save.saved1s   = 0;
        transfer->save.crate     = 0;

        // ctrl stream
        if (transfer->ctrl.func && !transfer->ctrl.func(transfer->istream, transfer->ostream, transfer->ctrl.priv)) break;

        // open and seek istream
        if (!tb_async_stream_open_seek(transfer->istream, transfer->offset, tb_transfer_istream_open_func, transfer)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed? stoped
    if (!ok) tb_atomic_set(&transfer->stoped, 1);

    // ok?
    return ok;
}
tb_bool_t tb_transfer_save(tb_handle_t handle, tb_transfer_save_func_t func, tb_cpointer_t priv)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && func, tb_false);

    // check state
    tb_assert_and_check_return_val(!tb_atomic_get(&transfer->stoped), tb_false);
    tb_assert_and_check_return_val(tb_atomic_get(&transfer->opened), tb_false);
    
    // check stream
    tb_assert_and_check_return_val(transfer->istream && transfer->ostream, tb_false);

    // save func
    transfer->save.func = func;
    transfer->save.priv = priv;

    // read it
    return tb_async_stream_read(transfer->istream, (tb_size_t)tb_atomic_get(&transfer->lrate), tb_transfer_istream_read_func, transfer);
}
tb_bool_t tb_transfer_open_save(tb_handle_t handle, tb_transfer_save_func_t func, tb_cpointer_t priv)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && func, tb_false);

    // no opened? open it first
    if (!tb_atomic_get(&transfer->opened))
    {
        transfer->save.func = func;
        transfer->save.priv = priv;
        return tb_transfer_open(transfer, tb_transfer_open_save_func, transfer);
    }

    // save it
    return tb_transfer_save(transfer, func, priv);
}
tb_void_t tb_transfer_kill(tb_handle_t handle)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)handle;
    tb_assert_and_check_return(transfer);

    // stop it
    if (tb_atomic_get(&transfer->opened) && !tb_atomic_fetch_and_set(&transfer->stoped, 1))
    {
        // trace
        tb_trace_d("kill: ..");

        // kill istream
        if (transfer->istream) tb_stream_kill(transfer->istream);

        // kill ostream
        if (transfer->ostream) tb_stream_kill(transfer->ostream);
    }
}
tb_bool_t tb_transfer_exit(tb_handle_t handle)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer, tb_false);

    // trace
    tb_trace_d("exit: ..");

    // kill it first
    tb_transfer_kill(handle);

    // wait for closing
    tb_size_t tryn = 30;
    while (tb_atomic_get(&transfer->opened) && tryn--)
    {
        // trace
        tb_trace_d("exit: wait: ..");

        // wait some time
        tb_msleep(200);
    }
    tb_assert_and_check_return_val(!tb_atomic_get(&transfer->opened), tb_false);

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
tb_void_t tb_transfer_pause(tb_handle_t handle)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)handle;
    tb_assert_and_check_return(transfer);

    // have been paused?
    tb_check_return(!tb_atomic_get(&transfer->paused));

    // pause it
    tb_atomic_set(&transfer->pausing, 1);
}
tb_bool_t tb_transfer_resume(tb_handle_t handle)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)handle;
    tb_assert_and_check_return_val(transfer && transfer->istream && transfer->ostream, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // stoped?
        tb_assert_and_check_break(!tb_atomic_get(&transfer->stoped));

        // not opened? failed
        tb_check_break(tb_atomic_get(&transfer->opened));

        // pausing? failed
        tb_check_break(!tb_atomic_get(&transfer->pausing));

        // not paused? resume ok
        tb_check_return_val(tb_atomic_fetch_and_set0(&transfer->paused), tb_true);

        // init some rate info
        transfer->save.base      = tb_aicp_time(tb_async_stream_aicp(transfer->istream));
        transfer->save.base1s    = transfer->save.base;
        transfer->save.saved1s   = 0;
        transfer->save.crate     = 0;

        // read it
        if (!tb_async_stream_read(transfer->istream, (tb_size_t)tb_atomic_get(&transfer->lrate), tb_transfer_istream_read_func, transfer)) 
        {
            // continue to be paused
            tb_atomic_set(&transfer->paused, 1);
            break;
        }

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    return ok;
}
tb_void_t tb_transfer_limitrate(tb_handle_t handle, tb_size_t rate)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)handle;
    tb_assert_and_check_return(transfer);

    // set the limit rate
    tb_atomic_set(&transfer->lrate, rate);
}
tb_void_t tb_transfer_timeout_set(tb_handle_t handle, tb_long_t timeout)
{
    // check
    tb_transfer_t* transfer = (tb_transfer_t*)handle;
    tb_assert_and_check_return(transfer && transfer->istream && transfer->ostream);

    // set timeout
    if (timeout)
    {
        // check
        tb_assert_and_check_return(tb_atomic_get(&transfer->stoped));
        tb_assert_and_check_return(!tb_atomic_get(&transfer->opened));

        // ctrl it
        tb_stream_ctrl(transfer->istream, TB_STREAM_CTRL_SET_TIMEOUT, timeout);
        tb_stream_ctrl(transfer->ostream, TB_STREAM_CTRL_SET_TIMEOUT, timeout);
    }
}
#endif
