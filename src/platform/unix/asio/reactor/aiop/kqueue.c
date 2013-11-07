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
 * @file		kqueue.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include <errno.h>
#include <sys/event.h>
#include <sys/time.h>
#ifndef TB_CONFIG_OS_ANDROID
# 	include <unistd.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

#ifndef EV_ENABLE
# 	define EV_ENABLE 	(0)
#endif

#ifndef NOTE_EOF
# 	define NOTE_EOF 	(0)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the kqueue reactor type
typedef struct __tb_aiop_reactor_kqueue_t
{
	// the reactor base
	tb_aiop_reactor_t 		base;

	// the kqueue fd
	tb_long_t 				kqfd;

	// the events
	struct kevent* 			evts;
	tb_size_t 				evtn;
	
}tb_aiop_reactor_kqueue_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aiop_reactor_kqueue_sync(tb_aiop_reactor_t* reactor, struct kevent* evts, tb_size_t evtn)
{
	tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0, tb_false);
	tb_assert_and_check_return_val(evts && evtn, tb_false);

	// change events
	struct timespec t = {0};
	if (kevent(rtor->kqfd, evts, evtn, tb_null, 0, &t) < 0) return tb_false;

	// ok
	return tb_true;
}
static tb_bool_t tb_aiop_reactor_kqueue_addo(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t aioe)
{
	tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0, tb_false);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, tb_false);

	// add event
	struct kevent 	e[2];
	tb_size_t 		n = 0;
	if (aioe & TB_AIOE_CODE_RECV || aioe & TB_AIOE_CODE_ACPT) 
	{
		EV_SET(&e[n], fd, EVFILT_READ, EV_ADD | EV_ENABLE, NOTE_EOF, 0, handle);
		n++;
	}
	if (aioe & TB_AIOE_CODE_SEND || aioe & TB_AIOE_CODE_CONN)
	{
		EV_SET(&e[n], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, NOTE_EOF, 0, handle);
		n++;
	}

	// ok
	return tb_aiop_reactor_kqueue_sync(reactor, e, n);
}
static tb_bool_t tb_aiop_reactor_kqueue_seto(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t aioe, tb_aioo_t const* obj)
{
	tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0 && obj, tb_false);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, tb_false);

	// change
	tb_size_t adde = aioe & ~obj->aioe;
	tb_size_t dele = ~aioe & obj->aioe;

	// add event
	struct kevent 	e[2];
	tb_size_t 		n = 0;
	if (adde & TB_AIOE_CODE_RECV || adde & TB_AIOE_CODE_ACPT) 
	{
		EV_SET(&e[n], fd, EVFILT_READ, EV_ADD | EV_ENABLE, NOTE_EOF, 0, handle);
		n++;
	}
	else if (dele & TB_AIOE_CODE_RECV || dele & TB_AIOE_CODE_ACPT) 
	{
		EV_SET(&e[n], fd, EVFILT_READ, EV_DELETE, 0, 0, handle);
		n++;
	}
	if (adde & TB_AIOE_CODE_SEND || adde & TB_AIOE_CODE_CONN)
	{
		EV_SET(&e[n], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, NOTE_EOF, 0, handle);
		n++;
	}
	else if (dele & TB_AIOE_CODE_SEND || dele & TB_AIOE_CODE_CONN)
	{
		EV_SET(&e[n], fd, EVFILT_WRITE, EV_DELETE, 0, 0, handle);
		n++;
	}

	// ok
	return tb_aiop_reactor_kqueue_sync(reactor, e, n);
}
static tb_void_t tb_aiop_reactor_kqueue_delo(tb_aiop_reactor_t* reactor, tb_handle_t handle)
{
	tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0, tb_false);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, tb_false);

	// del event
	struct kevent e[2];
	EV_SET(&e[0], fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
	EV_SET(&e[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);

	// ok
	return tb_aiop_reactor_kqueue_sync(reactor, e, 2);
}
static tb_long_t tb_aiop_reactor_kqueue_wait(tb_aiop_reactor_t* reactor, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout)
{	
	tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->kqfd >= 0 && reactor->aiop && reactor->aiop->hash, -1);

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
		rtor->evts = tb_nalloc0(rtor->evtn, sizeof(struct kevent));
		tb_assert_and_check_return_val(rtor->evts, -1);
	}

	// wait events
	tb_long_t evtn = kevent(rtor->kqfd, tb_null, 0, rtor->evts, rtor->evtn, timeout >= 0? &t : tb_null);
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
		rtor->evts = tb_ralloc(rtor->evts, rtor->evtn * sizeof(struct kevent));
		tb_assert_and_check_return_val(rtor->evts, -1);
	}
	tb_assert(evtn <= rtor->evtn);

	// limit 
	evtn = tb_min(evtn, objm);

	// sync
	tb_size_t i = 0;
	for (i = 0; i < evtn; i++)
	{
		struct kevent* 	e = rtor->evts + i;
		tb_aioo_t* 		o = objs + i;
		tb_aioo_t* 		p = tb_hash_get(reactor->aiop->hash, e->udata);
		tb_assert_and_check_return_val(p, -1);

		o->handle = (tb_handle_t)e->udata;
		o->aioe = 0;
		o->data = p->data;
		if (e->filter == EVFILT_READ) 
		{
			o->aioe |= TB_AIOE_CODE_RECV;
			if (p->aioe & TB_AIOE_CODE_ACPT) o->aioe |= TB_AIOE_CODE_ACPT;
		}
		if (e->filter == EVFILT_WRITE) 
		{
			o->aioe |= TB_AIOE_CODE_SEND;
			if (p->aioe & TB_AIOE_CODE_CONN) o->aioe |= TB_AIOE_CODE_CONN;
		}
		if (e->flags & EV_ERROR && !(o->aioe & TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND)) 
			o->aioe |= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND;
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

		// close kqfd
		if (rtor->kqfd >= 0) close(rtor->kqfd);

		// free it
		tb_free(rtor);
	}
}
static tb_void_t tb_aiop_reactor_kqueue_cler(tb_aiop_reactor_t* reactor)
{
	tb_aiop_reactor_kqueue_t* rtor = (tb_aiop_reactor_kqueue_t*)reactor;
	if (rtor)
	{
		// close kqfd
		if (rtor->kqfd >= 0)
		{
			// FIXME 
			close(rtor->kqfd);
			rtor->kqfd = kqueue();
		}
	}
}
static tb_aiop_reactor_t* tb_aiop_reactor_kqueue_init(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->maxn, tb_null);

	// alloc reactor
	tb_aiop_reactor_kqueue_t* rtor = tb_malloc0(sizeof(tb_aiop_reactor_kqueue_t));
	tb_assert_and_check_return_val(rtor, tb_null);

	// init base
	rtor->base.aiop = aiop;
	rtor->base.exit = tb_aiop_reactor_kqueue_exit;
	rtor->base.cler = tb_aiop_reactor_kqueue_cler;
	rtor->base.addo = tb_aiop_reactor_kqueue_addo;
	rtor->base.seto = tb_aiop_reactor_kqueue_seto;
	rtor->base.delo = tb_aiop_reactor_kqueue_delo;
	rtor->base.wait = tb_aiop_reactor_kqueue_wait;

	// init kqueue
	rtor->kqfd = kqueue();
	tb_assert_and_check_goto(rtor->kqfd >= 0, fail);

	// ok
	return (tb_aiop_reactor_t*)rtor;

fail:
	if (rtor) tb_aiop_reactor_kqueue_exit(rtor);
	return tb_null;
}

