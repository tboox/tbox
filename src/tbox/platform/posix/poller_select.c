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
 * @file        poller_select.c
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"
#ifdef TB_CONFIG_OS_WINDOWS
#   include "../windows/interface/interface.h"
#else
#   include <sys/select.h>
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

// the poller select type
typedef struct __tb_poller_select_t
{
    // the user private data
    tb_cpointer_t           priv;

    // the pair sockets for spak, kill ..
    tb_socket_ref_t         pair[2];

    // the select fd max
    tb_size_t               sfdm;

    // the select fds
    fd_set                  rfds;
    fd_set                  wfds;

    // the user private data hash (socket => priv)
    tb_hash_map_ref_t       hash;

}tb_poller_select_t, *tb_poller_select_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_poller_ref_t tb_poller_init(tb_cpointer_t priv)
{
    // done
    tb_bool_t               ok = tb_false;
    tb_poller_select_ref_t  poller = tb_null;
    do
    {
        // make poller
        poller = tb_malloc0_type(tb_poller_select_t);
        tb_assert_and_check_break(poller);

        // init hash
        poller->hash = tb_hash_map_init(tb_align8(tb_isqrti(FD_SETSIZE) + 1), tb_element_ptr(tb_null, tb_null), tb_element_ptr(tb_null, tb_null));
        tb_assert_and_check_break(poller->hash);

        // init user private data
        poller->priv = priv;

        // init fds
        FD_ZERO(&poller->rfds);
        FD_ZERO(&poller->wfds);

        // init pair sockets
        if (!tb_socket_pair(TB_SOCKET_TYPE_TCP, poller->pair)) break;

        // insert pair socket first
        if (!tb_poller_insert((tb_poller_ref_t)poller, poller->pair[1], TB_POLLER_EVENT_RECV, tb_null)) break;  

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (poller) tb_poller_exit((tb_poller_ref_t)poller);
        poller = tb_null;
    }

    // ok?
    return (tb_poller_ref_t)poller;
}
tb_void_t tb_poller_exit(tb_poller_ref_t self)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return(poller);

    // exit pair sockets
    if (poller->pair[0]) tb_socket_exit(poller->pair[0]);
    if (poller->pair[1]) tb_socket_exit(poller->pair[1]);
    poller->pair[0] = tb_null;
    poller->pair[1] = tb_null;

    // exit hash
    if (poller->hash) tb_hash_map_exit(poller->hash);
    poller->hash = tb_null;

    // clear fds
    FD_ZERO(&poller->rfds);
    FD_ZERO(&poller->wfds);

    // free it
    tb_free(poller);
}
tb_void_t tb_poller_clear(tb_poller_ref_t self)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return(poller);

    // clear fds
    poller->sfdm = 0;
    FD_ZERO(&poller->rfds);
    FD_ZERO(&poller->wfds);

    // clear hash
    if (poller->hash) tb_hash_map_clear(poller->hash);

    // spak it
    if (poller->pair[0]) tb_socket_send(poller->pair[0], (tb_byte_t const*)"p", 1);
}
tb_cpointer_t tb_poller_priv(tb_poller_ref_t self)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return_val(poller, tb_null);

    // get the user private data
    return poller->priv;
}
tb_void_t tb_poller_kill(tb_poller_ref_t self)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return(poller);

    // kill it
    if (poller->pair[0]) tb_socket_send(poller->pair[0], (tb_byte_t const*)"k", 1);
}
tb_void_t tb_poller_spak(tb_poller_ref_t self)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return(poller);

    // post it
    if (poller->pair[0]) tb_socket_send(poller->pair[0], (tb_byte_t const*)"p", 1);
}
tb_bool_t tb_poller_support(tb_poller_ref_t self, tb_size_t events)
{
    // all supported events 
    static tb_size_t events_supported = TB_POLLER_EVENT_EALL;

    // is supported?
    return (events_supported & events) == events;
}
tb_bool_t tb_poller_insert(tb_poller_ref_t self, tb_socket_ref_t sock, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->hash && sock, tb_false);

    // check size
    tb_assert_and_check_return_val(tb_hash_map_size(poller->hash) < FD_SETSIZE, tb_false);

    // save fd
    tb_long_t fd = tb_sock2fd(sock);
    if (fd > (tb_long_t)poller->sfdm) poller->sfdm = (tb_size_t)fd;
    if (events & TB_POLLER_EVENT_RECV) FD_SET(fd, &poller->rfds);
    if (events & TB_POLLER_EVENT_SEND) FD_SET(fd, &poller->wfds);

    // bind user private data to socket
    tb_hash_map_insert(poller->hash, sock, priv);

    // ok
    return tb_true;
}
tb_bool_t tb_poller_remove(tb_poller_ref_t self, tb_socket_ref_t sock)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->hash && sock, tb_false);

    // remove fds
    tb_long_t fd = tb_sock2fd(sock);
    FD_CLR(fd, &poller->rfds);
    FD_CLR(fd, &poller->wfds);

    // remove user private data from this socket
    tb_hash_map_remove(poller->hash, sock);

    // ok
    return tb_true;
}
tb_bool_t tb_poller_modify(tb_poller_ref_t self, tb_socket_ref_t sock, tb_size_t events, tb_cpointer_t priv)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->hash && sock, tb_false);

    // modify events
    tb_long_t fd = tb_sock2fd(sock);
    if (events & TB_POLLER_EVENT_RECV) FD_SET(fd, &poller->rfds); else FD_CLR(fd, &poller->rfds);
    if (events & TB_POLLER_EVENT_SEND) FD_SET(fd, &poller->wfds); else FD_CLR(fd, &poller->wfds);

    // modify user private data to socket
    tb_hash_map_insert(poller->hash, sock, priv);

    // ok
    return tb_true;
}
tb_long_t tb_poller_wait(tb_poller_ref_t self, tb_poller_event_func_t func, tb_long_t timeout)
{
    // check
    tb_poller_select_ref_t poller = (tb_poller_select_ref_t)self;
    tb_assert_and_check_return_val(poller && poller->hash && func, -1);

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
    while (!wait && !stop && (timeout < 0 || tb_mclock() < time + timeout))
    {
        // wait
#ifdef TB_CONFIG_OS_WINDOWS
        tb_long_t sfdn = tb_ws2_32()->select((tb_int_t) poller->sfdm + 1, &poller->rfds, &poller->wfds, tb_null, timeout >= 0? &t : tb_null);
#else
        tb_long_t sfdn = select(poller->sfdm + 1, &poller->rfds, &poller->wfds, tb_null, timeout >= 0? &t : tb_null);
#endif
        tb_assert_and_check_return_val(sfdn >= 0, -1);

        // timeout?
        tb_check_return_val(sfdn, 0);
        
        // sync
        tb_for_all_if (tb_hash_map_item_ref_t, item, poller->hash, item)
        {
            // end?
            tb_check_break(wait >= 0);

            // the sock
            tb_socket_ref_t sock = (tb_socket_ref_t)item->name;
            tb_assert_and_check_return_val(sock, -1);

            // spak?
            if (sock == poller->pair[1] && FD_ISSET(tb_sock2fd(poller->pair[1]), &poller->rfds))
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
            tb_check_continue(sock != poller->pair[1]);

            // init events
            tb_long_t fd = tb_sock2fd(sock);
            tb_size_t events = TB_POLLER_EVENT_NONE;
            if (FD_ISSET(fd, &poller->rfds)) events |= TB_POLLER_EVENT_RECV;
            if (FD_ISSET(fd, &poller->wfds)) events |= TB_POLLER_EVENT_SEND;

            // exists events?
            if (events)
            {
                // call event function
                func(self, sock, events, item->data);

                // update the events count
                wait++;
            }
        }
    }

    // ok
    return wait;
}

