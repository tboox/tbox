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
 * @file		prefix.h
 *
 */
#ifndef TB_PLATFROM_WINDOWS_PREFIX_H
#define TB_PLATFROM_WINDOWS_PREFIX_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../../libc/libc.h"
#include "windows.h"

/* ///////////////////////////////////////////////////////////////////////
 * inlines
 */

/* transform the path to the windows style
 *
 * /c/home/file.txt
 * file:///c/home/file.txt
 *
 * => C://home/file.txt
 */
static __tb_inline__ tb_char_t const* tb_path_to_windows(tb_char_t const* path, tb_char_t* data, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(path && data && maxn > 3, tb_null);

	// is windows path? .e.g c:/home/file.txt
	if (tb_isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || path[2] == '\\'))
		return path;
	// /c/home/file.txt => c:/home/file.txt
	else if (path[0] == '/' && path[2] == '/') 
	{
		data[0] = path[1];
		data[1] = ':';
		data[2] = '/';
		tb_strncpy(data + 3, path + 2, maxn - 3);
		//tb_trace("[file]: path: %s => %s", path, data);
		return data;
	}
	// file:///c/home/file.txt => c:/home/file.txt
	else if (!tb_strnicmp(path, "file://", 7)) 
	{
		data[0] = path[8];
		data[1] = ':';
		data[2] = '/';
		tb_strncpy(data + 3, path + 9, maxn - 3);
		//tb_trace("[file]: path: %s => %s", path, data);
		return data;
	}
	return tb_null;
}


#endif
