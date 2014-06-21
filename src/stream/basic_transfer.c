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
 * @file        basic_transfer.c
 * @ingroup     stream
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "basic_transfer"
#define TB_TRACE_MODULE_DEBUG               (1)
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stream.h"
#include "basic_transfer.h"
#include "../network/network.h"
#include "../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_hong_t tb_basic_transfer_done(tb_basic_stream_t* istream, tb_basic_stream_t* ostream, tb_size_t lrate, tb_basic_transfer_done_func_t func, tb_cpointer_t priv)
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
    tb_size_t   writ1s = 0;
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
                    if (lrate) delay = writ1s >= lrate? (tb_size_t)(base1s + 1000 - time) : 0;
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
        tb_size_t trate = (writ && (time > base))? (tb_size_t)((writ * 1000) / (time - base)) : (tb_size_t)writ;
    
        // done func
        func(TB_STATE_CLOSED, tb_stream_offset(istream), tb_stream_size(istream), writ, trate, priv);
    }

    // ok?
    return writ;
}
tb_hong_t tb_basic_transfer_done_stream_to_url(tb_basic_stream_t* istream, tb_char_t const* ourl, tb_size_t lrate, tb_basic_transfer_done_func_t func, tb_cpointer_t priv)
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
        size = tb_basic_transfer_done(istream, ostream, lrate, func, priv);

    } while (0);

    // exit ostream
    if (ostream) tb_basic_stream_exit(ostream);
    ostream = tb_null;

    // ok?
    return size;
}
tb_hong_t tb_basic_transfer_done_stream_to_data(tb_basic_stream_t* istream, tb_byte_t* odata, tb_size_t osize, tb_size_t lrate, tb_basic_transfer_done_func_t func, tb_cpointer_t priv)
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
        size = tb_basic_transfer_done(istream, ostream, lrate, func, priv);

    } while (0);

    // exit ostream
    if (ostream) tb_basic_stream_exit(ostream);
    ostream = tb_null;

    // ok?
    return size;
}
tb_hong_t tb_basic_transfer_done_url_to_url(tb_char_t const* iurl, tb_char_t const* ourl, tb_size_t lrate, tb_basic_transfer_done_func_t func, tb_cpointer_t priv)
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
        size = tb_basic_transfer_done(istream, ostream, lrate, func, priv);

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
tb_hong_t tb_basic_transfer_done_url_to_stream(tb_char_t const* iurl, tb_basic_stream_t* ostream, tb_size_t lrate, tb_basic_transfer_done_func_t func, tb_cpointer_t priv)
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
        size = tb_basic_transfer_done(istream, ostream, lrate, func, priv);

    } while (0);

    // exit istream
    if (istream) tb_basic_stream_exit(istream);
    istream = tb_null;

    // ok?
    return size;
}
tb_hong_t tb_basic_transfer_done_url_to_data(tb_char_t const* iurl, tb_byte_t* odata, tb_size_t osize, tb_size_t lrate, tb_basic_transfer_done_func_t func, tb_cpointer_t priv)
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
        size = tb_basic_transfer_done(istream, ostream, lrate, func, priv);

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
tb_hong_t tb_basic_transfer_done_data_to_url(tb_byte_t const* idata, tb_size_t isize, tb_char_t const* ourl, tb_size_t lrate, tb_basic_transfer_done_func_t func, tb_cpointer_t priv)
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
        size = tb_basic_transfer_done(istream, ostream, lrate, func, priv);

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
tb_hong_t tb_basic_transfer_done_data_to_stream(tb_byte_t const* idata, tb_size_t isize, tb_basic_stream_t* ostream, tb_size_t lrate, tb_basic_transfer_done_func_t func, tb_cpointer_t priv)
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
        size = tb_basic_transfer_done(istream, ostream, lrate, func, priv);

    } while (0);

    // exit istream
    if (istream) tb_basic_stream_exit(istream);
    istream = tb_null;

    // ok?
    return size;
}

