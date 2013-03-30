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

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_directory_create(tb_char_t const* path)
{
	// check
	tb_assert_and_check_return_val(path, tb_false);
	
	// unix path => windows 
	tb_char_t data[8192] = {0};
	path = tb_path_to_windows(path, data, 4096);
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
	path = tb_path_to_windows(path, data, 4096);
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

