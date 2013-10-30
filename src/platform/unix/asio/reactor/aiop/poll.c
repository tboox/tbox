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
 * @file		poll.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include <sys/poll.h>

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the poll reactor type
typedef struct __tb_aiop_reactor_poll_t
{
	// the reactor base
	tb_aiop_reactor_t 		base;

	// the poll fds
	tb_vector_t* 			pfds;

}tb_aiop_reactor_poll_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aiop_reactor_poll_addo(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t aioe)
{
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds, tb_false);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, tb_false);
	
	// init pfd
	struct pollfd pfd = {0};
	pfd.fd = fd;
	if (aioe & TB_AIOE_CODE_RECV || aioe & TB_AIOE_CODE_ACPT) pfd.events |= POLLIN;
	if (aioe & TB_AIOE_CODE_SEND || aioe & TB_AIOE_CODE_CONN) pfd.events |= POLLOUT;

	// add pfd
	tb_vector_insert_tail(rtor->pfds, &pfd);

	// ok
	return tb_true;
}
static tb_bool_t tb_aiop_reactor_poll_seto(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t aioe, tb_aioo_t const* obj)
{
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds, tb_false);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, tb_false);

	// set pfd
	tb_size_t itor = tb_iterator_head(rtor->pfds);
	tb_size_t tail = tb_iterator_tail(rtor->pfds);
	for (; itor != tail; itor = tb_iterator_next(rtor->pfds, itor))
	{
		struct pollfd* pfd = (struct pollfd*)tb_iterator_item(rtor->pfds, itor);
		if (pfd && pfd->fd == fd)
		{
			pfd->events = 0;
			if (aioe & TB_AIOE_CODE_RECV || aioe & TB_AIOE_CODE_ACPT) pfd->events |= POLLIN;
			if (aioe & TB_AIOE_CODE_SEND || aioe & TB_AIOE_CODE_CONN) pfd->events |= POLLOUT;
			break;
		}
	}
	tb_assert_and_check_return_val(itor != tail, tb_false);

	// ok
	return tb_true;
}
static tb_bool_t tb_aiop_reactor_poll_delo(tb_aiop_reactor_t* reactor, tb_handle_t handle)
{
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds, tb_false);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, tb_false);

	// find pfd
	tb_size_t itor = tb_iterator_head(rtor->pfds);
	tb_size_t tail = tb_iterator_tail(rtor->pfds);
	for (; itor != tail; itor = tb_iterator_next(rtor->pfds, itor))
	{
		struct pollfd* pfd = (struct pollfd*)tb_iterator_item(rtor->pfds, itor);
		if (pfd && pfd->fd == fd) break;
	}
	tb_assert_and_check_return_val(itor != tail, tb_false);
	
	// del pfd
	tb_vector_remove(rtor->pfds, itor);

	// ok
	return tb_true;
}
static tb_long_t tb_aiop_reactor_poll_wait(tb_aiop_reactor_t* reactor, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout)
{	
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds && reactor->aiop && reactor->aiop->hash, -1);

	// pfds
	struct pollfd* 	pfds = (struct pollfd*)tb_vector_data(rtor->pfds);
	tb_size_t 		pfdm = tb_vector_size(rtor->pfds);
	tb_assert_and_check_return_val(pfds && pfdm, -1);

	// wait
	tb_long_t pfdn = poll(pfds, pfdm, timeout);
	tb_assert_and_check_return_val(pfdn >= 0, -1);

	// timeout?
	tb_check_return_val(pfdn, 0);

	// sync
	tb_size_t i = 0;
	tb_size_t n = 0;
	for (i = 0; i < pfdm && n < objm; i++)
	{
		struct pollfd* 	p = pfds + i;
		tb_aioo_t* 	o = tb_hash_get(reactor->aiop->hash, (p->fd + 1));
		tb_assert_and_check_return_val(o, -1);
		
		// add event
		tb_long_t e = 0;
		if (p->revents & POLLIN)
		{
			e |= TB_AIOE_CODE_RECV;
			if (o->aioe & TB_AIOE_CODE_ACPT) e |= TB_AIOE_CODE_ACPT;
		}
		if (p->revents & POLLOUT) 
		{
			e |= TB_AIOE_CODE_SEND;
			if (o->aioe & TB_AIOE_CODE_CONN) e |= TB_AIOE_CODE_CONN;
		}
		if ((p->revents & POLLHUP) && !(e & (TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND))) 
			e |= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND;

		// add object
		if (e) objs[n++] = *o;
	}

	// ok
	return n;
}
static tb_void_t tb_aiop_reactor_poll_exit(tb_aiop_reactor_t* reactor)
{
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	if (rtor)
	{
		// exit pfds
		if (rtor->pfds) tb_vector_exit(rtor->pfds);

		// free it
		tb_free(rtor);
	}
}
static tb_void_t tb_aiop_reactor_poll_cler(tb_aiop_reactor_t* reactor)
{
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	if (rtor)
	{
		// clear pfds
		if (rtor->pfds) tb_vector_clear(rtor->pfds);
	}
}
static tb_aiop_reactor_t* tb_aiop_reactor_poll_init(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->maxn, tb_null);

	// alloc reactor
	tb_aiop_reactor_poll_t* rtor = tb_malloc0(sizeof(tb_aiop_reactor_poll_t));
	tb_assert_and_check_return_val(rtor, tb_null);

	// init base
	rtor->base.aiop = aiop;
	rtor->base.exit = tb_aiop_reactor_poll_exit;
	rtor->base.cler = tb_aiop_reactor_poll_cler;
	rtor->base.addo = tb_aiop_reactor_poll_addo;
	rtor->base.seto = tb_aiop_reactor_poll_seto;
	rtor->base.delo = tb_aiop_reactor_poll_delo;
	rtor->base.wait = tb_aiop_reactor_poll_wait;

	// init pfds
	rtor->pfds = tb_vector_init(tb_align8((aiop->maxn >> 3) + 1), tb_item_func_ifm(sizeof(struct pollfd), tb_null, tb_null));
	tb_assert_and_check_goto(rtor->pfds, fail);

	// ok
	return (tb_aiop_reactor_t*)rtor;

fail:
	if (rtor) tb_aiop_reactor_poll_exit(rtor);
	return tb_null;
}

