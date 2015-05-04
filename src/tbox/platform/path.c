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
#define TB_TRACE_MODULE_DEBUG               (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "path.h"
#include "directory.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the path separator
#ifdef TB_CONFIG_OS_WINDOWS
#   define TB_PATH_SEPARATOR        '\\'
#else
#   define TB_PATH_SEPARATOR        '/'
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_size_t tb_path_translate(tb_char_t* path, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(path, 0);

    // file:? skip it
    tb_char_t* p = path;
#ifdef TB_CONFIG_OS_WINDOWS
    if (tb_isalpha(p[0]) && p[1] == ':') p += 2;
#else
    if (!tb_strnicmp(p, "file:", 5)) p += 5;
#endif
    // is user directory?
    else if (p[0] == '~')
    {
        // get the home directory
        tb_char_t home[TB_PATH_MAXN];
        tb_size_t home_size = tb_directory_home(home, sizeof(home) - 1);
        tb_assert_and_check_return_val(home_size, 0);

        // check the path space
        tb_size_t path_size = tb_strlen(path);
        tb_assert_and_check_return_val(home_size + path_size - 1 < maxn, 0);

        // move the path and ensure the enough space for the home directory
        tb_memmov(path + home_size, path + 1, path_size - 1);

        // copy the home directory 
        tb_memcpy(path, home, home_size);
        path[home_size + path_size] = '\0';
    }

    // must be absolute path
    tb_assertf_and_check_return_val(*p == '/' || *p == '\\', 0, "invalid path: %s", path);

    // remove repeat '/' and replace '\\'
    tb_char_t*  q = path;
    tb_size_t   repeat = 0;
    for (; *p; p++)
    {
        if (*p == '/' || *p == '\\')
        {
            // save the separator if not exists
            if (!repeat) *q++ = TB_PATH_SEPARATOR;

            // repeat it
            repeat++;
        }
        else 
        {
            // save character
            *q++ = *p;

            // clear repeat
            repeat = 0;
        }
    }

    // end
    *q = '\0';

    // trace
    tb_trace_d("translate: %s", path);

    // ok
    return q - path;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_path_is_absolute(tb_char_t const* path)
{
    // check
    tb_assert_and_check_return_val(path, tb_false);

    // is absolute?
#ifdef TB_CONFIG_OS_WINDOWS
    return (    path[0] == '~'
            ||  (tb_isalpha(path[0]) && path[1] == ':' && (path[2] == '/' || path[2] == '\\')));
#else
    return (    path[0] == '/'
            ||  path[0] == '\\'
            ||  path[0] == '~'
            ||  !tb_strnicmp(path, "file:", 5));
#endif
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

    // the path is absolute?
    if (tb_path_is_absolute(path))
    {
        // copy it
        tb_strlcpy(data, path, maxn - 1);
        data[maxn - 1] = '\0';

        // translate it
        return tb_path_translate(data, maxn)? data : tb_null;
    }

    // get the root directory
    tb_size_t size = 0;
    if (root)
    {
        // get the root size
        size = tb_strlen(root);
        tb_assert_and_check_return_val(size < maxn, tb_null);

        // copy it
        tb_strlcpy(data, root, size);
        data[size] = '\0';
    }
    else
    {
        // get the current directory
        if (!(size = tb_directory_current(data, maxn))) return tb_null;
    }

    // translate the root directory
    size = tb_path_translate(data, maxn);

    // trace
    tb_trace_d("root: %s, size: %lu", data, size);

    // is windows path? skip the drive prefix
    tb_char_t* absolute = data;
    if (size > 2 && tb_isalpha(absolute[0]) && absolute[1] == ':' && absolute[2] == TB_PATH_SEPARATOR)
    {
        absolute    += 2;
        size        -= 2;
    }

    // remove the last separator
    if (size > 1 && absolute[size - 1] == TB_PATH_SEPARATOR)
        absolute[--size] = '\0';

    // path => data
    tb_char_t const*    p = path;
    tb_char_t const*    t = p;
    tb_char_t*          q = absolute + size;
    tb_char_t const*    e = absolute + maxn - 1;
    while (1)
    {
        if (*p == '/' || *p == '\\' || !*p)
        {
            // the item size
            tb_size_t n = p - t;

            // ..? remove item
            if (n == 2 && t[0] == '.' && t[1] == '.')
            {
                // find the last separator
                for (; q > absolute && *q != TB_PATH_SEPARATOR; q--) ;

                // strip it
                *q = '\0';
            }
            // .? continue it
            else if (n == 1 && t[0] == '.') ;
            // append item
            else if (n && q + 1 + n < e)
            {
                *q++ = TB_PATH_SEPARATOR;
                tb_strlcpy(q, t, n);
                q += n;
            }
            // empty item? remove repeat
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
    if (q > absolute) *q = '\0';
    // root?
    else
    {
        *q++ = TB_PATH_SEPARATOR;
        *q = '\0';
    }

    // trace    
    tb_trace_d("absolute: %s", data);
    
    // ok?
    return data;
}
