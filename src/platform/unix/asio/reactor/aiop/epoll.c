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

// the epoll aioo type
typedef struct __tb_epoll_aioo_t
{
	// the code
	tb_size_t 				code;

	// the data
	tb_pointer_t 			data;

}tb_epoll_aioo_t;

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

	// the hash
	tb_hash_t* 				hash;

	// the mutx
	tb_handle_t 			mutx;

	// the kill
	tb_handle_t 			kill[2];
	
}tb_aiop_reactor_epoll_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aiop_reactor_epoll_addo(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t code, tb_pointer_t data)
{
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd > 0 && handle, tb_false);

	// addo
	struct epoll_event e = {0};
	if (code & TB_AIOE_CODE_RECV || code & TB_AIOE_CODE_ACPT) e.events |= EPOLLIN;
	if (code & TB_AIOE_CODE_SEND || code & TB_AIOE_CODE_CONN) e.events |= EPOLLOUT;
	if (code & TB_AIOE_CODE_ONESHOT) e.events |= EPOLLONESHOT;
	e.data.u64 = (tb_hize_t)handle;
	if (epoll_ctl(rtor->epfd, EPOLL_CTL_ADD, ((tb_long_t)handle) - 1, &e) < 0) return tb_false;

	// add handle => aioo
	tb_bool_t ok = tb_false;
	if (rtor->mutx) tb_mutex_enter(rtor->mutx);
	if (rtor->hash) 
	{
		tb_epoll_aioo_t aioo;
		aioo.code = code;
		aioo.data = data;
		tb_hash_set(rtor->hash, handle, &aioo);
		ok = tb_true;
	}
	if (rtor->mutx) tb_mutex_leave(rtor->mutx);

	// ok?
	return ok;
}
static tb_void_t tb_aiop_reactor_epoll_delo(tb_aiop_reactor_t* reactor, tb_handle_t handle)
{
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return(rtor && rtor->epfd > 0 && handle);

	// delo
	struct epoll_event e = {0};
	if (epoll_ctl(rtor->epfd, EPOLL_CTL_DEL, ((tb_long_t)handle) - 1, &e) < 0) return ;

	// del handle => aioo
	if (rtor->mutx) tb_mutex_enter(rtor->mutx);
	if (rtor->hash) tb_hash_del(rtor->hash, handle);
	if (rtor->mutx) tb_mutex_leave(rtor->mutx);
}
static tb_bool_t tb_aiop_reactor_epoll_sete(tb_aiop_reactor_t* reactor, tb_aioe_t const* aioe)
{
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd > 0 && aioe && aioe->handle, tb_false);

	// init 
	struct epoll_event e = {0};
	if (aioe->code & TB_AIOE_CODE_RECV || aioe->code & TB_AIOE_CODE_ACPT) e.events |= EPOLLIN;
	if (aioe->code & TB_AIOE_CODE_SEND || aioe->code & TB_AIOE_CODE_CONN) e.events |= EPOLLOUT;
	if (aioe->code & TB_AIOE_CODE_ONESHOT) e.events |= EPOLLONESHOT;
	e.data.u64 = (tb_hize_t)aioe->handle;

	// sete
	if (epoll_ctl(rtor->epfd, EPOLL_CTL_MOD, ((tb_long_t)aioe->handle) - 1, &e) < 0) return tb_false;

	// set handle => aioo
	tb_bool_t ok = tb_false;
	if (rtor->mutx) tb_mutex_enter(rtor->mutx);
	if (rtor->hash) 
	{
		tb_epoll_aioo_t* aioo = (tb_epoll_aioo_t*)tb_hash_get(rtor->hash, aioe->handle);
		if (aioo)
		{
			aioo->code = aioe->code;
			aioo->data = aioe->data;
			ok = tb_true;
		}
	}
	if (rtor->mutx) tb_mutex_leave(rtor->mutx);

	// ok?
	return ok;
}
static tb_bool_t tb_aiop_reactor_epoll_post(tb_aiop_reactor_t* reactor, tb_aioe_t const* list, tb_size_t size)
{
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd > 0 && list && size, tb_false);

	// walk list
	tb_size_t i = 0;
	tb_size_t post = 0;
	for (i = 0; i < size; i++)
	{
		// the aioe
		tb_aioe_t const* aioe = &list[i];
		if (aioe)
		{
			if (tb_aiop_reactor_epoll_sete(reactor, aioe)) post++;
		}
	}

	// ok?
	return post? tb_true : tb_false;
}
static tb_long_t tb_aiop_reactor_epoll_wait(tb_aiop_reactor_t* reactor, tb_aioe_t* list, tb_size_t maxn, tb_long_t timeout)
{	
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->epfd > 0 && reactor->aiop, -1);

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
	for (i = 0; i < evtn; i++)
	{
		// the handle
		tb_handle_t handle = (tb_handle_t)rtor->evts[i].data.u64;
		tb_assert_and_check_return_val(handle, -1);

		// the events
		tb_size_t events = rtor->evts[i].events;

		// killed?
		if (handle == rtor->kill[1] && (events & EPOLLIN)) return -1;

		// the aioo
		tb_epoll_aioo_t aioo = {0};
		if (rtor->mutx) tb_mutex_enter(rtor->mutx);
		if (rtor->hash) 
		{
			tb_epoll_aioo_t const* item = (tb_epoll_aioo_t const*)tb_hash_get(rtor->hash, handle);
			if (item) aioo = *item;
		}
		if (rtor->mutx) tb_mutex_leave(rtor->mutx);
		tb_assert_and_check_return_val(aioo.code, -1);

		// save aioe
		tb_aioe_t* aioe = list + i;
		aioe->code = TB_AIOE_CODE_NONE;
		aioe->data = aioo.data;
		aioe->handle = handle;
		if (events & EPOLLIN) 
		{
			aioe->code |= TB_AIOE_CODE_RECV;
			if (aioo.code & TB_AIOE_CODE_ACPT) aioe->code |= TB_AIOE_CODE_ACPT;
		}
		if (events & EPOLLOUT) 
		{
			aioe->code |= TB_AIOE_CODE_SEND;
			if (aioo.code & TB_AIOE_CODE_CONN) aioe->code |= TB_AIOE_CODE_CONN;
		}
		if (events & (EPOLLHUP | EPOLLERR) && !(aioe->code & TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND)) 
			aioe->code |= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND;
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

		// exit kill
		if (rtor->kill[0]) tb_socket_close(rtor->kill[0]);
		if (rtor->kill[1]) tb_socket_close(rtor->kill[1]);
		rtor->kill[0] = tb_null;
		rtor->kill[1] = tb_null;

		// exit fd
		if (rtor->epfd) close(rtor->epfd);
		rtor->epfd = 0;

		// exit hash
		if (rtor->mutx) tb_mutex_enter(rtor->mutx);
		if (rtor->hash) tb_hash_exit(rtor->hash);
		rtor->hash = tb_null;
		if (rtor->mutx) tb_mutex_leave(rtor->mutx);

		// exit mutx
		if (rtor->mutx) tb_mutex_exit(rtor->mutx);
		rtor->mutx = tb_null;

		// exit it
		tb_free(rtor);
	}
}
static tb_void_t tb_aiop_reactor_epoll_kill(tb_aiop_reactor_t* reactor)
{
	// check
	tb_aiop_reactor_epoll_t* rtor = (tb_aiop_reactor_epoll_t*)reactor;
	tb_assert_and_check_return(rtor);

	// kill it
	if (rtor->kill[0]) tb_socket_send(rtor->kill[0], "k", 1);
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

		// clear hash
		if (rtor->mutx) tb_mutex_enter(rtor->mutx);
		if (rtor->hash) tb_hash_clear(rtor->hash);
		if (rtor->mutx) tb_mutex_leave(rtor->mutx);
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
	rtor->base.kill = tb_aiop_reactor_epoll_kill;
	rtor->base.addo = tb_aiop_reactor_epoll_addo;
	rtor->base.delo = tb_aiop_reactor_epoll_delo;
	rtor->base.post = tb_aiop_reactor_epoll_post;
	rtor->base.wait = tb_aiop_reactor_epoll_wait;

	// init epoll
	rtor->epfd = epoll_create(aiop->maxn);
	tb_assert_and_check_goto(rtor->epfd > 0, fail);

	// init mutx
	rtor->mutx = tb_mutex_init(tb_null);
	tb_assert_and_check_goto(rtor->mutx, fail);

	// init hash
	rtor->hash = tb_hash_init(tb_align8(tb_isqrti(aiop->maxn) + 1), tb_item_func_ptr(tb_null, tb_null), tb_item_func_ifm(sizeof(tb_epoll_aioo_t), tb_null, tb_null));
	tb_assert_and_check_goto(rtor->hash, fail);

	// init kill
	if (!tb_socket_pair(TB_SOCKET_TYPE_TCP, rtor->kill)) goto fail;

	// addo kill
	if (!tb_aiop_reactor_epoll_addo(rtor, rtor->kill[1], TB_AIOE_CODE_RECV | TB_AIOE_CODE_ONESHOT, tb_null)) goto fail;	

	// ok
	return (tb_aiop_reactor_t*)rtor;

fail:
	if (rtor) tb_aiop_reactor_epoll_exit(rtor);
	return tb_null;
}

