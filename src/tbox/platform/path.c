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
 * @path        path.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME                "path"
#define TB_TRACE_MODULE_DEBUG               (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "path.h"
#include "directory.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_path_is_absolute(tb_char_t const* path)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // is absolute?
    return (    path[0] == '/'
            ||  path[0] == '\\'
            ||  path[0] == '~'
            ||  !tb_strnicmp(path, "file://", 7)
            ||  (tb_isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || path[2] == '\\')));
}
tb_char_t const* tb_path_absolute(tb_char_t const* path, tb_char_t* data, tb_size_t maxn)
{
    return tb_path_absolute_to(tb_null, path, data, maxn);
}
tb_char_t const* tb_path_absolute_to(tb_char_t const* root, tb_char_t const* path, tb_char_t* data, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(path && data && maxn, tb_null);

    // trace
    tb_trace_d("path: %s", path);

    // is unix absolute path?
    if (path[0] == '/' || !tb_strnicmp(path, "file://", 7)) 
    {
        // skip prefix
        if (path[0] != '/') path += 7;

        // data? 
        if (path[0] == '/')
        {
            // copy it
            tb_strlcpy(data, path, maxn - 1);
            data[maxn - 1] = '\0';

            // trace
            tb_trace_d("unix absolute path: %s", data);

            // ok
            return data;
        }
    }
    // is windows absolute path?
    else if (tb_isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || path[2] == '\\'))
    {
        // copy it
        tb_strlcpy(data, path, maxn - 1);
        data[maxn - 1] = '\0';

        // using '\\'
        data[2] = '\\';

        // trace
        tb_trace_d("windows absolute path: %s", data);

        // ok
        return data;
    }
    // is user absolute path?
    else if (path[0] == '~')
    {
        // get the home directory
        tb_size_t size = tb_directory_home(data, maxn);
        tb_assert_and_check_return_val(size, tb_null);

        // remove the last '/' or '\\'
        if (size > 1 && (data[size - 1] == '/' || data[size - 1] == '\\'))
            data[--size] = '\0';

        // the real size
        tb_size_t real = size + tb_strlen(path + 1);
        tb_assert_and_check_return_val(real < maxn, tb_null);

        // append the path
        tb_strlcpy(data + size, path + 1, real);
        data[real] = '\0';

        // trace
        tb_trace_d("user absolute path: %s", data);

        // ok
        return data;
    }
    
    // must be relative path now
    tb_assertf_abort(!tb_path_is_absolute(path), "invalid path: %s", path);

    // get the root directory
    tb_size_t size = 0;
    if (root)
    {
        // the root path is the user path?
        if (root[0] == '~')
        {
            // get the home directory
            size = tb_directory_home(data, maxn);
            tb_assert_and_check_return_val(size, tb_null);

            // remove the last '/' or '\\'
            if (size > 1 && (data[size - 1] == '/' || data[size - 1] == '\\'))
                data[--size] = '\0';

            // the real size
            tb_size_t real = size + tb_strlen(root + 1);
            tb_assert_and_check_return_val(real < maxn, tb_null);

            // append the path
            tb_strlcpy(data + size, root + 1, real);
            data[real] = '\0';

            // update the size
            size = real;
        }
        else
        {
            // get the root size
            size = tb_strlen(root);
            tb_assert_and_check_return_val(size < maxn, tb_null);

            // copy it
            tb_strlcpy(data, root, size);
            data[size] = '\0';
        }
    }
    else
    {
        // get the current directory
        if (!(size = tb_directory_current(data, maxn))) return tb_null;
    }

    // trace
    tb_trace_d("root: %s, size: %lu", data, size);

    // is windows path? skip the drive prefix
    tb_size_t   path_is_windows = tb_false;
    tb_char_t*  path_prefix = data;
    if (size > 2 && tb_isalpha(path_prefix[0]) && path_prefix[1] == ':' && (path_prefix[2] == '/' || path_prefix[2] == '\\'))
    {
        // skip the drive prefix
        path_prefix += 2;
        size -= 2;

        // set root 
        path_prefix[0] = '\\';

        // windows path: true
        path_is_windows = tb_true;
    }

    // remove the last '/' or '\\'
    if (size > 1 && (path_prefix[size - 1] == '/' || path_prefix[size - 1] == '\\'))
        path_prefix[--size] = '\0';

    // path => data
    tb_char_t const*    p = path;
    tb_char_t const*    t = p;
    tb_char_t*          q = path_prefix + size;
    tb_char_t const*    e = path_prefix + maxn - 1;
    while (1)
    {
        if (*p == '/' || *p == '\\' || !*p)
        {
            // the item size
            tb_size_t n = p - t;

            // ..? remove item
            if (n == 2 && t[0] == '.' && t[1] == '.')
            {
                // find the last '/'
                for (; q > path_prefix && (*q != '/' && *q != '\\'); q--) ;

                // strip it
                *q = '\0';
            }
            // .? continue it
            else if (n == 1 && t[0] == '.') ;
            // append item
            else if (n && q + 1 + n < e)
            {
                *q++ = path_is_windows? '\\' : '/';
                tb_strlcpy(q, t, n);
                q += n;
            }
            // empty item
            else if (!n) ;
            // too small?
            else 
            {
                // trace
                tb_trace_e("the data path is too small for %s", path);
                return tb_null;
            }

            // break
            tb_check_break(*p);

            // next
            t = p + 1;
        }

        // next
        p++;
    }

    // end
    if (q > path_prefix) *q = '\0';
    // root?
    else
    {
        *q++ = path_is_windows? '\\' : '/';
        *q = '\0';
    }

    // trace    
    tb_trace_d("absolute: %s", data);
    
    // ok?
    return data;
}

