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
 * \file		poll.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include <sys/poll.h>

/* /////////////////////////////////////////////////////////
 * implemention
 */
static tb_long_t tb_aioo_reactor_poll_wait(tb_aioo_t* object, tb_long_t timeout)
{
	tb_assert_and_check_return_val(object, -1);

	// type
	tb_size_t otype = object->otype;
	tb_size_t etype = object->etype;
	tb_assert_and_check_return_val(otype == TB_AIOO_OTYPE_FILE || otype == TB_AIOO_OTYPE_SOCK, -1);

	// fd
	tb_long_t fd = ((tb_long_t)object->handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, -1);
	
	// init
	struct pollfd pfd = {0};
	pfd.fd = fd;
	if (etype & TB_AIOO_ETYPE_READ || etype & TB_AIOO_ETYPE_ACPT) pfd.events |= POLLIN;
	if (etype & TB_AIOO_ETYPE_WRIT || etype & TB_AIOO_ETYPE_CONN) pfd.events |= POLLOUT;

	// poll
	tb_long_t r = poll(&pfd, 1, timeout);
	tb_assert_and_check_return_val(r >= 0, -1);

	// timeout?
	tb_check_return_val(r, 0);

	// error?
	if (otype == TB_AIOO_OTYPE_SOCK)
	{
		tb_int_t o = 0;
		tb_int_t n = sizeof(tb_int_t);
		getsockopt(fd, SOL_SOCKET, SO_ERROR, &o, &n);
		if (o) return -1;
	}

	// ok
	tb_long_t e = 0;
	if (pfd.revents & POLLIN) 
	{
		e |= TB_AIOO_ETYPE_READ;
		if (etype & TB_AIOO_ETYPE_ACPT) e |= TB_AIOO_ETYPE_ACPT;
	}
	if (pfd.revents & POLLOUT) 
	{
		e |= TB_AIOO_ETYPE_WRIT;
		if (etype & TB_AIOO_ETYPE_CONN) e |= TB_AIOO_ETYPE_CONN;
	}
	if ((pfd.revents & POLLHUP) && !(e & (TB_AIOO_ETYPE_READ | TB_AIOO_ETYPE_WRIT))) 
		e |= TB_AIOO_ETYPE_READ | TB_AIOO_ETYPE_WRIT;
	return e;
}

