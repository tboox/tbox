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
 * @author		ruki
 * @file		aiop_epoll.c
 *
 */
/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#ifndef TB_CONFIG_OS_ANDROID
# 	include <sys/unistd.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the epoll reactor type
typedef struct __tb_aiop_reactor_epoll_t
{
	// the reactor base
	tb_aiop_reactor_t 		base;

	// the fd
	tb_long_t 				epfd;

	// the events
	struct epoll_event* 	evts;
	tb_size_t 				evtn;

	// the kill
	tb_handle_t 			kill[2];
	
}tb_aiop_reactor_epoll_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aiop_reactor_epoll_addo(tb_aiop_reactor_t* reactor, tb_aioo_t const* aioo)
{
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd > 0 && aioo && aioo->handle, tb_false);

	// the code
	tb_size_t code = aioo->code;

	// addo
	struct epoll_event e = {0};
	if (code & TB_AIOE_CODE_RECV || code & TB_AIOE_CODE_ACPT) e.events |= EPOLLIN;
	if (code & TB_AIOE_CODE_SEND || code & TB_AIOE_CODE_CONN) e.events |= EPOLLOUT;
#ifdef EPOLLONESHOT // FIXME: no oneshot for android
	if (code & TB_AIOE_CODE_ONESHOT) e.events |= EPOLLONESHOT;
#endif
	e.data.u64 = (tb_hize_t)aioo;
	return (epoll_ctl(rtor->epfd, EPOLL_CTL_ADD, ((tb_long_t)aioo->handle) - 1, &e) < 0)? tb_false : tb_true;
}
static tb_bool_t tb_aiop_reactor_epoll_delo(tb_aiop_reactor_t* reactor, tb_aioo_t const* aioo)
{
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd > 0 && aioo && aioo->handle, tb_false);

	// delo
	struct epoll_event e = {0};
	return (epoll_ctl(rtor->epfd, EPOLL_CTL_DEL, ((tb_long_t)aioo->handle) - 1, &e) < 0)? tb_false : tb_true;
}
static tb_bool_t tb_aiop_reactor_epoll_post(tb_aiop_reactor_t* reactor, tb_aioe_t const* aioe)
{
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd > 0 && aioe, tb_false);

	// the code
	tb_size_t 		code = aioe->code;

	// the data
	tb_pointer_t 	data = aioe->data;

	// the aioo
	tb_aioo_t* 		aioo = aioe->aioo;
	tb_assert_and_check_return_val(aioo && aioo->handle, tb_false);

	// init 
	struct epoll_event e = {0};
	if (code & TB_AIOE_CODE_RECV || code & TB_AIOE_CODE_ACPT) e.events |= EPOLLIN;
	if (code & TB_AIOE_CODE_SEND || code & TB_AIOE_CODE_CONN) e.events |= EPOLLOUT;
#ifdef EPOLLONESHOT // FIXME: no oneshot for android
	if (code & TB_AIOE_CODE_ONESHOT) e.events |= EPOLLONESHOT;
#endif
	e.data.u64 = (tb_hize_t)aioo;

	// save aioo
	tb_aioo_t prev = *aioo;
	aioo->code = code;
	aioo->data = data;

	// sete
	if (epoll_ctl(rtor->epfd, EPOLL_CTL_MOD, ((tb_long_t)aioo->handle) - 1, &e) < 0) 
	{
		// restore aioo
		*aioo = prev;
		return tb_false;
	}

	// ok
	return tb_true;
}
static tb_long_t tb_aiop_reactor_epoll_wait(tb_aiop_reactor_t* reactor, tb_aioe_t* list, tb_size_t maxn, tb_long_t timeout)
{	
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd > 0, -1);

	// the aiop
	tb_aiop_t* aiop = reactor->aiop;
	tb_assert_and_check_return_val(aiop, -1);

	// init grow
	tb_size_t grow = tb_align8((reactor->aiop->maxn >> 3) + 1);

	// init events
	if (!rtor->evts)
	{
		rtor->evtn = grow;
		rtor->evts = tb_nalloc0(rtor->evtn, sizeof(struct epoll_event));
		tb_assert_and_check_return_val(rtor->evts, -1);
	}
	
	// wait events
	tb_long_t evtn = epoll_wait(rtor->epfd, rtor->evts, rtor->evtn, timeout);

	// interrupted?(for gdb?) continue it
	if (evtn < 0 && errno == EINTR) return 0;

	// check error?
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
		rtor->evts = tb_ralloc(rtor->evts, rtor->evtn * sizeof(struct epoll_event));
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
		// the aioo
		tb_aioo_t const* aioo = (tb_aioo_t const*)rtor->evts[i].data.u64;
		tb_assert_and_check_return_val(aioo, -1);

		// the handle 
		tb_handle_t handle = aioo->handle;
		tb_assert_and_check_return_val(handle, -1);

		// the events
		tb_size_t events = rtor->evts[i].events;

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
		aioe->data = aioo->data;
		aioe->aioo = (tb_aioo_t*)aioo;
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
	}

	// ok
	return wait;
}
static tb_void_t tb_aiop_reactor_epoll_exit(tb_aiop_reactor_t* reactor)
{
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	if (rtor)
	{
		// exit events
		if (rtor->evts) tb_free(rtor->evts);
		rtor->evts = tb_null;

		// exit fd
		if (rtor->epfd) close(rtor->epfd);
		rtor->epfd = 0;

		// exit it
		tb_free(rtor);
	}
}
static tb_void_t tb_aiop_reactor_epoll_cler(tb_aiop_reactor_t* reactor)
{
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	if (rtor)
	{
		// close fd
		if (rtor->epfd > 0) close(rtor->epfd);
		rtor->epfd = 0;

		// re-init it
		if (rtor->base.aiop) rtor->epfd = epoll_create(rtor->base.aiop->maxn);
		tb_assert(rtor->epfd > 0);
	}
}
static tb_aiop_reactor_t* tb_aiop_reactor_epoll_init(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->maxn, tb_null);

	// make reactor
	tb_aiop_reactor_epoll_t* rtor = tb_malloc0(sizeof(tb_aiop_reactor_epoll_t));
	tb_assert_and_check_return_val(rtor, tb_null);

	// init base
	rtor->base.aiop = aiop;
	rtor->base.exit = tb_aiop_reactor_epoll_exit;
	rtor->base.cler = tb_aiop_reactor_epoll_cler;
	rtor->base.addo = tb_aiop_reactor_epoll_addo;
	rtor->base.delo = tb_aiop_reactor_epoll_delo;
	rtor->base.post = tb_aiop_reactor_epoll_post;
	rtor->base.wait = tb_aiop_reactor_epoll_wait;

	// init epoll
	rtor->epfd = epoll_create(aiop->maxn);
	tb_assert_and_check_goto(rtor->epfd > 0, fail);

	// ok
	return (tb_aiop_reactor_t*)rtor;

fail:
	if (rtor) tb_aiop_reactor_epoll_exit((tb_aiop_reactor_t*)rtor);
	return tb_null;
}

