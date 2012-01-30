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
 * \file		poll.c
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
 * implemention
 */
static tb_bool_t tb_aiop_reactor_poll_addo(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype)
{
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);
	
	// init pfd
	struct pollfd pfd = {0};
	pfd.fd = fd;
	if (etype & TB_AIOO_ETYPE_READ || etype & TB_AIOO_ETYPE_ACPT) pfd.events |= POLLIN;
	if (etype & TB_AIOO_ETYPE_WRIT || etype & TB_AIOO_ETYPE_CONN) pfd.events |= POLLOUT;

	// add pfd
	tb_vector_insert_tail(rtor->pfds, &pfd);

	// ok
	return TB_TRUE;
}
static tb_bool_t tb_aiop_reactor_poll_seto(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype, tb_aioo_t const* obj)
{
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);

	// set pfd
	tb_size_t itor = tb_vector_itor_head(rtor->pfds);
	tb_size_t tail = tb_vector_itor_tail(rtor->pfds);
	for (; itor != tail; itor = tb_vector_itor_next(rtor->pfds, itor))
	{
		struct pollfd* pfd = (struct pollfd*)tb_vector_itor_at(rtor->pfds, itor);
		if (pfd && pfd->fd == fd)
		{
			pfd->events = 0;
			if (etype & TB_AIOO_ETYPE_READ || etype & TB_AIOO_ETYPE_ACPT) pfd->events |= POLLIN;
			if (etype & TB_AIOO_ETYPE_WRIT || etype & TB_AIOO_ETYPE_CONN) pfd->events |= POLLOUT;
			break;
		}
	}
	tb_assert_and_check_return_val(itor != tail, TB_FALSE);

	// ok
	return TB_TRUE;
}
static tb_bool_t tb_aiop_reactor_poll_delo(tb_aiop_reactor_t* reactor, tb_handle_t handle)
{
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);

	// find pfd
	tb_size_t itor = tb_vector_itor_head(rtor->pfds);
	tb_size_t tail = tb_vector_itor_tail(rtor->pfds);
	for (; itor != tail; itor = tb_vector_itor_next(rtor->pfds, itor))
	{
		struct pollfd* pfd = (struct pollfd*)tb_vector_itor_at(rtor->pfds, itor);
		if (pfd && pfd->fd == fd) break;
	}
	tb_assert_and_check_return_val(itor != tail, TB_FALSE);
	
	// del pfd
	tb_vector_remove(rtor->pfds, itor);

	// ok
	return TB_TRUE;
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
			e |= TB_AIOO_ETYPE_READ;
			if (o->etype & TB_AIOO_ETYPE_ACPT) e |= TB_AIOO_ETYPE_ACPT;
		}
		if (p->revents & POLLOUT) 
		{
			e |= TB_AIOO_ETYPE_WRIT;
			if (o->etype & TB_AIOO_ETYPE_CONN) e |= TB_AIOO_ETYPE_CONN;
		}
		if ((p->revents & POLLHUP) && !(e & (TB_AIOO_ETYPE_READ | TB_AIOO_ETYPE_WRIT))) 
			e |= TB_AIOO_ETYPE_READ | TB_AIOO_ETYPE_WRIT;

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
static tb_aiop_reactor_t* tb_aiop_reactor_poll_init(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->maxn, TB_NULL);
	tb_assert_and_check_return_val(aiop->type == TB_AIOO_OTYPE_FILE || aiop->type == TB_AIOO_OTYPE_SOCK, TB_NULL);

	// alloc reactor
	tb_aiop_reactor_poll_t* rtor = tb_calloc(1, sizeof(tb_aiop_reactor_poll_t));
	tb_assert_and_check_return_val(rtor, TB_NULL);

	// init base
	rtor->base.aiop = aiop;
	rtor->base.exit = tb_aiop_reactor_poll_exit;
	rtor->base.addo = tb_aiop_reactor_poll_addo;
	rtor->base.seto = tb_aiop_reactor_poll_seto;
	rtor->base.delo = tb_aiop_reactor_poll_delo;
	rtor->base.wait = tb_aiop_reactor_poll_wait;

	// init pfds
	rtor->pfds = tb_vector_init(tb_align8((aiop->maxn >> 3) + 1), tb_item_func_ifm(sizeof(struct pollfd), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(rtor->pfds, fail);

	// ok
	return (tb_aiop_reactor_t*)rtor;

fail:
	if (rtor) tb_aiop_reactor_poll_exit(rtor);
	return TB_NULL;
}

