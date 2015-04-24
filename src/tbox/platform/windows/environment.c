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
 * @file        environment.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../environment.h"
#include "interface/interface.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_environment_get(tb_char_t const* name, tb_char_t* value, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(name && value && maxn, 0);

    // done
    tb_bool_t   ok = tb_false;
    tb_size_t   size = 0;
    tb_wchar_t* data = tb_null;
    do
    {
        // make data
        data = (tb_wchar_t*)tb_malloc0(sizeof(tb_wchar_t) * maxn);
        tb_assert_and_check_break(data);

        // make name
        tb_wchar_t  name_w[512] = {0};
        tb_size_t   name_n = tb_atow(name_w, name, tb_arrayn(name_w));
        tb_assert_and_check_break(name_n);

        // get it
        size = (tb_size_t)tb_kernel32()->GetEnvironmentVariableW(name_w, data, maxn);
        if (!size) 
        {
            // error?
            if (ERROR_ENVVAR_NOT_FOUND == GetLastError())
            {
                // trace
                tb_trace_d("environment variable(%s) does not exist", name);
            }

            break;
        }
        else if (size > maxn)
        {
            // trace
            tb_trace_d("environment variable(%s) has not enough space", name);
            break;
        }

        // ok
        ok = tb_true;

    } while (0);

    // ok?
    if (ok)
    {
        // save it
        size = tb_wtoa(value, data, maxn);
    }
    else size = 0;

    // exit data
    if (data) tb_free(data);
    data = tb_null;

    // ok?
    return size;
}
tb_bool_t tb_environment_set(tb_char_t const* name, tb_char_t const* value)
{
    // check
    tb_assert_and_check_return_val(name && value, tb_false);

    // done
    tb_bool_t   ok = tb_false;
    tb_size_t   size = 0;
    tb_wchar_t* value_w = tb_null;
    tb_size_t   value_n = 0;
    do
    {
        // make name
        tb_wchar_t  name_w[512] = {0};
        tb_size_t   name_n = tb_atow(name_w, name, tb_arrayn(name_w));
        tb_assert_and_check_break(name_n);

        // make value
        value_n = tb_strlen(value);
        value_w = (tb_wchar_t*)tb_malloc0(sizeof(tb_wchar_t) * (value_n + 1));
        tb_assert_and_check_break(value_w);

        // init value
        if (!tb_atow(value_w, value, value_n + 1)) break;

        // set it
        if (!tb_kernel32()->SetEnvironmentVariableW(name_w, value_w)) break;

        // ok
        ok = tb_true;

    } while (0);

    // exit data
    if (value_w) tb_free(value_w);
    value_w = tb_null;

    // ok?
    return ok;
}
