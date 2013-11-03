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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		epoll.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include <sys/epoll.h>
#include <fcntl.h>
#ifndef TB_CONFIG_OS_ANDROID
# 	include <sys/unistd.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the epoll reactor type
typedef struct __tb_aiop_reactor_epoll_t
{
	// the reactor base
	tb_aiop_reactor_t 		base;

	// the epoll fd
	tb_long_t 				epfd;

	// the epoll events
	struct epoll_event* 	evts;
	tb_size_t 				evtn;
	
}tb_aiop_reactor_epoll_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aiop_reactor_epoll_addo(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t aioe)
{
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd > 0 && handle, tb_false);

	// init 
	struct epoll_event e = {0};
	if (aioe & TB_AIOE_CODE_RECV || aioe & TB_AIOE_CODE_ACPT) e.events |= EPOLLIN;
	if (aioe & TB_AIOE_CODE_SEND || aioe & TB_AIOE_CODE_CONN) e.events |= EPOLLOUT;
	e.data.u64 = (tb_hize_t)handle;

	// ctrl
	if (epoll_ctl(rtor->epfd, EPOLL_CTL_ADD, ((tb_long_t)handle) - 1, &e) < 0) return tb_false;

	// ok
	return tb_true;
}
static tb_bool_t tb_aiop_reactor_epoll_seto(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t aioe, tb_aioo_t const* obj)
{
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd > 0 && handle, tb_false);

	// init 
	struct epoll_event e = {0};
	if (aioe & TB_AIOE_CODE_RECV || aioe & TB_AIOE_CODE_ACPT) e.events |= EPOLLIN;
	if (aioe & TB_AIOE_CODE_SEND || aioe & TB_AIOE_CODE_CONN) e.events |= EPOLLOUT;
	e.data.u64 = (tb_hize_t)handle;

	// ctrl
	if (epoll_ctl(rtor->epfd, EPOLL_CTL_MOD, ((tb_long_t)handle) - 1, &e) < 0) return tb_false;

	// ok
	return tb_true;
}
static tb_bool_t tb_aiop_reactor_epoll_delo(tb_aiop_reactor_t* reactor, tb_handle_t handle)
{
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd > 0 && handle, tb_false);

	// ctrl
	struct epoll_event e = {0};
	if (epoll_ctl(rtor->epfd, EPOLL_CTL_DEL, ((tb_long_t)handle) - 1, &e) < 0) return tb_false;

	// ok
	return tb_true;
}
static tb_long_t tb_aiop_reactor_epoll_wait(tb_aiop_reactor_t* reactor, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout)
{	
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd > 0 && reactor->aiop && reactor->aiop->hash, -1);

	// init grow
	tb_size_t maxn = reactor->aiop->maxn;
	tb_size_t grow = tb_align8((maxn >> 3) + 1);

	// init events
	if (!rtor->evts)
	{
		rtor->evtn = grow;
		rtor->evts = tb_nalloc0(rtor->evtn, sizeof(struct epoll_event));
		tb_assert_and_check_return_val(rtor->evts, -1);
	}
	
	// wait events
	tb_long_t evtn = epoll_wait(rtor->epfd, rtor->evts, rtor->evtn, timeout);
	tb_assert_and_check_return_val(evtn >= 0 && evtn <= rtor->evtn, -1);
	
	// timeout?
	tb_check_return_val(evtn, 0);

	// grow it if events is full
	if (evtn == rtor->evtn)
	{
		// grow size
		rtor->evtn += grow;
		if (rtor->evtn > maxn) rtor->evtn = maxn;

		// grow data
		rtor->evts = tb_ralloc(rtor->evts, rtor->evtn * sizeof(struct epoll_event));
		tb_assert_and_check_return_val(rtor->evts, -1);
	}
	tb_assert(evtn <= rtor->evtn);

	// limit 
	evtn = tb_min(evtn, objm);

	// sync
	tb_size_t i = 0;
	for (i = 0; i < evtn; i++)
	{
		struct epoll_event* e = rtor->evts + i;
		tb_aioo_t* 			o = objs + i;
		tb_handle_t 		h = (tb_handle_t)e->data.u64;
		tb_aioo_t* 			p = tb_hash_get(reactor->aiop->hash, h);
		tb_assert_and_check_return_val(p, -1);

		o->handle = h;
		o->aioe = 0;
		o->data = p->data;
		if (e->events & EPOLLIN) 
		{
			o->aioe |= TB_AIOE_CODE_RECV;
			if (p->aioe & TB_AIOE_CODE_ACPT) o->aioe |= TB_AIOE_CODE_ACPT;
		}
		if (e->events & EPOLLOUT) 
		{
			o->aioe |= TB_AIOE_CODE_SEND;
			if (p->aioe & TB_AIOE_CODE_CONN) o->aioe |= TB_AIOE_CODE_CONN;
		}
		if (e->events & (EPOLLHUP | EPOLLERR) && !(o->aioe & TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND)) 
			o->aioe |= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND;
	}

	// ok
	return evtn;
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

		// FIXME: re-init it
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
	rtor->base.seto = tb_aiop_reactor_epoll_seto;
	rtor->base.delo = tb_aiop_reactor_epoll_delo;
	rtor->base.wait = tb_aiop_reactor_epoll_wait;

	// init epoll
	rtor->epfd = epoll_create(aiop->maxn);
	tb_assert_and_check_goto(rtor->epfd > 0, fail);

	// ok
	return (tb_aiop_reactor_t*)rtor;

fail:
	if (rtor) tb_aiop_reactor_epoll_exit(rtor);
	return tb_null;
}

