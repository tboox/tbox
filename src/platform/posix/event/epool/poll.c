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
// the event pool type
typedef struct __tb_epool_t
{
	// the object maxn
	tb_size_t 				maxn;

	// the objects hash
	tb_hash_t* 				hash;

	// the poll fds
	tb_vector_t* 			pfds;
	
	// the objects
	tb_eobject_t* 			objs;
	tb_size_t 				objn;

}tb_epool_t;

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_handle_t tb_epool_init(tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, TB_NULL);

	// init pool
	tb_epool_t* ep = tb_calloc(1, sizeof(tb_epool_t));
	tb_assert_and_check_return_val(ep, TB_NULL);

	// init maxn
	ep->maxn = maxn;

	// init hash
	ep->hash = tb_hash_init(tb_align8((maxn >> 3) + 1), tb_item_func_int(), tb_item_func_ifm(sizeof(tb_eobject_t), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(ep->hash, fail);

	// init pfds
	ep->pfds = tb_vector_init(tb_align8((maxn >> 3) + 1), tb_item_func_ifm(sizeof(struct pollfd), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(ep->pfds, fail);

	// ok
	return (tb_handle_t)ep;

fail:
	if (ep) tb_epool_exit(ep);
	return TB_NULL;
}

tb_void_t tb_epool_exit(tb_handle_t pool)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	if (ep)
	{
		// free objs 
		if (ep->objs) tb_free(ep->objs);

		// exit pfds
		if (ep->pfds) tb_vector_exit(ep->pfds);

		// exit hash
		if (ep->hash) tb_hash_exit(ep->hash);

		// free pool
		tb_free(ep);
	}
}
tb_size_t tb_epool_addo(tb_handle_t pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && ep->hash && ep->pfds && handle, 0);
	tb_assert_and_check_return_val(otype == TB_EOTYPE_FILE || otype == TB_EOTYPE_SOCK, 0);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, 0);
	
	// init pfd
	struct pollfd pfd = {0};
	pfd.fd = fd;
	if (etype & TB_ETYPE_READ || etype & TB_ETYPE_ACPT) pfd.events |= POLLIN;
	if (etype & TB_ETYPE_WRIT || etype & TB_ETYPE_CONN) pfd.events |= POLLOUT;

	// init obj
	tb_eobject_t o;
	tb_eobject_seto(&o, handle, otype, etype);

	// add pfd
	tb_vector_insert_tail(ep->pfds, &pfd);

	// add obj
	tb_hash_set(ep->hash, fd, &o);
	
	// ok
	return tb_hash_size(ep->hash);
}
tb_size_t tb_epool_seto(tb_handle_t pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && ep->hash && ep->pfds && handle, 0);
	tb_assert_and_check_return_val(otype == TB_EOTYPE_FILE || otype == TB_EOTYPE_SOCK, 0);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, 0);

	// set pfd
	tb_size_t itor = tb_vector_itor_head(ep->pfds);
	tb_size_t tail = tb_vector_itor_tail(ep->pfds);
	for (; itor != tail; itor = tb_vector_itor_next(ep->pfds, itor))
	{
		struct pollfd* pfd = (struct pollfd*)tb_vector_itor_at(ep->pfds, itor);
		if (pfd && pfd->fd == fd)
		{
			pfd->events = 0;
			if (etype & TB_ETYPE_READ || etype & TB_ETYPE_ACPT) pfd->events |= POLLIN;
			if (etype & TB_ETYPE_WRIT || etype & TB_ETYPE_CONN) pfd->events |= POLLOUT;
			break;
		}
	}
	tb_assert_and_check_return_val(itor != tail, 0);

	// set obj
	tb_eobject_t* o = tb_hash_get(ep->hash, fd);
	if (o) tb_eobject_seto(o, handle, otype, etype);
	
	// ok
	return tb_hash_size(ep->hash);
}
tb_size_t tb_epool_delo(tb_handle_t pool, tb_handle_t handle)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && ep->hash && ep->pfds && handle, 0);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, 0);

	// find pfd
	tb_size_t itor = tb_vector_itor_head(ep->pfds);
	tb_size_t tail = tb_vector_itor_tail(ep->pfds);
	for (; itor != tail; itor = tb_vector_itor_next(ep->pfds, itor))
	{
		struct pollfd* pfd = (struct pollfd*)tb_vector_itor_at(ep->pfds, itor);
		if (pfd && pfd->fd == fd) break;
	}
	tb_assert_and_check_return_val(itor != tail, 0);
	
	// del pfd
	tb_vector_remove(ep->pfds, itor);

	// del obj
	tb_hash_del(ep->hash, fd);
	
	// ok
	return tb_hash_size(ep->hash);
}
tb_long_t tb_epool_wait(tb_handle_t pool, tb_eobject_t** objs, tb_long_t timeout)
{	
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && ep->hash && ep->pfds && objs, -1);

	// pfds
	struct pollfd* 	pfds = (struct pollfd*)tb_vector_at_head(ep->pfds);
	tb_size_t 		pfdm = tb_vector_size(ep->pfds);
	tb_assert_and_check_return_val(pfds && pfdm, -1);

	// wait
	tb_long_t pfdn = poll(pfds, pfdm, timeout);
	tb_assert_and_check_return_val(pfdn >= 0, -1);

	// timeout?
	tb_check_return_val(pfdn, 0);

	// init grow
	tb_size_t grow = tb_align8((ep->maxn >> 3) + 1);

	// init objs
	if (!ep->objs)
	{
		ep->objn = pfdn + grow;
		ep->objs = tb_calloc(ep->objn, sizeof(tb_eobject_t));
		tb_assert_and_check_return_val(ep->objs, -1);
	}
	// grow objs if not enough
	else if (pfdn > ep->objn)
	{
		// grow size
		ep->objn = pfdn + grow;
		if (ep->objn > ep->maxn) ep->objn = ep->maxn;

		// grow data
		ep->objs = tb_realloc(ep->objs, ep->objn * sizeof(struct pollfd));
		tb_assert_and_check_return_val(ep->objs, -1);
	}
	tb_assert(pfdn <= ep->objn);

	// update objects 
	tb_size_t i = 0;
	tb_size_t j = 0;
	for (i = 0; i < pfdm; i++)
	{
		struct pollfd* 	p = pfds + i;
		tb_eobject_t* 	o = tb_hash_get(ep->hash, p->fd);
		tb_assert_and_check_return_val(o, -1);
		
		// add event
		tb_long_t e = 0;
		if (p->revents & POLLIN)
		{
			e |= TB_ETYPE_READ;
			if (o->etype & TB_ETYPE_ACPT) e |= TB_ETYPE_ACPT;
		}
		if (p->revents & POLLOUT) 
		{
			e |= TB_ETYPE_WRIT;
			if (o->etype & TB_ETYPE_CONN) e |= TB_ETYPE_CONN;
		}
		if ((p->revents & POLLHUP) && !(e & (TB_ETYPE_READ | TB_ETYPE_WRIT))) 
			e |= TB_ETYPE_READ | TB_ETYPE_WRIT;

		// add object
		if (e && j < ep->objn) ep->objs[j++] = *o;
	}
	*objs = ep->objs;
	
	// ok
	return pfdn;
}

