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
#include <sys/eventfd.h>  
#ifndef TB_CONFIG_OS_ANDROID
# 	include <sys/unistd.h>
#endif


/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the poll aioo type
typedef struct __tb_poll_aioo_t
{
	// the code
	tb_size_t 				code;

	// the data
	tb_pointer_t 			data;

}tb_poll_aioo_t;

// the poll mutx type
typedef struct __tb_poll_mutx_t
{
	// the pfds
	tb_handle_t 			pfds;

	// the hash
	tb_handle_t 			hash;

}tb_poll_mutx_t;

// the poll reactor type
typedef struct __tb_aiop_reactor_poll_t
{
	// the reactor base
	tb_aiop_reactor_t 		base;

	// the poll fds
	tb_vector_t* 			pfds;

	// the copy fds
	tb_vector_t* 			cfds;

	// the hash
	tb_hash_t* 				hash;

	// the mutx
	tb_poll_mutx_t 			mutx;

	// the spak
	tb_long_t 				spak;
	
	// the kill
	tb_long_t 				kill;
	
}tb_aiop_reactor_poll_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_poll_walk_delo(tb_vector_t* vector, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return_val(vector && bdel && data, tb_false);

	// the fd
	tb_long_t fd = (tb_long_t)data;

	// find and remove it
	if (item)
	{
		// is this?
		struct pollfd* pfd = (struct pollfd*)*item;
		if (pfd && pfd->fd == fd) 
		{
			// remove it
			*bdel = tb_true;

			// break
			return tb_false;
		}
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_poll_walk_sete(tb_vector_t* vector, tb_pointer_t* item, tb_bool_t* bdel, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return_val(vector && bdel, tb_false);

	// the aioe
	tb_aioe_t const* aioe = (tb_size_t)data;

	// find and remove it
	if (item)
	{
		// is this?
		struct pollfd* pfd = (struct pollfd*)*item;
		if (pfd && pfd->fd == ((tb_long_t)aioe->handle - 1)) 
		{
			pfd->events = 0;
			if (aioe->code & TB_AIOE_CODE_RECV || aioe->code & TB_AIOE_CODE_ACPT) pfd->events |= POLLIN;
			if (aioe->code & TB_AIOE_CODE_SEND || aioe->code & TB_AIOE_CODE_CONN) pfd->events |= POLLOUT;

			// break
			return tb_false;
		}
	}

	// ok
	return tb_true;
}
static tb_bool_t tb_aiop_reactor_poll_addo(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t code, tb_pointer_t data)
{
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds && rtor->cfds && handle, tb_false);

	// init pfd
	struct pollfd pfd = {0};
	pfd.fd = ((tb_long_t)handle) - 1;
	if (code & TB_AIOE_CODE_RECV || code & TB_AIOE_CODE_ACPT) pfd.events |= POLLIN;
	if (code & TB_AIOE_CODE_SEND || code & TB_AIOE_CODE_CONN) pfd.events |= POLLOUT;

	// add pfd, TODO: addo by binary search
	if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);
	tb_vector_insert_tail(rtor->pfds, &pfd);
	if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);

	// add handle => aioo
	tb_bool_t ok = tb_false;
	if (rtor->mutx.hash) tb_mutex_enter(rtor->mutx.hash);
	if (rtor->hash) 
	{
		tb_poll_aioo_t aioo;
		aioo.code = code;
		aioo.data = data;
		tb_hash_set(rtor->hash, handle, &aioo);
		ok = tb_true;
	}
	if (rtor->mutx.hash) tb_mutex_leave(rtor->mutx.hash);

	// spak it
	if (rtor->spak > 0 && code)
	{
		tb_uint64_t spak = 1;
		if (sizeof(tb_uint64_t) != write(rtor->spak, &spak, sizeof(tb_uint64_t))) return tb_false;
	}

	// ok?
	return ok;
}
static tb_void_t tb_aiop_reactor_poll_delo(tb_aiop_reactor_t* reactor, tb_handle_t handle)
{
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return(rtor && rtor->pfds && rtor->cfds && handle);

	// delo it, TODO: delo by binary search
	if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);
	tb_vector_walk(rtor->pfds, tb_poll_walk_delo, (tb_pointer_t)(((tb_long_t)handle) - 1));
	if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);

	// del handle => aioo
	if (rtor->mutx.hash) tb_mutex_enter(rtor->mutx.hash);
	if (rtor->hash) tb_hash_del(rtor->hash, handle);
	if (rtor->mutx.hash) tb_mutex_leave(rtor->mutx.hash);

	// spak it
	if (rtor->spak > 0)
	{
		tb_uint64_t spak = 1;
		write(rtor->spak, &spak, sizeof(tb_uint64_t));
	}
}
static tb_bool_t tb_aiop_reactor_poll_sete(tb_aiop_reactor_t* reactor, tb_aioe_t const* aioe)
{
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds && rtor->cfds && aioe, tb_false);

	// sete it, TODO: sete by binary search
	if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);
	tb_vector_walk(rtor->pfds, tb_poll_walk_sete, aioe);
	if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);

	// set handle => aioo
	tb_bool_t ok = tb_false;
	if (rtor->mutx.hash) tb_mutex_enter(rtor->mutx.hash);
	if (rtor->hash) 
	{
		tb_poll_aioo_t* aioo = (tb_poll_aioo_t*)tb_hash_get(rtor->hash, aioe->handle);
		if (aioo)
		{
			aioo->code = aioe->code;
			aioo->data = aioe->data;
			ok = tb_true;
		}
	}
	if (rtor->mutx.hash) tb_mutex_leave(rtor->mutx.hash);

	// ok?
	return ok;
}
static tb_bool_t tb_aiop_reactor_poll_post(tb_aiop_reactor_t* reactor, tb_aioe_t const* list, tb_size_t size)
{
	// check
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds && list && size, tb_false);

	// walk list
	tb_size_t i = 0;
	tb_size_t post = 0;
	for (i = 0; i < size; i++)
	{
		// the aioe
		tb_aioe_t const* aioe = &list[i];
		if (aioe)
		{
			if (tb_aiop_reactor_poll_sete(reactor, aioe)) post++;
		}
	}

	// spak it
	if (post == size && rtor->spak > 0)
	{
		tb_uint64_t spak = 1;
		if (sizeof(tb_uint64_t) != write(rtor->spak, &spak, sizeof(tb_uint64_t))) return tb_false;
	}

	// ok?
	return post == size? tb_true : tb_false;
}
static tb_void_t tb_aiop_reactor_poll_kill(tb_aiop_reactor_t* reactor)
{
	// check
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return(rtor);

	// kill it
	if (rtor->kill > 0) 
	{
		tb_uint64_t kill = 1;
		write(rtor->kill, &kill, sizeof(tb_uint64_t));
	}
}
static tb_long_t tb_aiop_reactor_poll_wait(tb_aiop_reactor_t* reactor, tb_aioe_t* list, tb_size_t maxn, tb_long_t timeout)
{	
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	tb_assert_and_check_return_val(rtor && rtor->pfds && rtor->cfds && reactor->aiop && list && maxn, -1);

	// loop
	tb_long_t wait = 0;
	tb_hong_t time = tb_mclock();
	while (!wait && (timeout < 0 || tb_mclock() < time + timeout))
	{
		// copy pfds
		if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);
		tb_vector_copy(rtor->cfds, rtor->pfds);
		if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);

		// cfds
		struct pollfd* 	cfds = (struct pollfd*)tb_vector_data(rtor->cfds);
		tb_size_t 		cfdm = tb_vector_size(rtor->cfds);
		tb_assert_and_check_return_val(cfds && cfdm, -1);

		// wait
		tb_long_t cfdn = poll(cfds, cfdm, timeout);
		tb_assert_and_check_return_val(cfdn >= 0, -1);

		// timeout?
		tb_check_return_val(cfdn, 0);

		// sync
		tb_size_t i = 0;
		for (i = 0; i < cfdm && wait < maxn; i++)
		{
			// the handle
			tb_handle_t handle = (tb_handle_t)(cfds[i].fd + 1);
			tb_assert_and_check_return_val(handle, -1);

			// the events
			tb_size_t events = cfds[i].revents;
			tb_check_continue(events);

			// killed?
			if (handle == (tb_handle_t)(rtor->kill + 1) && (events & POLLIN)) return -1;

			// spak?
			if (handle == (tb_handle_t)(rtor->spak + 1) && (events & POLLIN))
			{
				// read spak
				tb_uint64_t spak = 0;
				if (sizeof(tb_uint64_t) != read(rtor->spak, &spak, sizeof(tb_uint64_t))) return -1;

				// continue it
				continue ;
			}

			// the aioo
			if (rtor->mutx.hash) tb_mutex_enter(rtor->mutx.hash);
			tb_poll_aioo_t aioo = {0};
			if (rtor->hash)
			{
				tb_poll_aioo_t* item = (tb_poll_aioo_t*)tb_hash_get(rtor->hash, handle);
				if (item) 
				{
					// save it
					aioo = *item;

					// oneshot? clear it
					if (aioo.code & TB_AIOE_CODE_ONESHOT)
					{
						item->code = TB_AIOE_CODE_NONE;
						item->data = tb_null;
					}
				}
			}
			if (rtor->mutx.hash) tb_mutex_leave(rtor->mutx.hash);
			tb_assert_and_check_return_val(aioo.code, -1);
			
			// init aioe
			tb_aioe_t 	aioe = {0};
			aioe.data 	= aioo.data;
			aioe.handle = handle;
			if (events & POLLIN)
			{
				aioe.code |= TB_AIOE_CODE_RECV;
				if (aioo.code & TB_AIOE_CODE_ACPT) aioe.code |= TB_AIOE_CODE_ACPT;
			}
			if (events & POLLOUT) 
			{
				aioe.code |= TB_AIOE_CODE_SEND;
				if (aioo.code & TB_AIOE_CODE_CONN) aioe.code |= TB_AIOE_CODE_CONN;
			}
			if ((events & POLLHUP) && !(aioo.code & (TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND))) 
				aioe.code |= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND;

			// save aioe
			list[wait++] = aioe;

			// oneshot?
			if (aioo.code & TB_AIOE_CODE_ONESHOT)
			{
				if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);
				struct pollfd* pfds = (struct pollfd*)tb_vector_data(rtor->pfds);
				if (pfds) pfds[i].events = 0;
				if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);
			}
		}
	}

	// ok
	return wait;
}
static tb_void_t tb_aiop_reactor_poll_exit(tb_aiop_reactor_t* reactor)
{
	tb_aiop_reactor_poll_t* rtor = (tb_aiop_reactor_poll_t*)reactor;
	if (rtor)
	{
		// exit pfds
		if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);
		if (rtor->pfds) tb_vector_exit(rtor->pfds);
		rtor->pfds = tb_null;
		if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);

		// exit cfds
		if (rtor->cfds) tb_vector_exit(rtor->cfds);
		rtor->cfds = tb_null;

		// exit hash
		if (rtor->mutx.hash) tb_mutex_enter(rtor->mutx.hash);
		if (rtor->hash) tb_hash_exit(rtor->hash);
		rtor->hash = tb_null;
		if (rtor->mutx.hash) tb_mutex_leave(rtor->mutx.hash);

		// exit spak
		if (rtor->spak > 0) close(rtor->spak);
		rtor->spak = tb_null;

		// exit kill
		if (rtor->kill > 0) close(rtor->kill);
		rtor->kill = tb_null;

		// exit mutx
		if (rtor->mutx.pfds) tb_mutex_exit(rtor->mutx.pfds);
		rtor->mutx.pfds = tb_null;
		if (rtor->mutx.hash) tb_mutex_exit(rtor->mutx.hash);
		rtor->mutx.hash = tb_null;

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
		if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);
		if (rtor->pfds) tb_vector_clear(rtor->pfds);
		if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);

		// clear hash
		if (rtor->mutx.hash) tb_mutex_enter(rtor->mutx.hash);
		if (rtor->hash) tb_hash_clear(rtor->hash);
		if (rtor->mutx.hash) tb_mutex_leave(rtor->mutx.hash);
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
	rtor->base.delo = tb_aiop_reactor_poll_delo;
	rtor->base.post = tb_aiop_reactor_poll_post;
	rtor->base.wait = tb_aiop_reactor_poll_wait;
	rtor->base.kill = tb_aiop_reactor_poll_kill;

	// init mutx
	rtor->mutx.pfds = tb_mutex_init(tb_null);
	rtor->mutx.hash = tb_mutex_init(tb_null);
	tb_assert_and_check_goto(rtor->mutx.pfds && rtor->mutx.hash, fail);

	// init pfds
	rtor->pfds = tb_vector_init(tb_align8((aiop->maxn >> 3) + 1), tb_item_func_ifm(sizeof(struct pollfd), tb_null, tb_null));
	tb_assert_and_check_goto(rtor->pfds, fail);

	// init cfds
	rtor->cfds = tb_vector_init(tb_align8((aiop->maxn >> 3) + 1), tb_item_func_ifm(sizeof(struct pollfd), tb_null, tb_null));
	tb_assert_and_check_goto(rtor->cfds, fail);

	// init hash
	rtor->hash = tb_hash_init(tb_align8(tb_isqrti(aiop->maxn) + 1), tb_item_func_ptr(tb_null, tb_null), tb_item_func_ifm(sizeof(tb_poll_aioo_t), tb_null, tb_null));
	tb_assert_and_check_goto(rtor->hash, fail);

	// init spak
	rtor->spak = eventfd(0, EFD_SEMAPHORE);
	tb_assert_and_check_goto(rtor->spak > 0, fail);

	// init kill
	rtor->kill = eventfd(0, EFD_SEMAPHORE);
	tb_assert_and_check_goto(rtor->kill > 0, fail);

	// addo spak
	if (!tb_aiop_reactor_poll_addo(rtor, (tb_handle_t)(rtor->spak + 1), TB_AIOE_CODE_RECV, tb_null)) goto fail;	

	// addo kill
	if (!tb_aiop_reactor_poll_addo(rtor, (tb_handle_t)(rtor->kill + 1), TB_AIOE_CODE_RECV | TB_AIOE_CODE_ONESHOT, tb_null)) goto fail;	

	// ok
	return (tb_aiop_reactor_t*)rtor;

fail:
	if (rtor) tb_aiop_reactor_poll_exit(rtor);
	return tb_null;
}

