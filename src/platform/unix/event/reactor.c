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

// eio
#if defined(TB_CONFIG_EVENT_HAVE_POLL)
# 	include "reactor/eio/poll.c"
#elif defined(TB_CONFIG_EVENT_HAVE_PORT)
# 	include "reactor/eio/port.c"
#elif defined(TB_CONFIG_EVENT_HAVE_SELECT)
# 	include "reactor/eio/select.c"
#elif defined(TB_CONFIG_EVENT_HAVE_EPOLL)
# 	include "reactor/eiop/epoll.c"
#elif defined(TB_CONFIG_EVENT_HAVE_KQUEUE)
# 	include "reactor/eio/kqueue.c"
#else
# 	error have not available event mode
#endif

// eiop
#if defined(TB_CONFIG_EVENT_HAVE_EPOLL)
# 	include "reactor/eiop/epoll.c"
#elif defined(TB_CONFIG_EVENT_HAVE_POLL)
# 	include "reactor/eiop/poll.c"
#elif defined(TB_CONFIG_EVENT_HAVE_PORT)
# 	include "reactor/eiop/port.c"
#elif defined(TB_CONFIG_EVENT_HAVE_SELECT)
# 	include "reactor/eiop/select.c"
#elif defined(TB_CONFIG_EVENT_HAVE_KQUEUE)
# 	include "reactor/eiop/kqueue.c"
#else
# 	error have not available event mode
#endif

/* /////////////////////////////////////////////////////////
 * eio
 */

#if defined(TB_CONFIG_EVENT_HAVE_POLL)
tb_long_t tb_eio_reactor_file_wait(tb_eio_t* object, tb_long_t timeout)
{
	return tb_eio_reactor_poll_wait(object, timeout);
}
tb_long_t tb_eio_reactor_sock_wait(tb_eio_t* object, tb_long_t timeout)
{
	return tb_eio_reactor_poll_wait(object, timeout);
}
#elif defined(TB_CONFIG_EVENT_HAVE_SELECT)
tb_long_t tb_eio_reactor_file_wait(tb_eio_t* object, tb_long_t timeout)
{
	return tb_eio_reactor_select_wait(object, timeout);
}
tb_long_t tb_eio_reactor_sock_wait(tb_eio_t* object, tb_long_t timeout)
{
	return tb_eio_reactor_select_wait(object, timeout);
}
#endif

/* /////////////////////////////////////////////////////////
 * eiop
 */
#if defined(TB_CONFIG_EVENT_HAVE_EPOLL)
tb_eiop_reactor_t* tb_eiop_reactor_file_init(tb_eiop_t* eiop)
{
	return tb_eiop_reactor_epoll_init(eiop);
}
tb_eiop_reactor_t* tb_eiop_reactor_sock_init(tb_eiop_t* eiop)
{
	return tb_eiop_reactor_epoll_init(eiop);
}
#elif defined(TB_CONFIG_EVENT_HAVE_POLL)
tb_eiop_reactor_t* tb_eiop_reactor_file_init(tb_eiop_t* eiop)
{
	return tb_eiop_reactor_poll_init(eiop);
}
tb_eiop_reactor_t* tb_eiop_reactor_sock_init(tb_eiop_t* eiop)
{
	return tb_eiop_reactor_poll_init(eiop);
}
#elif defined(TB_CONFIG_EVENT_HAVE_SELECT)
tb_eiop_reactor_t* tb_eiop_reactor_file_init(tb_eiop_t* eiop)
{
	return tb_eiop_reactor_select_init(eiop);
}
tb_eiop_reactor_t* tb_eiop_reactor_sock_init(tb_eiop_t* eiop)
{
	return tb_eiop_reactor_select_init(eiop);
}
#endif

