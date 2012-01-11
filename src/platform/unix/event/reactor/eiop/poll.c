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
/* /////////////////////////////////////////////////////////
 * includes
 */
#include <sys/poll.h>

/* /////////////////////////////////////////////////////////
 * types
 */

// the poll reactor type
typedef struct __tb_eiop_reactor_poll_t
{
	// the reactor base
	tb_eiop_reactor_t 		base;

	// the objects hash
	tb_hash_t* 				hash;

	// the poll fds
	tb_vector_t* 			pfds;

}tb_eiop_reactor_poll_t;

/* /////////////////////////////////////////////////////////
 * implemention
 */
static tb_bool_t tb_eiop_reactor_poll_addo(tb_eiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype)
{
	tb_eiop_reactor_poll_t* rtor = (tb_eiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds && rtor->hash && reactor->eiop, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);
	
	// init pfd
	struct pollfd pfd = {0};
	pfd.fd = fd;
	if (etype & TB_EIO_ETYPE_READ || etype & TB_EIO_ETYPE_ACPT) pfd.events |= POLLIN;
	if (etype & TB_EIO_ETYPE_WRIT || etype & TB_EIO_ETYPE_CONN) pfd.events |= POLLOUT;

	// init obj
	tb_eio_t o;
	tb_eio_seto(&o, handle, reactor->eiop->type, etype);

	// add pfd
	tb_vector_insert_tail(rtor->pfds, &pfd);

	// add obj
	tb_hash_set(rtor->hash, fd, &o);
	
	// ok
	return TB_TRUE;
}
static tb_bool_t tb_eiop_reactor_poll_seto(tb_eiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype)
{
	tb_eiop_reactor_poll_t* rtor = (tb_eiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds && rtor->hash && reactor->eiop, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);

	// get obj
	tb_eio_t* o = tb_hash_get(rtor->hash, fd);
	tb_assert_and_check_return_val(o, TB_FALSE);

	// set pfd
	tb_size_t itor = tb_vector_itor_head(rtor->pfds);
	tb_size_t tail = tb_vector_itor_tail(rtor->pfds);
	for (; itor != tail; itor = tb_vector_itor_next(rtor->pfds, itor))
	{
		struct pollfd* pfd = (struct pollfd*)tb_vector_itor_at(rtor->pfds, itor);
		if (pfd && pfd->fd == fd)
		{
			pfd->events = 0;
			if (etype & TB_EIO_ETYPE_READ || etype & TB_EIO_ETYPE_ACPT) pfd->events |= POLLIN;
			if (etype & TB_EIO_ETYPE_WRIT || etype & TB_EIO_ETYPE_CONN) pfd->events |= POLLOUT;
			break;
		}
	}
	tb_assert_and_check_return_val(itor != tail, TB_FALSE);

	// set obj
	tb_eio_seto(o, handle, reactor->eiop->type, etype);

	// ok
	return TB_TRUE;
}
static tb_bool_t tb_eiop_reactor_poll_delo(tb_eiop_reactor_t* reactor, tb_handle_t handle)
{
	tb_eiop_reactor_poll_t* rtor = (tb_eiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds && rtor->hash, TB_FALSE);

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

	// del obj
	tb_hash_del(rtor->hash, fd);
	
	// ok
	return TB_TRUE;
}
static tb_long_t tb_eiop_reactor_poll_wait(tb_eiop_reactor_t* reactor, tb_long_t timeout)
{	
	tb_eiop_reactor_poll_t* rtor = (tb_eiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->hash && rtor->pfds, -1);

	// pfds
	struct pollfd* 	pfds = (struct pollfd*)tb_vector_data(rtor->pfds);
	tb_size_t 		pfdm = tb_vector_size(rtor->pfds);
	tb_assert_and_check_return_val(pfds && pfdm, -1);

	// wait
	tb_long_t pfdn = poll(pfds, pfdm, timeout);
	tb_assert_and_check_return_val(pfdn >= 0, -1);

	// timeout?
	tb_check_return_val(pfdn, 0);

	// ok
	return pfdn;
}
static tb_void_t tb_eiop_reactor_poll_sync(tb_eiop_reactor_t* reactor, tb_size_t evtn)
{	
	tb_eiop_reactor_poll_t* rtor = (tb_eiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return(rtor && rtor->hash && rtor->pfds && reactor->eiop && reactor->eiop->objs);

	// pfds
	struct pollfd* 	pfds = (struct pollfd*)tb_vector_at_head(rtor->pfds);
	tb_size_t 		pfdm = tb_vector_size(rtor->pfds);
	tb_assert_and_check_return(pfds && pfdm);

	// sync
	tb_size_t i = 0;
	tb_size_t j = 0;
	for (i = 0; i < pfdm; i++)
	{
		struct pollfd* 	p = pfds + i;
		tb_eio_t* 	o = tb_hash_get(rtor->hash, p->fd);
		tb_assert_and_check_return_val(o, -1);
		
		// add event
		tb_long_t e = 0;
		if (p->revents & POLLIN)
		{
			e |= TB_EIO_ETYPE_READ;
			if (o->etype & TB_EIO_ETYPE_ACPT) e |= TB_EIO_ETYPE_ACPT;
		}
		if (p->revents & POLLOUT) 
		{
			e |= TB_EIO_ETYPE_WRIT;
			if (o->etype & TB_EIO_ETYPE_CONN) e |= TB_EIO_ETYPE_CONN;
		}
		if ((p->revents & POLLHUP) && !(e & (TB_EIO_ETYPE_READ | TB_EIO_ETYPE_WRIT))) 
			e |= TB_EIO_ETYPE_READ | TB_EIO_ETYPE_WRIT;

		// add object
		if (e && j < reactor->eiop->objn) reactor->eiop->objs[j++] = *o;
	}
}
static tb_void_t tb_eiop_reactor_poll_exit(tb_eiop_reactor_t* reactor)
{
	tb_eiop_reactor_poll_t* rtor = (tb_eiop_reactor_poll_t*)reactor;
	if (rtor)
	{
		// exit pfds
		if (rtor->pfds) tb_vector_exit(rtor->pfds);

		// exit hash
		if (rtor->hash) tb_hash_exit(rtor->hash);

		// free it
		tb_free(rtor);
	}
}
static tb_eiop_reactor_t* tb_eiop_reactor_poll_init(tb_eiop_t* eiop)
{
	// check
	tb_assert_and_check_return_val(eiop && eiop->maxn, TB_NULL);
	tb_assert_and_check_return_val(eiop->type == TB_EIO_OTYPE_FILE || eiop->type == TB_EIO_OTYPE_SOCK, TB_NULL);

	// alloc reactor
	tb_eiop_reactor_poll_t* rtor = tb_calloc(1, sizeof(tb_eiop_reactor_poll_t));
	tb_assert_and_check_return_val(rtor, TB_NULL);

	// init base
	rtor->base.eiop = eiop;
	rtor->base.exit = tb_eiop_reactor_poll_exit;
	rtor->base.addo = tb_eiop_reactor_poll_addo;
	rtor->base.seto = tb_eiop_reactor_poll_seto;
	rtor->base.delo = tb_eiop_reactor_poll_delo;
	rtor->base.wait = tb_eiop_reactor_poll_wait;
	rtor->base.sync = tb_eiop_reactor_poll_sync;

	// init hash
	rtor->hash = tb_hash_init(tb_align8(tb_int32_sqrt(eiop->maxn) + 1), tb_item_func_int(), tb_item_func_ifm(sizeof(tb_eio_t), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(rtor->hash, fail);

	// init pfds
	rtor->pfds = tb_vector_init(tb_align8((eiop->maxn >> 3) + 1), tb_item_func_ifm(sizeof(struct pollfd), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(rtor->pfds, fail);

	// ok
	return (tb_eiop_reactor_t*)rtor;

fail:
	if (rtor) tb_eiop_reactor_poll_exit(rtor);
	return TB_NULL;
}

