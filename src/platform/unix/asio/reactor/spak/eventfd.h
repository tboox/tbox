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
 * @file		spak.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFROM_UNIX_ASIO_REACTOR_SPAK_EVENTFD_H
#define TB_PLATFROM_UNIX_ASIO_REACTOR_SPAK_EVENTFD_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include <sys/eventfd.h>  
#ifndef TB_CONFIG_OS_ANDROID
# 	include <sys/unistd.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */

static tb_handle_t tb_spak_init()
{
	// init event fd
	tb_long_t fd = eventfd(0, EFD_SEMAPHORE);
	tb_assert_and_check_return_val(fd >= 0, tb_null);

	// ok
	return fd + 1;
}
static tb_void_t tb_spak_exit(tb_handle_t handle)
{
	if (handle) close((tb_long_t)handle - 1);
}
static tb_bool_t tb_spak_post(tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(handle, tb_false);

	// post it
	tb_uint64_t spak = 1;
	return (sizeof(tb_uint64_t) == write((tb_long_t)handle - 1, &spak, sizeof(tb_uint64_t)))? tb_true : tb_false;
}
static tb_bool_t tb_spak_cler(tb_handle_t handle)
{
	// check
	tb_assert_and_check_return_val(handle, tb_false);

	// clear it
	tb_uint64_t spak = 0;
	return (sizeof(tb_uint64_t) == read((tb_long_t)handle - 1, &spak, sizeof(tb_uint64_t)))? tb_true : tb_false;
}
	
#endif
