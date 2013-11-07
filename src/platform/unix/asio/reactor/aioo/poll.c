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
 * @file		poll.c
 *
 */
/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include <sys/poll.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_long_t tb_aioo_reactor_poll_wait(tb_handle_t handle, tb_size_t code, tb_long_t timeout)
{
	// check
	tb_assert_and_check_return_val(handle, -1);

	// init
	struct pollfd pfd = {0};
	pfd.fd = ((tb_long_t)handle) - 1;
	if (code & TB_AIOE_CODE_RECV || code & TB_AIOE_CODE_ACPT) pfd.events |= POLLIN;
	if (code & TB_AIOE_CODE_SEND || code & TB_AIOE_CODE_CONN) pfd.events |= POLLOUT;

	// poll
	tb_long_t r = poll(&pfd, 1, timeout);
	tb_assert_and_check_return_val(r >= 0, -1);

	// timeout?
	tb_check_return_val(r, 0);

	// error?
	tb_int_t o = 0;
	tb_int_t n = sizeof(tb_int_t);
	getsockopt(pfd.fd, SOL_SOCKET, SO_ERROR, &o, &n);
	if (o) return -1;

	// ok
	tb_long_t e = 0;
	if (pfd.revents & POLLIN) 
	{
		e |= TB_AIOE_CODE_RECV;
		if (code & TB_AIOE_CODE_ACPT) e |= TB_AIOE_CODE_ACPT;
	}
	if (pfd.revents & POLLOUT) 
	{
		e |= TB_AIOE_CODE_SEND;
		if (code & TB_AIOE_CODE_CONN) e |= TB_AIOE_CODE_CONN;
	}
	if ((pfd.revents & POLLHUP) && !(e & (TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND))) 
		e |= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND;
	return e;
}

