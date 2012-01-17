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
 * \file		port.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include <poll.h>
#include <port.h>
#include <errno.h>
#include <sys/time.h>

/* /////////////////////////////////////////////////////////
 * types
 */

// the port reactor type
typedef struct __tb_aiop_reactor_port_t
{
	// the reactor base
	tb_aiop_reactor_t 		base;

	// the port fd
	tb_long_t 				ptfd;

	// the events
	port_event_t* 			evts;
	tb_size_t 				evtn;
	
}tb_aiop_reactor_port_t;

/* /////////////////////////////////////////////////////////
 * implemention
 */
static tb_bool_t tb_aiop_reactor_port_addo(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype)
{
	tb_aiop_reactor_port_t* rtor = (tb_aiop_reactor_port_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->ptfd >= 0, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);

	// ok
	return TB_TRUE;
}
static tb_bool_t tb_aiop_reactor_port_seto(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype, tb_aioo_t const* obj)
{
	tb_aiop_reactor_port_t* rtor = (tb_aiop_reactor_port_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->ptfd >= 0, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);

	// ok
	return TB_TRUE;
}
static tb_bool_t tb_aiop_reactor_port_delo(tb_aiop_reactor_t* reactor, tb_handle_t handle)
{
	tb_aiop_reactor_port_t* rtor = (tb_aiop_reactor_port_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->ptfd >= 0, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);

	// ok
	return TB_TRUE;
}
static tb_long_t tb_aiop_reactor_port_wait(tb_aiop_reactor_t* reactor, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout)
{	
	tb_aiop_reactor_port_t* rtor = (tb_aiop_reactor_port_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->ptfd >= 0 && reactor->aiop && reactor->aiop->hash, -1);

	// init time
	struct timespec t = {0};
	if (timeout > 0)
	{
		t.tv_sec = timeout / 1000;
		t.tv_nsec+= (timeout % 1000) * 1000000;
	}

	// init grow
	tb_size_t maxn = reactor->aiop->maxn;
	tb_size_t grow = tb_align8((maxn >> 3) + 1);

	// init events
	if (!rtor->evts)
	{
		rtor->evtn = grow;
		rtor->evts = tb_calloc(rtor->evtn, sizeof(port_event_t));
		tb_assert_and_check_return_val(rtor->evts, -1);
	}
	
	// get the event number first
	tb_uint_t getn = 0;
	if (port_getn(rtor->ptfd, rtor->evts, 0, &getn, timeout >= 0? &t : TB_NULL) < 0) return -1;

	// wait for at least one event
	if (!getn) getn = 1;

	// wait events
	tb_uint_t evtn = getn;
	if (port_getn(rtor->ptfd, rtor->evts, rtor->evtn, &evtn, timeout >= 0? &t : TB_NULL) < 0)
	{
		// error?
		tb_assert_and_check_return_val(errno != ETIME && errno != EINTR, -1);
	}
	tb_assert_and_check_return_val(evtn >= 0 && evtn <= getn && evtn <= rtor->evtn, -1);
	
	// timeout?
	tb_check_return_val(evtn, 0);

	// grow it if events is full
	if (evtn == rtor->evtn)
	{
		// grow size
		rtor->evtn += grow;
		if (rtor->evtn > maxn) rtor->evtn = maxn;

		// grow data
		rtor->evts = tb_realloc(rtor->evts, rtor->evtn * sizeof(port_event_t));
		tb_assert_and_check_return_val(rtor->evts, -1);
	}
	tb_assert(evtn <= rtor->evtn);

	// limit 
	evtn = tb_min(evtn, objm);

	// sync
	tb_size_t i = 0;
	for (i = 0; i < evtn; i++)
	{
		port_event_t* 	e = rtor->evts + i;
		tb_aioo_t* 		o = objs + i;
		tb_handle_t 	h = (tb_handle_t)e->portev_user;
		tb_aioo_t* 		p = tb_hash_get(reactor->aiop->hash, h);
		tb_assert_and_check_return_val(p, -1);

		o->handle = h;
		o->otype = reactor->aiop->type;
		o->etype = 0;
		if (e->portev_events & EPOLLIN) 
		{
			o->etype |= TB_AIOO_ETYPE_READ;
			if (p->etype & TB_AIOO_ETYPE_ACPT) o->etype |= TB_AIOO_ETYPE_ACPT;
		}
		if (e->portev_events & EPOLLOUT) 
		{
			o->etype |= TB_AIOO_ETYPE_WRIT;
			if (p->etype & TB_AIOO_ETYPE_CONN) o->etype |= TB_AIOO_ETYPE_CONN;
		}
		if (e->portev_events & (EPOLLHUP | EPOLLERR) && !(o->etype & TB_AIOO_ETYPE_READ | TB_AIOO_ETYPE_WRIT)) 
			o->etype |= TB_AIOO_ETYPE_READ | TB_AIOO_ETYPE_WRIT;
	}

	// ok
	return evtn;
}
static tb_void_t tb_aiop_reactor_port_exit(tb_aiop_reactor_t* reactor)
{
	tb_aiop_reactor_port_t* rtor = (tb_aiop_reactor_port_t*)reactor;
	if (rtor)
	{
		// free events
		if (rtor->evts) tb_free(rtor->evts);

		// close fd
		if (rtor->ptfd) close(rtor->ptfd);

		// free it
		tb_free(rtor);
	}
}
static tb_aiop_reactor_t* tb_aiop_reactor_port_init(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->maxn, TB_NULL);
	tb_assert_and_check_return_val(aiop->type == TB_AIOO_OTYPE_FILE || aiop->type == TB_AIOO_OTYPE_SOCK, TB_NULL);

	// alloc reactor
	tb_aiop_reactor_port_t* rtor = tb_calloc(1, sizeof(tb_aiop_reactor_port_t));
	tb_assert_and_check_return_val(rtor, TB_NULL);

	// init base
	rtor->base.aiop = aiop;
	rtor->base.exit = tb_aiop_reactor_port_exit;
	rtor->base.addo = tb_aiop_reactor_port_addo;
	rtor->base.seto = tb_aiop_reactor_port_seto;
	rtor->base.delo = tb_aiop_reactor_port_delo;
	rtor->base.wait = tb_aiop_reactor_port_wait;

	// init port
	rtor->ptfd = port_create();
	tb_assert_and_check_goto(rtor->ptfd >= 0, fail);

	// ok
	return (tb_aiop_reactor_t*)rtor;

fail:
	if (rtor) tb_aiop_reactor_port_exit(rtor);
	return TB_NULL;
}

