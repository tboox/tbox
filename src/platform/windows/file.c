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
 * \author		ruki
 * \file		file.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../file.h"
#include "../../libc/libc.h"
#include <windows.h>

/* ///////////////////////////////////////////////////////////////////////
 * path
 */

/* transform the file path to the windows style
 *
 * /c/home/file.txt
 * file:///c/home/file.txt
 *
 * => C://home/file.txt
 */
static tb_char_t const* tb_file_path_to_windows(tb_char_t const* path, tb_char_t* data, tb_size_t maxn)
{
	tb_assert_and_check_return_val(path && data && maxn > 3, TB_NULL);
	if (path[0] == '/' && path[2] == '/') 
	{
		data[0] = path[1];
		data[1] = ':';
		data[2] = '/';
		tb_strncpy(data + 3, path + 2, maxn - 3);
		//tb_trace("[file]: path: %s => %s", path, data);
		return data;
	}
	else if (!tb_strnicmp(path, "file://", 7)) 
	{
		data[0] = path[8];
		data[1] = ':';
		data[2] = '/';
		tb_strncpy(data + 3, path + 9, maxn - 3);
		//tb_trace("[file]: path: %s => %s", path, data);
		return data;
	}
	return TB_NULL;
}

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */

// file
tb_handle_t tb_file_init(tb_char_t const* path, tb_size_t flags)
{
	tb_assert_and_check_return_val(path, TB_NULL);

	// unix path => windows 
	tb_char_t data[4096];
	path = tb_file_path_to_windows(path, data, 4096);
	tb_assert_and_check_return_val(path, TB_NULL);

	DWORD access = GENERIC_READ;
	if (flags & TB_FILE_RO) access = GENERIC_READ;
	else if (flags & TB_FILE_WO) access = GENERIC_WRITE;
	else if (flags & TB_FILE_RW) access = GENERIC_READ | GENERIC_WRITE;

	DWORD share = FILE_SHARE_READ;
	if (flags & TB_FILE_RO) share = FILE_SHARE_READ;
	else if (flags & TB_FILE_WO) share = FILE_SHARE_WRITE;
	else if (flags & TB_FILE_RW) share = FILE_SHARE_READ | FILE_SHARE_WRITE;

	DWORD cflag = 0;
	if (flags & TB_FILE_CREAT) cflag |= CREATE_ALWAYS;
	else if (flags & TB_FILE_TRUNC) cflag |= TRUNCATE_EXISTING;
	if (!cflag) cflag |= OPEN_EXISTING;

	HANDLE hfile = CreateFile(path, access, share, NULL, cflag, FILE_ATTRIBUTE_NORMAL, NULL);
	return hfile != INVALID_HANDLE_VALUE? (tb_handle_t)hfile : TB_NULL;
}
tb_bool_t tb_file_exit(tb_handle_t hfile)
{
	tb_assert_and_check_return_val(hfile, TB_FALSE);
	return CloseHandle(hfile)? TB_TRUE : TB_FALSE;
}
tb_long_t tb_file_read(tb_handle_t hfile, tb_byte_t* data, tb_size_t size)
{
	tb_assert_and_check_return_val(hfile, -1);
	DWORD real_size = 0;
	return ReadFile(hfile, data, size, &real_size, NULL)? (tb_long_t)real_size : -1;
}
tb_long_t tb_file_writ(tb_handle_t hfile, tb_byte_t const* data, tb_size_t size)
{
	tb_assert_and_check_return_val(hfile, -1);
	DWORD real_size = 0;
	return WriteFile(hfile, data, size, &real_size, NULL)? (tb_long_t)real_size : -1;
}
tb_void_t tb_file_sync(tb_handle_t hfile)
{
	if (hfile) FlushFileBuffers(hfile);
}
tb_hong_t tb_file_seek(tb_handle_t hfile, tb_hong_t offset, tb_size_t flags)
{
	tb_assert_and_check_return_val(hfile, -1);

	DWORD method = 0;
	if (flags & TB_FILE_SEEK_BEG) method = FILE_BEGIN;
	if (flags & TB_FILE_SEEK_CUR) method = FILE_CURRENT;
	if (flags & TB_FILE_SEEK_END) method = FILE_END;

	LARGE_INTEGER o = {0};
	LARGE_INTEGER p = {0};
	o.QuadPart = (LONGLONG)offset;
	return SetFilePointerEx(hfile, o, &p, method)? (tb_hong_t)p.QuadPart : -1;
}
tb_handle_t tb_file_bare(tb_handle_t hfile)
{
	return hfile;
}
tb_hize_t tb_file_size(tb_handle_t hfile)
{
	tb_assert_and_check_return_val(hfile, 0);

	LARGE_INTEGER p = {0};
	return GetFileSizeEx(hfile, &p)? (tb_hong_t)p.QuadPart : 0;
}
tb_bool_t tb_file_create(tb_char_t const* path, tb_size_t type)
{
	tb_assert_and_check_return_val(path, TB_FALSE);
	
	// unix path => windows 
	tb_char_t data[4096];
	path = tb_file_path_to_windows(path, data, 4096);
	tb_assert_and_check_return_val(path, TB_FALSE);

	switch (type)
	{
	case TB_FILE_TYPE_DIR:
		return CreateDirectory(path, NULL)? TB_TRUE : TB_FALSE;
	case TB_FILE_TYPE_FILE:
		{
			HANDLE hfile = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hfile != INVALID_HANDLE_VALUE) 
			{
				CloseHandle(hfile);
				return TB_TRUE;
			}
		}
	default:
		tb_assert(0);
		break;
	}
	return TB_FALSE;
}
tb_void_t tb_file_delete(tb_char_t const* path, tb_size_t type)
{
	tb_assert_and_check_return(path);
	
	// unix path => windows 
	tb_char_t data[4096];
	path = tb_file_path_to_windows(path, data, 4096);
	tb_assert_and_check_return(path);

	switch (type)
	{
	case TB_FILE_TYPE_DIR:
		RemoveDirectory(path);
		break;
	case TB_FILE_TYPE_FILE:
		DeleteFile(path);
		break;
	default:
		tb_assert(0);
		break;
	}
}
tb_bool_t tb_file_info(tb_char_t const* path, tb_file_info_t* info)
{
	tb_assert_and_check_return_val(path, TB_FALSE);

	// unix path => windows 
	tb_char_t data[4096];
	path = tb_file_path_to_windows(path, data, 4096);
	tb_assert_and_check_return_val(path, TB_FALSE);

	// get attributes
	WIN32_FILE_ATTRIBUTE_DATA st = {0};
	if (!GetFileAttributesEx(path, GetFileExInfoStandard, &st)) return TB_FALSE;

	// get info
	if (info)
	{
		// init info
		tb_memset(info, 0, sizeof(tb_file_info_t));

		// file type
		if (st.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) info->type = TB_FILE_TYPE_DIR;
		else if (st.dwFileAttributes != 0xffffffff) info->type = TB_FILE_TYPE_FILE;

		// file size
		info->size = ((tb_hong_t)st.nFileSizeHigh << 32) | st.nFileSizeLow;

		// ok
		return TB_TRUE;
	}

	return TB_FALSE;
}

