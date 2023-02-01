/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        poller_poll.c
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../time.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"
#include <poll.h>
#include <fcntl.h>
#include <errno.h>
#if defined(TB_CONFIG_OS_HAIKU)
#   include <unistd.h>
#elif !defined(TB_CONFIG_OS_ANDROID)
#   include <sys/unistd.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the poller poll type
typedef struct __tb_poller_poll_t
{
    // the poller base
    tb_poller_t             base;

    // the pair sockets for spak, kill ..
    tb_socket_ref_t         pair[2];

    // the poll fds
    tb_vector_ref_t         pfds;

    // the copied poll fds
    tb_vector_ref_t         cfds;

    // the socket data
    tb_pollerdata_t           pollerdata;

}tb_poller_poll_t, *tb_poller_poll_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_bool_t tb_poller_poll_walk_remove(tb_iterator_ref_t iterator, tb_cpointer_t item, tb_cpointer_t priv)
{
    // check
    tb_assert(priv);

    // the fd
    tb_long_t fd = (tb_long_t)priv;

    // the poll fd
    struct pollfd* pfd = (struct pollfd*)item;

    // remove it?
    return (pfd && pfd->fd == fd);
}
static tb_bool_t tb_poller_poll_walk_modify(tb_iterator_ref_t iterator, tb_pointer_t item, tb_cpointer_t priv)
{
    // check
    tb_value_ref_t tuple = (tb_value_ref_t)priv;
    tb_assert(tuple);

    // the fd
    tb_long_t fd = tuple[0].l;

    // is this?
    struct pollfd* pfd = (struct pollfd*)item;
    if (pfd && pfd->fd == fd)
    {
        // the events
        tb_size_t events = tuple[1].ul;

        // modify events
        pfd->events = 0;
        if (events & TB_POLLER_EVENT_RECV) pfd->events |= POLLIN;
        if (events & TB_POLLER_EVENT_SEND) pfd->events |= POLLOUT;

        // break
        return tb_false;
    }

    // ok
    return tb_true;
}
static tb_void_t tb_poller_poll_exit(tb_poller_t* self)
{
    // check
    tb_poller_poll_ref_t poller = (tb_poller_poll_ref_t)self;
    tb_assert_and_check_return(poller);

    // exit pair sockets
    if (poller->pair[0]) tb_socket_exit(poller->pair[0]);
    if (poller->pair[1]) tb_socket_exit(poller->pair[1]);
    poller->pair[0] = tb_null;
    poller->pair[1] = tb_null;

    // close pfds
    if (poller->pfds) tb_vector_exit(poller->pfds);
    poller->pfds = tb_null;

    // close cfds
    if (poller->cfds) tb_vector_exit(poller->cfds);
    poller->cfds = tb_null;

    // exit socket data
    tb_pollerdata_exit(&poller->pollerdata);

    // free it
    tb_free(poller);
}
static tb_void_t tb_poller_poll_kill(tb_poller_t* self)
{
    // check
    tb_poller_poll_ref_t poller = (tb_poller_poll_ref_t)self;
    tb_assert_and_check_return(poller);

    // kill it
    if (poller->pair[0]) tb_socket_send(poller->pair[0], (tb_byte_t const*)"k", 1);
}
static tb_void_t tb_poller_poll_spak(tb_poller_t* self)
{
    // check
    tb_poller_poll_ref_t poller = (tb_poller_poll_ref_t)self;
    tb_assert_and_check_return(poller);

    // post it
    if (poller->pair[0]) tb_socket_send(poller->pair[0], (tb_byte_t const*)"p", 1);
}
static tb_bool_t tb_poller_poll_insert(tb_poller_t* self, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_poll_ref_t poller = (tb_poller_poll_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->pfds && object, tb_false);

    // oneshot is not supported now
    tb_assertf(!(events & TB_POLLER_EVENT_ONESHOT), "cannot insert events with oneshot, not supported!");

    // init events
    struct pollfd pfd = {0};
    if (events & TB_POLLER_EVENT_RECV) pfd.events |= POLLIN;
    if (events & TB_POLLER_EVENT_SEND) pfd.events |= POLLOUT;

    // save fd, TODO uses binary search
    pfd.fd = tb_ptr2fd(object->ref.ptr);
    tb_vector_insert_tail(poller->pfds, &pfd);

    // bind the object type to the private data
    priv = tb_poller_priv_set_object_type(object, priv);

    // bind user private data to socket
    if (!(events & TB_POLLER_EVENT_NOEXTRA) || object->type == TB_POLLER_OBJECT_PIPE)
        tb_pollerdata_set(&poller->pollerdata, object, priv);

    // ok
    return tb_true;
}
static tb_bool_t tb_poller_poll_remove(tb_poller_t* self, tb_poller_object_ref_t object)
{
    // check
    tb_poller_poll_ref_t poller = (tb_poller_poll_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->pfds && object, tb_false);

    // remove this object and events, TODO uses binary search
    tb_remove_first_if(poller->pfds, tb_poller_poll_walk_remove, (tb_cpointer_t)(tb_long_t)tb_ptr2fd(object->ref.ptr));

    // remove user private data from this socket
    tb_pollerdata_reset(&poller->pollerdata, object);

    // ok
    return tb_true;
}
static tb_bool_t tb_poller_poll_modify(tb_poller_t* self, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_poll_ref_t poller = (tb_poller_poll_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->pfds && object, tb_false);

    // oneshot is not supported now
    tb_assertf(!(events & TB_POLLER_EVENT_ONESHOT), "cannot insert events with oneshot, not supported!");

    // modify events, TODO uses binary search
    tb_value_t tuple[2];
    tuple[0].l       = tb_ptr2fd(object->ref.ptr);
    tuple[1].ul      = events;
    tb_walk_all(poller->pfds, tb_poller_poll_walk_modify, tuple);

    // bind the object type to the private data
    priv = tb_poller_priv_set_object_type(object, priv);

    // modify user private data to socket
    if (!(events & TB_POLLER_EVENT_NOEXTRA) || object->type == TB_POLLER_OBJECT_PIPE)
        tb_pollerdata_set(&poller->pollerdata, object, priv);

    // ok
    return tb_true;
}
static tb_long_t tb_poller_poll_wait(tb_poller_t* self, tb_poller_event_func_t func, tb_long_t timeout)
{
    // check
    tb_poller_poll_ref_t poller = (tb_poller_poll_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->pfds && poller->cfds && func, -1);

    // loop
    tb_long_t wait = 0;
    tb_bool_t stop = tb_false;
    tb_hong_t time = tb_mclock();
    while (!wait && !stop && (timeout < 0 || tb_mclock() < time + timeout))
    {
        // pfds
        struct pollfd*  pfds = (struct pollfd*)tb_vector_data(poller->pfds);
        tb_size_t       pfdm = tb_vector_size(poller->pfds);
        tb_assert_and_check_return_val(pfds && pfdm, -1);

        // wait
        tb_long_t pfdn = poll(pfds, pfdm, timeout);

        // timeout or interrupted?
        if (!pfdn || (pfdn == -1 && errno == EINTR))
            return 0;

        // error?
        tb_assert_and_check_return_val(pfdn >= 0, -1);

        // copy fds
        tb_vector_copy(poller->cfds, poller->pfds);

        // walk the copied fds
        pfds = (struct pollfd*)tb_vector_data(poller->cfds);
        pfdm = tb_vector_size(poller->cfds);

        // sync
        tb_size_t i = 0;
        tb_poller_object_t object;
        for (i = 0; i < pfdm; i++)
        {
            // get the object pointer
            object.ref.ptr = tb_fd2ptr(pfds[i].fd);
            tb_assert_and_check_return_val(object.ref.ptr, -1);

            // the poll events
            tb_size_t poll_events = pfds[i].revents;
            tb_check_continue(poll_events);

            // spak?
            if (object.ref.sock == poller->pair[1] && (poll_events & POLLIN))
            {
                // read spak
                tb_char_t spak = '\0';
                if (1 != tb_socket_recv(poller->pair[1], (tb_byte_t*)&spak, 1)) return -1;

                // killed?
                if (spak == 'k') return -1;

                // stop to wait
                stop = tb_true;

                // continue it
                continue ;
            }

            // skip spak
            tb_check_continue(object.ref.sock != poller->pair[1]);

            // init events
            tb_size_t events = TB_POLLER_EVENT_NONE;
            if (poll_events & POLLIN) events |= TB_POLLER_EVENT_RECV;
            if (poll_events & POLLOUT) events |= TB_POLLER_EVENT_SEND;
            if ((poll_events & POLLHUP) && !(events & (TB_POLLER_EVENT_RECV | TB_POLLER_EVENT_SEND)))
                events |= TB_POLLER_EVENT_RECV | TB_POLLER_EVENT_SEND;

            // call event function
            tb_cpointer_t priv = tb_pollerdata_get(&poller->pollerdata, &object);
            object.type = tb_poller_priv_get_object_type(priv);
            func((tb_poller_ref_t)self, &object, events, tb_poller_priv_get_original(priv));

            // update the events count
            wait++;
        }
    }

    // ok
    return wait;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_poller_t* tb_poller_poll_init()
{
    tb_bool_t               ok = tb_false;
    tb_poller_poll_ref_t    poller = tb_null;
    do
    {
        // make poller
        poller = tb_malloc0_type(tb_poller_poll_t);
        tb_assert_and_check_break(poller);

        // init base
        poller->base.type   = TB_POLLER_TYPE_POLL;
        poller->base.exit   = tb_poller_poll_exit;
        poller->base.kill   = tb_poller_poll_kill;
        poller->base.spak   = tb_poller_poll_spak;
        poller->base.wait   = tb_poller_poll_wait;
        poller->base.insert = tb_poller_poll_insert;
        poller->base.remove = tb_poller_poll_remove;
        poller->base.modify = tb_poller_poll_modify;
        poller->base.supported_events = TB_POLLER_EVENT_EALL;

        // init poller data
        tb_pollerdata_init(&poller->pollerdata);

        // init poll fds
        poller->pfds = tb_vector_init(0, tb_element_mem(sizeof(struct pollfd), tb_null, tb_null));
        tb_assert_and_check_break(poller->pfds);

        // init copied poll fds
        poller->cfds = tb_vector_init(0, tb_element_mem(sizeof(struct pollfd), tb_null, tb_null));
        tb_assert_and_check_break(poller->cfds);

        // init pair sockets
        if (!tb_socket_pair(TB_SOCKET_TYPE_TCP, poller->pair)) break;

        // insert pair socket first
        tb_poller_object_t object;
        object.type = TB_POLLER_OBJECT_SOCK;
        object.ref.sock = poller->pair[1];
        if (!tb_poller_poll_insert((tb_poller_t*)poller, &object, TB_POLLER_EVENT_RECV, tb_null)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (poller) tb_poller_poll_exit((tb_poller_t*)poller);
        poller = tb_null;
    }

    // ok?
    return (tb_poller_t*)poller;
}

