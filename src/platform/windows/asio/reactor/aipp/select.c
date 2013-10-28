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
 * types
 */

// the select reactor type
typedef struct __tb_aipp_reactor_select_t
{
	// the reactor base
	tb_aipp_reactor_t 		base;

	// the fd max
	tb_size_t 				sfdm;

	// the select fds
	fd_set 					rfdi;
	fd_set 					wfdi;
	fd_set 					efdi;

	fd_set 					rfdo;
	fd_set 					wfdo;
	fd_set 					efdo;
	
}tb_aipp_reactor_select_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_aipp_reactor_select_addo(tb_aipp_reactor_t* reactor, tb_handle_t handle, tb_size_t etype)
{
	tb_aipp_reactor_select_t* rtor = (tb_aipp_reactor_select_t*)reactor;
	tb_assert_and_check_return_val(rtor && reactor->aipp && reactor->aipp->hash, tb_false);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, tb_false);
	tb_assert_and_check_return_val(tb_hash_size(reactor->aipp->hash) < FD_SETSIZE, tb_false);

	// update fd max
	if (fd > rtor->sfdm) rtor->sfdm = fd;
	
	// init fds
	fd_set* prfds = (etype & TB_AIOO_ETYPE_READ || etype & TB_AIOO_ETYPE_ACPT)? &rtor->rfdi : tb_null;
	fd_set* pwfds = (etype & TB_AIOO_ETYPE_WRIT || etype & TB_AIOO_ETYPE_CONN)? &rtor->wfdi : tb_null;
	if (prfds) FD_SET(fd, prfds);
	if (pwfds) FD_SET(fd, pwfds);
	FD_SET(fd, &rtor->efdi);

	// ok
	return tb_true;
}
static tb_bool_t tb_aipp_reactor_select_seto(tb_aipp_reactor_t* reactor, tb_handle_t handle, tb_size_t etype, tb_aioo_t const* obj)
{
	tb_aipp_reactor_select_t* rtor = (tb_aipp_reactor_select_t*)reactor;
	tb_assert_and_check_return_val(rtor, tb_false);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, tb_false);

	// set fds
	fd_set* prfds = (etype & TB_AIOO_ETYPE_READ || etype & TB_AIOO_ETYPE_ACPT)? &rtor->rfdi : tb_null;
	fd_set* pwfds = (etype & TB_AIOO_ETYPE_WRIT || etype & TB_AIOO_ETYPE_CONN)? &rtor->wfdi : tb_null;
	if (prfds) FD_SET(fd, prfds); else FD_CLR(fd, prfds);
	if (pwfds) FD_SET(fd, pwfds); else FD_CLR(fd, pwfds);
	if (prfds || pwfds) FD_SET(fd, &rtor->efdi); else FD_CLR(fd, &rtor->efdi);

	// ok
	return tb_true;
}
static tb_bool_t tb_aipp_reactor_select_delo(tb_aipp_reactor_t* reactor, tb_handle_t handle)
{
	tb_aipp_reactor_select_t* rtor = (tb_aipp_reactor_select_t*)reactor;
	tb_assert_and_check_return_val(rtor, tb_false);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, tb_false);

	// del fds
	FD_CLR(fd, &rtor->rfdi);
	FD_CLR(fd, &rtor->wfdi);
	FD_CLR(fd, &rtor->efdi);

	// ok
	return tb_true;
}
static tb_long_t tb_aipp_reactor_select_wait(tb_aipp_reactor_t* reactor, tb_aioo_t* objs, tb_size_t objm, tb_long_t timeout)
{	
	tb_aipp_reactor_select_t* rtor = (tb_aipp_reactor_select_t*)reactor;
	tb_assert_and_check_return_val(rtor && reactor->aipp && reactor->aipp->hash, -1);

	// init time
	struct timeval t = {0};
	if (timeout > 0)
	{
		t.tv_sec = timeout / 1000;
		t.tv_usec = (timeout % 1000) * 1000;
	}

	// init fdo
	tb_memcpy(&rtor->rfdo, &rtor->rfdi, sizeof(fd_set));
	tb_memcpy(&rtor->wfdo, &rtor->wfdi, sizeof(fd_set));
	tb_memcpy(&rtor->efdo, &rtor->efdi, sizeof(fd_set));

	// wait
	tb_long_t sfdn = select(rtor->sfdm + 1, &rtor->rfdo, &rtor->wfdo, &rtor->efdo, timeout >= 0? &t : tb_null);
	tb_assert_and_check_return_val(sfdn >= 0, -1);

	// timeout?
	tb_check_return_val(sfdn, 0);
	
	// sync
	tb_size_t n = 0;
	tb_size_t itor = tb_iterator_head(reactor->aipp->hash);
	tb_size_t tail = tb_iterator_tail(reactor->aipp->hash);
	for (; itor != tail && n < objm; itor = tb_iterator_next(reactor->aipp->hash, itor))
	{
		tb_hash_item_t* item = tb_iterator_item(reactor->aipp->hash, itor);
		if (item)
		{
			tb_long_t 		fd = (tb_long_t)item->name - 1;
			tb_aioo_t* 		o = (tb_aioo_t*)item->data;
			if (fd >= 0 && o)
			{
				tb_long_t e = 0;
				if (FD_ISSET(fd, &rtor->rfdo)) 
				{
					e |= TB_AIOO_ETYPE_READ;
					if (o->etype & TB_AIOO_ETYPE_ACPT) e |= TB_AIOO_ETYPE_ACPT;
				}
				if (FD_ISSET(fd, &rtor->wfdo)) 
				{
					e |= TB_AIOO_ETYPE_WRIT;
					if (o->etype & TB_AIOO_ETYPE_CONN) e |= TB_AIOO_ETYPE_CONN;
				}
				if (FD_ISSET(fd, &rtor->efdo) && !(e & (TB_AIOO_ETYPE_READ | TB_AIOO_ETYPE_WRIT))) 
					e |= TB_AIOO_ETYPE_READ | TB_AIOO_ETYPE_WRIT;
					
				// add object
				if (e) objs[n++] = *o;
			}
		}
	}

	// ok
	return n;
}
static tb_void_t tb_aipp_reactor_select_exit(tb_aipp_reactor_t* reactor)
{
	tb_aipp_reactor_select_t* rtor = (tb_aipp_reactor_select_t*)reactor;
	if (rtor)
	{
		// free fds
		FD_ZERO(&rtor->rfdi);
		FD_ZERO(&rtor->wfdi);
		FD_ZERO(&rtor->efdi);
		FD_ZERO(&rtor->rfdo);
		FD_ZERO(&rtor->wfdo);
		FD_ZERO(&rtor->efdo);

		// free it
		tb_free(rtor);
	}
}
static tb_void_t tb_aipp_reactor_select_cler(tb_aipp_reactor_t* reactor)
{
	tb_aipp_reactor_select_t* rtor = (tb_aipp_reactor_select_t*)reactor;
	if (rtor)
	{
		// free fds
		FD_ZERO(&rtor->rfdi);
		FD_ZERO(&rtor->wfdi);
		FD_ZERO(&rtor->efdi);
		FD_ZERO(&rtor->rfdo);
		FD_ZERO(&rtor->wfdo);
		FD_ZERO(&rtor->efdo);
	}
}
static tb_aipp_reactor_t* tb_aipp_reactor_select_init(tb_aipp_t* aipp)
{
	// check
	tb_assert_and_check_return_val(aipp && aipp->maxn, tb_null);
	tb_assert_and_check_return_val(aipp->type == TB_AIOO_OTYPE_FILE || aipp->type == TB_AIOO_OTYPE_SOCK, tb_null);

	// alloc reactor
	tb_aipp_reactor_select_t* rtor = tb_malloc0(sizeof(tb_aipp_reactor_select_t));
	tb_assert_and_check_return_val(rtor, tb_null);

	// init base
	rtor->base.aipp = aipp;
	rtor->base.exit = tb_aipp_reactor_select_exit;
	rtor->base.cler = tb_aipp_reactor_select_cler;
	rtor->base.addo = tb_aipp_reactor_select_addo;
	rtor->base.seto = tb_aipp_reactor_select_seto;
	rtor->base.delo = tb_aipp_reactor_select_delo;
	rtor->base.wait = tb_aipp_reactor_select_wait;

	// init fds
	FD_ZERO(&rtor->rfdi);
	FD_ZERO(&rtor->wfdi);
	FD_ZERO(&rtor->efdi);
	FD_ZERO(&rtor->rfdo);
	FD_ZERO(&rtor->wfdo);
	FD_ZERO(&rtor->efdo);

	// ok
	return (tb_aipp_reactor_t*)rtor;

fail:
	if (rtor) tb_aipp_reactor_select_exit(rtor);
	return tb_null;
}

