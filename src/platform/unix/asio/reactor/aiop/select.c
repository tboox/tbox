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
 * @file		select.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#ifndef TB_CONFIG_OS_WINDOWS
# 	include <sys/select.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the poll aioo type
typedef struct __tb_select_aioo_t
{
	// the code
	tb_size_t 				code;

	// the data
	tb_pointer_t 			data;

}tb_select_aioo_t;

// the poll mutx type
typedef struct __tb_select_mutx_t
{
	// the pfds
	tb_handle_t 			pfds;

	// the hash
	tb_handle_t 			hash;

}tb_select_mutx_t;

// the select reactor type
typedef struct __tb_aiop_reactor_select_t
{
	// the reactor base
	tb_aiop_reactor_t 		base;

	// the fd max
	tb_size_t 				sfdm;

	// the select fds
	fd_set 					rfdi;
	fd_set 					wfdi;
	fd_set 					efdi;

	fd_set 					rfdo;
	fd_set 					wfdo;
	fd_set 					efdo;

	// the hash
	tb_hash_t* 				hash;

	// the mutx
	tb_select_mutx_t 		mutx;

}tb_aiop_reactor_select_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aiop_reactor_select_addo(tb_aiop_reactor_t* reactor, tb_handle_t handle, tb_size_t code, tb_pointer_t data)
{
	// check
	tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
	tb_assert_and_check_return_val(rtor && reactor->aiop && handle, tb_false);

	// check size
	if (rtor->mutx.hash) tb_mutex_enter(rtor->mutx.hash);
	tb_size_t size = tb_hash_size(rtor->hash);
	if (rtor->mutx.hash) tb_mutex_leave(rtor->mutx.hash);
	tb_assert_and_check_return_val(size < FD_SETSIZE, tb_false);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;

	// enter
	if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);

	// update fd max
	if (fd > rtor->sfdm) rtor->sfdm = fd;
	
	// init fds
	fd_set* prfds = (code & TB_AIOE_CODE_RECV || code & TB_AIOE_CODE_ACPT)? &rtor->rfdi : tb_null;
	fd_set* pwfds = (code & TB_AIOE_CODE_SEND || code & TB_AIOE_CODE_CONN)? &rtor->wfdi : tb_null;
	if (prfds) FD_SET(fd, prfds);
	if (pwfds) FD_SET(fd, pwfds);
	FD_SET(fd, &rtor->efdi);

	// leave
	if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);

	// add handle => aioo
	tb_bool_t ok = tb_false;
	if (rtor->mutx.hash) tb_mutex_enter(rtor->mutx.hash);
	if (rtor->hash) 
	{
		tb_select_aioo_t aioo;
		aioo.code = code;
		aioo.data = data;
		tb_hash_set(rtor->hash, handle, &aioo);
		ok = tb_true;
	}
	if (rtor->mutx.hash) tb_mutex_leave(rtor->mutx.hash);

	// ok?
	return ok;
}
static tb_void_t tb_aiop_reactor_select_delo(tb_aiop_reactor_t* reactor, tb_handle_t handle)
{
	// check
	tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
	tb_assert_and_check_return(rtor && handle);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;

	// enter
	if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);

	// del fds
	FD_CLR(fd, &rtor->rfdi);
	FD_CLR(fd, &rtor->wfdi);
	FD_CLR(fd, &rtor->efdi);

	// leave
	if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);

	// del handle => aioo
	if (rtor->mutx.hash) tb_mutex_enter(rtor->mutx.hash);
	if (rtor->hash) tb_hash_del(rtor->hash, handle);
	if (rtor->mutx.hash) tb_mutex_leave(rtor->mutx.hash);
}
static tb_bool_t tb_aiop_reactor_select_sete(tb_aiop_reactor_t* reactor, tb_aioe_t const* aioe)
{
	// check
	tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
	tb_assert_and_check_return_val(rtor && aioe && aioe->handle, tb_false);

	// fd
	tb_long_t fd = ((tb_long_t)aioe->handle) - 1;

	// enter
	if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);

	// set fds
	fd_set* prfds = (aioe->code & TB_AIOE_CODE_RECV || aioe->code & TB_AIOE_CODE_ACPT)? &rtor->rfdi : tb_null;
	fd_set* pwfds = (aioe->code & TB_AIOE_CODE_SEND || aioe->code & TB_AIOE_CODE_CONN)? &rtor->wfdi : tb_null;
	if (prfds) FD_SET(fd, prfds); else FD_CLR(fd, prfds);
	if (pwfds) FD_SET(fd, pwfds); else FD_CLR(fd, pwfds);
	if (prfds || pwfds) FD_SET(fd, &rtor->efdi); else FD_CLR(fd, &rtor->efdi);

	// leave
	if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);

	// set handle => aioo
	tb_bool_t ok = tb_false;
	if (rtor->mutx.hash) tb_mutex_enter(rtor->mutx.hash);
	if (rtor->hash) 
	{
		tb_select_aioo_t* aioo = (tb_select_aioo_t*)tb_hash_get(rtor->hash, aioe->handle);
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
static tb_bool_t tb_aiop_reactor_select_post(tb_aiop_reactor_t* reactor, tb_aioe_t const* list, tb_size_t size)
{
	// check
	tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
	tb_assert_and_check_return_val(rtor && list && size, tb_false);

	// walk list
	tb_size_t i = 0;
	tb_size_t post = 0;
	for (i = 0; i < size; i++)
	{
		// the aioe
		tb_aioe_t const* aioe = &list[i];
		if (aioe)
		{
			if (tb_aiop_reactor_select_sete(reactor, aioe)) post++;
		}
	}

	// ok?
	return post? tb_true : tb_false;
}
static tb_long_t tb_aiop_reactor_select_wait(tb_aiop_reactor_t* reactor, tb_aioe_t* list, tb_size_t maxn, tb_long_t timeout)
{	
	// check
	tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
	tb_assert_and_check_return_val(rtor && reactor->aiop && list && maxn, -1);

	// init time
	struct timeval t = {0};
	if (timeout > 0)
	{
		t.tv_sec = timeout / 1000;
		t.tv_usec = (timeout % 1000) * 1000;
	}

	// enter
	if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);

	// init fdo
	tb_size_t sfdm = rtor->sfdm;
	tb_memcpy(&rtor->rfdo, &rtor->rfdi, sizeof(fd_set));
	tb_memcpy(&rtor->wfdo, &rtor->wfdi, sizeof(fd_set));
	tb_memcpy(&rtor->efdo, &rtor->efdi, sizeof(fd_set));

	// leave
	if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);

	// wait
	tb_long_t sfdn = select(sfdm + 1, &rtor->rfdo, &rtor->wfdo, &rtor->efdo, timeout >= 0? &t : tb_null);
	tb_assert_and_check_return_val(sfdn >= 0, -1);

	// timeout?
	tb_check_return_val(sfdn, 0);
	
	// sync
	tb_size_t n = 0;
	tb_size_t itor = tb_iterator_head(rtor->hash);
	tb_size_t tail = tb_iterator_tail(rtor->hash);
	for (; itor != tail && n < maxn; itor = tb_iterator_next(rtor->hash, itor))
	{
		tb_hash_item_t* item = tb_iterator_item(rtor->hash, itor);
		if (item)
		{
			// the handle
			tb_handle_t 			handle = (tb_handle_t)item->name;
			tb_assert_and_check_return_val(handle, -1);

			// the fd
			tb_long_t 				fd = (tb_long_t)item->name - 1;

			// the aioo
			tb_select_aioo_t const* aioo = (tb_select_aioo_t const*)item->data;
			tb_assert_and_check_return_val(aioo, -1);

			// init aioe
			tb_aioe_t aioe = {0};
			aioe.data 	= aioo->data;
			aioe.handle = handle;
			if (FD_ISSET(fd, &rtor->rfdo)) 
			{
				aioe.code |= TB_AIOE_CODE_RECV;
				if (aioo->code & TB_AIOE_CODE_ACPT) aioe.code |= TB_AIOE_CODE_ACPT;
			}
			if (FD_ISSET(fd, &rtor->wfdo)) 
			{
				aioe.code |= TB_AIOE_CODE_SEND;
				if (aioo->code & TB_AIOE_CODE_CONN) aioe.code |= TB_AIOE_CODE_CONN;
			}
			if (FD_ISSET(fd, &rtor->efdo) && !(aioe.code & (TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND))) 
				aioe.code |= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND;
				
			// save aioe
			if (aioe.code) list[n++] = aioe;
		}
	}

	// ok
	return n;
}
static tb_void_t tb_aiop_reactor_select_exit(tb_aiop_reactor_t* reactor)
{
	tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
	if (rtor)
	{
		// free fds
		if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);
		FD_ZERO(&rtor->rfdi);
		FD_ZERO(&rtor->wfdi);
		FD_ZERO(&rtor->efdi);
		FD_ZERO(&rtor->rfdo);
		FD_ZERO(&rtor->wfdo);
		FD_ZERO(&rtor->efdo);
		if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);

		// exit hash
		if (rtor->mutx.hash) tb_mutex_enter(rtor->mutx.hash);
		if (rtor->hash) tb_hash_exit(rtor->hash);
		rtor->hash = tb_null;
		if (rtor->mutx.hash) tb_mutex_leave(rtor->mutx.hash);

		// exit mutx
		if (rtor->mutx.pfds) tb_mutex_exit(rtor->mutx.pfds);
		rtor->mutx.pfds = tb_null;
		if (rtor->mutx.hash) tb_mutex_exit(rtor->mutx.hash);
		rtor->mutx.hash = tb_null;

		// free it
		tb_free(rtor);
	}
}
static tb_void_t tb_aiop_reactor_select_cler(tb_aiop_reactor_t* reactor)
{
	tb_aiop_reactor_select_t* rtor = (tb_aiop_reactor_select_t*)reactor;
	if (rtor)
	{
		// free fds
		if (rtor->mutx.pfds) tb_mutex_enter(rtor->mutx.pfds);
		rtor->sfdm = 0;
		FD_ZERO(&rtor->rfdi);
		FD_ZERO(&rtor->wfdi);
		FD_ZERO(&rtor->efdi);
		FD_ZERO(&rtor->rfdo);
		FD_ZERO(&rtor->wfdo);
		FD_ZERO(&rtor->efdo);
		if (rtor->mutx.pfds) tb_mutex_leave(rtor->mutx.pfds);
	
		// clear hash
		if (rtor->mutx.hash) tb_mutex_enter(rtor->mutx.hash);
		if (rtor->hash) tb_hash_clear(rtor->hash);
		if (rtor->mutx.hash) tb_mutex_leave(rtor->mutx.hash);
	}
}
static tb_aiop_reactor_t* tb_aiop_reactor_select_init(tb_aiop_t* aiop)
{
	// check
	tb_assert_and_check_return_val(aiop && aiop->maxn, tb_null);

	// alloc reactor
	tb_aiop_reactor_select_t* rtor = tb_malloc0(sizeof(tb_aiop_reactor_select_t));
	tb_assert_and_check_return_val(rtor, tb_null);

	// init base
	rtor->base.aiop = aiop;
	rtor->base.exit = tb_aiop_reactor_select_exit;
	rtor->base.cler = tb_aiop_reactor_select_cler;
	rtor->base.addo = tb_aiop_reactor_select_addo;
	rtor->base.delo = tb_aiop_reactor_select_delo;
	rtor->base.post = tb_aiop_reactor_select_post;
	rtor->base.wait = tb_aiop_reactor_select_wait;

	// init fds
	FD_ZERO(&rtor->rfdi);
	FD_ZERO(&rtor->wfdi);
	FD_ZERO(&rtor->efdi);
	FD_ZERO(&rtor->rfdo);
	FD_ZERO(&rtor->wfdo);
	FD_ZERO(&rtor->efdo);

	// init mutx
	rtor->mutx.pfds = tb_mutex_init(tb_null);
	rtor->mutx.hash = tb_mutex_init(tb_null);
	tb_assert_and_check_goto(rtor->mutx.pfds && rtor->mutx.hash, fail);

	// init hash
	rtor->hash = tb_hash_init(tb_align8(tb_isqrti(aiop->maxn) + 1), tb_item_func_ptr(tb_null, tb_null), tb_item_func_ifm(sizeof(tb_select_aioo_t), tb_null, tb_null));
	tb_assert_and_check_goto(rtor->hash, fail);

	// ok
	return (tb_aiop_reactor_t*)rtor;

fail:
	if (rtor) tb_aiop_reactor_select_exit(rtor);
	return tb_null;
}

