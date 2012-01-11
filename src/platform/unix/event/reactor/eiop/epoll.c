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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		epoll.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include <sys/epoll.h>

/* /////////////////////////////////////////////////////////
 * types
 */

// the epoll reactor type
typedef struct __tb_eiop_reactor_epoll_t
{
	// the reactor base
	tb_eiop_reactor_t 		base;

	// the epoll fd
	tb_long_t 				epfd;

	// the events
	struct epoll_event* 	evts;
	tb_size_t 				evtn;
	
}tb_eiop_reactor_epoll_t;

/* /////////////////////////////////////////////////////////
 * implemention
 */
static tb_bool_t tb_eiop_reactor_epoll_addo(tb_eiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype)
{
	tb_eiop_reactor_epoll_t* rtor = (tb_eiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd >= 0, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);

	// init 
	struct epoll_event e = {0};
	if (etype & TB_EIO_ETYPE_READ || etype & TB_EIO_ETYPE_ACPT) e.events |= EPOLLIN;
	if (etype & TB_EIO_ETYPE_WRIT || etype & TB_EIO_ETYPE_CONN) e.events |= EPOLLOUT;
	e.events |= EPOLLET;
	e.data.u64 = (((tb_uint64_t)etype << 32) | (tb_uint64_t)(tb_uint32_t)handle);

	// ctrl
	if (epoll_ctl(rtor->epfd, EPOLL_CTL_ADD, fd, &e) < 0) return TB_FALSE;

	// ok
	return TB_TRUE;
}
static tb_bool_t tb_eiop_reactor_epoll_seto(tb_eiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype)
{
	tb_eiop_reactor_epoll_t* rtor = (tb_eiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd >= 0, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);

	// init 
	struct epoll_event e = {0};
	if (etype & TB_EIO_ETYPE_READ || etype & TB_EIO_ETYPE_ACPT) e.events |= EPOLLIN | EPOLLET;
	if (etype & TB_EIO_ETYPE_WRIT || etype & TB_EIO_ETYPE_CONN) e.events |= EPOLLOUT | EPOLLET;
	e.data.u64 = (((tb_uint64_t)etype << 32) | (tb_uint64_t)(tb_uint32_t)handle);

	// ctrl
	if (epoll_ctl(rtor->epfd, EPOLL_CTL_MOD, fd, &e) < 0) return TB_FALSE;

	// ok
	return TB_TRUE;
}
static tb_bool_t tb_eiop_reactor_epoll_delo(tb_eiop_reactor_t* reactor, tb_handle_t handle)
{
	tb_eiop_reactor_epoll_t* rtor = (tb_eiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd >= 0, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);

	// ctrl
	struct epoll_event e = {0};
	if (epoll_ctl(rtor->epfd, EPOLL_CTL_DEL, fd, &e) < 0) return TB_FALSE;

	// ok
	return TB_TRUE;
}
static tb_long_t tb_eiop_reactor_epoll_wait(tb_eiop_reactor_t* reactor, tb_long_t timeout)
{	
	tb_eiop_reactor_epoll_t* rtor = (tb_eiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd >= 0 && reactor->eiop, -1);

	// init grow
	tb_size_t maxn = reactor->eiop->maxn;
	tb_size_t grow = tb_align8((maxn >> 3) + 1);

	// init events
	if (!rtor->evts)
	{
		rtor->evtn = grow;
		rtor->evts = tb_calloc(rtor->evtn, sizeof(struct epoll_event));
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
		rtor->evts = tb_realloc(rtor->evts, rtor->evtn * sizeof(struct epoll_event));
		tb_assert_and_check_return_val(rtor->evts, -1);
	}
	tb_assert(evtn <= rtor->evtn);

	// ok
	return evtn;
}
static tb_void_t tb_eiop_reactor_epoll_sync(tb_eiop_reactor_t* reactor, tb_size_t evtn)
{	
	tb_eiop_reactor_epoll_t* rtor = (tb_eiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return(rtor && rtor->epfd >= 0 && reactor->eiop && reactor->eiop->objs);

	// sync
	tb_size_t i = 0;
	for (i = 0; i < evtn; i++)
	{
		struct epoll_event* e = rtor->evts + i;
		tb_eio_t* 		o = reactor->eiop->objs + i;
		tb_size_t 			etype = (tb_size_t)((e->data.u64 >> 32) & 0x00ffffff);

		o->handle = (tb_handle_t)(tb_uint32_t)e->data.u64;
		o->otype = reactor->eiop->type;
		o->etype = 0;
		if (e->events & EPOLLIN) 
		{
			o->etype |= TB_EIO_ETYPE_READ;
			if (etype & TB_EIO_ETYPE_ACPT) o->etype |= TB_EIO_ETYPE_ACPT;
		}
		if (e->events & EPOLLOUT) 
		{
			o->etype |= TB_EIO_ETYPE_WRIT;
			if (etype & TB_EIO_ETYPE_CONN) o->etype |= TB_EIO_ETYPE_CONN;
		}
		if (e->events & (EPOLLHUP | EPOLLERR) && !(o->etype & TB_EIO_ETYPE_READ | TB_EIO_ETYPE_WRIT)) 
			o->etype |= TB_EIO_ETYPE_READ | TB_EIO_ETYPE_WRIT;
	}
}
static tb_void_t tb_eiop_reactor_epoll_exit(tb_eiop_reactor_t* reactor)
{
	tb_eiop_reactor_epoll_t* rtor = (tb_eiop_reactor_epoll_t*)reactor;
	if (rtor)
	{
		// free events
		if (rtor->evts) tb_free(rtor->evts);

		// close fd
		if (rtor->epfd) close(rtor->epfd);

		// free it
		tb_free(rtor);
	}
}
static tb_eiop_reactor_t* tb_eiop_reactor_epoll_init(tb_eiop_t* eiop)
{
	// check
	tb_assert_and_check_return_val(eiop && eiop->maxn, TB_NULL);
	tb_assert_and_check_return_val(eiop->type == TB_EIO_OTYPE_FILE || eiop->type == TB_EIO_OTYPE_SOCK, TB_NULL);

	// alloc reactor
	tb_eiop_reactor_epoll_t* rtor = tb_calloc(1, sizeof(tb_eiop_reactor_epoll_t));
	tb_assert_and_check_return_val(rtor, TB_NULL);

	// init base
	rtor->base.eiop = eiop;
	rtor->base.exit = tb_eiop_reactor_epoll_exit;
	rtor->base.addo = tb_eiop_reactor_epoll_addo;
	rtor->base.seto = tb_eiop_reactor_epoll_seto;
	rtor->base.delo = tb_eiop_reactor_epoll_delo;
	rtor->base.wait = tb_eiop_reactor_epoll_wait;
	rtor->base.sync = tb_eiop_reactor_epoll_sync;

	// init epoll
	rtor->epfd = epoll_create(eiop->maxn);
	tb_assert_and_check_goto(rtor->epfd >= 0, fail);

	// ok
	return (tb_eiop_reactor_t*)rtor;

fail:
	if (rtor) tb_eiop_reactor_epoll_exit(rtor);
	return TB_NULL;
}

