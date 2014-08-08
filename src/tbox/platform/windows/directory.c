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
 * @file        directory.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../file.h"
#include "../path.h"
#include "../print.h"
#include "../directory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * callback
 */
static tb_void_t tb_directory_walk_remove(tb_char_t const* path, tb_file_info_t const* info, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(path && info);

    // remove file
    if (info->type == TB_FILE_TYPE_FILE) tb_file_remove(path);
    // remvoe directory
    else if (info->type == TB_FILE_TYPE_DIRECTORY)
    {
        tb_wchar_t temp[TB_PATH_MAXN] = {0};
        if (tb_atow(temp, path, TB_PATH_MAXN))
            RemoveDirectoryW(temp);
    }
}
static tb_void_t tb_directory_walk_copy(tb_char_t const* path, tb_file_info_t const* info, tb_cpointer_t priv)
{
    // check
    tb_value_t* tuple = (tb_value_t*)priv;
    tb_assert_and_check_return(path && info && tuple);

    // the dest directory
    tb_char_t const* dest = tuple[0].cstr;
    tb_assert_and_check_return(dest);

    // the file name
    tb_size_t size = tuple[1].ul;
    tb_char_t const* name = path + size;

    // the dest file path
    tb_char_t dpath[8192] = {0};
    tb_snprintf(dpath, 8192, "%s\\%s", dest, name[0] == '\\'? name + 1 : name);
//  tb_trace_i("%s => %s", path, dpath);

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
        if (!tb_file_copy(path, dpath)) tuple[2].b = tb_false;
        break;
    case TB_FILE_TYPE_DIRECTORY:
        if (!tb_directory_create(dpath)) tuple[2].b = tb_false;
        break;
    default:
        break;
    }
}
static tb_void_t tb_directory_walk_impl(tb_wchar_t const* path, tb_bool_t recursion, tb_bool_t prefix, tb_directory_walk_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(path && func);

    // last
    tb_long_t           last = tb_wcslen(path) - 1;
    tb_assert_and_check_return(last >= 0);

    // add \*.*
    tb_wchar_t          temp_w[4096] = {0};
    tb_char_t           temp_a[4096] = {0};
    tb_swprintf(temp_w, 4095, L"%s%s*.*", path, path[last] == L'\\'? L"" : L"\\");

    // init info
    WIN32_FIND_DATAW    find = {0};
    HANDLE              directory = INVALID_HANDLE_VALUE;
    if (INVALID_HANDLE_VALUE != (directory = FindFirstFileW(temp_w, &find)))
    {
        // walk
        do
        {
            // check
            if (tb_wcscmp(find.cFileName, L".") && tb_wcscmp(find.cFileName, L".."))
            {
                // the temp path
                tb_long_t n = tb_swprintf(temp_w, 4095, L"%s%s%s", path, path[last] == L'\\'? L"" : L"\\", find.cFileName);
                if (n >= 0 && n < 4096) temp_w[n] = L'\0';

                // wtoa temp
                n = tb_wtoa(temp_a, temp_w, 4095);
                if (n >= 0 && n < 4096) temp_a[n] = '\0';

                // the file info
                tb_file_info_t info = {0};
                if (tb_file_info(temp_a, &info))
                {
                    // do callback
                    if (prefix) func(temp_a, &info, priv);

                    // walk to the next directory
                    if (info.type == TB_FILE_TYPE_DIRECTORY && recursion) tb_directory_walk_impl(temp_w, recursion, prefix, func, priv);
    
                    // do callback
                    if (!prefix) func(temp_a, &info, priv);
                }
            }

        } while (FindNextFileW(directory, &find));

        // exit directory
        FindClose(directory);
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_directory_create(tb_char_t const* path)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // the full path
    tb_wchar_t full[TB_PATH_MAXN];
    if (!tb_path_full_w(path, full, TB_PATH_MAXN)) return tb_false;

    // make it
    tb_bool_t ok = CreateDirectoryW(full, tb_null)? tb_true : tb_false;
    if (!ok)
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

        // make it again
        ok = CreateDirectoryW(full, tb_null)? tb_true : tb_false;
    }

    // ok?
    return ok;
}
tb_bool_t tb_directory_remove(tb_char_t const* path)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // the full path
    tb_wchar_t full[TB_PATH_MAXN];
    if (!tb_path_full_w(path, full, TB_PATH_MAXN)) return tb_false;

    // walk remove
    tb_directory_walk_impl(full, tb_true, tb_false, tb_directory_walk_remove, tb_null);

    // remove it
    return RemoveDirectoryW(full)? tb_true : tb_false;
}
tb_size_t tb_directory_temp(tb_char_t* path, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(path && maxn > 4, 0);

    // the temporary directory
    tb_wchar_t  temp[TB_PATH_MAXN] = {0};
    GetTempPathW(TB_PATH_MAXN, temp);

    // wtoa
    return tb_wtoa(path, temp, maxn);
}
tb_size_t tb_directory_curt(tb_char_t* path, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(path && maxn > 4, 0);

    // the current directory
    tb_wchar_t  curt[TB_PATH_MAXN] = {0};
    GetCurrentDirectoryW(TB_PATH_MAXN, curt);

    // wtoa
    return tb_wtoa(path, curt, maxn);
}
tb_void_t tb_directory_walk(tb_char_t const* path, tb_bool_t recursion, tb_bool_t prefix, tb_directory_walk_func_t func, tb_cpointer_t priv)
{
    // check
    tb_assert_and_check_return(path && func);

    // the full path
    tb_wchar_t full[TB_PATH_MAXN];
    if (tb_path_full_w(path, full, TB_PATH_MAXN))
        tb_directory_walk_impl(full, recursion, prefix, func, priv);
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
    tb_value_t tuple[3];
    tuple[0].cstr = dest;
    tuple[1].ul = tb_strlen(path);
    tuple[2].b = tb_true;
    tb_directory_walk(path, tb_true, tb_true, tb_directory_walk_copy, tuple);

    // ok?
    return tuple[2].b;
}

