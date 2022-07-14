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
 * @file        poller_epoll.c
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#ifdef TB_CONFIG_POSIX_HAVE_GETRLIMIT
#   include <sys/resource.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the epoll poller type
typedef struct __tb_poller_epoll_t
{
    // the poller base
    tb_poller_t             base;

    // the maxn
    tb_size_t               maxn;

    // the pair sockets for spak, kill ..
    tb_socket_ref_t         pair[2];

    // the epoll fd
    tb_long_t               epfd;

    // the events
    struct epoll_event*     events;

    // the events count
    tb_size_t               events_count;

    // the socket data
    tb_pollerdata_t         pollerdata;

}tb_poller_epoll_t, *tb_poller_epoll_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_size_t tb_poller_epoll_maxfds()
{
    // attempt to get it from getdtablesize
    tb_size_t maxfds = 0;
#ifdef TB_CONFIG_POSIX_HAVE_GETDTABLESIZE
    if (!maxfds) maxfds = getdtablesize();
#endif

    // attempt to get it from getrlimit
#ifdef TB_CONFIG_POSIX_HAVE_GETRLIMIT
	struct rlimit rl;
    if (!maxfds && !getrlimit(RLIMIT_NOFILE, &rl))
        maxfds = rl.rlim_cur;
#endif

    // attempt to get it from sysconf
    if (!maxfds) maxfds = sysconf(_SC_OPEN_MAX);

    // ok?
    return maxfds;
}
static tb_void_t tb_poller_epoll_exit(tb_poller_t* self)
{
    // check
    tb_poller_epoll_ref_t poller = (tb_poller_epoll_ref_t)self;
    tb_assert_and_check_return(poller);

    // exit pair sockets
    if (poller->pair[0]) tb_socket_exit(poller->pair[0]);
    if (poller->pair[1]) tb_socket_exit(poller->pair[1]);
    poller->pair[0] = tb_null;
    poller->pair[1] = tb_null;

    // exit events
    if (poller->events) tb_free(poller->events);
    poller->events          = tb_null;
    poller->events_count    = 0;

    // close epfd
    if (poller->epfd > 0) close(poller->epfd);
    poller->epfd = 0;

    // exit socket data
    tb_pollerdata_exit(&poller->pollerdata);

    // free it
    tb_free(poller);
}
static tb_void_t tb_poller_epoll_kill(tb_poller_t* self)
{
    // check
    tb_poller_epoll_ref_t poller = (tb_poller_epoll_ref_t)self;
    tb_assert_and_check_return(poller);

    // kill it
    if (poller->pair[0]) tb_socket_send(poller->pair[0], (tb_byte_t const*)"k", 1);
}
static tb_void_t tb_poller_epoll_spak(tb_poller_t* self)
{
    // check
    tb_poller_epoll_ref_t poller = (tb_poller_epoll_ref_t)self;
    tb_assert_and_check_return(poller);

    // post it
    if (poller->pair[0]) tb_socket_send(poller->pair[0], (tb_byte_t const*)"p", 1);
}
static tb_bool_t tb_poller_epoll_insert(tb_poller_t* self, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_epoll_ref_t poller = (tb_poller_epoll_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->epfd > 0 && object, tb_false);

    // init event
    struct epoll_event e = {0};
    if (events & TB_POLLER_EVENT_RECV) e.events |= EPOLLIN;
    if (events & TB_POLLER_EVENT_SEND) e.events |= EPOLLOUT;
    if (events & TB_POLLER_EVENT_CLEAR)
    {
        e.events |= EPOLLET;
#ifdef EPOLLRDHUP
        e.events |= EPOLLRDHUP;
#endif
    }
#ifdef EPOLLONESHOT
    if (events & TB_POLLER_EVENT_ONESHOT) e.events |= EPOLLONESHOT;
#else
    // oneshot is not supported now
    tb_assertf(!(events & TB_POLLER_EVENT_ONESHOT), "cannot insert events with oneshot, not supported!");
#endif

    // save fd
    e.data.fd = (tb_int_t)tb_ptr2fd(object->ref.ptr);

    // bind the object type to the private data
    priv = tb_poller_priv_set_object_type(object, priv);

    // bind user private data to object
    if (!(events & TB_POLLER_EVENT_NOEXTRA) || object->type == TB_POLLER_OBJECT_PIPE)
        tb_pollerdata_set(&poller->pollerdata, object, priv);

    // add socket and events
    if (epoll_ctl(poller->epfd, EPOLL_CTL_ADD, e.data.fd, &e) < 0)
    {
        // trace
        tb_trace_e("insert object(%p) events: %lu failed, errno: %d", object->ref.ptr, events, errno);
        return tb_false;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_poller_epoll_remove(tb_poller_t* self, tb_poller_object_ref_t object)
{
    // check
    tb_poller_epoll_ref_t poller = (tb_poller_epoll_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->epfd > 0 && object, tb_false);

    // remove object and events
    struct epoll_event  e = {0};
    tb_long_t           fd = tb_ptr2fd(object->ref.ptr);
    if (epoll_ctl(poller->epfd, EPOLL_CTL_DEL, fd, &e) < 0)
    {
        // trace
        tb_trace_e("remove object(%p) failed, errno: %d", object->ref.ptr, errno);
        return tb_false;
    }

    // remove user private data from this object
    tb_pollerdata_reset(&poller->pollerdata, object);
    return tb_true;
}
static tb_bool_t tb_poller_epoll_modify(tb_poller_t* self, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_epoll_ref_t poller = (tb_poller_epoll_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->epfd > 0 && object, tb_false);

    // init event
    struct epoll_event e = {0};
    if (events & TB_POLLER_EVENT_RECV) e.events |= EPOLLIN;
    if (events & TB_POLLER_EVENT_SEND) e.events |= EPOLLOUT;
    if (events & TB_POLLER_EVENT_CLEAR)
    {
        e.events |= EPOLLET;
#ifdef EPOLLRDHUP
        e.events |= EPOLLRDHUP;
#endif
    }
#ifdef EPOLLONESHOT
    if (events & TB_POLLER_EVENT_ONESHOT) e.events |= EPOLLONESHOT;
#else
    // oneshot is not supported now
    tb_assertf(!(events & TB_POLLER_EVENT_ONESHOT), "cannot insert events with oneshot, not supported!");
#endif

    // save fd
    e.data.fd = (tb_int_t)tb_ptr2fd(object->ref.ptr);

    // bind the object type to the private data
    priv = tb_poller_priv_set_object_type(object, priv);

    // bind user private data to object
    if (!(events & TB_POLLER_EVENT_NOEXTRA) || object->type == TB_POLLER_OBJECT_PIPE)
        tb_pollerdata_set(&poller->pollerdata, object, priv);

    // modify events
    if (epoll_ctl(poller->epfd, EPOLL_CTL_MOD, e.data.fd, &e) < 0)
    {
        // trace
        tb_trace_e("modify object(%p) events: %lu failed, errno: %d", object->ref.ptr, events, errno);
        return tb_false;
    }
    return tb_true;
}
static tb_long_t tb_poller_epoll_wait(tb_poller_t* self, tb_poller_event_func_t func, tb_long_t timeout)
{
    // check
    tb_poller_epoll_ref_t poller = (tb_poller_epoll_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->epfd > 0 && poller->maxn && func, -1);

    // init events
    tb_size_t grow = tb_align8((poller->maxn >> 3) + 1);
    if (!poller->events)
    {
        poller->events_count = grow;
        poller->events = tb_nalloc_type(poller->events_count, struct epoll_event);
        tb_assert_and_check_return_val(poller->events, -1);
    }

    // wait events
    tb_long_t events_count = epoll_wait(poller->epfd, poller->events, poller->events_count, timeout);

    // timeout or interrupted?
    if (!events_count || (events_count == -1 && errno == EINTR))
        return 0;

    // check error?
    tb_assert_and_check_return_val(events_count >= 0 && events_count <= poller->events_count, -1);

    // grow it if events is full
    if (events_count == poller->events_count)
    {
        // grow size
        poller->events_count += grow;
        if (poller->events_count > poller->maxn) poller->events_count = poller->maxn;

        // grow data
        poller->events = (struct epoll_event*)tb_ralloc(poller->events, poller->events_count * sizeof(struct epoll_event));
        tb_assert_and_check_return_val(poller->events, -1);
    }
    tb_assert(events_count <= poller->events_count);

    // limit
    events_count = tb_min(events_count, poller->maxn);

    // handle events
    tb_size_t           i = 0;
    tb_size_t           wait = 0;
    struct epoll_event* e = tb_null;
    tb_socket_ref_t     pair = poller->pair[1];
    tb_poller_object_t  object;
    for (i = 0; i < events_count; i++)
    {
        // the epoll event
        e = poller->events + i;

        // the events for epoll
        tb_size_t epoll_events = e->events;

        // the socket
        tb_long_t fd = e->data.fd;
        object.ref.ptr = tb_fd2ptr(fd);
        tb_assert(object.ref.ptr);

        // spank socket events?
        if (object.ref.sock == pair && (epoll_events & EPOLLIN))
        {
            // read spak
            tb_char_t spak = '\0';
            if (1 != tb_socket_recv(pair, (tb_byte_t*)&spak, 1)) return -1;

            // killed?
            if (spak == 'k') return -1;

            // continue it
            continue ;
        }
        tb_check_continue(object.ref.sock != pair);

        // init events
        tb_size_t events = TB_POLLER_EVENT_NONE;
        if (epoll_events & EPOLLIN) events |= TB_POLLER_EVENT_RECV;
        if (epoll_events & EPOLLOUT) events |= TB_POLLER_EVENT_SEND;
        if (epoll_events & (EPOLLHUP | EPOLLERR) && !(events & (TB_POLLER_EVENT_RECV | TB_POLLER_EVENT_SEND)))
            events |= TB_POLLER_EVENT_RECV | TB_POLLER_EVENT_SEND;

#ifdef EPOLLRDHUP
        // connection closed for the edge trigger?
        if (epoll_events & EPOLLRDHUP) events |= TB_POLLER_EVENT_EOF;
#endif

        // call event function
        tb_cpointer_t priv = tb_pollerdata_get(&poller->pollerdata, &object);
        object.type = tb_poller_priv_get_object_type(priv);
        func((tb_poller_ref_t)self, &object, events, tb_poller_priv_get_original(priv));

        // update the events count
        wait++;
    }

    // ok
    return wait;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_poller_t* tb_poller_epoll_init()
{
    // done
    tb_bool_t               ok = tb_false;
    tb_poller_epoll_ref_t   poller = tb_null;
    do
    {
        // make poller
        poller = tb_malloc0_type(tb_poller_epoll_t);
        tb_assert_and_check_break(poller);

        // init base
        poller->base.type   = TB_POLLER_TYPE_EPOLL;
        poller->base.exit   = tb_poller_epoll_exit;
        poller->base.kill   = tb_poller_epoll_kill;
        poller->base.spak   = tb_poller_epoll_spak;
        poller->base.wait   = tb_poller_epoll_wait;
        poller->base.insert = tb_poller_epoll_insert;
        poller->base.remove = tb_poller_epoll_remove;
        poller->base.modify = tb_poller_epoll_modify;
#ifdef EPOLLONESHOT
        poller->base.supported_events = TB_POLLER_EVENT_EALL | TB_POLLER_EVENT_CLEAR | TB_POLLER_EVENT_ONESHOT;
#else
        poller->base.supported_events = TB_POLLER_EVENT_EALL | TB_POLLER_EVENT_CLEAR;
#endif

        // init poller data
        tb_pollerdata_init(&poller->pollerdata);

        // init maxn
        poller->maxn = tb_poller_epoll_maxfds();
        tb_assert_and_check_break(poller->maxn);

        // init epoll
        poller->epfd = epoll_create(poller->maxn);
        tb_assert_and_check_break(poller->epfd > 0);

        // init pair sockets
        if (!tb_socket_pair(TB_SOCKET_TYPE_TCP, poller->pair)) break;

        // insert pair socket first
        tb_poller_object_t object;
        object.type = TB_POLLER_OBJECT_SOCK;
        object.ref.sock = poller->pair[1];
        if (!tb_poller_epoll_insert((tb_poller_t*)poller, &object, TB_POLLER_EVENT_RECV, tb_null)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (poller) tb_poller_epoll_exit((tb_poller_t*)poller);
        poller = tb_null;
    }

    // ok?
    return (tb_poller_t*)poller;
}

