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
#include "../file.h"
#include "../directory.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#ifndef TB_CONFIG_OS_ANDROID
# 	include <sys/unistd.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_void_t tb_directory_walk_remove(tb_char_t const* path, tb_file_info_t const* info, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(path && info);

	// remove file
	if (info->type == TB_FILE_TYPE_FILE) tb_file_remove(path);
	// remvoe directory
	else if (info->type == TB_FILE_TYPE_DIRECTORY) remove(path);
}

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
	// path => unix
	path = tb_path_to_unix(path);
	tb_assert_and_check_return_val(path, tb_false);

	// walk remove
	tb_directory_walk(path, tb_true, tb_directory_walk_remove, tb_null);

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
tb_void_t tb_directory_walk(tb_char_t const* path, tb_bool_t recursion, tb_directory_walk_func_t func, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(path && func);

	// path => unix
	path = tb_path_to_unix(path);
	tb_assert_and_check_return(path);

	// last
	tb_long_t 		last = tb_strlen(path) - 1;
	tb_assert_and_check_return(last >= 0);

	// init info
	tb_char_t 		temp[4096] = {0};
	DIR* 			directory = tb_null;
	if (directory = opendir(path))
	{
		// walk
		struct dirent* item = tb_null;
		while (item = readdir(directory))
		{
			// check
			tb_assert_and_check_continue(item->d_name && item->d_reclen);

			// the item name
			tb_char_t name[1024] = {0};
			tb_strncpy(name, item->d_name, tb_min(item->d_reclen, 1023));
			if (tb_strcmp(name, ".") && tb_strcmp(name, ".."))
			{
				// the temp path
				tb_long_t n = tb_snprintf(temp, 4095, "%s%s%s", path, path[last] == '/'? "" : "/", name);
				if (n >= 0) temp[n] = '\0';

				// the file info
				tb_file_info_t info = {0};
				if (tb_file_info(temp, &info))
				{
					// walk to the next directory
					if (info.type == TB_FILE_TYPE_DIRECTORY && recursion) tb_directory_walk(temp, recursion, func, data);
	
					// do callback
					func(temp, &info, data);
				}
			}
		}

		// exit directory
		closedir(directory);
	}
}
