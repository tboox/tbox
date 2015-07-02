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
#include "../../string/string.h"
#include "../../container/container.h"
#include "../../algorithm/algorithm.h"
#include "interface/interface.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_char_t* tb_environment_get_impl(tb_char_t const* name, tb_size_t* psize)
{
    // check
    tb_assert_and_check_return_val(name, 0);

    // done
    tb_bool_t   ok = tb_false;
    tb_size_t   size = 0;
    tb_size_t   maxn = 256;
    tb_char_t*  value = tb_null;
    tb_wchar_t* value_w = tb_null;
    do
    {
        // make value_w
        value_w = (tb_wchar_t*)tb_malloc0(sizeof(tb_wchar_t) * maxn);
        tb_assert_and_check_break(value_w);

        // make name
        tb_wchar_t  name_w[512] = {0};
        tb_size_t   name_n = tb_atow(name_w, name, tb_arrayn(name_w));
        tb_assert_and_check_break(name_n);

        // get it
        size = (tb_size_t)tb_kernel32()->GetEnvironmentVariableW(name_w, value_w, maxn);
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
            // grow space 
            value_w = (tb_wchar_t*)tb_ralloc(value_w, sizeof(tb_wchar_t) * (size + 1));
            tb_assert_and_check_break(value_w);

            // get it
            size = (tb_size_t)tb_kernel32()->GetEnvironmentVariableW(name_w, value_w, size + 1);
            tb_assert_and_check_break(size);
        }

        // make value
        value = (tb_char_t*)tb_malloc0(sizeof(tb_char_t) * (size + 1));
        tb_assert_and_check_break(value);

        // save value
        if (!(size = tb_wtoa(value, value_w, size))) break;

        // save size
        if (psize) *psize = size;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit value
        if (value) tb_free(value);
        value = tb_null;
    }

    // exit value_w
    if (value_w) tb_free(value_w);
    value_w = tb_null;

    // ok?
    return value;
}
static tb_bool_t tb_environment_set_impl(tb_char_t const* name, tb_char_t const* value)
{
    // check
    tb_assert_and_check_return_val(name, tb_false);

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

        // exists value?
        if (value)
        {
            // make value
            value_n = tb_strlen(value);
            value_w = (tb_wchar_t*)tb_malloc0(sizeof(tb_wchar_t) * (value_n + 1));
            tb_assert_and_check_break(value_w);

            // init value
            if (!tb_atow(value_w, value, value_n + 1)) break;

            // set it
            if (!tb_kernel32()->SetEnvironmentVariableW(name_w, value_w)) break;
        }
        // remove this variable
        else
        {
            // remove it
            if (!tb_kernel32()->SetEnvironmentVariableW(name_w, tb_null)) break;
        }

        // ok
        ok = tb_true;

    } while (0);

    // exit data
    if (value_w) tb_free(value_w);
    value_w = tb_null;

    // ok?
    return ok;
}
/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_environment_load(tb_environment_ref_t environment, tb_char_t const* name)
{
    // check
    tb_assert_and_check_return_val(environment && name, 0);

    // clear environment first
    tb_vector_clear(environment);

    // get values
    tb_char_t const* values = tb_environment_get_impl(name, tb_null);
    tb_check_return_val(values, 0);

    // init value string
    tb_string_t value;
    if (tb_string_init(&value)) 
    {
        // done
        tb_char_t const*    p = values;
        tb_char_t           c = '\0';
        while (1)
        {
            // the character
            c = *p++;

            // make value
            if (c != ';' && c) tb_string_chrcat(&value, c);
            else 
            {
                // save value to environment
                if (tb_string_size(&value)) 
                    tb_vector_insert_tail(environment, tb_string_cstr(&value));

                // clear value 
                tb_string_clear(&value);

                // end?
                tb_check_break(c);
            }
        }

        // exit value string
        tb_string_exit(&value);
    }

    // exit values
    if (values) tb_free(values);
    values = tb_null;

    // ok?
    return tb_vector_size(environment);
}
tb_bool_t tb_environment_save(tb_environment_ref_t environment, tb_char_t const* name)
{
    // check
    tb_assert_and_check_return_val(environment && name, tb_false);

    // empty? remove this environment variable
    if (!tb_vector_size(environment)) return tb_environment_set_impl(name, tb_null);

    // init values string
    tb_string_t values;
    if (!tb_string_init(&values)) return tb_false;

    // make values string
    tb_for_all_if (tb_char_t const*, value, environment, value)
    {
        // the single value cannot exist ';'
        tb_assertf_abort(!tb_strchr(value, ';'), "invalid value: %s", value);

        // append value
        tb_string_cstrcat(&values, value);
        tb_string_chrcat(&values, ';');
    }

    // strip the last ';'
    tb_string_strip(&values, tb_string_size(&values) - 1);

    // save variable
    tb_bool_t ok = tb_environment_set_impl(name, tb_string_cstr(&values));

    // exit values string
    tb_string_exit(&values);

    // ok?
    return ok;
}
tb_size_t tb_environment_get_one(tb_char_t const* name, tb_char_t* value, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(name && value && maxn, 0);

    // get it
    tb_size_t   size = 0;
    tb_char_t*  data = tb_environment_get_impl(name, &size);
    if (data && size < maxn)
    {
        // copy it
        tb_strncpy(value, data, size);
        value[size] = '\0';

        // only get the first one if exists multiple values
        tb_char_t* p = tb_strchr(value, ';');
        if (p)
        {
            // strip it
            *p = '\0';

            // update size
            size = p - value;
        }
    }
    else size = 0;

    // exit data
    if (data) tb_free(data);
    data = tb_null;

    // ok?
    return size;
}
tb_bool_t tb_environment_set_one(tb_char_t const* name, tb_char_t const* value)
{
    // check
    tb_assert_and_check_return_val(name, tb_false);

    // the single value cannot exist ';'
    tb_assertf_abort(!value || !tb_strchr(value, ';'), "invalid value: %s", value);

    // set it
    return tb_environment_set_impl(name, value);
}
