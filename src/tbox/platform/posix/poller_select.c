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
 * @file        poller_select.c
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../time.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"
#ifdef TB_CONFIG_OS_WINDOWS
#   include "../windows/interface/interface.h"
#else
#   include <sys/socket.h>
#   include <sys/select.h>
#   include <errno.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// FD_ISSET
#ifdef TB_CONFIG_OS_WINDOWS
#   undef FD_ISSET
#   define FD_ISSET(fd, set) tb_ws2_32()->__WSAFDIsSet((SOCKET)(fd), (fd_set FAR *)(set))
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the poller private data type
typedef struct __tb_poller_select_data_t
{
    // the fd
    tb_long_t               fd;

    // the user private data
    tb_cpointer_t           priv;

    // the socket events
    tb_size_t               events;

}tb_poller_select_data_t, *tb_poller_select_data_ref_t;

// the poller select type
typedef struct __tb_poller_select_t
{
    // the poller base
    tb_poller_t             base;

    // the pair sockets for spak, kill ..
    tb_socket_ref_t         pair[2];

    // the select fd max
    tb_size_t               sfdm;

    // the select fds
    fd_set                  rfds;
    fd_set                  wfds;

    // the copied fds
    fd_set                  rfdc;
    fd_set                  wfdc;

    // the socket count
    tb_size_t               count;

    /* the user private data list
     *
     * do not use hash_map because it is too heavy in micro mode
     */
    tb_poller_select_data_ref_t    list;

    // the user private data list size
    tb_size_t               list_size;

}tb_poller_select_t, *tb_poller_select_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_poller_select_list_set(tb_poller_select_ref_t poller, tb_long_t fd, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_assert(poller);

    // no list? init it first
    if (!poller->list)
    {
        // init list
        poller->list = tb_nalloc0_type(FD_SETSIZE, tb_poller_select_data_t);
        tb_assert_and_check_return(poller->list);
    }

    // insert or update the user private data to the list in the increasing order (TODO binary search)
    tb_size_t i = 0;
    tb_size_t n = poller->list_size;
    for (i = 0; i < n; i++) if (fd <= poller->list[i].fd) break;

    // update the private data
    if (i < n && fd == poller->list[i].fd)
    {
        poller->list[i].priv   = priv;
        poller->list[i].events = events;
    }
    else
    {
        // insert the private data
        if (i < n) tb_memmov_(poller->list + i + 1, poller->list + i, (n - i) * sizeof(tb_poller_select_data_t));
        poller->list[i].fd = fd;
        poller->list[i].priv = priv;
        poller->list[i].events = events;

        // update the list size
        poller->list_size++;
    }
}
static tb_void_t tb_poller_select_list_del(tb_poller_select_ref_t poller, tb_long_t fd)
{
    // check
    tb_assert(poller);

    // exists list?
    if (poller->list)
    {
        // remove the user private data from the list (TODO binary search)
        tb_size_t i = 0;
        tb_size_t n = poller->list_size;
        for (i = 0; i < n; i++) if (fd == poller->list[i].fd) break;

        // found and remove it
        if (i < n)
        {
            if (i + 1 < n) tb_memmov_(poller->list + i, poller->list + i + 1, (n - i - 1) * sizeof(tb_poller_select_data_t));
            poller->list_size--;
        }
    }
}
static tb_void_t tb_poller_select_exit(tb_poller_t* self)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return(poller);

    // exit pair sockets
    if (poller->pair[0]) tb_socket_exit(poller->pair[0]);
    if (poller->pair[1]) tb_socket_exit(poller->pair[1]);
    poller->pair[0] = tb_null;
    poller->pair[1] = tb_null;

    // exit list
    if (poller->list) tb_free(poller->list);
    poller->list        = tb_null;
    poller->list_size   = 0;

    // clear fds
    FD_ZERO(&poller->rfds);
    FD_ZERO(&poller->wfds);
    FD_ZERO(&poller->rfdc);
    FD_ZERO(&poller->wfdc);

    // free it
    tb_free(poller);
}
static tb_void_t tb_poller_select_kill(tb_poller_t* self)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return(poller);

    // kill it
    if (poller->pair[0]) tb_socket_send(poller->pair[0], (tb_byte_t const*)"k", 1);
}
static tb_void_t tb_poller_select_spak(tb_poller_t* self)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return(poller);

    // post it
    if (poller->pair[0]) tb_socket_send(poller->pair[0], (tb_byte_t const*)"p", 1);
}
static tb_bool_t tb_poller_select_insert(tb_poller_t* self, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return_val(poller && object, tb_false);

    // check size
    tb_assert_and_check_return_val(poller->count < FD_SETSIZE, tb_false);

#ifdef TB_CONFIG_OS_WINDOWS
    // do not support pipe fd on windows
    tb_assert_and_check_return_val(object->type != TB_POLLER_OBJECT_PIPE, tb_false);
#endif

    // save fd
    tb_long_t fd = tb_ptr2fd(object->ref.ptr);
    if (fd > (tb_long_t)poller->sfdm) poller->sfdm = (tb_size_t)fd;
    if (events & TB_POLLER_EVENT_RECV) FD_SET(fd, &poller->rfds);
    if (events & TB_POLLER_EVENT_SEND) FD_SET(fd, &poller->wfds);

    // bind the object type to the private data
    priv = tb_poller_priv_set_object_type(object, priv);

    // bind user private data to socket
    tb_poller_select_list_set(poller, fd, events, priv);

    // update socket count
    poller->count++;

    // ok
    return tb_true;
}
static tb_bool_t tb_poller_select_remove(tb_poller_t* self, tb_poller_object_ref_t object)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return_val(poller && object, tb_false);

    // remove fds
    tb_long_t fd = tb_ptr2fd(object->ref.ptr);
    FD_CLR(fd, &poller->rfds);
    FD_CLR(fd, &poller->wfds);

    // remove user private data from this socket
    tb_poller_select_list_del(poller, fd);

    // update socket count
    if (poller->count > 0) poller->count--;

    // ok
    return tb_true;
}
static tb_bool_t tb_poller_select_modify(tb_poller_t* self, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return_val(poller && object, tb_false);

    // modify events
    tb_long_t fd = tb_ptr2fd(object->ref.ptr);
    if (events & TB_POLLER_EVENT_RECV) FD_SET(fd, &poller->rfds); else FD_CLR(fd, &poller->rfds);
    if (events & TB_POLLER_EVENT_SEND) FD_SET(fd, &poller->wfds); else FD_CLR(fd, &poller->wfds);

    // bind the object type to the private data
    priv = tb_poller_priv_set_object_type(object, priv);

    // modify user private data to socket
    tb_poller_select_list_set(poller, fd, events, priv);

    // ok
    return tb_true;
}
static tb_long_t tb_poller_select_wait(tb_poller_t* self, tb_poller_event_func_t func, tb_long_t timeout)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return_val(poller && func, -1);

    // init time
    struct timeval t = {0};
    if (timeout > 0)
    {
#ifdef TB_CONFIG_OS_WINDOWS
        t.tv_sec = (LONG)(timeout / 1000);
#else
        t.tv_sec = (timeout / 1000);
#endif
        t.tv_usec = (timeout % 1000) * 1000;
    }

    // loop
    tb_long_t wait = 0;
    tb_bool_t stop = tb_false;
    tb_hong_t time = tb_mclock();
    tb_poller_select_data_t poller_events[512];
    while (!wait && !stop && (timeout < 0 || tb_mclock() < time + timeout))
    {
        // copy fds
        tb_memcpy(&poller->rfdc, &poller->rfds, sizeof(fd_set));
        tb_memcpy(&poller->wfdc, &poller->wfds, sizeof(fd_set));

        // wait
#ifdef TB_CONFIG_OS_WINDOWS
        tb_long_t sfdn = tb_ws2_32()->select((tb_int_t) poller->sfdm + 1, &poller->rfdc, &poller->wfdc, tb_null, timeout >= 0? &t : tb_null);
        if (!sfdn) return 0; // timeout
#else
        tb_long_t sfdn = select(poller->sfdm + 1, &poller->rfdc, &poller->wfdc, tb_null, timeout >= 0? &t : tb_null);
        if (!sfdn || (sfdn == -1 && errno == EINTR)) // timeout or interrupted?
            return 0;
#endif

        // error?
        tb_assert_and_check_return_val(sfdn >= 0, -1);

        // dispatch events
        tb_size_t i = 0;
        tb_size_t n = poller->list_size;
        tb_size_t eventn = 0;
        for (i = 0; i < n; i++)
        {
            // end?
            tb_check_break(wait >= 0);

            // check
            tb_assert_and_check_return_val(poller->list, -1);

            // get the object pointer
            tb_long_t     fd = poller->list[i].fd;
            tb_cpointer_t ptr = tb_fd2ptr(fd);
            tb_assert_and_check_return_val(ptr, -1);

            // spank socket events?
            if ((tb_socket_ref_t)ptr == poller->pair[1] && FD_ISSET(tb_sock2fd(poller->pair[1]), &poller->rfdc))
            {
                // read spak
                tb_char_t spak = '\0';
                if (1 != tb_socket_recv(poller->pair[1], (tb_byte_t*)&spak, 1)) wait = -1;

                // killed?
                if (spak == 'k') wait = -1;
                tb_check_break(wait >= 0);

                // stop to wait
                stop = tb_true;

                // continue it
                continue ;
            }

            // filter spak
            tb_check_continue((tb_socket_ref_t)ptr != poller->pair[1]);

            // init events
            tb_size_t events = TB_POLLER_EVENT_NONE;
            if (FD_ISSET(fd, &poller->rfdc)) events |= TB_POLLER_EVENT_RECV;
            if (FD_ISSET(fd, &poller->wfdc)) events |= TB_POLLER_EVENT_SEND;

            // check socket error?
#ifdef TB_CONFIG_OS_WINDOWS
            tb_int_t error = 0;
            tb_int_t n = sizeof(tb_int_t);
            if (!tb_ws2_32()->getsockopt(fd, SOL_SOCKET, SO_ERROR, (tb_char_t*)&error, &n) && error)
                events |= TB_POLLER_EVENT_ERROR;
#else
            tb_int_t error = 0;
            socklen_t n = sizeof(socklen_t);
            if (!getsockopt(fd, SOL_SOCKET, SO_ERROR, (tb_char_t*)&error, &n) && error)
                events |= TB_POLLER_EVENT_ERROR;
#endif

            // exists events?
            if (events)
            {
                // oneshot?
                if (poller->list[i].events & TB_POLLER_EVENT_ONESHOT)
                    events |= TB_POLLER_EVENT_ONESHOT;

                // save triggered events
                tb_assert_and_check_break(eventn < tb_arrayn(poller_events));
                poller_events[eventn].fd     = fd;
                poller_events[eventn].priv   = poller->list[i].priv;
                poller_events[eventn].events = events;
                eventn++;

                // update the events count
                wait++;
            }
        }

        // call events function
        tb_size_t eventi;
        tb_poller_object_t object;
        for (eventi = 0; eventi < eventn; eventi++)
        {
            // get events and private data
            tb_size_t     events = poller_events[eventi].events;
            tb_cpointer_t priv = poller_events[eventi].priv;

            // get the poller object
            object.type = tb_poller_priv_get_object_type(priv);
            object.ref.ptr = tb_fd2ptr(poller_events[eventi].fd);
            if (events & TB_POLLER_EVENT_ONESHOT)
            {
                events &= ~TB_POLLER_EVENT_ONESHOT;
                tb_poller_select_remove(self, &object);
            }
            func((tb_poller_ref_t)self, &object, events, tb_poller_priv_get_original(priv));
        }
    }

    // ok
    return wait;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_poller_t* tb_poller_select_init()
{
    tb_bool_t               ok = tb_false;
    tb_poller_select_ref_t  poller = tb_null;
    do
    {
        // make poller
        poller = tb_malloc0_type(tb_poller_select_t);
        tb_assert_and_check_break(poller);

        // init base
        poller->base.type   = TB_POLLER_TYPE_SELECT;
        poller->base.exit   = tb_poller_select_exit;
        poller->base.kill   = tb_poller_select_kill;
        poller->base.spak   = tb_poller_select_spak;
        poller->base.wait   = tb_poller_select_wait;
        poller->base.insert = tb_poller_select_insert;
        poller->base.remove = tb_poller_select_remove;
        poller->base.modify = tb_poller_select_modify;
        poller->base.supported_events = TB_POLLER_EVENT_EALL | TB_POLLER_EVENT_ONESHOT;

        // init fds
        FD_ZERO(&poller->rfds);
        FD_ZERO(&poller->wfds);
        FD_ZERO(&poller->rfdc);
        FD_ZERO(&poller->wfdc);

        // init pair sockets
        if (!tb_socket_pair(TB_SOCKET_TYPE_TCP, poller->pair)) break;

        // insert pair socket first
        tb_poller_object_t object;
        object.type = TB_POLLER_OBJECT_SOCK;
        object.ref.sock = poller->pair[1];
        if (!tb_poller_select_insert((tb_poller_t*)poller, &object, TB_POLLER_EVENT_RECV, tb_null)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (poller) tb_poller_select_exit((tb_poller_t*)poller);
        poller = tb_null;
    }

    // ok?
    return (tb_poller_t*)poller;
}

