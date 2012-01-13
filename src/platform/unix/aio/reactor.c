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
#include "../../../aio/aio.h"
#include "../../../math/math.h"
#include "../../../memory/memory.h"
#include "../../../container/container.h"
#include <sys/types.h>
#include <sys/socket.h>

// aio
#if defined(TB_CONFIG_AIO_HAVE_POLL)
# 	include "reactor/aioo/poll.c"
#elif defined(TB_CONFIG_AIO_HAVE_PORT)
# 	include "reactor/aioo/port.c"
#elif defined(TB_CONFIG_AIO_HAVE_SELECT)
# 	include "reactor/aioo/select.c"
#elif defined(TB_CONFIG_AIO_HAVE_EPOLL)
# 	include "reactor/aiop/epoll.c"
#elif defined(TB_CONFIG_AIO_HAVE_KQUEUE)
# 	include "reactor/aioo/kqueue.c"
#else
# 	error have not available event mode
#endif

// aiop
#if defined(TB_CONFIG_AIO_HAVE_EPOLL)
# 	include "reactor/aiop/epoll.c"
#elif defined(TB_CONFIG_AIO_HAVE_KQUEUE)
# 	include "reactor/aiop/kqueue.c"
#elif defined(TB_CONFIG_AIO_HAVE_POLL)
# 	include "reactor/aiop/poll.c"
#elif defined(TB_CONFIG_AIO_HAVE_PORT)
# 	include "reactor/aiop/port.c"
#elif defined(TB_CONFIG_AIO_HAVE_SELECT)
# 	include "reactor/aiop/select.c"
#else
# 	error have not available event mode
#endif

/* /////////////////////////////////////////////////////////
 * aio
 */

#if defined(TB_CONFIG_AIO_HAVE_POLL)
tb_long_t tb_aioo_reactor_file_wait(tb_aioo_t* object, tb_long_t timeout)
{
	return tb_aioo_reactor_poll_wait(object, timeout);
}
tb_long_t tb_aioo_reactor_sock_wait(tb_aioo_t* object, tb_long_t timeout)
{
	return tb_aioo_reactor_poll_wait(object, timeout);
}
#elif defined(TB_CONFIG_AIO_HAVE_SELECT)
tb_long_t tb_aioo_reactor_file_wait(tb_aioo_t* object, tb_long_t timeout)
{
	return tb_aioo_reactor_select_wait(object, timeout);
}
tb_long_t tb_aioo_reactor_sock_wait(tb_aioo_t* object, tb_long_t timeout)
{
	return tb_aioo_reactor_select_wait(object, timeout);
}
#endif

/* /////////////////////////////////////////////////////////
 * aiop
 */
#if defined(TB_CONFIG_AIO_HAVE_EPOLL)
tb_aiop_reactor_t* tb_aiop_reactor_file_init(tb_aiop_t* aiop)
{
	return tb_aiop_reactor_epoll_init(aiop);
}
tb_aiop_reactor_t* tb_aiop_reactor_sock_init(tb_aiop_t* aiop)
{
	return tb_aiop_reactor_epoll_init(aiop);
}
#elif defined(TB_CONFIG_AIO_HAVE_KQUEUE)
tb_aiop_reactor_t* tb_aiop_reactor_file_init(tb_aiop_t* aiop)
{
	return tb_aiop_reactor_kqueue_init(aiop);
}
tb_aiop_reactor_t* tb_aiop_reactor_sock_init(tb_aiop_t* aiop)
{
	return tb_aiop_reactor_kqueue_init(aiop);
}
#elif defined(TB_CONFIG_AIO_HAVE_POLL)
tb_aiop_reactor_t* tb_aiop_reactor_file_init(tb_aiop_t* aiop)
{
	return tb_aiop_reactor_poll_init(aiop);
}
tb_aiop_reactor_t* tb_aiop_reactor_sock_init(tb_aiop_t* aiop)
{
	return tb_aiop_reactor_poll_init(aiop);
}
#elif defined(TB_CONFIG_AIO_HAVE_SELECT)
tb_aiop_reactor_t* tb_aiop_reactor_file_init(tb_aiop_t* aiop)
{
	return tb_aiop_reactor_select_init(aiop);
}
tb_aiop_reactor_t* tb_aiop_reactor_sock_init(tb_aiop_t* aiop)
{
	return tb_aiop_reactor_select_init(aiop);
}
#endif

