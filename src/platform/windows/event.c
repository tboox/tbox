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
 * \file		event.c
 *
 */
/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../event/eobject.h"
#include "winsock2.h"
#include "windows.h"

/* /////////////////////////////////////////////////////////
 * implemention
 */

#if defined(TB_CONFIG_EVENT_HAVE_SELECT)
tb_long_t tb_event_wait_sock_fd(tb_long_t fd, tb_size_t etype, tb_long_t timeout)
{
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
	fd_set* prfds = etype & TB_ETYPE_READ? &rfds : TB_NULL;
	fd_set* pwfds = etype & TB_ETYPE_WRIT? &wfds : TB_NULL;

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
						, timeout >= 0? &t : TB_NULL);
	tb_assert_and_check_return_val(r >= 0, -1);

	// timeout?
	tb_check_return_val(r, 0);

	// ok
	etype = 0;
	if (prfds && FD_ISSET(fd, &rfds)) etype |= TB_ETYPE_READ;
	if (pwfds && FD_ISSET(fd, &wfds)) etype |= TB_ETYPE_WRIT;
	if (FD_ISSET(fd, &efds)) etype = TB_ETYPE_EXIT;
	return etype;
}
#else
# 	error have not available event mode
#endif

tb_long_t tb_event_wait_fd(tb_long_t fd, tb_size_t otype, tb_size_t etype, tb_long_t timeout)
{
	tb_assert_and_check_return_val(fd >= 0, -1);
	tb_assert_and_check_return_val(otype == TB_EOTYPE_FILE || otype == TB_EOTYPE_SOCK, -1);

	return 	( 	otype == TB_EOTYPE_SOCK?
				tb_event_wait_sock_fd(fd, etype, timeout)
			: 	tb_event_wait_file_fd(fd, etype, timeout));
}
