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
#include <errno.h>
#include <sys/event.h>
#include <sys/time.h>

/* /////////////////////////////////////////////////////////
 * macros
 */

#ifndef EV_ENABLE
# 	define EV_ENABLE 	(0)
#endif

#ifndef NOTE_EOF
# 	define NOTE_EOF 	(0)
#endif

/* /////////////////////////////////////////////////////////
 * types
 */

// the kqueue reactor type
typedef struct __tb_aiop_reactor_kqueue_t
{
	// the reactor base
	tb_aiop_reactor_t 		base;

	// the kqueue fd
	tb_long_t 				kqfd;

	// the kqueue fds
	tb_vector_t* 			kevs;

	// the events
	struct kevent64_s* 		evts;
	tb_size_t 				evtn;
	
}tb_aiop_reactor_kqueue_t;

/* /////////////////////////////////////////////////////////
 * implemention
 */
static tb_bool_t tb_aiop_reactor_kqueue_addo(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype)
{
	tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);

	// add event
	struct kevent64_s e;
	if (etype & TB_AIOO_ETYPE_READ || etype & TB_AIOO_ETYPE_ACPT) 
	{
		EV_SET64(&e, fd, EVFILT_READ, EV_ADD | EV_CLEAR | EV_ENABLE, NOTE_EOF, 0, (tb_uint64_t)handle, etype, 0);
		tb_vector_insert_tail(rtor->kevs, &e);
	}
	if (etype & TB_AIOO_ETYPE_WRIT || etype & TB_AIOO_ETYPE_CONN)
	{
		EV_SET64(&e, fd, EVFILT_WRITE, EV_ADD | EV_CLEAR | EV_ENABLE, NOTE_EOF, 0, (tb_uint64_t)handle, etype, 0);
		tb_vector_insert_tail(rtor->kevs, &e);
	}

	// ok
	return TB_TRUE;
}
static tb_bool_t tb_aiop_reactor_kqueue_seto(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t etype)
{
	tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);

	// add event
	struct kevent64_s e;
	if (etype & TB_AIOO_ETYPE_READ || etype & TB_AIOO_ETYPE_ACPT) 
	{
		EV_SET64(&e, fd, EVFILT_READ, EV_ADD | EV_CLEAR | EV_ENABLE, NOTE_EOF, 0, (tb_uint64_t)handle, etype, 0);
		tb_vector_insert_tail(rtor->kevs, &e);
	}
	if (etype & TB_AIOO_ETYPE_WRIT || etype & TB_AIOO_ETYPE_CONN)
	{
		EV_SET64(&e, fd, EVFILT_WRITE, EV_ADD | EV_CLEAR | EV_ENABLE, NOTE_EOF, 0, (tb_uint64_t)handle, etype, 0);
		tb_vector_insert_tail(rtor->kevs, &e);
	}

	// ok
	return TB_TRUE;
}
static tb_bool_t tb_aiop_reactor_kqueue_delo(tb_aiop_reactor_t* reactor, tb_handle_t handle)
{
	tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0, TB_FALSE);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, TB_FALSE);

	// del event
	struct kevent64_s e;
	EV_SET64(&e, fd, EVFILT_READ, EV_DELETE, 0, 0, 0, 0, 0);
	tb_vector_insert_tail(rtor->kevs, &e);

	EV_SET64(&e, fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0, 0, 0);
	tb_vector_insert_tail(rtor->kevs, &e);

	// ok
	return TB_TRUE;
}
static tb_long_t tb_aiop_reactor_kqueue_wait(tb_aiop_reactor_t* reactor, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout)
{	
	tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0 && rtor->kevs && reactor->aiop, -1);

	// init time
	struct timespec t = {0};
	t.tv_sec = time(TB_NULL);
	if (timeout > 0)
	{
		t.tv_sec += timeout / 1000;
		t.tv_nsec += (timeout % 1000) * 1000000;
	}

	// init kevs
	struct kevent64_s* 	kevs = (struct kevent64_s*)tb_vector_data(rtor->kevs);
	tb_size_t 			kevn = tb_vector_size(rtor->kevs);
	tb_assert_and_check_return_val(kevs, -1);
	tb_check_return_val(kevn, 0);

	// init grow
	tb_size_t maxn = reactor->aiop->maxn;
	tb_size_t grow = tb_align8((maxn >> 3) + 1);

	// init events
	if (!rtor->evts)
	{
		rtor->evtn = grow;
		rtor->evts = tb_calloc(rtor->evtn, sizeof(struct kevent64_s));
		tb_assert_and_check_return_val(rtor->evts, -1);
	}
	
	// wait events
	tb_long_t evtn = kevent64(rtor->kqfd, kevs, kevn, rtor->evts, rtor->evtn, 0, timeout >= 0? &t : TB_NULL);
	tb_assert_and_check_return_val(evtn >= 0 && evtn <= rtor->evtn, -1);
	
	// timeout?
	tb_check_return_val(evtn, 0);

	// clear kevs
	tb_vector_clear(rtor->kevs);

	// grow it if events is full
	if (evtn == rtor->evtn)
	{
		// grow size
		rtor->evtn += grow;
		if (rtor->evtn > maxn) rtor->evtn = maxn;

		// grow data
		rtor->evts = tb_realloc(rtor->evts, rtor->evtn * sizeof(struct kevent64_s));
		tb_assert_and_check_return_val(rtor->evts, -1);
	}
	tb_assert(evtn <= rtor->evtn);

	// limit 
	evtn = tb_min(evtn, objm);

	// sync
	tb_size_t i = 0;
	for (i = 0; i < evtn; i++)
	{
		struct kevent64_s* 	e = rtor->evts + i;
		tb_aioo_t* 			o = objs + i;
		tb_size_t 			etype = (tb_size_t)e->ext[0];

		o->handle = (tb_handle_t)e->udata;
		o->otype = reactor->aiop->type;
		o->etype = 0;
		if (e->filter == EVFILT_READ) 
		{
			o->etype |= TB_AIOO_ETYPE_READ;
			if (etype & TB_AIOO_ETYPE_ACPT) o->etype |= TB_AIOO_ETYPE_ACPT;
		}
		if (e->filter == EVFILT_WRITE) 
		{
			o->etype |= TB_AIOO_ETYPE_WRIT;
			if (etype & TB_AIOO_ETYPE_CONN) o->etype |= TB_AIOO_ETYPE_CONN;
		}
		if (e->flags & EV_ERROR && !(o->etype & TB_AIOO_ETYPE_READ | TB_AIOO_ETYPE_WRIT)) 
			o->etype |= TB_AIOO_ETYPE_READ | TB_AIOO_ETYPE_WRIT;
	}

	// ok
	return evtn;
}
static tb_void_t tb_aiop_reactor_kqueue_exit(tb_aiop_reactor_t* reactor)
{
	tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
	if (rtor)
	{
		// free events
		if (rtor->evts) tb_free(rtor->evts);

		// exit kevs
		if (rtor->kevs) tb_vector_exit(rtor->kevs);

		// close kqfd?
		// ...

		// free it
		tb_free(rtor);
	}
}
static tb_aiop_reactor_t* tb_aiop_reactor_kqueue_init(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->maxn, TB_NULL);
	tb_assert_and_check_return_val(aiop->type == TB_AIOO_OTYPE_FILE || aiop->type == TB_AIOO_OTYPE_SOCK, TB_NULL);

	// alloc reactor
	tb_aiop_reactor_kqueue_t* rtor = tb_calloc(1, sizeof(tb_aiop_reactor_kqueue_t));
	tb_assert_and_check_return_val(rtor, TB_NULL);

	// init base
	rtor->base.aiop = aiop;
	rtor->base.exit = tb_aiop_reactor_kqueue_exit;
	rtor->base.addo = tb_aiop_reactor_kqueue_addo;
	rtor->base.seto = tb_aiop_reactor_kqueue_seto;
	rtor->base.delo = tb_aiop_reactor_kqueue_delo;
	rtor->base.wait = tb_aiop_reactor_kqueue_wait;

	// init kqueue
	rtor->kqfd = kqueue();
	tb_assert_and_check_goto(rtor->kqfd >= 0, fail);

	// init kevs
	rtor->kevs = tb_vector_init(tb_align8((aiop->maxn >> 3) + 1), tb_item_func_ifm(sizeof(struct kevent64_s), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(rtor->kevs, fail);

	// ok
	return (tb_aiop_reactor_t*)rtor;

fail:
	if (rtor) tb_aiop_reactor_kqueue_exit(rtor);
	return TB_NULL;
}

