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
 * @author		ruki
 * @file		prefix.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "async_stream"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_async_stream_clos_opening(tb_async_stream_t* stream, tb_size_t state, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(stream);

    // trace
    tb_trace_d("clos: opening: %s, state: %s", tb_url_get(&stream->base.url), tb_state_cstr(stream->clos_opening.state));

    // closed
	tb_atomic_set(&stream->base.istate, TB_STATE_CLOSED);

    // done func
    if (stream->clos_opening.func) stream->clos_opening.func(stream, stream->clos_opening.state, stream->clos_opening.priv);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * private interfaces
 */
tb_void_t tb_async_stream_clear(tb_async_stream_t* stream)
{
    // check
    tb_assert_and_check_return(stream);

	// clear rcache
	tb_scoped_buffer_clear(&stream->rcache_data);

	// clear wcache
	tb_scoped_buffer_clear(&stream->wcache_data);

    // clear istate
	tb_atomic_set(&stream->base.istate, TB_STATE_CLOSED);
}
tb_bool_t tb_async_stream_open_func(tb_async_stream_t* stream, tb_size_t state, tb_async_stream_open_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return_val(stream && stream->clos, tb_false);

    // ok?
    tb_bool_t ok = tb_true;
    if (state == TB_STATE_OK) 
    {
        // opened
        tb_atomic_set(&stream->base.istate, TB_STATE_OPENED);

        // done func
        if (func) ok = func(stream, state, priv);
    }
    // failed? 
    else 
    {
        // init func and state
        stream->clos_opening.func   = func;
        stream->clos_opening.priv   = priv;
        stream->clos_opening.state  = state;

        // close it
        ok = stream->clos(stream, tb_async_stream_clos_opening, tb_null);
    }

    // ok?
    return ok;
}

