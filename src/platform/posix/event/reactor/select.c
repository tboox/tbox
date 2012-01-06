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
 * \file		select.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include <sys/select.h>

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

	// the fd max
	tb_size_t 				sfdm;

	// the select fds
	fd_set 					rfdi;
	fd_set 					wfdi;
	fd_set 					efdi;

	fd_set 					rfdo;
	fd_set 					wfdo;
	fd_set 					efdo;
	
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
	ep->hash = tb_hash_init(tb_align8(tb_int32_sqrt(maxn) + 1), tb_item_func_int(), tb_item_func_ifm(sizeof(tb_eobject_t), TB_NULL, TB_NULL));
	tb_assert_and_check_goto(ep->hash, fail);

	// init fds
	FD_ZERO(&ep->rfdi);
	FD_ZERO(&ep->wfdi);
	FD_ZERO(&ep->efdi);

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
		// free pfds
		FD_ZERO(&ep->rfdi);
		FD_ZERO(&ep->wfdi);
		FD_ZERO(&ep->efdi);

		// free objs 
		if (ep->objs) tb_free(ep->objs);

		// exit hash
		if (ep->hash) tb_hash_exit(ep->hash);

		// free pool
		tb_free(ep);
	}
}
tb_size_t tb_epool_addo(tb_handle_t pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && ep->hash && handle, 0);
	tb_assert_and_check_return_val(otype == TB_EOTYPE_FILE || otype == TB_EOTYPE_SOCK, 0);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, 0);
	tb_assert_and_check_return_val(tb_hash_size(ep->hash) < FD_SETSIZE, 0);

	// update fd max
	if (fd > ep->sfdm) ep->sfdm = fd;
	
	// init fds
	fd_set* prfds = (etype & TB_ETYPE_READ || etype & TB_ETYPE_ACPT)? &ep->rfdi : TB_NULL;
	fd_set* pwfds = (etype & TB_ETYPE_WRIT || etype & TB_ETYPE_CONN)? &ep->wfdi : TB_NULL;
	if (prfds) FD_SET(fd, prfds);
	if (pwfds) FD_SET(fd, pwfds);
	FD_SET(fd, &ep->efdi);

	// init obj
	tb_eobject_t o;
	tb_eobject_seto(&o, handle, otype, etype);

	// add obj
	tb_hash_set(ep->hash, fd, &o);
	
	// ok
	return tb_hash_size(ep->hash);
}
tb_size_t tb_epool_seto(tb_handle_t pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && ep->hash && handle, 0);
	tb_assert_and_check_return_val(otype == TB_EOTYPE_FILE || otype == TB_EOTYPE_SOCK, 0);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, 0);

	// get obj
	tb_eobject_t* o = tb_hash_get(ep->hash, fd);
	tb_assert_and_check_return_val(o, -1);

	// set fds
	fd_set* prfds = (etype & TB_ETYPE_READ || etype & TB_ETYPE_ACPT)? &ep->rfdi : TB_NULL;
	fd_set* pwfds = (etype & TB_ETYPE_WRIT || etype & TB_ETYPE_CONN)? &ep->wfdi : TB_NULL;
	if (prfds) FD_SET(fd, prfds); else FD_CLR(fd, prfds);
	if (pwfds) FD_SET(fd, pwfds); else FD_CLR(fd, pwfds);
	if (prfds || pwfds) FD_SET(fd, &ep->efdi); else FD_CLR(fd, &ep->efdi);

	// set obj
	tb_eobject_seto(o, handle, otype, etype);

	// ok
	return tb_hash_size(ep->hash);
}
tb_size_t tb_epool_delo(tb_handle_t pool, tb_handle_t handle)
{
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && ep->hash && handle, 0);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, 0);

	// del fds
	FD_CLR(fd, &ep->rfdi);
	FD_CLR(fd, &ep->wfdi);
	FD_CLR(fd, &ep->efdi);

	// del obj
	tb_hash_del(ep->hash, fd);
	
	// ok
	return tb_hash_size(ep->hash);
}
tb_long_t tb_epool_wait(tb_handle_t pool, tb_eobject_t** objs, tb_long_t timeout)
{	
	tb_epool_t* ep = (tb_epool_t*)pool;
	tb_assert_and_check_return_val(ep && ep->hash && objs, -1);

	// init time
	struct timeval t = {0};
	if (timeout > 0)
	{
		t.tv_sec = timeout / 1000;
		t.tv_usec = (timeout % 1000) * 1000;
	}

	// init fdo
	tb_memcpy(&ep->rfdo, &ep->rfdi, sizeof(fd_set));
	tb_memcpy(&ep->wfdo, &ep->wfdi, sizeof(fd_set));
	tb_memcpy(&ep->efdo, &ep->efdi, sizeof(fd_set));

	// wait
	tb_long_t sfdn = select(ep->sfdm + 1, &ep->rfdo, &ep->wfdo, &ep->efdo, timeout >= 0? &t : TB_NULL);
	tb_assert_and_check_return_val(sfdn >= 0, -1);

	// timeout?
	tb_check_return_val(sfdn, 0);

	// init grow
	tb_size_t grow = tb_align8((ep->maxn >> 3) + 1);

	// init objs
	if (!ep->objs)
	{
		ep->objn = sfdn + grow;
		ep->objs = tb_calloc(ep->objn, sizeof(tb_eobject_t));
		tb_assert_and_check_return_val(ep->objs, -1);
	}
	// grow objs if not enough
	else if (sfdn > ep->objn)
	{
		// grow size
		ep->objn = sfdn + grow;
		if (ep->objn > ep->maxn) ep->objn = ep->maxn;

		// grow data
		ep->objs = tb_realloc(ep->objs, ep->objn * sizeof(tb_eobject_t));
		tb_assert_and_check_return_val(ep->objs, -1);
	}
	tb_assert(sfdn <= ep->objn);

	// update objects 
	tb_size_t i = 0;
	tb_size_t itor = tb_hash_itor_head(ep->hash);
	tb_size_t tail = tb_hash_itor_tail(ep->hash);
	for (; itor != tail; itor = tb_hash_itor_next(ep->hash, itor))
	{
		tb_hash_item_t* item = tb_hash_itor_at(ep->hash, itor);
		if (item)
		{
			tb_long_t 		fd = (tb_long_t)item->name;
			tb_eobject_t* 	o = (tb_eobject_t*)item->data;
			if (fd >= 0 && o)
			{
				tb_long_t e = 0;
				if (FD_ISSET(fd, &ep->rfdo)) 
				{
					e |= TB_ETYPE_READ;
					if (o->etype & TB_ETYPE_ACPT) e |= TB_ETYPE_ACPT;
				}
				if (FD_ISSET(fd, &ep->wfdo)) 
				{
					e |= TB_ETYPE_WRIT;
					if (o->etype & TB_ETYPE_CONN) e |= TB_ETYPE_CONN;
				}
				if (FD_ISSET(fd, &ep->efdo) && !(e & (TB_ETYPE_READ | TB_ETYPE_WRIT))) 
					e |= TB_ETYPE_READ | TB_ETYPE_WRIT;
					
				// add object
				if (e && i < ep->objn) ep->objs[i++] = *o;
			}
		}
	}
	*objs = ep->objs;
	
	// ok
	return sfdn;
}

