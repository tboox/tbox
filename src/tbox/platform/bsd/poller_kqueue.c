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
 * @file        poller_kqueue.c
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#ifdef TB_CONFIG_POSIX_HAVE_GETRLIMIT
#   include <sys/resource.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifndef EV_ENABLE
#   define EV_ENABLE    (0)
#endif

#ifndef NOTE_EOF
#   define NOTE_EOF     (0)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the kqueue poller type
typedef struct __tb_poller_kqueue_t
{
    // the poller base
    tb_poller_t             base;

    // the maxn
    tb_size_t               maxn;

    // the pair sockets for spak, kill ..
    tb_socket_ref_t         pair[2];

    // the kqueue fd
    tb_long_t               kqfd;

    // the events
    struct kevent*          events;

    // the events count
    tb_size_t               events_count;

    // the poller data
    tb_pollerdata_t         pollerdata;

}tb_poller_kqueue_t, *tb_poller_kqueue_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_size_t tb_poller_kqueue_maxfds()
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
static tb_bool_t tb_poller_kqueue_change(tb_poller_kqueue_ref_t poller, struct kevent* events, tb_size_t count)
{
    // check
    tb_assert_and_check_return_val(events && count, tb_false);

    // change events
    struct timespec t = {0};
    if (kevent(poller->kqfd, events, count, tb_null, 0, &t) < 0)
    {
        // trace
        tb_trace_e("change kevent failed, errno: %d", errno);

        // failed
        return tb_false;
    }

    // ok
    return tb_true;
}
static tb_void_t tb_poller_kqueue_exit(tb_poller_t* self)
{
    // check
    tb_poller_kqueue_ref_t poller = (tb_poller_kqueue_ref_t)self;
    tb_assert_and_check_return(poller);

    // exit pair sockets
    if (poller->pair[0]) tb_socket_exit(poller->pair[0]);
    if (poller->pair[1]) tb_socket_exit(poller->pair[1]);
    poller->pair[0] = tb_null;
    poller->pair[1] = tb_null;

    // exit events
    if (poller->events) tb_free(poller->events);
    poller->events = tb_null;
    poller->events_count = 0;

    // close kqfd
    if (poller->kqfd > 0) close(poller->kqfd);
    poller->kqfd = 0;

    // exit socket data
    tb_pollerdata_exit(&poller->pollerdata);

    // free it
    tb_free(poller);
}
static tb_void_t tb_poller_kqueue_kill(tb_poller_t* self)
{
    // check
    tb_poller_kqueue_ref_t poller = (tb_poller_kqueue_ref_t)self;
    tb_assert_and_check_return(poller);

    // kill it
    if (poller->pair[0]) tb_socket_send(poller->pair[0], (tb_byte_t const*)"k", 1);
}
static tb_void_t tb_poller_kqueue_spak(tb_poller_t* self)
{
    // check
    tb_poller_kqueue_ref_t poller = (tb_poller_kqueue_ref_t)self;
    tb_assert_and_check_return(poller);

    // post it
    if (poller->pair[0]) tb_socket_send(poller->pair[0], (tb_byte_t const*)"p", 1);
}
static tb_bool_t tb_poller_kqueue_insert(tb_poller_t* self, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_kqueue_ref_t poller = (tb_poller_kqueue_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->kqfd > 0 && object, tb_false);

    // init the add event
    tb_size_t adde = EV_ADD | EV_ENABLE;
    if (events & TB_POLLER_EVENT_CLEAR) adde |= EV_CLEAR;
    if (events & TB_POLLER_EVENT_ONESHOT) adde |= EV_ONESHOT;

    // bind the object type to the private data
    priv = tb_poller_priv_set_object_type(object, priv);

    // insert socket and add events
    struct kevent   e[2];
    tb_size_t       n = 0;
    tb_int_t        fd = tb_ptr2fd(object->ref.ptr);
    if (events & TB_POLLER_EVENT_RECV)
    {
        EV_SET(&e[n], fd, EVFILT_READ, adde, NOTE_EOF, 0, (tb_pointer_t)priv); n++;
    }
    if (events & TB_POLLER_EVENT_SEND)
    {
        EV_SET(&e[n], fd, EVFILT_WRITE, adde, NOTE_EOF, 0, (tb_pointer_t)priv); n++;
    }

    // change it
    tb_bool_t ok = n? tb_poller_kqueue_change(poller, e, n) : tb_true;

    // save events to object
    if (ok) tb_pollerdata_set(&poller->pollerdata, object, (tb_cpointer_t)events);

    // ok?
    return ok;
}
static tb_bool_t tb_poller_kqueue_remove(tb_poller_t* self, tb_poller_object_ref_t object)
{
    // check
    tb_poller_kqueue_ref_t poller = (tb_poller_kqueue_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->kqfd > 0 && object, tb_false);

    // get the previous events
    tb_size_t events = (tb_size_t)tb_pollerdata_get(&poller->pollerdata, object);

    // remove this socket and events
    struct kevent   e[2];
    tb_size_t       n = 0;
    tb_int_t        fd = tb_ptr2fd(object->ref.ptr);
    if (events & TB_POLLER_EVENT_RECV)
    {
        EV_SET(&e[n], fd, EVFILT_READ, EV_DELETE, 0, 0, tb_null);
        n++;
    }
    if (events & TB_POLLER_EVENT_SEND)
    {
        EV_SET(&e[n], fd, EVFILT_WRITE, EV_DELETE, 0, 0, tb_null);
        n++;
    }

    // change it
    tb_bool_t ok = n? tb_poller_kqueue_change(poller, e, n) : tb_true;

    // remove events from object
    if (ok) tb_pollerdata_reset(&poller->pollerdata, object);

    // ok?
    return ok;
}
static tb_bool_t tb_poller_kqueue_modify(tb_poller_t* self, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_kqueue_ref_t poller = (tb_poller_kqueue_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->kqfd > 0 && object, tb_false);

    // get the previous events
    tb_size_t events_old = (tb_size_t)tb_pollerdata_get(&poller->pollerdata, object);

    // get changed events
    tb_size_t adde = events & ~events_old;
    tb_size_t dele = ~events & events_old;

    // init the add event
    tb_size_t add_event = EV_ADD | EV_ENABLE;
    if (events & TB_POLLER_EVENT_CLEAR) add_event |= EV_CLEAR;
    if (events & TB_POLLER_EVENT_ONESHOT) add_event |= EV_ONESHOT;

    // bind the object type to the private data
    priv = tb_poller_priv_set_object_type(object, priv);

    // modify events
    struct kevent   e[2];
    tb_size_t       n = 0;
    tb_int_t        fd = tb_ptr2fd(object->ref.ptr);
    if (adde & TB_SOCKET_EVENT_RECV)
    {
        EV_SET(&e[n], fd, EVFILT_READ, add_event, NOTE_EOF, 0, (tb_pointer_t)priv);
        n++;
    }
    else if (dele & TB_SOCKET_EVENT_RECV)
    {
        EV_SET(&e[n], fd, EVFILT_READ, EV_DELETE, 0, 0, (tb_pointer_t)priv);
        n++;
    }
    if (adde & TB_SOCKET_EVENT_SEND)
    {
        EV_SET(&e[n], fd, EVFILT_WRITE, add_event, NOTE_EOF, 0, (tb_pointer_t)priv);
        n++;
    }
    else if (dele & TB_SOCKET_EVENT_SEND)
    {
        EV_SET(&e[n], fd, EVFILT_WRITE, EV_DELETE, 0, 0, (tb_pointer_t)priv);
        n++;
    }

    // change it
    tb_bool_t ok = n? tb_poller_kqueue_change(poller, e, n) : tb_true;

    // save events to socket
    if (ok) tb_pollerdata_set(&poller->pollerdata, object, (tb_cpointer_t)events);

    // ok?
    return ok;
}
static tb_long_t tb_poller_kqueue_wait(tb_poller_t* self, tb_poller_event_func_t func, tb_long_t timeout)
{
    // check
    tb_poller_kqueue_ref_t poller = (tb_poller_kqueue_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->kqfd > 0 && poller->maxn && func, -1);

    // init time
    struct timespec t = {0};
    if (timeout > 0)
    {
        t.tv_sec = timeout / 1000;
        t.tv_nsec = (timeout % 1000) * 1000000;
    }

    // init events
    tb_size_t grow = tb_align8((poller->maxn >> 3) + 1);
    if (!poller->events)
    {
        poller->events_count = grow;
        poller->events = tb_nalloc_type(poller->events_count, struct kevent);
        tb_assert_and_check_return_val(poller->events, -1);
    }

    // wait events
    tb_long_t events_count = kevent(poller->kqfd, tb_null, 0, poller->events, poller->events_count, timeout >= 0? &t : tb_null);

    // timeout or interrupted?
    if (!events_count || (events_count == -1 && errno == EINTR))
        return 0;

    // error?
    tb_assert_and_check_return_val(events_count >= 0 && events_count <= poller->events_count, -1);

    // grow it if events is full
    if (events_count == poller->events_count)
    {
        // grow size
        poller->events_count += grow;
        if (poller->events_count > poller->maxn) poller->events_count = poller->maxn;

        // grow data
        poller->events = (struct kevent*)tb_ralloc(poller->events, poller->events_count * sizeof(struct kevent));
        tb_assert_and_check_return_val(poller->events, -1);
    }
    tb_assert(events_count <= poller->events_count);

    // limit
    events_count = tb_min(events_count, poller->maxn);

    // handle events
    tb_size_t          i = 0;
    tb_size_t          wait = 0;
    struct kevent*     e = tb_null;
    tb_socket_ref_t    pair = poller->pair[1];
    tb_poller_object_t object;
    for (i = 0; i < events_count; i++)
    {
        // the kevents
        e = poller->events + i;

        // get the object pointer
        object.ref.ptr = tb_fd2ptr(e->ident);
        tb_assert(object.ref.ptr);

        // spank socket events?
        if (object.ref.sock == pair && e->filter == EVFILT_READ)
        {
            // read spak
            tb_char_t spak = '\0';
            if (1 != tb_socket_recv(pair, (tb_byte_t*)&spak, 1)) return -1;

            // killed?
            if (spak == 'k') return -1;

            // continue it
            continue ;
        }

        // skip spak
        tb_check_continue(object.ref.sock != pair);

        // init events
        tb_size_t events = TB_POLLER_EVENT_NONE;
        if (e->filter == EVFILT_READ) events |= TB_POLLER_EVENT_RECV;
        if (e->filter == EVFILT_WRITE) events |= TB_POLLER_EVENT_SEND;
        if ((e->flags & EV_ERROR) && !(events & (TB_POLLER_EVENT_RECV | TB_POLLER_EVENT_SEND)))
            events |= TB_POLLER_EVENT_RECV | TB_POLLER_EVENT_SEND;

        // connection closed for the edge trigger?
        if (e->flags & EV_EOF)
            events |= TB_POLLER_EVENT_EOF;

        // call event function
        object.type = tb_poller_priv_get_object_type(e->udata);
        func((tb_poller_ref_t)self, &object, events, tb_poller_priv_get_original(e->udata));

        // update the events count
        wait++;
    }

    // ok
    return wait;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_poller_t* tb_poller_kqueue_init()
{
    tb_bool_t               ok = tb_false;
    tb_poller_kqueue_ref_t  poller = tb_null;
    do
    {
        // make poller
        poller = tb_malloc0_type(tb_poller_kqueue_t);
        tb_assert_and_check_break(poller);

        // init base
        poller->base.type   = TB_POLLER_TYPE_KQUEUE;
        poller->base.exit   = tb_poller_kqueue_exit;
        poller->base.kill   = tb_poller_kqueue_kill;
        poller->base.spak   = tb_poller_kqueue_spak;
        poller->base.wait   = tb_poller_kqueue_wait;
        poller->base.insert = tb_poller_kqueue_insert;
        poller->base.remove = tb_poller_kqueue_remove;
        poller->base.modify = tb_poller_kqueue_modify;
        poller->base.supported_events = TB_POLLER_EVENT_EALL | TB_POLLER_EVENT_CLEAR | TB_POLLER_EVENT_ONESHOT;

        // init socket data
        tb_pollerdata_init(&poller->pollerdata);

        // init kqueue
        poller->kqfd = kqueue();
        tb_assert_and_check_break(poller->kqfd > 0);

        // init maxn
        poller->maxn = tb_poller_kqueue_maxfds();
        tb_assert_and_check_break(poller->maxn);

        // init pair sockets
        if (!tb_socket_pair(TB_SOCKET_TYPE_TCP, poller->pair)) break;

        // insert pair socket first
        tb_poller_object_t pair1;
        pair1.type     = TB_POLLER_OBJECT_SOCK;
        pair1.ref.sock = poller->pair[1];
        if (!tb_poller_kqueue_insert((tb_poller_t*)poller, &pair1, TB_POLLER_EVENT_RECV, tb_null)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (poller) tb_poller_kqueue_exit((tb_poller_t*)poller);
        poller = tb_null;
    }

    // ok?
    return (tb_poller_t*)poller;
}

