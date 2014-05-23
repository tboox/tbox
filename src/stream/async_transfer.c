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

// the transfer type
typedef struct __tb_async_transfer_t
{


}tb_async_transfer_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces implementation
 */
tb_handle_t tb_async_transfer_init()
{
    return tb_null;
}
tb_bool_t tb_async_transfer_init_istream(tb_handle_t handle, tb_async_stream_t* stream)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_init_istream_from_url(tb_handle_t handle, tb_char_t const* url)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_init_istream_from_data(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_init_ostream(tb_handle_t handle, tb_async_stream_t* stream)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_init_ostream_from_url(tb_handle_t handle, tb_char_t const* url)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_init_ostream_from_data(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_ctrl_istream(tb_handle_t handle, tb_size_t ctrl, ...)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_ctrl_ostream(tb_handle_t handle, tb_size_t ctrl, ...)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_open(tb_handle_t handle, tb_async_transfer_open_func_t func, tb_cpointer_t priv)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_done(tb_handle_t handle, tb_async_transfer_done_func_t func, tb_cpointer_t priv)
{
    return tb_false;
}
tb_bool_t tb_async_transfer_open_done(tb_handle_t handle, tb_async_transfer_done_func_t func, tb_cpointer_t priv)
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
