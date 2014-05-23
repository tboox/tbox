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

// the async transfer type
typedef struct __tb_async_transfer_t
{
    // the aicp
    tb_aicp_t*                      aicp;

    // the istream
    tb_async_stream_t*              istream;

    // the ostream
    tb_async_stream_t*              ostream;

    // the istream is owner?
    tb_uint8_t                      iowner : 1;

    // the ostream is owner?
    tb_uint8_t                      oowner : 1;

    /* state
     *
     * TB_STATE_CLOSED
     * TB_STATE_OPENED
     * TB_STATE_OPENING
     * TB_STATE_KILLING
     */
    tb_atomic_t                     state;

    /* pause state
     *
     * TB_STATE_OK
     * TB_STATE_PAUSED
     * TB_STATE_PAUSING
     */
    tb_atomic_t                     state_pause;

    // the start offset
    tb_hize_t                       start_offset;

    // the limited rate
    tb_atomic_t                     limited_rate;

}tb_async_transfer_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_async_transfer_init(tb_aicp_t* aicp)
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

        // protocol => type
        tb_size_t type = TB_STREAM_TYPE_NONE;
        switch (protocol)
        {
        case TB_URL_PROTOCOL_FILE: type = TB_STREAM_TYPE_FILE; break;
        case TB_URL_PROTOCOL_HTTP: type = TB_STREAM_TYPE_HTTP; break;
        case TB_URL_PROTOCOL_SOCK: type = TB_STREAM_TYPE_SOCK; break;
        case TB_URL_PROTOCOL_DATA: type = TB_STREAM_TYPE_DATA; break;
        default:
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
tb_bool_t tb_async_transfer_init_istream_from_data(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
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
        transfer->istream = tb_async_stream_init_from_data(transfer->aicp, data, size);
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

        // protocol => type
        tb_size_t type = TB_STREAM_TYPE_NONE;
        switch (protocol)
        {
        case TB_URL_PROTOCOL_FILE: type = TB_STREAM_TYPE_FILE; break;
        case TB_URL_PROTOCOL_HTTP: type = TB_STREAM_TYPE_HTTP; break;
        case TB_URL_PROTOCOL_SOCK: type = TB_STREAM_TYPE_SOCK; break;
        case TB_URL_PROTOCOL_DATA: type = TB_STREAM_TYPE_DATA; break;
        default:
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
    return tb_false;
}
tb_bool_t tb_async_transfer_ctrl_ostream(tb_handle_t handle, tb_size_t ctrl, ...)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_open(tb_handle_t handle, tb_hize_t offset, tb_async_transfer_open_func_t func, tb_cpointer_t priv)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_done(tb_handle_t handle, tb_async_transfer_done_func_t func, tb_cpointer_t priv)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_open_done(tb_handle_t handle, tb_hize_t offset, tb_async_transfer_done_func_t func, tb_cpointer_t priv)
{
    return tb_false;
}
tb_void_t tb_async_transfer_kill(tb_handle_t handle)
{
}
tb_bool_t tb_async_transfer_exit(tb_handle_t handle)
{
    return tb_false;
}
tb_void_t tb_async_transfer_pause(tb_handle_t handle)
{
}
tb_bool_t tb_async_transfer_resume(tb_handle_t handle)
{
    return tb_false;
}
tb_void_t tb_async_transfer_limitrate(tb_handle_t handle, tb_size_t rate)
{
}
tb_void_t tb_async_transfer_timeout_set(tb_handle_t handle, tb_long_t timeout)
{
}
