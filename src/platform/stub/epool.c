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

// the epool type
typedef struct __tb_epool_t
{
	// the maxn
	tb_size_t 		maxn;

	// the spank event
	tb_handle_t 	spak;

	// the event list
	tb_dlist_t* 	list;

	// the objects
	tb_vector_t* 	objs;

}tb_epool_t;

/* /////////////////////////////////////////////////////////
 * free
 */
static tb_void_t tb_epool_event_free(tb_item_func_t* func, tb_pointer_t item)
{
	tb_assert_and_check_return(func && item);
	tb_eobject_t* o = (tb_eobject_t*)item;

	// exit event
	if (o->evet) tb_event_exit(o->evet);

	// clean
	tb_memset(o, 0, sizeof(tb_eobject_t));
}

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

	// init spank
	ep->spak = tb_event_init(TB_NULL);
	tb_assert_and_check_goto(ep->spak, fail);

	// init events
	ep->list = tb_dlist_init(tb_align8((ep->maxn >> 3) + 1), tb_item_func_ifm(sizeof(tb_eobject_t), tb_epool_event_free, TB_NULL));
	tb_assert_and_check_goto(ep->list, fail);
	
	// init objs
	ep->objs = tb_vector_init(tb_align8((ep->maxn >> 3) + 1), tb_item_func_ifm(sizeof(tb_eobject_t), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(ep->objs, fail);

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

		// exit spank
		if (ep->spak) tb_event_exit(ep->spak);

		// exit list
		if (ep->list) tb_dlist_exit(ep->list);

		// free objs
		if (ep->objs) tb_vector_exit(ep->objs);

		// free it
		tb_free(ep);
	}
}
tb_size_t tb_epool_maxn(tb_handle_t epool)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep, 0);

	return ep->maxn;
}
tb_size_t tb_epool_size(tb_handle_t epool)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep && ep->list, 0);

	return tb_dlist_size(ep->list);
}
tb_handle_t tb_epool_adde(tb_handle_t epool, tb_pointer_t edata)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep && ep->list && tb_dlist_size(ep->list) < ep->maxn, TB_NULL);

	// init event
	tb_handle_t e = tb_event_init(TB_NULL);
	tb_assert_and_check_return_val(e, TB_NULL);

	// init object
	tb_eobject_t o;
	o.evet = e;
	o.data = edata;

	// add event
	tb_size_t itor = tb_dlist_insert_tail(ep->list, &o);
	tb_assert_and_check_goto(itor, fail);

	// ok
	return (tb_handle_t)itor;

fail:
	if (e) tb_event_exit(e);
	return TB_NULL;
}
tb_void_t tb_epool_dele(tb_handle_t epool, tb_handle_t event)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return(ep && ep->list && tb_dlist_size(ep->list) && event);

	// post first
	tb_epool_post(epool, event);

	// remove it
	tb_dlist_remove(ep->list, (tb_size_t)event);
}
tb_void_t tb_epool_post(tb_handle_t epool, tb_handle_t event)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return(ep && ep->spak && ep->list && tb_dlist_size(ep->list) && event);

	// get object
	tb_eobject_t const* o = (tb_eobject_t const*)tb_dlist_itor_const_at(ep->list, event);
	tb_assert_and_check_return(o && o->evet);

	// post event 
	tb_event_post(o->evet);

	// post spank
	tb_event_post(ep->spak);
}
tb_void_t tb_epool_kill(tb_handle_t epool)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return(ep && ep->spak && ep->list);

	// post events
	tb_size_t itor = tb_dlist_itor_head(ep->list);
	tb_size_t tail = tb_dlist_itor_tail(ep->list);
	for (; itor != tail; itor = tb_dlist_itor_next(ep->list, itor))
	{
		// get event
		tb_eobject_t const* o = (tb_eobject_t const*)tb_dlist_itor_const_at(ep->list, itor);
		tb_assert_and_check_continue(o && o->evet);

		// post event
		tb_event_post(o->evet);
	}

	// post spank
	tb_event_post(ep->spak);
}
tb_long_t tb_epool_wait(tb_handle_t epool, tb_long_t timeout)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep && ep->spak && ep->list && ep->objs, -1);

	// wait spank
	tb_long_t r = tb_event_wait(ep->spak, timeout);
    tb_check_return_val(r >= 0, -1);

	// timeout?
    tb_check_return_val(r, 0);

	// clear objs
	tb_vector_clear(ep->objs);

	// wait events
	tb_size_t itor = tb_dlist_itor_head(ep->list);
	tb_size_t tail = tb_dlist_itor_tail(ep->list);
	for (; itor != tail; itor = tb_dlist_itor_next(ep->list, itor))
	{
		// get event
		tb_eobject_t const* o = (tb_eobject_t const*)tb_dlist_itor_const_at(ep->list, itor);
		tb_assert_and_check_continue(o && o->evet);

		// wait event
		r = tb_event_wait(o->evet, 0);
    	tb_check_return_val(r >= 0, -1);

		// timeout? next 
		tb_check_continue(r);
		
		// has signal? add it to the returned objects
		tb_vector_insert_tail(ep->objs, o);
	}

	// ok
	return tb_vector_size(ep->objs);
}
tb_eobject_t* tb_epool_objs(tb_handle_t epool)
{
	tb_epool_t* ep = (tb_epool_t*)epool;
	tb_assert_and_check_return_val(ep && ep->objs, TB_NULL);

	return (tb_eobject_t*)tb_vector_data(ep->objs);
}

