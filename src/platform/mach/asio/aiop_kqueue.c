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
 * @file        aiop_kqueue.c
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <errno.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>

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

// the kqueue reactor type
typedef struct __tb_aiop_reactor_kqueue_t
{
    // the reactor base
    tb_aiop_reactor_t       base;

    // the kqueue fd
    tb_long_t               kqfd;

    // the events
    struct kevent*          evts;
    tb_size_t               evtn;
    
}tb_aiop_reactor_kqueue_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aiop_reactor_kqueue_sync(tb_aiop_reactor_t* reactor, struct kevent* evts, tb_size_t evtn)
{
    // check
    tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
    tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0, tb_false);
    tb_assert_and_check_return_val(evts && evtn, tb_false);

    // change events
    struct timespec t = {0};
    if (kevent(rtor->kqfd, evts, evtn, tb_object_null, 0, &t) < 0) return tb_false;

    // ok
    return tb_true;
}
static tb_bool_t tb_aiop_reactor_kqueue_addo(tb_aiop_reactor_t* reactor, tb_aioo_t const* aioo)
{
    // check
    tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
    tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0 && aioo && aioo->handle, tb_false);

    // fd
    tb_int_t fd = ((tb_int_t)aioo->handle) - 1;

    // the code
    tb_size_t code = aioo->code;

    // add event
    struct kevent   e[2];
    tb_size_t       n = 0;
    tb_size_t       oneshot = (code & TB_AIOE_CODE_ONESHOT)? EV_ONESHOT : 0;
    if (code & TB_AIOE_CODE_RECV || code & TB_AIOE_CODE_ACPT) 
    {
        EV_SET(&e[n], fd, EVFILT_READ, oneshot | EV_ADD | EV_ENABLE, NOTE_EOF, 0, (tb_pointer_t)aioo); n++;
    }
    if (code & TB_AIOE_CODE_SEND || code & TB_AIOE_CODE_CONN)
    {
        EV_SET(&e[n], fd, EVFILT_WRITE, oneshot | EV_ADD | EV_ENABLE, NOTE_EOF, 0, (tb_pointer_t)aioo); n++;
    }

    // ok?
    return n? tb_aiop_reactor_kqueue_sync(reactor, e, n) : tb_true;
}
static tb_bool_t tb_aiop_reactor_kqueue_delo(tb_aiop_reactor_t* reactor, tb_aioo_t const* aioo)
{
    // check
    tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
    tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0 && aioo && aioo->handle, tb_false);

    // fd
    tb_int_t fd = ((tb_int_t)aioo->handle) - 1;

    // del event
    struct kevent e[2];
    EV_SET(&e[0], fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
    EV_SET(&e[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);

    // ok?
    return tb_aiop_reactor_kqueue_sync(reactor, e, 2);
}
static tb_bool_t tb_aiop_reactor_kqueue_post(tb_aiop_reactor_t* reactor, tb_aioe_t const* aioe)
{
    // check
    tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
    tb_assert_and_check_return_val(rtor && aioe, tb_false);

    // the aioo
    tb_aioo_t* aioo = aioe->aioo;
    tb_assert_and_check_return_val(aioo && aioo->handle, tb_false);

    // fd
    tb_int_t fd = ((tb_int_t)aioo->handle) - 1;

    // change
    tb_size_t adde = aioe->code & ~aioo->code;
    tb_size_t dele = ~aioe->code & aioo->code;

    // oneshot?
    tb_size_t oneshot = (aioe->code & TB_AIOE_CODE_ONESHOT)? EV_ONESHOT : 0;

    // save aioo
    aioo->code = aioe->code;
    aioo->priv = aioe->priv;

    // add event
    struct kevent   e[2];
    tb_size_t       n = 0;
    if (adde & TB_AIOE_CODE_RECV || adde & TB_AIOE_CODE_ACPT) 
    {
        EV_SET(&e[n], fd, EVFILT_READ, oneshot | EV_ADD | EV_ENABLE, NOTE_EOF, 0, aioo);
        n++;
    }
    else if (dele & TB_AIOE_CODE_RECV || dele & TB_AIOE_CODE_ACPT) 
    {
        EV_SET(&e[n], fd, EVFILT_READ, EV_DELETE, 0, 0, aioo);
        n++;
    }
    if (adde & TB_AIOE_CODE_SEND || adde & TB_AIOE_CODE_CONN)
    {
        EV_SET(&e[n], fd, EVFILT_WRITE, oneshot | EV_ADD | EV_ENABLE, NOTE_EOF, 0, aioo);
        n++;
    }
    else if (dele & TB_AIOE_CODE_SEND || dele & TB_AIOE_CODE_CONN)
    {
        EV_SET(&e[n], fd, EVFILT_WRITE, EV_DELETE, 0, 0, aioo);
        n++;
    }

    // ok?
    return n? tb_aiop_reactor_kqueue_sync(reactor, e, n) : tb_true;
}
static tb_long_t tb_aiop_reactor_kqueue_wait(tb_aiop_reactor_t* reactor, tb_aioe_t* list, tb_size_t maxn, tb_long_t timeout)
{   
    // check
    tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
    tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0 && reactor->aiop && list && maxn, -1);

    // the aiop
    tb_aiop_t* aiop = reactor->aiop;
    tb_assert_and_check_return_val(aiop, -1);

    // init time
    struct timespec t = {0};
    if (timeout > 0)
    {
        t.tv_sec = timeout / 1000;
        t.tv_nsec = (timeout % 1000) * 1000000;
    }

    // init grow
    tb_size_t grow = tb_align8((reactor->aiop->maxn >> 3) + 1);

    // init events
    if (!rtor->evts)
    {
        rtor->evtn = grow;
        rtor->evts = tb_nalloc0(rtor->evtn, sizeof(struct kevent));
        tb_assert_and_check_return_val(rtor->evts, -1);
    }

    // wait events
    tb_long_t evtn = kevent(rtor->kqfd, tb_object_null, 0, rtor->evts, rtor->evtn, timeout >= 0? &t : tb_object_null);
    tb_assert_and_check_return_val(evtn >= 0 && evtn <= rtor->evtn, -1);
    
    // timeout?
    tb_check_return_val(evtn, 0);

    // grow it if events is full
    if (evtn == rtor->evtn)
    {
        // grow size
        rtor->evtn += grow;
        if (rtor->evtn > reactor->aiop->maxn) rtor->evtn = reactor->aiop->maxn;

        // grow data
        rtor->evts = tb_ralloc(rtor->evts, rtor->evtn * sizeof(struct kevent));
        tb_assert_and_check_return_val(rtor->evts, -1);
    }
    tb_assert(evtn <= rtor->evtn);

    // limit 
    evtn = tb_min(evtn, maxn);

    // sync
    tb_size_t i = 0;
    tb_size_t wait = 0;
    for (i = 0; i < evtn; i++)
    {
        // the kevents 
        struct kevent* e = rtor->evts + i;

        // the aioo
        tb_aioo_t* aioo = (tb_aioo_t*)e->udata;
        tb_assert_and_check_return_val(aioo && aioo->handle, -1);
        
        // the handle 
        tb_handle_t handle = aioo->handle;

        // spak?
        if (handle == aiop->spak[1] && e->filter == EVFILT_READ) 
        {
            // read spak
            tb_char_t spak = '\0';
            if (1 != tb_socket_recv(aiop->spak[1], (tb_byte_t*)&spak, 1)) return -1;

            // killed?
            if (spak == 'k') return -1;

            // continue it
            continue ;
        }

        // skip spak
        tb_check_continue(handle != aiop->spak[1]);

        // init the aioe
        tb_aioe_t* aioe = &list[wait++];
        aioe->code = TB_AIOE_CODE_NONE;
        aioe->aioo = aioo;
        aioe->priv = aioo->priv;
        if (e->filter == EVFILT_READ) 
        {
            aioe->code |= TB_AIOE_CODE_RECV;
            if (aioo->code & TB_AIOE_CODE_ACPT) aioe->code |= TB_AIOE_CODE_ACPT;
        }
        if (e->filter == EVFILT_WRITE) 
        {
            aioe->code |= TB_AIOE_CODE_SEND;
            if (aioo->code & TB_AIOE_CODE_CONN) aioe->code |= TB_AIOE_CODE_CONN;
        }
        if ((e->flags & EV_ERROR) && !(aioe->code & (TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND))) 
            aioe->code |= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND;

        // oneshot? clear it
        if (aioo->code & TB_AIOE_CODE_ONESHOT) 
        {
            aioo->code = TB_AIOE_CODE_NONE;
            aioo->priv = tb_object_null;
        }
    }

    // ok
    return wait;
}
static tb_void_t tb_aiop_reactor_kqueue_exit(tb_aiop_reactor_t* reactor)
{
    tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
    if (rtor)
    {
        // free events
        if (rtor->evts) tb_free(rtor->evts);

        // close kqfd
        if (rtor->kqfd >= 0) close(rtor->kqfd);

        // free it
        tb_free(rtor);
    }
}
static tb_void_t tb_aiop_reactor_kqueue_cler(tb_aiop_reactor_t* reactor)
{
    tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
    if (rtor)
    {
        // close kqfd
        if (rtor->kqfd >= 0)
        {
            close(rtor->kqfd);
            rtor->kqfd = kqueue();
        }
    }
}
static tb_aiop_reactor_t* tb_aiop_reactor_kqueue_init(tb_aiop_t* aiop)
{
    // check
    tb_assert_and_check_return_val(aiop && aiop->maxn, tb_object_null);

    // alloc reactor
    tb_aiop_reactor_kqueue_t* rtor = tb_malloc0(sizeof(tb_aiop_reactor_kqueue_t));
    tb_assert_and_check_return_val(rtor, tb_object_null);

    // init base
    rtor->base.aiop = aiop;
    rtor->base.exit = tb_aiop_reactor_kqueue_exit;
    rtor->base.cler = tb_aiop_reactor_kqueue_cler;
    rtor->base.addo = tb_aiop_reactor_kqueue_addo;
    rtor->base.delo = tb_aiop_reactor_kqueue_delo;
    rtor->base.post = tb_aiop_reactor_kqueue_post;
    rtor->base.wait = tb_aiop_reactor_kqueue_wait;

    // init kqueue
    rtor->kqfd = kqueue();
    tb_assert_and_check_goto(rtor->kqfd >= 0, fail);

    // ok
    return (tb_aiop_reactor_t*)rtor;

fail:
    if (rtor) tb_aiop_reactor_kqueue_exit((tb_aiop_reactor_t*)rtor);
    return tb_object_null;
}

