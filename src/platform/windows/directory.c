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

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_directory_create(tb_char_t const* path)
{
	// check
	tb_assert_and_check_return_val(path, tb_false);
	
	// unix path => windows 
	tb_char_t data[8192] = {0};
	path = tb_path_to_windows(path, data, 8192);
	tb_assert_and_check_return_val(path, tb_false);

	// create it
	return CreateDirectoryA(path, tb_null)? tb_true : tb_false;
}
tb_bool_t tb_directory_remove(tb_char_t const* path)
{
	// check
	tb_assert_and_check_return_val(path, tb_false);
	
	// unix path => windows 
	tb_char_t data[8192] = {0};
	path = tb_path_to_windows(path, data, 8192);
	tb_assert_and_check_return_val(path, tb_false);

	// remove it
	return RemoveDirectoryA(path)? tb_true : tb_false;
}
tb_size_t tb_directory_temp(tb_char_t* path, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(path && maxn > 4, 0);

	// the temporary directory
	return (tb_size_t)GetTempPathA(maxn, path);
}
tb_size_t tb_directory_curt(tb_char_t* path, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(path && maxn, 0);

	// the current directory
	return (tb_size_t)GetCurrentDirectoryA(maxn, path);
}
tb_void_t tb_directory_walk(tb_char_t const* path, tb_bool_t recursion, tb_directory_walk_func_t func, tb_cpointer_t data)
{
	// check
	tb_assert_and_check_return(path && func);
	
	// unix path => windows 
	tb_char_t buff[4096] = {0};
	path = tb_path_to_windows(path, buff, 4096);
	tb_assert_and_check_return(path);

	// last
	tb_long_t 			last = tb_strlen(path) - 1;
	tb_assert_and_check_return(last >= 0);

	// add \*.*
	tb_char_t 			temp[4096] = {0};
	tb_snprintf(temp, 4095, "%s%s*.*", path, path[last] == '\\'? "" : "\\");

	// init info
	WIN32_FIND_DATAA 	find = {0};
	HANDLE 				directory = INVALID_HANDLE_VALUE;
	if (INVALID_HANDLE_VALUE != (directory = FindFirstFileA(temp, &find)))
	{
		// walk
		do
		{
			// check
			if (tb_strcmp(find.cFileName, ".") && tb_strcmp(find.cFileName, ".."))
			{
				// the temp path
				tb_long_t n = tb_snprintf(temp, 4095, "%s%s%s", path, path[last] == '\\'? "" : "\\", find.cFileName);
				if (n >= 0) temp[n] = '\0';

				// the file info
				tb_file_info_t info = {0};
				if (tb_file_info(temp, &info))
				{
					// do callback
					func(temp, &info, data);

					// walk to the next directory
					if (info.type == TB_FILE_TYPE_DIR && recursion) tb_directory_walk(temp, recursion, func, data);
				}
			}

		} while (FindNextFile(directory, &find));

		// exit directory
		FindClose(directory);
	}
}
