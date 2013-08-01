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
 * @file		file.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../file.h"
#include "../path.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_file_init(tb_char_t const* path, tb_size_t mode)
{
	// check
	tb_assert_and_check_return_val(path, tb_null);

	// the full path
	tb_char_t full[TB_PATH_MAXN];
	path = tb_path_full(path, full, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_null);

	// init access
	DWORD access = GENERIC_READ;
	if (mode & TB_FILE_MODE_RO) access = GENERIC_READ;
	else if (mode & TB_FILE_MODE_WO) access = GENERIC_WRITE;
	else if (mode & TB_FILE_MODE_RW) access = GENERIC_READ | GENERIC_WRITE;

	// init share
	DWORD share = FILE_SHARE_READ;
	if (mode & TB_FILE_MODE_RO) share = FILE_SHARE_READ;
	else if (mode & TB_FILE_MODE_WO) share = FILE_SHARE_WRITE;
	else if (mode & TB_FILE_MODE_RW) share = FILE_SHARE_READ | FILE_SHARE_WRITE;

	// init flag
	DWORD cflag = 0;
	if (mode & (TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC)) cflag |= CREATE_ALWAYS;
	else if (mode & TB_FILE_MODE_CREAT) cflag |= CREATE_NEW;
	else if (mode & TB_FILE_MODE_TRUNC) cflag |= TRUNCATE_EXISTING;
	if (!cflag) cflag |= OPEN_EXISTING;

	// init file
	HANDLE file = CreateFileA(path, access, share, tb_null, cflag, FILE_ATTRIBUTE_NORMAL, tb_null);
	if (file == INVALID_HANDLE_VALUE && (mode & TB_FILE_MODE_CREAT))
	{
		// make directory
		tb_char_t 			temp[TB_PATH_MAXN] = {0};
		tb_char_t const* 	p = full;
		tb_char_t* 			t = temp;
		tb_char_t const* 	e = temp + TB_PATH_MAXN - 1;
		for (; t < e && *p; t++) 
		{
			*t = *p;
			if (*p == '\\' || *p == '/')
			{
				// make directory if not exists
				if (!tb_file_info(temp, tb_null)) CreateDirectoryA(temp, tb_null);

				// skip repeat '\\' or '/'
				while (*p && (*p == '\\' || *p == '/')) p++;
			}
			else p++;
		}

		// init it again
		file = CreateFileA(path, access, share, tb_null, cflag, FILE_ATTRIBUTE_NORMAL, tb_null);
	}

	// append?
	if (file != INVALID_HANDLE_VALUE && (mode & TB_FILE_MODE_APPEND))
	{
		// seek to end
		tb_hize_t size = tb_file_size((tb_handle_t)file);
		if (size) tb_file_seek((tb_handle_t)file, size);
	}

	// ok?
	return file != INVALID_HANDLE_VALUE? (tb_handle_t)file : tb_null;
}
tb_bool_t tb_file_exit(tb_handle_t file)
{
	// check
	tb_assert_and_check_return_val(file, tb_false);

	// close it
	return CloseHandle(file)? tb_true : tb_false;
}
tb_long_t tb_file_read(tb_handle_t file, tb_byte_t* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(file && data, -1);

	// no size
	tb_check_return_val(size, 0);

	// read
	DWORD real_size = 0;
	return ReadFile(file, data, size, &real_size, tb_null)? (tb_long_t)real_size : -1;
}
tb_long_t tb_file_writ(tb_handle_t file, tb_byte_t const* data, tb_size_t size)
{
	// check
	tb_assert_and_check_return_val(file && data, -1);

	// no size
	tb_check_return_val(size, 0);

	// writ
	DWORD real_size = 0;
	return WriteFile(file, data, size, &real_size, tb_null)? (tb_long_t)real_size : -1;
}
tb_void_t tb_file_sync(tb_handle_t file)
{
	if (file) FlushFileBuffers(file);
}
tb_bool_t tb_file_seek(tb_handle_t file, tb_hize_t offset)
{
	// check
	tb_assert_and_check_return_val(file, tb_false);

	// seek
	LARGE_INTEGER o = {0};
	LARGE_INTEGER p = {0};
	o.QuadPart = (LONGLONG)offset;
	return SetFilePointerEx(file, o, &p, FILE_BEGIN)? tb_true : tb_false;
}
tb_bool_t tb_file_skip(tb_handle_t file, tb_hize_t size)
{
	// check
	tb_assert_and_check_return_val(file, tb_false);

	// skip
	LARGE_INTEGER o = {0};
	LARGE_INTEGER p = {0};
	o.QuadPart = (LONGLONG)size;
	return SetFilePointerEx(file, o, &p, FILE_CURRENT)? tb_true : tb_false;
}
tb_hize_t tb_file_size(tb_handle_t file)
{
	// check
	tb_assert_and_check_return_val(file, 0);

	// the file size
	LARGE_INTEGER p = {0};
	return GetFileSizeEx(file, &p)? (tb_hong_t)p.QuadPart : 0;
}
tb_bool_t tb_file_info(tb_char_t const* path, tb_file_info_t* info)
{
	// check
	tb_assert_and_check_return_val(path, tb_false);

	// the full path
	tb_char_t full[TB_PATH_MAXN];
	path = tb_path_full(path, full, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// get attributes
	WIN32_FILE_ATTRIBUTE_DATA st = {0};
	if (!GetFileAttributesExA(path, GetFileExInfoStandard, &st)) return tb_false;

	// get info
	if (info)
	{
		// init info
		tb_memset(info, 0, sizeof(tb_file_info_t));

		// file type
		if (st.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) info->type = TB_FILE_TYPE_DIRECTORY;
		else if (st.dwFileAttributes != 0xffffffff) info->type = TB_FILE_TYPE_FILE;

		// file size
		info->size = ((tb_hize_t)st.nFileSizeHigh << 32) | (tb_hize_t)st.nFileSizeLow;

		// the last access time
		info->atime = tb_filetime_to_time(st.ftLastAccessTime);

		// the last modify time
		info->mtime = tb_filetime_to_time(st.ftLastWriteTime);
	}

	// ok
	return tb_true;
}
tb_bool_t tb_file_copy(tb_char_t const* path, tb_char_t const* dest)
{
	// check
	tb_assert_and_check_return_val(path && dest, tb_false);

	// the full path
	tb_char_t full0[TB_PATH_MAXN];
	path = tb_path_full(path, full0, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// the dest path
	tb_char_t full1[TB_PATH_MAXN];
	dest = tb_path_full(dest, full1, TB_PATH_MAXN);
	tb_assert_and_check_return_val(dest, tb_false);

	// copy
	return CopyFileA(path, dest, FALSE)? tb_true : tb_false;
}
tb_bool_t tb_file_create(tb_char_t const* path)
{
	// check
	tb_assert_and_check_return_val(path, tb_false);

	// make it
	tb_handle_t file = tb_file_init(path, TB_FILE_MODE_CREAT | TB_FILE_MODE_WO | TB_FILE_MODE_TRUNC);
	if (file) tb_file_exit(file);

	// ok?
	return file? tb_true : tb_false;
}
tb_bool_t tb_file_remove(tb_char_t const* path)
{
	// check
	tb_assert_and_check_return_val(path, tb_false);
	
	// the full path
	tb_char_t full[TB_PATH_MAXN];
	path = tb_path_full(path, full, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// remote it
	return DeleteFileA(path)? tb_true : tb_false;
}
tb_bool_t tb_file_rename(tb_char_t const* path, tb_char_t const* dest)
{
	// check
	tb_assert_and_check_return_val(path && dest, tb_false);
	
	// the full path
	tb_char_t full0[TB_PATH_MAXN];
	path = tb_path_full(path, full0, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// the dest path
	tb_char_t full1[TB_PATH_MAXN];
	dest = tb_path_full(dest, full1, TB_PATH_MAXN);
	tb_assert_and_check_return_val(dest, tb_false);

	// rename it
	return MoveFileExA(path, dest, MOVEFILE_REPLACE_EXISTING);
}
tb_bool_t tb_file_link(tb_char_t const* path, tb_char_t const* dest)
{
#if 0
	// check
	tb_assert_and_check_return_val(path && dest, tb_false);

	// the full path
	tb_char_t full0[TB_PATH_MAXN];
	path = tb_path_full(path, full0, TB_PATH_MAXN);
	tb_assert_and_check_return_val(path, tb_false);

	// the dest path
	tb_char_t full1[TB_PATH_MAXN];
	dest = tb_path_full(dest, full1, TB_PATH_MAXN);
	tb_assert_and_check_return_val(dest, tb_false);

	// not exists?
	tb_file_info_t info = {0};
	if (!tb_file_info(path, &info)) return tb_false;

	// symlink, supported: >= vista
	return !CreateSymbolicLinkA(dest, path, info.bdir? SYMBOLIC_LINK_FLAG_DIRECTORY : 0)? tb_true : tb_false;
#else
	tb_trace_noimpl();
	return tb_false;
#endif
}
