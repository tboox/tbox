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
 * \file		aioo.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

#if defined(TB_CONFIG_AIO_HAVE_POLL)
# 	include "aioo/poll.c"
#elif defined(TB_CONFIG_AIO_HAVE_SELECT)
# 	include "aioo/select.c"
#else
# 	error have not available event mode
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implemention
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

