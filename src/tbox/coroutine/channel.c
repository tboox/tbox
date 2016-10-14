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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        channel.h
 * @ingroup     channel
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "channel"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "channel.h"
#include "coroutine.h"
#include "scheduler.h"
#include "../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the channel type
typedef struct __tb_channel_t
{
}tb_channel_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_channel_ref_t tb_channel_init()
{
    // done
    tb_bool_t       ok = tb_false;
    tb_channel_t*   channel = tb_null;
    do
    {
        // make channel
        channel = tb_malloc0_type(tb_channel_t);
        tb_assert_and_check_break(channel);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (channel) tb_channel_exit((tb_channel_ref_t)channel);
        channel = tb_null;
    }

    // ok?
    return (tb_channel_ref_t)channel;
}
tb_void_t tb_channel_exit(tb_channel_ref_t self)
{
    // check
    tb_channel_t* channel = (tb_channel_t*)self;
    tb_assert_and_check_return(channel);

    // exit the channel
    tb_free(channel);
}

