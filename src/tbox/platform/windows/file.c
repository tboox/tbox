/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        file.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../file.h"
#include "../path.h"
#include "../print.h"
#include "interface/interface.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifndef SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE
#   define SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE (0x2)
#endif

#ifndef SYMBOLIC_LINK_FLAG_DIRECTORY
#   define SYMBOLIC_LINK_FLAG_DIRECTORY                 (0x1)
#endif

// https://github.com/xmake-io/xmake/discussions/5821
#ifndef COPY_FILE_COPY_SYMLINK
#   define COPY_FILE_COPY_SYMLINK 0x800
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_void_t tb_file_mkdir(tb_wchar_t const* path)
{
    // make directory
    tb_wchar_t          temp[TB_PATH_MAXN] = {0};
    tb_wchar_t const*   p = path;
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
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_file_ref_t tb_file_init(tb_char_t const* path, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(path, tb_null);

    // the full path
    tb_wchar_t full[TB_PATH_MAXN];
    if (!tb_path_absolute_w(path, full, TB_PATH_MAXN)) return tb_null;

    // init access
    DWORD access = GENERIC_READ;
    if (mode & TB_FILE_MODE_RO) access = GENERIC_READ;
    else if (mode & TB_FILE_MODE_WO) access = GENERIC_WRITE;
    else if (mode & TB_FILE_MODE_RW) access = GENERIC_READ | GENERIC_WRITE;

    // init share
    DWORD share = FILE_SHARE_READ | FILE_SHARE_WRITE;

    // init flag
    DWORD cflag = 0;
    if (mode & TB_FILE_MODE_CREAT)
    {
        // always create a new empty file
        if (mode & TB_FILE_MODE_TRUNC) cflag |= CREATE_ALWAYS;
        // create or open and append file
        else if (mode & TB_FILE_MODE_APPEND) cflag |= OPEN_ALWAYS;
        // create a new file only if file not exists
        else cflag |= CREATE_NEW;
    }
    // open and truncate an existing file
    else if (mode & TB_FILE_MODE_TRUNC) cflag |= TRUNCATE_EXISTING;
    // open an existing file
    if (!cflag) cflag |= OPEN_EXISTING;

    // init attr
    DWORD attr = FILE_ATTRIBUTE_NORMAL;
    if (mode & TB_FILE_MODE_DIRECT) attr |= FILE_FLAG_NO_BUFFERING;

    // init file
    HANDLE file = CreateFileW(full, access, share, tb_null, cflag, attr, tb_null);
    if (file == INVALID_HANDLE_VALUE && (mode & TB_FILE_MODE_CREAT))
    {
        // make directory
        tb_file_mkdir(full);

        // init it again
        file = CreateFileW(full, access, share, tb_null, cflag, attr, tb_null);
    }

    // append?
    if (file != INVALID_HANDLE_VALUE && (mode & TB_FILE_MODE_APPEND))
    {
        // seek to end
        tb_hize_t size = tb_file_size((tb_file_ref_t)file);
        if (size)
        {
            if (size != tb_file_seek((tb_file_ref_t)file, size, TB_FILE_SEEK_BEG))
            {
                tb_file_exit((tb_file_ref_t)file);
                file = INVALID_HANDLE_VALUE;
            }
        }
    }

    // ok?
    return file != INVALID_HANDLE_VALUE? (tb_file_ref_t)file : tb_null;
}
tb_bool_t tb_file_exit(tb_file_ref_t file)
{
    // check
    tb_assert_and_check_return_val(file, tb_false);

    // close it
    return CloseHandle((HANDLE)file)? tb_true : tb_false;
}
tb_long_t tb_file_read(tb_file_ref_t file, tb_byte_t* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(file && data, -1);
    tb_check_return_val(size, 0);

    // read
    DWORD real_size = 0;
    if (ReadFile((HANDLE)file, data, (DWORD)size, &real_size, tb_null))
        return (tb_long_t)real_size;
    return -1;
}
tb_long_t tb_file_writ(tb_file_ref_t file, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(file && data, -1);
    tb_check_return_val(size, 0);

    // write
    DWORD real_size = 0;
    if (WriteFile((HANDLE)file, data, (DWORD)size, &real_size, tb_null))
        return (tb_long_t)real_size;
    return -1;
}
tb_long_t tb_file_pread(tb_file_ref_t file, tb_byte_t* data, tb_size_t size, tb_hize_t offset)
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
tb_long_t tb_file_pwrit(tb_file_ref_t file, tb_byte_t const* data, tb_size_t size, tb_hize_t offset)
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
tb_long_t tb_file_readv(tb_file_ref_t file, tb_iovec_t const* list, tb_size_t size)
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
tb_long_t tb_file_writv(tb_file_ref_t file, tb_iovec_t const* list, tb_size_t size)
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
tb_hong_t tb_file_writf(tb_file_ref_t file, tb_file_ref_t ifile, tb_hize_t offset, tb_hize_t size)
{
    // check
    tb_assert_and_check_return_val(file && ifile && size, -1);

    // read data
    tb_byte_t data[8192];
    tb_long_t read = tb_file_pread(ifile, data, sizeof(data), offset);
    tb_check_return_val(read > 0, read);

    // writ data
    tb_long_t writ = 0;
    while (writ < read)
    {
        tb_long_t real = tb_file_writ(file, data + writ, read - writ);
        if (real > 0) writ += real;
        else break;
    }

    // ok?
    return writ == read? writ : -1;
}
tb_long_t tb_file_preadv(tb_file_ref_t file, tb_iovec_t const* list, tb_size_t size, tb_hize_t offset)
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
tb_long_t tb_file_pwritv(tb_file_ref_t file, tb_iovec_t const* list, tb_size_t size, tb_hize_t offset)
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
tb_bool_t tb_file_sync(tb_file_ref_t file)
{
    // check
    tb_assert_and_check_return_val(file, tb_false);

    // sync it
    return FlushFileBuffers((HANDLE)file)? tb_true : tb_false;
}
tb_hong_t tb_file_seek(tb_file_ref_t file, tb_hong_t offset, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(file, -1);

    // seek
    LARGE_INTEGER o = {{0}};
    LARGE_INTEGER p = {{0}};
    o.QuadPart = (LONGLONG)offset;
    return SetFilePointerEx((HANDLE)file, o, &p, (DWORD)mode)? (tb_hong_t)p.QuadPart : -1;
}
tb_hong_t tb_file_offset(tb_file_ref_t file)
{
    // check
    tb_assert_and_check_return_val(file, -1);

    // the file size
    return tb_file_seek(file, (tb_hong_t)0, TB_FILE_SEEK_CUR);
}
tb_hize_t tb_file_size(tb_file_ref_t file)
{
    // check
    tb_assert_and_check_return_val(file, 0);

    // the GetFileSizeEx func
    tb_kernel32_GetFileSizeEx_t pGetFileSizeEx = tb_kernel32()->GetFileSizeEx;
    tb_assert_and_check_return_val(pGetFileSizeEx, 0);

    // the file size
    LARGE_INTEGER p = {{0}};
    return pGetFileSizeEx((HANDLE)file, &p)? (tb_hong_t)p.QuadPart : 0;
}
tb_bool_t tb_file_info(tb_char_t const* path, tb_file_info_t* info)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // the full path
    tb_wchar_t full[TB_PATH_MAXN];
    if (!tb_path_absolute_w(path, full, TB_PATH_MAXN)) return tb_false;

    // get attributes
    WIN32_FILE_ATTRIBUTE_DATA st = {0};
    if (!GetFileAttributesExW(full, GetFileExInfoStandard, &st)) return tb_false;

    // get info
    if (info)
    {
        // init info
        tb_memset(info, 0, sizeof(tb_file_info_t));

        // get file type
        if (st.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) info->type = TB_FILE_TYPE_DIRECTORY;
        else if (st.dwFileAttributes != 0xffffffff) info->type = TB_FILE_TYPE_FILE;

        // is symlink?
        info->flags = TB_FILE_FLAG_NONE;
        if (st.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
            info->flags |= TB_FILE_FLAG_LINK;

        // file size
        info->size = ((tb_hize_t)st.nFileSizeHigh << 32) | (tb_hize_t)st.nFileSizeLow;

        // the last access time
        info->atime = tb_filetime_to_time(&st.ftLastAccessTime);

        // the last modify time
        info->mtime = tb_filetime_to_time(&st.ftLastWriteTime);
    }

    // ok
    return tb_true;
}
tb_bool_t tb_file_copy(tb_char_t const* path, tb_char_t const* dest, tb_size_t flags)
{
    // check
    tb_assert_and_check_return_val(path && dest, tb_false);

    // the full path
    tb_wchar_t full0[TB_PATH_MAXN];
    if (!tb_path_absolute_w(path, full0, TB_PATH_MAXN)) return tb_false;

    // the dest path
    tb_wchar_t full1[TB_PATH_MAXN];
    if (!tb_path_absolute_w(dest, full1, TB_PATH_MAXN)) return tb_false;

    // copy link
    tb_file_info_t info = {0};
    if (flags & TB_FILE_COPY_LINK && tb_file_info(path, &info) && info.flags & TB_FILE_FLAG_LINK)
    {
        tb_file_mkdir(full1);
        if (tb_kernel32()->CopyFileExW && tb_kernel32()->CopyFileExW(full0, full1, tb_null, tb_null, FALSE, COPY_FILE_COPY_SYMLINK))
            return tb_true;

        // we should read file content to copy it
        tb_bool_t ok = tb_false;
        tb_file_ref_t ifile = tb_file_init(path, TB_FILE_MODE_RW);
        tb_file_ref_t ofile = tb_file_init(dest, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_TRUNC);
        if (ifile && ofile)
        {
            tb_hize_t writ = 0;
            tb_hize_t size = tb_file_size(ifile);
            while (writ < size)
            {
                tb_hong_t real = tb_file_writf(ofile, ifile, writ, size - writ);
                if (real > 0) writ += real;
                else break;
            }
            if (writ == size) ok = tb_true;
        }

        // exit file
        if (ifile) tb_file_exit(ifile);
        if (ofile) tb_file_exit(ofile);
        return ok;
    }

    // copy it
    if (!CopyFileW(full0, full1, FALSE))
    {
        tb_file_mkdir(full1);
        return (tb_bool_t)CopyFileW(full0, full1, FALSE);
    }
    return tb_true;
}
tb_bool_t tb_file_create(tb_char_t const* path)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // make it
    tb_file_ref_t file = tb_file_init(path, TB_FILE_MODE_CREAT | TB_FILE_MODE_WO | TB_FILE_MODE_TRUNC);
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
    if (!tb_path_absolute_w(path, full, TB_PATH_MAXN)) return tb_false;

    // remove readonly first
    DWORD attrs = GetFileAttributesW(full);
    if (attrs & FILE_ATTRIBUTE_READONLY)
        SetFileAttributesW(full, attrs & ~FILE_ATTRIBUTE_READONLY);

    // remove it
    return DeleteFileW(full)? tb_true : tb_false;
}
tb_bool_t tb_file_rename(tb_char_t const* path, tb_char_t const* dest)
{
    // check
    tb_assert_and_check_return_val(path && dest, tb_false);

    // the full path
    tb_wchar_t full0[TB_PATH_MAXN];
    if (!tb_path_absolute_w(path, full0, TB_PATH_MAXN)) return tb_false;

    // the dest path
    tb_wchar_t full1[TB_PATH_MAXN];
    if (!tb_path_absolute_w(dest, full1, TB_PATH_MAXN)) return tb_false;

    // rename it
    DWORD flags = MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH;
    if (MoveFileExW(full0, full1, flags)) return tb_true;

    // try to create directories and rename it again
    tb_file_mkdir(full1);
    if (MoveFileExW(full0, full1, flags)) return tb_true;

    // MoveFileExW will fail if it crosses drive, we can use copy/delete to rename file
    if (CopyFileW(full0, full1, FALSE))
    {
        DWORD attrs = GetFileAttributesW(full0);
        if (attrs & FILE_ATTRIBUTE_READONLY)
            SetFileAttributesW(full0, attrs & ~FILE_ATTRIBUTE_READONLY);
        return DeleteFileW(full0);
    }
    return tb_false;
}
tb_bool_t tb_file_link(tb_char_t const* path, tb_char_t const* dest)
{
    // check
    tb_assert_and_check_return_val(path && dest, tb_false);

    // support symbolic link? >= vista
    tb_kernel32_CreateSymbolicLinkW_t pCreateSymbolicLinkW = tb_kernel32()->CreateSymbolicLinkW;
    tb_check_return_val(pCreateSymbolicLinkW, tb_false);

    // not exists?
    tb_file_info_t info = {0};
    if (!tb_file_info(path, &info)) return tb_false;

    /* we only translate to wchar path
     * @note we should not use absolute path, dest -> path (may be relative path)
     */
    tb_wchar_t path0[TB_PATH_MAXN];
    if (tb_atow(path0, path, TB_PATH_MAXN) == (tb_size_t)-1) return tb_false;

    // the dest path
    tb_wchar_t full1[TB_PATH_MAXN];
    if (!tb_path_absolute_w(dest, full1, TB_PATH_MAXN)) return tb_false;

    // make directory
    tb_file_mkdir(full1);

    // attempt to link it directly without admin privilege.
    tb_bool_t isdir = (info.type == TB_FILE_TYPE_DIRECTORY);
    if (pCreateSymbolicLinkW(full1, path0, (isdir? SYMBOLIC_LINK_FLAG_DIRECTORY : 0) | SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE))
        return tb_true;

    // attempt to link it directly with admin privilege
    return (tb_bool_t)pCreateSymbolicLinkW(full1, path0, isdir? SYMBOLIC_LINK_FLAG_DIRECTORY : 0);
}
tb_bool_t tb_file_access(tb_char_t const* path, tb_size_t mode)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // the full path
    tb_wchar_t full[TB_PATH_MAXN];
    if (!tb_path_absolute_w(path, full, TB_PATH_MAXN)) return tb_false;

    // init permission
    DWORD perm = FILE_TRAVERSE | SYNCHRONIZE;
    if (mode & TB_FILE_MODE_RW) perm = GENERIC_READ | GENERIC_WRITE;
    else
    {
        if (mode & TB_FILE_MODE_RO) perm |= GENERIC_READ;
        if (mode & TB_FILE_MODE_WO) perm |= GENERIC_WRITE;
    }

    /* The windows POSIX implementation: _access_s and _waccess_s don't work well.
     * For example, _access_s reports the folder "C:\System Volume Information" can be accessed.
     * So using the win32 API "CreateFile" here which works much better.
     */
    HANDLE h = CreateFileW(full, perm, FILE_SHARE_READ | FILE_SHARE_WRITE, tb_null, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, tb_null);
    tb_check_return_val(h != INVALID_HANDLE_VALUE, tb_false);
    CloseHandle(h);
    return tb_true;
}
tb_bool_t tb_file_touch(tb_char_t const* path, tb_time_t atime, tb_time_t mtime)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // the full path
    tb_wchar_t full[TB_PATH_MAXN];
    if (!tb_path_absolute_w(path, full, TB_PATH_MAXN)) return tb_false;

    // get file info
    tb_bool_t ok = tb_false;
    HANDLE file = tb_null;
    WIN32_FILE_ATTRIBUTE_DATA st = {0};
    if (GetFileAttributesExW(full, GetFileExInfoStandard, &st))
    {
        if (st.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            file = CreateFileW(full, GENERIC_WRITE, FILE_SHARE_READ, tb_null, OPEN_EXISTING, FILE_ATTRIBUTE_DIRECTORY | FILE_FLAG_BACKUP_SEMANTICS, tb_null);
        else if (st.dwFileAttributes != 0xffffffff)
            file = CreateFileW(full, GENERIC_WRITE, FILE_SHARE_READ, tb_null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, tb_null);
    }
    // we create an empty file if it does not exist
    else file = CreateFileW(full, GENERIC_WRITE, FILE_SHARE_READ, tb_null, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, tb_null);
    if (file && file != INVALID_HANDLE_VALUE)
    {
        if (atime > 0 || mtime > 0)
        {
            FILETIME atime_ft, mtime_ft;
            if (atime > 0) tb_time_to_filetime(atime, &atime_ft);
            else
            {
                atime_ft.dwLowDateTime = 0xffffffff;
                atime_ft.dwHighDateTime = 0xffffffff;
            }
            if (mtime > 0) tb_time_to_filetime(mtime, &mtime_ft);
            else
            {
                mtime_ft.dwLowDateTime = 0;
                mtime_ft.dwHighDateTime = 0;
            }
            ok = SetFileTime(file, tb_null, &atime_ft, &mtime_ft);
        }
        else ok = tb_true;
        CloseHandle(file);
    }
    return ok;
}
