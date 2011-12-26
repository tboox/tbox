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
#include "../../../event/epool.h"
#include "../../../memory/memory.h"
#include <sys/types.h>
#include <sys/socket.h>
#if defined(TB_CONFIG_EVENT_HAVE_EPOLL)
# 	include <sys/epoll.h>
#elif defined(TB_CONFIG_EVENT_HAVE_POLL)
# 	include <sys/poll.h>
#elif defined(TB_CONFIG_EVENT_HAVE_SELECT)
# 	include <sys/select.h>
#endif

/* /////////////////////////////////////////////////////////
 * types
 */
#if defined(TB_CONFIG_EVENT_HAVE_EPOLL)

// the event pool type
typedef struct __tb_epool_epoll_t
{
	// the base
	tb_epool_t 				base;

	// the fd
	tb_long_t 				epfd;

	// the events
	tb_pbuffer_t 			evts;
	
}tb_epool_epoll_t;

#elif defined(TB_CONFIG_EVENT_HAVE_POLL)
#elif defined(TB_CONFIG_EVENT_HAVE_SELECT)
#else
# 	error have not available event mode
#endif
/* /////////////////////////////////////////////////////////
 * implemention
 */

#if defined(TB_CONFIG_EVENT_HAVE_EPOLL)
tb_epool_t* tb_epool_init_impl(tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(maxn, TB_NULL);

	// alloc pool
	tb_epool_epoll_t* ep = tb_calloc(1, sizeof(tb_epool_epoll_t));
	tb_assert_and_check_return_val(ep, TB_NULL);

	// init epoll
	ep->epfd = epoll_create(maxn);
	tb_assert_and_check_goto(ep->epfd >= 0, fail);
	
	// init events
	if (!tb_pbuffer_init(&ep->evts)) goto fail;

	// ok
	return (tb_epool_t*)ep;

fail:
	if (ep) tb_epool_exit_impl(ep);
	return TB_NULL;
}
tb_void_t tb_epool_exit_impl(tb_epool_t* pool)
{
	tb_epool_epoll_t* ep = (tb_epool_epoll_t*)pool;
	if (pool)
	{
		// free events
		tb_pbuffer_exit(&ep->evts);
		
		// close fd
		if (ep->epfd) close(ep->epfd);

		// free pool
		tb_free(pool);
	}
}
tb_size_t tb_epool_addo_impl(tb_epool_t* pool, tb_handle_t handle, tb_size_t otype, tb_size_t etype)
{
	tb_assert_and_check_return_val(pool && handle, 0);
	tb_assert_and_check_return_val(otype == TB_EOTYPE_FILE || otype == TB_EOTYPE_SOCK, 0);

	struct epoll_event e = {0};
	if (!epoll_ctl(pool->epfd, EPOLL_CTL_ADD, (tb_long_t)handle - 1, &e)) return 0;
	return 0;
}
tb_size_t tb_epool_delo_impl(tb_epool_t* pool, tb_handle_t handle)
{
	return 0;
}
tb_size_t tb_epool_sete_impl(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype)
{
	return 0;
}
tb_size_t tb_epool_adde_impl(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype)
{
	return 0;
}
tb_size_t tb_epool_dele_impl(tb_epool_t* pool, tb_handle_t handle, tb_size_t etype)
{
	return 0;
}
#elif defined(TB_CONFIG_EVENT_HAVE_POLL)
#elif defined(TB_CONFIG_EVENT_HAVE_SELECT)
#else
# 	error have not available event mode
#endif
