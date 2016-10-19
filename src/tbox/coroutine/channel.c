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
 * @ingroup     coroutine
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

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the coroutine channel type
typedef struct __tb_co_channel_t
{
    // the queue
    tb_circle_queue_ref_t   queue;

    // the channel size
    tb_size_t               size;
   
    // the first data is dirty?
    tb_bool_t               dirty;

    // the send semaphore 
    tb_co_semaphore_ref_t   send;

    // the recv semaphore 
    tb_co_semaphore_ref_t   recv;

}tb_co_channel_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_co_channel_ref_t tb_co_channel_init(tb_size_t size, tb_element_t element)
{
    // done
    tb_bool_t           ok = tb_false;
    tb_co_channel_t*    channel = tb_null;
    do
    {
        // make channel
        channel = tb_malloc0_type(tb_co_channel_t);
        tb_assert_and_check_break(channel);

        // init queue 
        channel->queue = tb_circle_queue_init(size, element);
        tb_assert_and_check_break(channel->queue);

        // save the channel size
        channel->size = size;

        // init dirty
        channel->dirty = tb_false;

        // init send semaphore
        channel->send = tb_co_semaphore_init(0);
        tb_assert_and_check_break(channel->send);

        // init recv semaphore
        channel->recv = tb_co_semaphore_init(0);
        tb_assert_and_check_break(channel->recv);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (channel) tb_co_channel_exit((tb_co_channel_ref_t)channel);
        channel = tb_null;
    }

    // ok?
    return (tb_co_channel_ref_t)channel;
}
tb_void_t tb_co_channel_exit(tb_co_channel_ref_t self)
{
    // check
    tb_co_channel_t* channel = (tb_co_channel_t*)self;
    tb_assert_and_check_return(channel);

    // exit queue
    if (channel->queue) tb_circle_queue_exit(channel->queue);
    channel->queue = tb_null;

    // exit send semaphore
    if (channel->send) tb_co_semaphore_exit(channel->send);
    channel->send = tb_null;

    // exit recv semaphore
    if (channel->recv) tb_co_semaphore_exit(channel->recv);
    channel->recv = tb_null;

    // exit the channel
    tb_free(channel);
}
tb_void_t tb_co_channel_send(tb_co_channel_ref_t self, tb_cpointer_t data)
{
    // check
    tb_co_channel_t* channel = (tb_co_channel_t*)self;
    tb_assert_and_check_return(channel && channel->queue && channel->send && channel->recv);

    // done
    do
    {
        // exists dirty data?
        if (channel->dirty)
        {
            // check
            tb_assert(!tb_circle_queue_null(channel->queue));

            // trace
            tb_trace_d("send[%p]: clear dirty", tb_coroutine_self());

            // pop the dirty data first
            tb_circle_queue_pop(channel->queue);
            channel->dirty = tb_false;
        }

        // put data into queue if be not full
        tb_bool_t is_full = tb_circle_queue_full(channel->queue);
        if (!is_full)
        {
            // trace
            tb_trace_d("send[%p]: put data(%p)", tb_coroutine_self(), data);

            // put data
            tb_circle_queue_put(channel->queue, data);

            // notify to recv data
            tb_co_semaphore_post(channel->recv, 1);

            // send ok
            break;
        }
        // wait it if be full
        else
        {
            // trace
            tb_trace_d("send[%p]: wait(%lu) ..", tb_coroutine_self(), tb_co_semaphore_value(channel->send));

            // wait send
            tb_long_t ok = tb_co_semaphore_wait(channel->send, -1);
            tb_assert_and_check_return(ok > 0);
 
            // trace
            tb_trace_d("send[%p]: wait(%lu) ok", tb_coroutine_self(), tb_co_semaphore_value(channel->send));
        }

    } while (1);
 
    // trace
    tb_trace_d("send[%p]: ok", tb_coroutine_self());
}
tb_pointer_t tb_co_channel_recv(tb_co_channel_ref_t self)
{
    // check
    tb_co_channel_t* channel = (tb_co_channel_t*)self;
    tb_assert_and_check_return_val(channel && channel->queue && channel->send && channel->recv, tb_null);

    // done
    tb_pointer_t data = tb_null;
    do
    {
        // exists dirty data?
        if (channel->dirty)
        {
            // check
            tb_assert(!tb_circle_queue_null(channel->queue));

            // trace
            tb_trace_d("recv[%p]: clear dirty", tb_coroutine_self());

            // pop the dirty data first
            tb_circle_queue_pop(channel->queue);
            channel->dirty = tb_false;
        }

        // recv data from channel if be not null
        tb_bool_t is_null = tb_circle_queue_null(channel->queue);
        if (!is_null)
        {
            // get data
            data = tb_circle_queue_get(channel->queue);

            // trace
            tb_trace_d("recv[%p]: get data(%p)", tb_coroutine_self(), data);

            // mark it as dirty and pop it delay
            channel->dirty = tb_true;

            // notify to send data
            tb_co_semaphore_post(channel->send, 1);

            // recv ok
            break;
        }
        // wait it if be null
        else
        {
            // trace
            tb_trace_d("recv[%p]: wait(%p) ..", tb_coroutine_self(), tb_co_semaphore_value(channel->send));

            // wait recv
            tb_long_t ok = tb_co_semaphore_wait(channel->recv, -1);
            tb_assert_and_check_return_val(ok > 0, tb_null);

            // trace
            tb_trace_d("recv[%p]: wait(%p) ok", tb_coroutine_self(), tb_co_semaphore_value(channel->send));
        }

    } while (1);
 
    // trace
    tb_trace_d("recv[%p]: ok", tb_coroutine_self());

    // get data
    return data;
}

