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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author		ruki
 * @file		directory.c
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../file.h"
#include "../path.h"
#include "../directory.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

/* ///////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_void_t tb_directory_walk_remove(tb_char_t const* path, tb_file_info_t const* info, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(path && info);

	// remove
	switch (info->type)
	{
	case TB_FILE_TYPE_FILE:
		tb_file_remove(path);
		break;
	case TB_FILE_TYPE_DIRECTORY:
		remove(path);
		break;
	default:
		break;
	}
}
static tb_void_t tb_directory_walk_copy(tb_char_t const* path, tb_file_info_t const* info, tb_pointer_t data)
{
	// check
	tb_assert_and_check_return(path && info && data);

	// the dest directory
	tb_char_t const* dest = (tb_char_t const*)((tb_cpointer_t*)data)[0];
	tb_assert_and_check_return(dest);

	// the file name
	tb_size_t size = (tb_size_t)((tb_cpointer_t*)data)[1];
	tb_char_t const* name = path + size;

	// the ok pointer
	tb_size_t* ok = (tb_size_t*)&((tb_cpointer_t*)data)[2];
	tb_check_return(*ok);

	// the dest file path
	tb_char_t dpath[8192] = {0};
	tb_snprintf(dpath, 8192, "%s/%s", dest, name[0] == '/'? name + 1 : name);
//	tb_trace_i("%s => %s", path, dpath);

	// remove the dest file first
	tb_file_info_t dinfo = {0};
	if (tb_file_info(dpath, &dinfo))
	{
		if (dinfo.type == TB_FILE_TYPE_FILE)
			tb_file_remove(dpath);
		if (dinfo.type == TB_FILE_TYPE_DIRECTORY)
			tb_directory_remove(dpath);
	}

	// copy 
	switch (info->type)
	{
	case TB_FILE_TYPE_FILE:
		if (!tb_file_copy(path, dpath)) *ok = 0;
		break;
	case TB_FILE_TYPE_DIRECTORY:
		if (!tb_directory_create(dpath)) *ok = 0;
		break;
	default:
		break;
	}
}
static tb_void_t tb_directory_walk_impl(tb_char_t const* path, tb_bool_t recursion, tb_bool_t prefix, tb_directory_walk_func_t func, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(path && func);

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
			tb_strlcpy(name, item->d_name, tb_min(item->d_reclen, 1023));
			if (tb_strcmp(name, ".") && tb_strcmp(name, ".."))
			{
				// the temp path
				tb_long_t n = tb_snprintf(temp, 4095, "%s%s%s", path, path[last] == '/'? "" : "/", name);
				if (n >= 0) temp[n] = '\0';

				// the file info
				tb_file_info_t info = {0};
				if (tb_file_info(temp, &info))
				{
					// do callback
					if (prefix) func(temp, &info, data);

					// walk to the next directory
					if (info.type == TB_FILE_TYPE_DIRECTORY && recursion) tb_directory_walk_impl(temp, recursion, prefix, func, data);
	
					// do callback
					if (!prefix) func(temp, &info, data);
				}
			}
		}

		// exit directory
		closedir(directory);
	}
}
/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_directory_create(tb_char_t const* path)
{
	// check
	tb_assert_and_check_return_val(path, tb_false);

	// the full path
	tb_char_t full[TB_PATH_MAXN];
	path = tb_path_full(path, full, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// make it
	tb_bool_t ok = !mkdir(path, S_IRWXU)? tb_true : tb_false;
	if (!ok)
	{
		// make directory
		tb_char_t 			temp[TB_PATH_MAXN] = {0};
		tb_char_t const* 	p = full;
		tb_char_t* 			t = temp;
		tb_char_t const* 	e = temp + TB_PATH_MAXN - 1;
		for (; t < e && *p; t++) 
		{
			*t = *p;
			if (*p == '/')
			{
				// make directory if not exists
				if (!tb_file_info(temp, tb_null)) mkdir(temp, S_IRWXU);

				// skip repeat '/'
				while (*p && *p == '/') p++;
			}
			else p++;
		}

		// make it again
		ok = !mkdir(path, S_IRWXU)? tb_true : tb_false;
	}

	// ok?
	return ok;
}
tb_bool_t tb_directory_remove(tb_char_t const* path)
{
	// the full path
	tb_char_t full[TB_PATH_MAXN];
	path = tb_path_full(path, full, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// walk remove
	tb_directory_walk_impl(path, tb_true, tb_false, tb_directory_walk_remove, tb_null);

	// remove it
	return !remove(path)? tb_true : tb_false;
}
tb_size_t tb_directory_temp(tb_char_t* path, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(path && maxn > 4, 0);

	// the temporary directory
	tb_strlcpy(path, "/tmp", maxn - 1);
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
tb_void_t tb_directory_walk(tb_char_t const* path, tb_bool_t recursion, tb_bool_t prefix, tb_directory_walk_func_t func, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(path && func);

	// exists?
	tb_file_info_t info = {0};
	if (tb_file_info(path, &info) && info.type == TB_FILE_TYPE_DIRECTORY) 
		tb_directory_walk_impl(path, recursion, prefix, func, data);
	else
	{
		// the full path
		tb_char_t full[TB_PATH_MAXN];
		path = tb_path_full(path, full, TB_PATH_MAXN);
		tb_assert_and_check_return(path);

		// walk
		tb_directory_walk_impl(path, recursion, prefix, func, data);
	}
}
tb_bool_t tb_directory_copy(tb_char_t const* path, tb_char_t const* dest)
{
	// the full path
	tb_char_t full0[TB_PATH_MAXN];
	path = tb_path_full(path, full0, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// the dest path
	tb_char_t full1[TB_PATH_MAXN];
	dest = tb_path_full(dest, full1, TB_PATH_MAXN);
	tb_assert_and_check_return_val(dest, tb_false);

	// walk copy
	tb_cpointer_t data[3] = {0};
	data[0] = (tb_cpointer_t)dest;
	data[1] = (tb_cpointer_t)tb_strlen(path);
	data[2] = (tb_cpointer_t)1;
	tb_directory_walk_impl(path, tb_true, tb_true, tb_directory_walk_copy, data);

	// ok?
	return data[2]? tb_true : tb_false;
}

