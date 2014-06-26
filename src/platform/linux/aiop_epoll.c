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
 * @file        aiop_epoll.c
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../asio/impl/impl.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#ifndef TB_CONFIG_OS_ANDROID
#   include <sys/unistd.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the epoll rtor type
typedef struct __tb_aiop_rtor_epoll_impl_t
{
    // the rtor base
    tb_aiop_rtor_impl_t     base;

    // the fd
    tb_long_t               epfd;

    // the events
    struct epoll_event*     evts;
    tb_size_t               evtn;

    // the kill
    tb_handle_t             kill[2];
    
}tb_aiop_rtor_epoll_impl_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aiop_rtor_epoll_addo(tb_aiop_rtor_impl_t* rtor, tb_aioo_t const* aioo)
{
    // check
    tb_aiop_rtor_epoll_impl_t* impl = (tb_aiop_rtor_epoll_impl_t*)rtor;
    tb_assert_and_check_return_val(impl && impl->epfd > 0 && aioo && aioo->handle, tb_false);

    // the code
    tb_size_t code = aioo->code;

    // init event
    struct epoll_event e = {0};
    if (code & TB_AIOE_CODE_RECV || code & TB_AIOE_CODE_ACPT) e.events |= EPOLLIN;
    if (code & TB_AIOE_CODE_SEND || code & TB_AIOE_CODE_CONN) e.events |= EPOLLOUT;
#ifdef EPOLLONESHOT 
    if (code & TB_AIOE_CODE_ONESHOT) e.events |= EPOLLONESHOT;
#endif
    e.data.u64 = (tb_hize_t)aioo;

    // add aioo
    if (epoll_ctl(impl->epfd, EPOLL_CTL_ADD, tb_handle2fd(aioo->handle), &e) < 0)
    {
        // trace
        tb_trace_e("addo aioo[%p], code: %lu failed, errno: %d", aioo, code, errno);
        return tb_false;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_aiop_rtor_epoll_delo(tb_aiop_rtor_impl_t* rtor, tb_aioo_t const* aioo)
{
    // check
    tb_aiop_rtor_epoll_impl_t* impl = (tb_aiop_rtor_epoll_impl_t*)rtor;
    tb_assert_and_check_return_val(impl && impl->epfd > 0 && aioo && aioo->handle, tb_false);

    // init event
    struct epoll_event e = {0};
    if (epoll_ctl(impl->epfd, EPOLL_CTL_DEL, ((tb_long_t)aioo->handle) - 1, &e) < 0)
    {
        // trace
//        tb_trace_e("delo aioo[%p] failed, errno: %d", aioo, errno);
        return tb_false;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_aiop_rtor_epoll_post(tb_aiop_rtor_impl_t* rtor, tb_aioe_t const* aioe)
{
    // check
    tb_aiop_rtor_epoll_impl_t* impl = (tb_aiop_rtor_epoll_impl_t*)rtor;
    tb_assert_and_check_return_val(impl && impl->epfd > 0 && aioe, tb_false);

    // the code
    tb_size_t       code = aioe->code;

    // the priv
    tb_cpointer_t   priv = aioe->priv;

    // the aioo
    tb_aioo_t*      aioo = aioe->aioo;
    tb_assert_and_check_return_val(aioo && aioo->handle, tb_false);

    // init event
    struct epoll_event e = {0};
    if (code & TB_AIOE_CODE_RECV || code & TB_AIOE_CODE_ACPT) e.events |= EPOLLIN;
    if (code & TB_AIOE_CODE_SEND || code & TB_AIOE_CODE_CONN) e.events |= EPOLLOUT;
#ifdef EPOLLONESHOT 
    if (code & TB_AIOE_CODE_ONESHOT) e.events |= EPOLLONESHOT;
#endif
    e.data.u64 = (tb_hize_t)aioo;

    // save aioo
    tb_aioo_t prev = *aioo;
    aioo->code = code;
    aioo->priv = priv;

    // sete
    if (epoll_ctl(impl->epfd, EPOLL_CTL_MOD, tb_handle2fd(aioo->handle), &e) < 0) 
    {
        // re-add it 
#ifndef EPOLLONESHOT 
        if (errno == ENOENT && epoll_ctl(impl->epfd, EPOLL_CTL_ADD, tb_handle2fd(aioo->handle), &e) >= 0) 
            return tb_true;
#endif

        // trace
        tb_trace_e("post aice code: %lu failed, errno: %d", code, errno);

        // restore aioo
        *aioo = prev;
        return tb_false;
    }

    // ok
    return tb_true;
}
static tb_long_t tb_aiop_rtor_epoll_wait(tb_aiop_rtor_impl_t* rtor, tb_aioe_t* list, tb_size_t maxn, tb_long_t timeout)
{   
    // check
    tb_aiop_rtor_epoll_impl_t* impl = (tb_aiop_rtor_epoll_impl_t*)rtor;
    tb_assert_and_check_return_val(impl && impl->epfd > 0, -1);

    // the aiop
    tb_aiop_impl_t* aiop = rtor->aiop;
    tb_assert_and_check_return_val(aiop, -1);

    // init grow
    tb_size_t grow = tb_align8((rtor->aiop->maxn >> 3) + 1);

    // init events
    if (!impl->evts)
    {
        impl->evtn = grow;
        impl->evts = tb_nalloc0(impl->evtn, sizeof(struct epoll_event));
        tb_assert_and_check_return_val(impl->evts, -1);
    }
    
    // wait events
    tb_long_t evtn = epoll_wait(impl->epfd, impl->evts, impl->evtn, timeout);

    // interrupted?(for gdb?) continue it
    if (evtn < 0 && errno == EINTR) return 0;

    // check error?
    tb_assert_and_check_return_val(evtn >= 0 && evtn <= impl->evtn, -1);
    
    // timeout?
    tb_check_return_val(evtn, 0);

    // grow it if events is full
    if (evtn == impl->evtn)
    {
        // grow size
        impl->evtn += grow;
        if (impl->evtn > rtor->aiop->maxn) impl->evtn = rtor->aiop->maxn;

        // grow data
        impl->evts = tb_ralloc(impl->evts, impl->evtn * sizeof(struct epoll_event));
        tb_assert_and_check_return_val(impl->evts, -1);
    }
    tb_assert(evtn <= impl->evtn);

    // limit 
    evtn = tb_min(evtn, maxn);

    // sync
    tb_size_t i = 0;
    tb_size_t wait = 0; 
    for (i = 0; i < evtn; i++)
    {
        // the aioo
        tb_aioo_t* aioo = (tb_aioo_t*)impl->evts[i].data.u64;
        tb_assert_and_check_return_val(aioo, -1);

        // the handle 
        tb_handle_t handle = aioo->handle;
        tb_assert_and_check_return_val(handle, -1);

        // the events
        tb_size_t events = impl->evts[i].events;

        // spak?
        if (handle == aiop->spak[1] && (events & EPOLLIN)) 
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

        // save aioe
        tb_aioe_t* aioe = &list[wait++];
        aioe->code = TB_AIOE_CODE_NONE;
        aioe->priv = aioo->priv;
        aioe->aioo = aioo;
        if (events & EPOLLIN) 
        {
            aioe->code |= TB_AIOE_CODE_RECV;
            if (aioo->code & TB_AIOE_CODE_ACPT) aioe->code |= TB_AIOE_CODE_ACPT;
        }
        if (events & EPOLLOUT) 
        {
            aioe->code |= TB_AIOE_CODE_SEND;
            if (aioo->code & TB_AIOE_CODE_CONN) aioe->code |= TB_AIOE_CODE_CONN;
        }
        if (events & (EPOLLHUP | EPOLLERR) && !(aioe->code & (TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND))) 
            aioe->code |= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND;

        // oneshot? clear it
        if (aioo->code & TB_AIOE_CODE_ONESHOT)
        {
            // clear code
            aioo->code = TB_AIOE_CODE_NONE;
            aioo->priv = tb_null;

            // clear events manually if no epoll oneshot
#ifndef EPOLLONESHOT
            struct epoll_event e = {0};
            if (epoll_ctl(impl->epfd, EPOLL_CTL_DEL, tb_handle2fd(aioo->handle), &e) < 0) 
            {
                // trace
                tb_trace_e("clear aioo[%p] failed manually for oneshot, error: %d", aioo, errno);
            }
#endif
        }
    }

    // ok
    return wait;
}
static tb_void_t tb_aiop_rtor_epoll_exit(tb_aiop_rtor_impl_t* rtor)
{
    tb_aiop_rtor_epoll_impl_t* impl = (tb_aiop_rtor_epoll_impl_t*)rtor;
    if (impl)
    {
        // exit events
        if (impl->evts) tb_free(impl->evts);
        impl->evts = tb_null;

        // exit fd
        if (impl->epfd) close(impl->epfd);
        impl->epfd = 0;

        // exit it
        tb_free(impl);
    }
}
static tb_void_t tb_aiop_rtor_epoll_cler(tb_aiop_rtor_impl_t* rtor)
{
    tb_aiop_rtor_epoll_impl_t* impl = (tb_aiop_rtor_epoll_impl_t*)rtor;
    if (impl)
    {
        // close fd
        if (impl->epfd > 0) close(impl->epfd);
        impl->epfd = 0;

        // re-init it
        if (impl->base.aiop) impl->epfd = epoll_create(impl->base.aiop->maxn);
        tb_assert(impl->epfd > 0);
    }
}
static tb_aiop_rtor_impl_t* tb_aiop_rtor_epoll_init(tb_aiop_impl_t* aiop)
{
    // check
    tb_assert_and_check_return_val(aiop && aiop->maxn, tb_null);

    // done
    tb_bool_t                   ok = tb_false;
    tb_aiop_rtor_epoll_impl_t*  impl = tb_null;
    do
    {
        // make impl
        impl = tb_malloc0_type(tb_aiop_rtor_epoll_impl_t);
        tb_assert_and_check_break(impl);

        // init base
        impl->base.aiop = aiop;
        impl->base.exit = tb_aiop_rtor_epoll_exit;
        impl->base.cler = tb_aiop_rtor_epoll_cler;
        impl->base.addo = tb_aiop_rtor_epoll_addo;
        impl->base.delo = tb_aiop_rtor_epoll_delo;
        impl->base.post = tb_aiop_rtor_epoll_post;
        impl->base.wait = tb_aiop_rtor_epoll_wait;

        // init epoll
        impl->epfd = epoll_create(aiop->maxn);
        tb_assert_and_check_break(impl->epfd > 0);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (impl) tb_aiop_rtor_epoll_exit((tb_aiop_rtor_impl_t*)impl);
        impl = tb_null;
    }

    // ok?
    return (tb_aiop_rtor_impl_t*)impl;
}

