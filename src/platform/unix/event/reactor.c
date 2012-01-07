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
 * \file		reactor.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../../event/event.h"
#include "../../../math/math.h"
#include "../../../memory/memory.h"
#include "../../../container/container.h"
#include <sys/types.h>
#include <sys/socket.h>

// eobject
#if defined(TB_CONFIG_EVENT_HAVE_POLL)
# 	include "reactor/eobject/poll.c"
#elif defined(TB_CONFIG_EVENT_HAVE_PORT)
# 	include "reactor/eobject/port.c"
#elif defined(TB_CONFIG_EVENT_HAVE_SELECT)
# 	include "reactor/eobject/select.c"
#elif defined(TB_CONFIG_EVENT_HAVE_EPOLL)
# 	include "reactor/epool/epoll.c"
#elif defined(TB_CONFIG_EVENT_HAVE_KQUEUE)
# 	include "reactor/eobject/kqueue.c"
#else
# 	error have not available event mode
#endif

// epool
#if defined(TB_CONFIG_EVENT_HAVE_EPOLL)
# 	include "reactor/epool/epoll.c"
#elif defined(TB_CONFIG_EVENT_HAVE_POLL)
# 	include "reactor/epool/poll.c"
#elif defined(TB_CONFIG_EVENT_HAVE_PORT)
# 	include "reactor/epool/port.c"
#elif defined(TB_CONFIG_EVENT_HAVE_SELECT)
# 	include "reactor/epool/select.c"
#elif defined(TB_CONFIG_EVENT_HAVE_KQUEUE)
# 	include "reactor/epool/kqueue.c"
#else
# 	error have not available event mode
#endif

/* /////////////////////////////////////////////////////////
 * eobject
 */

#if defined(TB_CONFIG_EVENT_HAVE_POLL)
tb_long_t tb_eobject_reactor_file_wait(tb_eobject_t* object, tb_long_t timeout)
{
	return tb_eobject_reactor_poll_wait(object, timeout);
}
tb_long_t tb_eobject_reactor_sock_wait(tb_eobject_t* object, tb_long_t timeout)
{
	return tb_eobject_reactor_poll_wait(object, timeout);
}
#elif defined(TB_CONFIG_EVENT_HAVE_SELECT)
tb_long_t tb_eobject_reactor_file_wait(tb_eobject_t* object, tb_long_t timeout)
{
	return tb_eobject_reactor_select_wait(object, timeout);
}
tb_long_t tb_eobject_reactor_sock_wait(tb_eobject_t* object, tb_long_t timeout)
{
	return tb_eobject_reactor_select_wait(object, timeout);
}
#endif

tb_long_t tb_eobject_reactor_evet_wait(tb_eobject_t* object, tb_long_t timeout)
{
	tb_assert_and_check_return_val(object && object->handle && object->otype == TB_EOTYPE_EVET, -1);
	return tb_event_wait(object->handle, timeout);
}

/* /////////////////////////////////////////////////////////
 * epool
 */
#if defined(TB_CONFIG_EVENT_HAVE_EPOLL)
tb_epool_reactor_t* tb_epool_reactor_file_init(tb_epool_t* epool)
{
	return tb_epool_reactor_epoll_init(epool);
}
tb_epool_reactor_t* tb_epool_reactor_sock_init(tb_epool_t* epool)
{
	return tb_epool_reactor_epoll_init(epool);
}
#elif defined(TB_CONFIG_EVENT_HAVE_POLL)
tb_epool_reactor_t* tb_epool_reactor_file_init(tb_epool_t* epool)
{
	return tb_epool_reactor_poll_init(epool);
}
tb_epool_reactor_t* tb_epool_reactor_sock_init(tb_epool_t* epool)
{
	return tb_epool_reactor_poll_init(epool);
}
#elif defined(TB_CONFIG_EVENT_HAVE_SELECT)
tb_epool_reactor_t* tb_epool_reactor_file_init(tb_epool_t* epool)
{
	return tb_epool_reactor_select_init(epool);
}
tb_epool_reactor_t* tb_epool_reactor_sock_init(tb_epool_t* epool)
{
	return tb_epool_reactor_select_init(epool);
}
#endif

tb_epool_reactor_t* tb_epool_reactor_evet_init(tb_epool_t* epool)
{
	tb_trace_noimpl();
	return TB_NULL;
}
