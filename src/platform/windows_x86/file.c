/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		file.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../string/string.h"
#include <windows.h>

/* /////////////////////////////////////////////////////////
 * details
 */

/* transform the file path to the unix style
 *
 * C://home/file.txt
 * C:\\\\home\\file.txt
 *
 * => /c/home/file.txt
 */
static tb_char_t const* tb_file_path_to_unix(tb_char_t const* path, tb_char_t* data, tb_size_t maxn)
{
	tb_trace_noimpl();
	return TB_NULL;
}
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
		tb_cstring_ncopy(data + 3, path + 2, maxn - 3);
		//tb_trace("[file]: path: %s => %s", path, data);
		return data;
	}
	else if ( 	path[0] == 'f'
			&& 	path[1] == 'i'
			&& 	path[2] == 'l'
			&& 	path[3] == 'e'
			&& 	path[4] == ':'
			&& 	path[5] == '/'
			&& 	path[6] == '/'
			) 
	{
		data[0] = path[8];
		data[1] = ':';
		data[2] = '/';
		tb_cstring_ncopy(data + 3, path + 9, maxn - 3);
		//tb_trace("[file]: path: %s => %s", path, data);
		return data;
	}
	return TB_NULL;
}

/* /////////////////////////////////////////////////////////
 * implemention
 */

// file
tb_handle_t tb_file_open(tb_char_t const* path, tb_int_t flags)
{
	tb_assert_and_check_return_val(path, TB_NULL);

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
tb_void_t tb_file_close(tb_handle_t hfile)
{
	if (hfile) CloseHandle(hfile);
}
tb_int_t tb_file_read(tb_handle_t hfile, tb_byte_t* data, tb_int_t size)
{
	DWORD real_size = 0;
	if (hfile && ReadFile(hfile, data, size, &real_size, NULL)) return (tb_int_t)real_size;
	return -1;
}
tb_int_t tb_file_write(tb_handle_t hfile, tb_byte_t const* data, tb_int_t size)
{
	DWORD real_size = 0;
	if (hfile && WriteFile(hfile, data, size, &real_size, NULL)) return (tb_int_t)real_size;
	return -1;
}
tb_void_t tb_file_flush(tb_handle_t hfile)
{
	if (hfile) FlushFileBuffers(hfile);
}
tb_int_t tb_file_seek(tb_handle_t hfile, tb_int_t offset, tb_int_t flags)
{
	if (hfile) 
	{
		if (flags == TB_FILE_SEEK_SIZE)
		{
			// FIXME:
			// GetFileSizeEx for 64-bits
			return (tb_int_t)GetFileSize(hfile, NULL);
		}
		else
		{
			DWORD method = 0;
			if (flags & TB_FILE_SEEK_BEG) method = FILE_BEGIN;
			if (flags & TB_FILE_SEEK_CUR) method = FILE_CURRENT;
			if (flags & TB_FILE_SEEK_END) method = FILE_END;

			// FIXME: SetFilePointerEx for 64-bits
			DWORD pos = SetFilePointer(hfile, (LONG)offset, NULL, method);
			return (pos != INVALID_SET_FILE_POINTER? pos : -1);
		}
	}
	return -1;
}
tb_size_t tb_file_size(tb_char_t const* path, tb_file_type_t type)
{
	tb_trace_noimpl();
	return 0;
}
tb_bool_t tb_file_exists(tb_char_t const* path)
{
	tb_assert_and_check_return_val(path, TB_FALSE);

	tb_char_t data[4096];
	path = tb_file_path_to_windows(path, data, 4096);
	tb_assert_and_check_return_val(path, TB_FALSE);

#if 0
	switch (type)
	{
	case TB_FILE_TYPE_DIR:
		{
			DWORD attr = GetFileAttributes(path);
			return ((attr & FILE_ATTRIBUTE_DIRECTORY) && attr != 0xffffffff)? TB_TRUE : TB_FALSE;
		}
	case TB_FILE_TYPE_FILE:
		{
			DWORD attr = GetFileAttributes(path);
			return ((attr & FILE_ATTRIBUTE_DIRECTORY) || attr == 0xffffffff)? TB_FALSE : TB_TRUE;
		}
	default:
		tb_assert(0);
		break;
	}
	return TB_FALSE;
#else
	// FIXME:
	DWORD attr = GetFileAttributes(path);
	return (attr != 0xffffffff)? TB_TRUE : TB_FALSE;
#endif
}
tb_bool_t tb_file_create(tb_char_t const* path, tb_file_type_t type)
{
	tb_assert_and_check_return_val(path, TB_FALSE);
	
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
tb_bool_t tb_file_delete(tb_char_t const* path, tb_file_type_t type)
{
	tb_assert_and_check_return_val(path, TB_FALSE);
	
	tb_char_t data[4096];
	path = tb_file_path_to_windows(path, data, 4096);
	tb_assert_and_check_return_val(path, TB_FALSE);

	switch (type)
	{
	case TB_FILE_TYPE_DIR:
		return RemoveDirectory(path)? TB_TRUE : TB_FALSE;
	case TB_FILE_TYPE_FILE:
		return DeleteFile(path)? TB_TRUE : TB_FALSE;
	default:
		tb_assert(0);
		break;
	}
	return TB_FALSE;
}

// open file list
tb_handle_t tb_file_list_open(tb_char_t const* dir)
{
	tb_trace_noimpl();
	return TB_NULL;
}

// get file list entry, end: return NULL
tb_file_entry_t const* tb_file_list_entry(tb_handle_t hflist)
{
	tb_trace_noimpl();
	return TB_NULL;
}

// close file list
tb_void_t tb_file_list_close(tb_handle_t hflist)
{
	tb_trace_noimpl();
}

