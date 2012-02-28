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
 * \author		ruki
 * \file		aipp.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

#if defined(TB_CONFIG_AIO_HAVE_EPOLL)
# 	include "aipp/epoll.c"
#elif defined(TB_CONFIG_AIO_HAVE_KQUEUE)
# 	include "aipp/kqueue.c"
#elif defined(TB_CONFIG_AIO_HAVE_POLL)
# 	include "aipp/poll.c"
#elif defined(TB_CONFIG_AIO_HAVE_SELECT)
# 	include "aipp/select.c"
#else
# 	error have not available event mode
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
#if defined(TB_CONFIG_AIO_HAVE_EPOLL)
tb_aipp_reactor_t* tb_aipp_reactor_file_init(tb_aipp_t* aipp)
{
	return tb_aipp_reactor_epoll_init(aipp);
}
tb_aipp_reactor_t* tb_aipp_reactor_sock_init(tb_aipp_t* aipp)
{
	return tb_aipp_reactor_epoll_init(aipp);
}
#elif defined(TB_CONFIG_AIO_HAVE_KQUEUE)
tb_aipp_reactor_t* tb_aipp_reactor_file_init(tb_aipp_t* aipp)
{
	return tb_aipp_reactor_kqueue_init(aipp);
}
tb_aipp_reactor_t* tb_aipp_reactor_sock_init(tb_aipp_t* aipp)
{
	return tb_aipp_reactor_kqueue_init(aipp);
}
#elif defined(TB_CONFIG_AIO_HAVE_POLL)
tb_aipp_reactor_t* tb_aipp_reactor_file_init(tb_aipp_t* aipp)
{
	return tb_aipp_reactor_poll_init(aipp);
}
tb_aipp_reactor_t* tb_aipp_reactor_sock_init(tb_aipp_t* aipp)
{
	return tb_aipp_reactor_poll_init(aipp);
}
#elif defined(TB_CONFIG_AIO_HAVE_SELECT)
tb_aipp_reactor_t* tb_aipp_reactor_file_init(tb_aipp_t* aipp)
{
	return tb_aipp_reactor_select_init(aipp);
}
tb_aipp_reactor_t* tb_aipp_reactor_sock_init(tb_aipp_t* aipp)
{
	return tb_aipp_reactor_select_init(aipp);
}
#endif

