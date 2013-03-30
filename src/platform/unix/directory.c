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
 * @file		directory.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../directory.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#ifndef TB_CONFIG_OS_ANDROID
# 	include <sys/unistd.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_directory_create(tb_char_t const* path)
{
	// check
	tb_assert_and_check_return_val(path, tb_false);

	// path => unix
	path = tb_path_to_unix(path);
	tb_assert_and_check_return_val(path, tb_false);

	// create it
	return !mkdir(path, S_IRWXU)? tb_true : tb_false;
}
tb_bool_t tb_directory_remove(tb_char_t const* path)
{
	// check
	tb_assert_and_check_return_val(path, tb_false);

	// path => unix
	path = tb_path_to_unix(path);
	tb_assert_and_check_return_val(path, tb_false);

	// remove it
	return !remove(path)? tb_true : tb_false;
}
tb_size_t tb_directory_temp(tb_char_t* path, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(path && maxn > 4, 0);

	// the temporary directory
	tb_strncpy(path, "/tmp", maxn - 1);
	path[4] = '\0';

	// ok
	return 4;
}
tb_size_t tb_directory_curt(tb_char_t* path, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(path && maxn, 0);

	// the current directory
	tb_size_t size = 0;
	if (getcwd(path, maxn - 1)) size = tb_strlen(path);

	// ok?
	return size;
}

