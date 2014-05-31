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
 * @author      ruki
 * @file        file.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "api.h"
#include "../file.h"
#include "../path.h"
#include "../print.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_file_init(tb_char_t const* path, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(path, tb_null);

    // the full path
    tb_wchar_t full[TB_PATH_MAXN];
    if (!tb_path_full_w(path, full, TB_PATH_MAXN)) return tb_null;

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

    // init attr
    DWORD attr = FILE_ATTRIBUTE_NORMAL;
    if (mode & TB_FILE_MODE_AICP) attr |= FILE_FLAG_OVERLAPPED;
    if (mode & TB_FILE_MODE_DIRECT) attr |= FILE_FLAG_NO_BUFFERING;

    // init file
    HANDLE file = CreateFileW(full, access, share, tb_null, cflag, attr, tb_null);
    if (file == INVALID_HANDLE_VALUE && (mode & TB_FILE_MODE_CREAT))
    {
        // make directory
        tb_wchar_t          temp[TB_PATH_MAXN] = {0};
        tb_wchar_t const*   p = full;
        tb_wchar_t*         t = temp;
        tb_wchar_t const*   e = temp + TB_PATH_MAXN - 1;
        for (; t < e && *p; t++) 
        {
            *t = *p;
            if (*p == L'\\' || *p == L'/')
            {
                // make directory if not exists
                if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(temp)) CreateDirectoryW(temp, tb_null);

                // skip repeat '\\' or '/'
                while (*p && (*p == L'\\' || *p == L'/')) p++;
            }
            else p++;
        }

        // init it again
        file = CreateFileW(full, access, share, tb_null, cflag, attr, tb_null);
    }

    // append?
    if (file != INVALID_HANDLE_VALUE && (mode & TB_FILE_MODE_APPEND))
    {
        // seek to end
        tb_hize_t size = tb_file_size((tb_handle_t)file);
        if (size) 
        {
            if (size != tb_file_seek((tb_handle_t)file, size, TB_FILE_SEEK_BEG)) 
            {
                tb_file_exit((tb_handle_t)file);
                file = INVALID_HANDLE_VALUE;
            }
        }
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
tb_long_t tb_file_pread(tb_handle_t file, tb_byte_t* data, tb_size_t size, tb_hize_t offset)
{
    // check
    tb_assert_and_check_return_val(file && data, -1);

    // save offset
    tb_hong_t current = tb_file_offset(file);
    tb_assert_and_check_return_val(current >= 0, -1);

    // seek it
    if (current != offset && tb_file_seek(file, offset, TB_FILE_SEEK_BEG) != offset) return -1;

    // read it
    tb_long_t real = tb_file_read(file, data, size);

    // restore offset
    if (current != offset && tb_file_seek(file, current, TB_FILE_SEEK_BEG) != current) return -1;

    // ok
    return real;
}
tb_long_t tb_file_pwrit(tb_handle_t file, tb_byte_t const* data, tb_size_t size, tb_hize_t offset)
{
    // check
    tb_assert_and_check_return_val(file && data, -1);

    // save offset
    tb_hong_t current = tb_file_offset(file);
    tb_assert_and_check_return_val(current >= 0, -1);

    // seek it
    if (current != offset && tb_file_seek(file, offset, TB_FILE_SEEK_BEG) != offset) return -1;

    // writ it
    tb_long_t real = tb_file_writ(file, data, size);

    // restore offset
    if (current != offset && tb_file_seek(file, current, TB_FILE_SEEK_BEG) != current) return -1;

    // ok
    return real;
}
tb_long_t tb_file_readv(tb_handle_t file, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(file && list && size, -1);

    // walk read
    tb_size_t i = 0;
    tb_size_t read = 0;
    for (i = 0; i < size; i++)
    {
        // the data & size
        tb_byte_t*  data = list[i].data;
        tb_size_t   need = list[i].size;
        tb_check_break(data && need);

        // read it
        tb_long_t real = tb_file_read(file, data, need);

        // full? next it
        if (real == need)
        {
            read += real;
            continue ;
        }

        // failed?
        tb_check_return_val(real >= 0, -1);

        // ok?
        if (real > 0) read += real;

        // end
        break;
    }

    // ok?
    return read;
}
tb_long_t tb_file_writv(tb_handle_t file, tb_iovec_t const* list, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(file && list && size, -1);

    // walk writ
    tb_size_t i = 0;
    tb_size_t writ = 0;
    for (i = 0; i < size; i++)
    {
        // the data & size
        tb_byte_t*  data = list[i].data;
        tb_size_t   need = list[i].size;
        tb_check_break(data && need);

        // writ it
        tb_long_t real = tb_file_writ(file, data, need);

        // full? next it
        if (real == need)
        {
            writ += real;
            continue ;
        }

        // failed?
        tb_check_return_val(real >= 0, -1);

        // ok?
        if (real > 0) writ += real;

        // end
        break;
    }

    // ok?
    return writ;
}
tb_hong_t tb_file_writf(tb_handle_t file, tb_handle_t ifile, tb_hize_t offset, tb_hize_t size)
{
    // check
    tb_assert_and_check_return_val(file && ifile && size, -1);

    // read data
    tb_byte_t data[8192];
    tb_long_t read = tb_file_pread(ifile, data, sizeof(data), offset);
    tb_check_return_val(read > 0, read);

    // writ data
    tb_size_t writ = 0;
    while (writ < read)
    {
        tb_long_t real = tb_file_writ(file, data + writ, read - writ);
        if (real > 0) writ += real;
        else break;
    }

    // ok?
    return writ == read? writ : -1;
}
tb_long_t tb_file_preadv(tb_handle_t file, tb_iovec_t const* list, tb_size_t size, tb_hize_t offset)
{
    // check
    tb_assert_and_check_return_val(file && list && size, -1);

    // save offset
    tb_hong_t current = tb_file_offset(file);
    tb_assert_and_check_return_val(current >= 0, -1);

    // seek it
    if (current != offset && tb_file_seek(file, offset, TB_FILE_SEEK_BEG) != offset) return -1;

    // read it
    tb_long_t real = tb_file_readv(file, list, size);

    // restore offset
    if (current != offset && tb_file_seek(file, current, TB_FILE_SEEK_BEG) != current) return -1;

    // ok
    return real;
}
tb_long_t tb_file_pwritv(tb_handle_t file, tb_iovec_t const* list, tb_size_t size, tb_hize_t offset)
{
    // check
    tb_assert_and_check_return_val(file && list && size, -1);

    // save offset
    tb_hong_t current = tb_file_offset(file);
    tb_assert_and_check_return_val(current >= 0, -1);

    // seek it
    if (current != offset && tb_file_seek(file, offset, TB_FILE_SEEK_BEG) != offset) return -1;

    // writ it
    tb_long_t real = tb_file_writv(file, list, size);

    // restore offset
    if (current != offset && tb_file_seek(file, current, TB_FILE_SEEK_BEG) != current) return -1;

    // ok
    return real;
}
tb_bool_t tb_file_sync(tb_handle_t file)
{
    // check
    tb_assert_and_check_return_val(file, tb_false);

    // sync it
    return FlushFileBuffers(file)? tb_true : tb_false;
}
tb_hong_t tb_file_seek(tb_handle_t file, tb_hong_t offset, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(file, -1);

    // seek
    LARGE_INTEGER o = {{0}};
    LARGE_INTEGER p = {{0}};
    o.QuadPart = (LONGLONG)offset;
    return SetFilePointerEx(file, o, &p, mode)? (tb_hong_t)p.QuadPart : -1;
}
tb_hong_t tb_file_offset(tb_handle_t file)
{
    // check
    tb_assert_and_check_return_val(file, -1);

    // the file size
    return tb_file_seek(file, (tb_hong_t)0, TB_FILE_SEEK_CUR);
}
tb_hize_t tb_file_size(tb_handle_t file)
{
    // check
    tb_assert_and_check_return_val(file, 0);

    // the GetFileSizeEx func
    tb_api_GetFileSizeEx_t pGetFileSizeEx = tb_api_GetFileSizeEx();
    tb_assert_and_check_return_val(pGetFileSizeEx, 0);

    // the file size
    LARGE_INTEGER p = {{0}};
    return pGetFileSizeEx(file, &p)? (tb_hong_t)p.QuadPart : 0;
}
tb_bool_t tb_file_info(tb_char_t const* path, tb_file_info_t* info)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // the full path
    tb_wchar_t full[TB_PATH_MAXN];
    if (!tb_path_full_w(path, full, TB_PATH_MAXN)) return tb_false;

    // get attributes
    WIN32_FILE_ATTRIBUTE_DATA st = {0};
    if (!GetFileAttributesExW(full, GetFileExInfoStandard, &st)) return tb_false;

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
    tb_wchar_t full0[TB_PATH_MAXN];
    if (!tb_path_full_w(path, full0, TB_PATH_MAXN)) return tb_false;

    // the dest path
    tb_wchar_t full1[TB_PATH_MAXN];
    if (!tb_path_full_w(dest, full1, TB_PATH_MAXN)) return tb_false;

    // copy
    return CopyFileW(full0, full1, FALSE)? tb_true : tb_false;
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
    tb_wchar_t full[TB_PATH_MAXN];
    if (!tb_path_full_w(path, full, TB_PATH_MAXN)) return tb_false;

    // remote it
    return DeleteFileW(full)? tb_true : tb_false;
}
tb_bool_t tb_file_rename(tb_char_t const* path, tb_char_t const* dest)
{
    // check
    tb_assert_and_check_return_val(path && dest, tb_false);
    
    // the full path
    tb_wchar_t full0[TB_PATH_MAXN];
    if (!tb_path_full_w(path, full0, TB_PATH_MAXN)) return tb_false;

    // the dest path
    tb_wchar_t full1[TB_PATH_MAXN];
    if (!tb_path_full_w(dest, full1, TB_PATH_MAXN)) return tb_false;

    // rename it
    return MoveFileExW(full0, full1, MOVEFILE_REPLACE_EXISTING);
}
tb_bool_t tb_file_link(tb_char_t const* path, tb_char_t const* dest)
{
#if 0
    // check
    tb_assert_and_check_return_val(path && dest, tb_false);

    // the full path
    tb_wchar_t full0[TB_PATH_MAXN];
    if (!tb_path_full_w(path, full0, TB_PATH_MAXN)) return tb_false;

    // the dest path
    tb_wchar_t full1[TB_PATH_MAXN];
    if (!tb_path_full_w(dest, full1, TB_PATH_MAXN)) return tb_false;

    // not exists?
    tb_file_info_t info = {0};
    if (!tb_file_info(full0, &info)) return tb_false;

    // symlink, supported: >= vista
    return !CreateSymbolicLinkW(full1, full0, info.bdir? SYMBOLIC_LINK_FLAG_DIRECTORY : 0)? tb_true : tb_false;
#else
    tb_trace_noimpl();
    return tb_false;
#endif
}
