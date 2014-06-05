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
 * @file        aiop_select.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../spinlock.h"
#ifdef TB_CONFIG_OS_WINDOWS
#   include "../../windows/interface/interface.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the poll lock type
typedef struct __tb_select_lock_t
{
    // the pfds
    tb_spinlock_t           pfds;

    // the hash
    tb_spinlock_t           hash;

}tb_select_lock_t;

// the select reactor type
typedef struct __tb_aiop_reactor_select_t
{
    // the reactor base
    tb_aiop_reactor_t       base;

    // the fd max
    tb_size_t               sfdm;

    // the select fds
    fd_set                  rfdi;
    fd_set                  wfdi;
    fd_set                  efdi;

    fd_set                  rfdo;
    fd_set                  wfdo;
    fd_set                  efdo;

    // the hash
    tb_hash_t*              hash;

    // the lock
    tb_select_lock_t        lock;

}tb_aiop_reactor_select_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aiop_reactor_select_addo(tb_aiop_reactor_t* reactor, tb_aioo_t const* aioo)
{
    // check
    tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
    tb_assert_and_check_return_val(rtor && reactor->aiop && aioo && aioo->handle, tb_false);

    // the aiop
    tb_aiop_t* aiop = reactor->aiop;
    tb_assert_and_check_return_val(aiop, tb_false);

    // check size
    tb_spinlock_enter(&rtor->lock.hash);
    tb_size_t size = tb_hash_size(rtor->hash);
    tb_spinlock_leave(&rtor->lock.hash);
    tb_assert_and_check_return_val(size < FD_SETSIZE, tb_false);

    // add handle => aioo
    tb_bool_t ok = tb_false;
    tb_spinlock_enter(&rtor->lock.hash);
    if (rtor->hash) 
    {
        tb_hash_set(rtor->hash, aioo->handle, aioo);
        ok = tb_true;
    }
    tb_spinlock_leave(&rtor->lock.hash);
    tb_assert_and_check_return_val(ok, tb_false);

    // the fd
    tb_long_t fd = ((tb_long_t)aioo->handle) - 1;

    // the code
    tb_size_t code = aioo->code;

    // enter
    tb_spinlock_enter(&rtor->lock.pfds);

    // update fd max
    if (fd > rtor->sfdm) rtor->sfdm = fd;
    
    // init fds
    if (code & (TB_AIOE_CODE_RECV | TB_AIOE_CODE_ACPT)) FD_SET(fd, &rtor->rfdi);
    if (code & (TB_AIOE_CODE_SEND | TB_AIOE_CODE_CONN)) FD_SET(fd, &rtor->wfdi);
    FD_SET(fd, &rtor->efdi);

    // leave
    tb_spinlock_leave(&rtor->lock.pfds);

    // spak it
    if (aiop->spak[0] && code) tb_socket_send(aiop->spak[0], (tb_byte_t const*)"p", 1);

    // ok?
    return ok;
}
static tb_bool_t tb_aiop_reactor_select_delo(tb_aiop_reactor_t* reactor, tb_aioo_t const* aioo)
{
    // check
    tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
    tb_assert_and_check_return_val(rtor && aioo && aioo->handle, tb_false);

    // the aiop
    tb_aiop_t* aiop = reactor->aiop;
    tb_assert_and_check_return_val(aiop, tb_false);

    // fd
    tb_long_t fd = ((tb_long_t)aioo->handle) - 1;

    // enter
    tb_spinlock_enter(&rtor->lock.pfds);

    // del fds
    FD_CLR(fd, &rtor->rfdi);
    FD_CLR(fd, &rtor->wfdi);
    FD_CLR(fd, &rtor->efdi);

    // leave
    tb_spinlock_leave(&rtor->lock.pfds);

    // del handle => aioo
    tb_spinlock_enter(&rtor->lock.hash);
    if (rtor->hash) tb_hash_del(rtor->hash, aioo->handle);
    tb_spinlock_leave(&rtor->lock.hash);

    // spak it
    if (aiop->spak[0]) tb_socket_send(aiop->spak[0], (tb_byte_t const*)"p", 1);

    // ok
    return tb_true;
}
static tb_bool_t tb_aiop_reactor_select_post(tb_aiop_reactor_t* reactor, tb_aioe_t const* aioe)
{
    // check
    tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
    tb_assert_and_check_return_val(rtor && aioe, tb_false);

    // the aiop
    tb_aiop_t* aiop = reactor->aiop;
    tb_assert_and_check_return_val(aiop, tb_false);

    // the aioo
    tb_aioo_t* aioo = aioe->aioo;
    tb_assert_and_check_return_val(aioo && aioo->handle, tb_false);

    // save aioo
    aioo->code = aioe->code;
    aioo->priv = aioe->priv;

    // fd
    tb_long_t fd = ((tb_long_t)aioo->handle) - 1;

    // enter
    tb_spinlock_enter(&rtor->lock.pfds);

    // set fds
    if (aioe->code & (TB_AIOE_CODE_RECV | TB_AIOE_CODE_ACPT)) FD_SET(fd, &rtor->rfdi); else FD_CLR(fd, &rtor->rfdi);
    if (aioe->code & (TB_AIOE_CODE_SEND | TB_AIOE_CODE_CONN)) FD_SET(fd, &rtor->wfdi); else FD_CLR(fd, &rtor->wfdi);
    if (    (aioe->code & (TB_AIOE_CODE_RECV | TB_AIOE_CODE_ACPT))
        ||  (aioe->code & (TB_AIOE_CODE_SEND | TB_AIOE_CODE_CONN)))
    {
        FD_SET(fd, &rtor->efdi); 
    }
    else 
    {
        FD_CLR(fd, &rtor->efdi);
    }

    // leave
    tb_spinlock_leave(&rtor->lock.pfds);

    // spak it
    if (aiop->spak[0]) tb_socket_send(aiop->spak[0], (tb_byte_t const*)"p", 1);

    // ok
    return tb_true;
}
static tb_long_t tb_aiop_reactor_select_wait(tb_aiop_reactor_t* reactor, tb_aioe_t* list, tb_size_t maxn, tb_long_t timeout)
{   
    // check
    tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
    tb_assert_and_check_return_val(rtor && reactor->aiop && list && maxn, -1);

    // the aiop
    tb_aiop_t* aiop = reactor->aiop;
    tb_assert_and_check_return_val(aiop, tb_false);

    // init time
    struct timeval t = {0};
    if (timeout > 0)
    {
        t.tv_sec = timeout / 1000;
        t.tv_usec = (timeout % 1000) * 1000;
    }

    // loop
    tb_long_t wait = 0;
    tb_bool_t stop = tb_false;
    tb_hong_t time = tb_mclock();
    while (!wait && !stop && (timeout < 0 || tb_mclock() < time + timeout))
    {
        // enter
        tb_spinlock_enter(&rtor->lock.pfds);

        // init fdo
        tb_size_t sfdm = rtor->sfdm;
        tb_memcpy(&rtor->rfdo, &rtor->rfdi, sizeof(fd_set));
        tb_memcpy(&rtor->wfdo, &rtor->wfdi, sizeof(fd_set));
        tb_memcpy(&rtor->efdo, &rtor->efdi, sizeof(fd_set));

        // leave
        tb_spinlock_leave(&rtor->lock.pfds);

        // wait
#ifdef TB_CONFIG_OS_WINDOWS
        tb_long_t sfdn = tb_ws2_32()->select(sfdm + 1, &rtor->rfdo, &rtor->wfdo, &rtor->efdo, timeout >= 0? &t : tb_null);
#else
        tb_long_t sfdn = select(sfdm + 1, &rtor->rfdo, &rtor->wfdo, &rtor->efdo, timeout >= 0? &t : tb_null);
#endif
        tb_assert_and_check_return_val(sfdn >= 0, -1);

        // timeout?
        tb_check_return_val(sfdn, 0);
        
        // enter
        tb_spinlock_enter(&rtor->lock.hash);

        // sync
        tb_size_t itor = tb_iterator_head(rtor->hash);
        tb_size_t tail = tb_iterator_tail(rtor->hash);
        for (; itor != tail && wait >= 0 && wait < maxn; itor = tb_iterator_next(rtor->hash, itor))
        {
            tb_hash_item_t* item = tb_iterator_item(rtor->hash, itor);
            if (item)
            {
                // the handle
                tb_handle_t handle = (tb_handle_t)item->name;
                tb_assert_and_check_return_val(handle, -1);

                // spak?
                if (handle == aiop->spak[1] && FD_ISSET(((tb_long_t)aiop->spak[1] - 1), &rtor->rfdo))
                {
                    // read spak
                    tb_char_t spak = '\0';
                    if (1 != tb_socket_recv(aiop->spak[1], (tb_byte_t*)&spak, 1)) wait = -1;

                    // killed?
                    if (spak == 'k') wait = -1;
                    tb_check_break(wait >= 0);

                    // stop to wait
                    stop = tb_true;

                    // continue it
                    continue ;
                }

                // filter spak
                tb_check_continue(handle != aiop->spak[1]);

                // the fd
                tb_long_t fd = (tb_long_t)item->name - 1;

                // the aioo
                tb_aioo_t* aioo = (tb_aioo_t*)item->data;
                tb_assert_and_check_return_val(aioo && aioo->handle == handle, -1);

                // init aioe
                tb_aioe_t aioe = {0};
                aioe.priv   = aioo->priv;
                aioe.aioo   = aioo;
                if (FD_ISSET(fd, &rtor->rfdo)) 
                {
                    aioe.code |= TB_AIOE_CODE_RECV;
                    if (aioo->code & TB_AIOE_CODE_ACPT) aioe.code |= TB_AIOE_CODE_ACPT;
                }
                if (FD_ISSET(fd, &rtor->wfdo)) 
                {
                    aioe.code |= TB_AIOE_CODE_SEND;
                    if (aioo->code & TB_AIOE_CODE_CONN) aioe.code |= TB_AIOE_CODE_CONN;
                }
                if (FD_ISSET(fd, &rtor->efdo) && !(aioe.code & (TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND))) 
                    aioe.code |= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND;
                    
                // ok?
                if (aioe.code) 
                {
                    // save aioe
                    list[wait++] = aioe;

                    // oneshot? clear it
                    if (aioo->code & TB_AIOE_CODE_ONESHOT)
                    {
                        // clear aioo
                        aioo->code = TB_AIOE_CODE_NONE;
                        aioo->priv = tb_null;

                        // clear events
                        tb_spinlock_enter(&rtor->lock.pfds);
                        FD_CLR(fd, &rtor->rfdi);
                        FD_CLR(fd, &rtor->wfdi);
                        FD_CLR(fd, &rtor->efdi);
                        tb_spinlock_leave(&rtor->lock.pfds);
                    }
                }
            }
        }

        // leave
        tb_spinlock_leave(&rtor->lock.hash);
    }

    // ok
    return wait;
}
static tb_void_t tb_aiop_reactor_select_exit(tb_aiop_reactor_t* reactor)
{
    tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
    if (rtor)
    {
        // free fds
        tb_spinlock_enter(&rtor->lock.pfds);
        FD_ZERO(&rtor->rfdi);
        FD_ZERO(&rtor->wfdi);
        FD_ZERO(&rtor->efdi);
        FD_ZERO(&rtor->rfdo);
        FD_ZERO(&rtor->wfdo);
        FD_ZERO(&rtor->efdo);
        tb_spinlock_leave(&rtor->lock.pfds);

        // exit hash
        tb_spinlock_enter(&rtor->lock.hash);
        if (rtor->hash) tb_hash_exit(rtor->hash);
        rtor->hash = tb_null;
        tb_spinlock_leave(&rtor->lock.hash);

        // exit lock
        tb_spinlock_exit(&rtor->lock.pfds);
        tb_spinlock_exit(&rtor->lock.hash);

        // free it
        tb_free(rtor);
    }
}
static tb_void_t tb_aiop_reactor_select_cler(tb_aiop_reactor_t* reactor)
{
    tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
    if (rtor)
    {
        // free fds
        tb_spinlock_enter(&rtor->lock.pfds);
        rtor->sfdm = 0;
        FD_ZERO(&rtor->rfdi);
        FD_ZERO(&rtor->wfdi);
        FD_ZERO(&rtor->efdi);
        FD_ZERO(&rtor->rfdo);
        FD_ZERO(&rtor->wfdo);
        FD_ZERO(&rtor->efdo);
        tb_spinlock_leave(&rtor->lock.pfds);

        // clear hash
        tb_spinlock_enter(&rtor->lock.hash);
        if (rtor->hash) tb_hash_clear(rtor->hash);
        tb_spinlock_leave(&rtor->lock.hash);

        // spak it
        if (reactor->aiop && reactor->aiop->spak[0]) tb_socket_send(reactor->aiop->spak[0], (tb_byte_t const*)"p", 1);
    }
}
static tb_aiop_reactor_t* tb_aiop_reactor_select_init(tb_aiop_t* aiop)
{
    // check
    tb_assert_and_check_return_val(aiop && aiop->maxn, tb_null);

    // done
    tb_bool_t                   ok = tb_false;
    tb_aiop_reactor_select_t*   rtor = tb_null;
    do
    {
        // make reactor
        rtor = (tb_aiop_reactor_select_t*)tb_malloc0(sizeof(tb_aiop_reactor_select_t));
        tb_assert_and_check_break(rtor);

        // init base
        rtor->base.aiop = aiop;
        rtor->base.exit = tb_aiop_reactor_select_exit;
        rtor->base.cler = tb_aiop_reactor_select_cler;
        rtor->base.addo = tb_aiop_reactor_select_addo;
        rtor->base.delo = tb_aiop_reactor_select_delo;
        rtor->base.post = tb_aiop_reactor_select_post;
        rtor->base.wait = tb_aiop_reactor_select_wait;

        // init fds
        FD_ZERO(&rtor->rfdi);
        FD_ZERO(&rtor->wfdi);
        FD_ZERO(&rtor->efdi);
        FD_ZERO(&rtor->rfdo);
        FD_ZERO(&rtor->wfdo);
        FD_ZERO(&rtor->efdo);

        // init lock
        if (!tb_spinlock_init(&rtor->lock.pfds)) break;
        if (!tb_spinlock_init(&rtor->lock.hash)) break;

        // init hash
        rtor->hash = tb_hash_init(tb_align8(tb_isqrti(aiop->maxn) + 1), tb_item_func_ptr(tb_null, tb_null), tb_item_func_ptr(tb_null, tb_null));
        tb_assert_and_check_break(rtor->hash);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (rtor) tb_aiop_reactor_select_exit((tb_aiop_reactor_t*)rtor);
        rtor = tb_null;
    }

    // ok?
    return (tb_aiop_reactor_t*)rtor;
}

