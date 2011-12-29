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
 * \file		kqueue.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include <sys/kqueue.h>

/* /////////////////////////////////////////////////////////
 * types
 */
// the event pool type
typedef struct __tb_epool_t
{
	// the object maxn
	tb_size_t 				maxn;

	// the object size
	tb_size_t 				size;

	// the kqueue fd
	tb_long_t 				epfd;

	// the events
	struct kqueue_event* 	evts;
	tb_size_t 				evtn;
	
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

	// init kqueue
	ep->epfd = kqueue_create(maxn);
	tb_assert_and_check_goto(ep->epfd >= 0, fail);

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
		// free objects 
		if (ep->objs) tb_free(ep->objs);

		// free events
		if (ep->evts) tb_free(ep->evts);

		// close fd
		if (ep->epfd) close(ep->epfd);

		// free pool
		tb_free(ep);
	}
}
tb_size_t tb_epool_addo(tb_handle_t pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && ep->epfd >= 0 && ep->size < ep->maxn && handle, 0);
	tb_assert_and_check_return_val(otype == TB_EOTYPE_FILE || otype == TB_EOTYPE_SOCK, 0);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, 0);

	// init 
	struct kqueue_event e = {0};
	if (etype & TB_ETYPE_READ || etype & TB_ETYPE_ACPT) e.events |= EPOLLIN;
	if (etype & TB_ETYPE_WRIT || etype & TB_ETYPE_CONN) e.events |= EPOLLOUT;
	e.events |= EPOLLET;
	e.data.u64 = (((tb_uint64_t)otype << 56) | ((tb_uint64_t)etype << 32) | (tb_uint64_t)(tb_uint32_t)handle);

	// ctrl
	if (kqueue_ctl(ep->epfd, EPOLL_CTL_ADD, fd, &e) < 0) return 0;

	// ok
	return ++ep->size;
}
tb_size_t tb_epool_seto(tb_handle_t pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && ep->epfd >= 0 && ep->size && handle, 0);
	tb_assert_and_check_return_val(otype == TB_EOTYPE_FILE || otype == TB_EOTYPE_SOCK, 0);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, 0);

	// init 
	struct kqueue_event e = {0};
	if (etype & TB_ETYPE_READ || etype & TB_ETYPE_ACPT) e.events |= EPOLLIN | EPOLLET;
	if (etype & TB_ETYPE_WRIT || etype & TB_ETYPE_CONN) e.events |= EPOLLOUT | EPOLLET;
	e.data.u64 = (((tb_uint64_t)otype << 56) | ((tb_uint64_t)etype << 32) | (tb_uint64_t)(tb_uint32_t)handle);

	// ctrl
	if (kqueue_ctl(ep->epfd, EPOLL_CTL_MOD, fd, &e) < 0) return 0;

	// ok
	return ep->size;
}
tb_size_t tb_epool_delo(tb_handle_t pool, tb_handle_t handle)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && ep->epfd >= 0 && ep->size && handle, 0);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, 0);

	// ctrl
	struct kqueue_event e = {0};
	if (kqueue_ctl(ep->epfd, EPOLL_CTL_DEL, fd, &e) < 0) return 0;

	// ok
	return --ep->size;
}
tb_long_t tb_epool_wait(tb_handle_t pool, tb_eobject_t** objs, tb_long_t timeout)
{	
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && ep->epfd >= 0 && objs, -1);

	// init grow
	tb_size_t grow = tb_align8((ep->maxn >> 3) + 1);

	// init events
	if (!ep->evts)
	{
		ep->evtn = grow;
		ep->evts = tb_calloc(ep->evtn, sizeof(struct kqueue_event));
		tb_assert_and_check_return_val(ep->evts, -1);
	}
	
	// wait events
	tb_long_t evtn = kqueue_wait(ep->epfd, ep->evts, ep->evtn, timeout);
	tb_assert_and_check_return_val(evtn >= 0 && evtn <= ep->evtn, -1);
	
	// timeout?
	tb_check_return_val(evtn, 0);

	// grow it if events is full
	if (evtn == ep->evtn)
	{
		// grow size
		ep->evtn += grow;
		if (ep->evtn > ep->maxn) ep->evtn = ep->maxn;

		// grow data
		ep->evts = tb_realloc(ep->evts, ep->evtn * sizeof(struct kqueue_event));
		tb_assert_and_check_return_val(ep->evts, -1);
	}

	// init objs
	if (!ep->objs)
	{
		ep->objn = evtn + grow;
		ep->objs = tb_calloc(ep->objn, sizeof(tb_eobject_t));
		tb_assert_and_check_return_val(ep->objs, -1);
	}
	// grow objs if not enough
	else if (evtn > ep->objn)
	{
		// grow size
		ep->objn = evtn + grow;
		if (ep->objn > ep->maxn) ep->objn = ep->maxn;

		// grow data
		ep->objs = tb_realloc(ep->objs, ep->objn * sizeof(tb_eobject_t));
		tb_assert_and_check_return_val(ep->objs, -1);
	}
	tb_assert(evtn <= ep->evtn && evtn <= ep->objn);

	// update objects 
	tb_size_t i = 0;
	for (i = 0; i < evtn; i++)
	{
		struct kqueue_event* e = ep->evts + i;
		tb_eobject_t* 		o = ep->objs + i;
		tb_size_t 			etype = (tb_size_t)((e->data.u64 >> 32) & 0x00ffffff);

		o->handle = (tb_handle_t)(tb_uint32_t)e->data.u64;
		o->otype = (tb_size_t)(e->data.u64 >> 56);
		o->etype = 0;
		if (e->events & EPOLLIN) 
		{
			o->etype |= TB_ETYPE_READ;
			if (etype & TB_ETYPE_ACPT) o->etype |= TB_ETYPE_ACPT;
		}
		if (e->events & EPOLLOUT) 
		{
			o->etype |= TB_ETYPE_WRIT;
			if (etype & TB_ETYPE_CONN) o->etype |= TB_ETYPE_CONN;
		}
		if (e->events & (EPOLLHUP | EPOLLERR) && !(o->etype & TB_ETYPE_READ | TB_ETYPE_WRIT)) 
			o->etype |= TB_ETYPE_READ | TB_ETYPE_WRIT;
	}
	*objs = ep->objs;
	
	// ok
	return evtn;
}

