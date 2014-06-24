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
 * @file        prefix.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_stream_impl_init(tb_stream_impl_t* stream, tb_size_t type, tb_size_t cache)
{
    // check
    tb_assert_and_check_return_val(stream, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // init type
        stream->type = type;

        // init timeout, 10s
        stream->timeout = TB_STREAM_DEFAULT_TIMEOUT;

        // init internal state
        stream->istate = TB_STATE_CLOSED;

        // init url
        if (!tb_url_init(&stream->url)) break;

        // init cache
        if (!tb_queue_buffer_init(&stream->cache, cache)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed? exit it
    if (!ok) tb_queue_buffer_exit(&stream->cache);

    // ok?
    return ok;
}
