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
 * \file		epool.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../epool.h"
#include "../event.h"
#include "../../container/container.h"

/* /////////////////////////////////////////////////////////
 * types
 */

// the event type
typedef struct __tb_event_t
{
	// post
	tb_size_t 		post; 

	// data
	tb_pointer_t 	data;

}tb_event_t;

// the epool type
typedef struct __tb_epool_t
{
	// the maxn
	tb_size_t 		maxn;

	// the spank event
	tb_handle_t 	spak;

	// the event list
	tb_dlist_t* 	list;

	// the mutex
	tb_handle_t 	mutx;

}tb_epool_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_handle_t tb_epool_init(tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, TB_NULL);

	// alloc
	tb_epool_t* ep = tb_calloc(1, sizeof(tb_epool_t));
	tb_assert_and_check_return_val(ep, TB_NULL);

	// init
	ep->maxn = maxn;

	// init mutx
	ep->mutx = tb_mutex_init(TB_NULL);
	tb_assert_and_check_goto(ep->mutx, fail);

	// init spank
	ep->spak = tb_event_init(TB_NULL);
	tb_assert_and_check_goto(ep->spak, fail);

	// init events
	ep->list = tb_dlist_init(tb_align8((ep->maxn >> 3) + 1), tb_item_func_ifm(sizeof(tb_event_t), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(ep->list, fail);
	
	// ok
	return (tb_handle_t)ep;

fail:
	if (ep) tb_epool_exit(ep);
	return TB_NULL;
}
tb_void_t tb_epool_exit(tb_handle_t epool)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	if (ep)
	{
		// kill 
		tb_epool_kill(ep);

		// enter
		if (!ep->mutx || tb_mutex_enter(ep->mutx))
		{
			// exit spank
			if (ep->spak) tb_event_exit(ep->spak);

			// exit list
			if (ep->list) tb_dlist_exit(ep->list);

			// leave
			if (ep->mutx) tb_mutex_leave(ep->mutx);
		}

		// exit mutx
		if (ep->mutx) tb_mutex_exit(ep->mutx);

		// free it
		tb_free(ep);
	}
}
tb_size_t tb_epool_maxn(tb_handle_t epool)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep && ep->mutx, 0);

	// enter
	tb_size_t maxn = 0;
	if (tb_mutex_enter(ep->mutx))
	{
		// maxn
		maxn = ep->maxn;

		// leave
		tb_mutex_leave(ep->mutx);
	}

	return maxn;
}
tb_size_t tb_epool_size(tb_handle_t epool)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep && ep->mutx, 0);

	// enter
	tb_size_t size = 0;
	if (tb_mutex_enter(ep->mutx))
	{
		// check
		tb_assert_and_check_goto(ep->list, end);

		// size
		size = tb_dlist_size(ep->list);
	}

end:

	// leave
	tb_mutex_leave(ep->mutx);

	return size;
}
tb_handle_t tb_epool_adde(tb_handle_t epool, tb_pointer_t edata)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep && ep->mutx, TB_NULL);

	// enter
	tb_size_t itor = 0;
	if (tb_mutex_enter(ep->mutx))
	{
		// check
		tb_assert_and_check_goto(ep->list && tb_dlist_size(ep->list) < ep->maxn, end);

		// init event
		tb_event_t e;
		e.post = 0;
		e.data = edata;

		// add event
		itor = tb_dlist_insert_tail(ep->list, &e);
	}

end:

	// leave
	tb_mutex_leave(ep->mutx);

	return (tb_handle_t)itor;
}
tb_void_t tb_epool_dele(tb_handle_t epool, tb_handle_t event)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return(ep && ep->mutx && event);

	// post first
	tb_epool_post(epool, event);

	// enter
	if (tb_mutex_enter(ep->mutx))
	{	
		// check
		tb_assert_and_check_goto(ep->list && tb_dlist_size(ep->list), end);

		// remove it
		tb_dlist_remove(ep->list, (tb_size_t)event);
	}
end:
	// leave
	tb_mutex_leave(ep->mutx);
}
tb_void_t tb_epool_post(tb_handle_t epool, tb_handle_t event)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return(ep && ep->mutx && event);

	// enter
	if (tb_mutex_enter(ep->mutx))
	{	
		// check
		tb_assert_and_check_goto(ep->spak && ep->list && tb_dlist_size(ep->list), end);

		// get event
		tb_event_t* e = (tb_event_t*)tb_dlist_itor_at(ep->list, event);
		tb_assert_and_check_return(e);

		// post event 
		e->post = 1;

		// post spank
		tb_event_post(ep->spak);

	}
end:
	// leave
	tb_mutex_leave(ep->mutx);
}
tb_void_t tb_epool_kill(tb_handle_t epool)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return(ep && ep->mutx);

	// enter
	if (tb_mutex_enter(ep->mutx))
	{
		// check
		tb_assert_and_check_goto(ep->spak && ep->list, end);

		// post events
		tb_size_t itor = tb_dlist_itor_head(ep->list);
		tb_size_t tail = tb_dlist_itor_tail(ep->list);
		for (; itor != tail; itor = tb_dlist_itor_next(ep->list, itor))
		{
			// get event
			tb_event_t* e = (tb_event_t*)tb_dlist_itor_at(ep->list, itor);
			tb_assert_and_check_continue(e);

			// post event
			e->post = 1;
		}

		// post spank
		tb_event_post(ep->spak);
	}
end:
	// leave
	tb_mutex_leave(ep->mutx);
}
tb_long_t tb_epool_wait(tb_handle_t epool, tb_eobject_t* objs, tb_size_t maxn, tb_long_t timeout)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep && ep->mutx && objs && maxn, -1);

	// get spak
	tb_long_t 	r = -1;
	tb_handle_t spak = TB_NULL;
	if (tb_mutex_enter(ep->mutx))
	{
		// check
		tb_assert_and_check_goto(ep->spak && ep->list, end);

		// spak
		spak = ep->spak;

		// leave
		tb_mutex_leave(ep->mutx);
	}

	// wait 
	r = tb_event_wait(spak, timeout);

	// error?
	tb_assert_and_check_return_val(r >= 0, -1);

	// timeout?
	tb_check_return_val(r, 0);

	// enter
	r = -1;
	if (!tb_mutex_enter(ep->mutx)) return -1;

	// wait events
	tb_size_t n = 0;
	tb_size_t itor = tb_dlist_itor_head(ep->list);
	tb_size_t tail = tb_dlist_itor_tail(ep->list);
	for (; itor != tail && n < maxn; itor = tb_dlist_itor_next(ep->list, itor))
	{
		// get event
		tb_event_t* e = (tb_event_t*)tb_dlist_itor_at(ep->list, itor);
		tb_assert_and_check_continue(e);

		// wait post
		tb_size_t post = e->post;
		tb_assert_and_check_goto(!post || post == 1, end);

		// timeout? next 
		tb_check_continue(post);

		// reset post
		e->post = 0;
		
		// has signal? add it to the returned objects
		objs[n].evet = (tb_handle_t)itor;
		objs[n].data = e->data;
		n++;
	}

	// ok
	r = n;

end:
	// leave
	if (!tb_mutex_leave(ep->mutx)) return -1;
	return r;
}

