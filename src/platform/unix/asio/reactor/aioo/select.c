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
 * includes
 */
#ifndef TB_CONFIG_OS_WINDOWS
# 	include <sys/select.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_long_t tb_aioo_reactor_select_wait(tb_handle_t handle, tb_size_t code, tb_long_t timeout)
{
	// check
	tb_assert_and_check_return_val(handle, -1);

	// fd
	tb_long_t fd = ((tb_long_t)handle) - 1;
	tb_assert_and_check_return_val(fd >= 0, -1);
	
	// init time
	struct timeval t = {0};
	if (timeout > 0)
	{
		t.tv_sec = timeout / 1000;
		t.tv_usec = (timeout % 1000) * 1000;
	}

	// init fds
	fd_set 	rfds;
	fd_set 	wfds;
	fd_set 	efds;
	fd_set* prfds = (code & TB_AIOE_CODE_RECV || code & TB_AIOE_CODE_ACPT)? &rfds : tb_null;
	fd_set* pwfds = (code & TB_AIOE_CODE_SEND || code & TB_AIOE_CODE_CONN)? &wfds : tb_null;

	if (prfds)
	{
		FD_ZERO(prfds);
		FD_SET(fd, prfds);
	}

	if (pwfds)
	{
		FD_ZERO(pwfds);
		FD_SET(fd, pwfds);
	}
	
	FD_ZERO(&efds);
	FD_SET(fd, &efds);

	// select
	tb_long_t r = select(fd + 1
						, prfds
						, pwfds
						, &efds
						, timeout >= 0? &t : tb_null);
	tb_assert_and_check_return_val(r >= 0, -1);

	// timeout?
	tb_check_return_val(r, 0);

	// error?
	tb_int_t o = 0;
	tb_int_t n = sizeof(tb_int_t);
	getsockopt(fd, SOL_SOCKET, SO_ERROR, &o, &n);
	if (o) return -1;

	// ok
	tb_long_t e = 0;
	if (prfds && FD_ISSET(fd, &rfds)) 
	{
		e |= TB_AIOE_CODE_RECV;
		if (code & TB_AIOE_CODE_ACPT) e |= TB_AIOE_CODE_ACPT;
	}
	if (pwfds && FD_ISSET(fd, &wfds)) 
	{
		e |= TB_AIOE_CODE_SEND;
		if (code & TB_AIOE_CODE_CONN) e |= TB_AIOE_CODE_CONN;
	}
	if (FD_ISSET(fd, &efds) && !(e & (TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND))) 
		e |= TB_AIOE_CODE_RECV | TB_AIOE_CODE_SEND;
	return e;
}

