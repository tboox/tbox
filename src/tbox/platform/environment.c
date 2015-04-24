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
#include "environment.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_OS_WINDOWS
#   include "windows/environment.c"
#else
#   include "libc/environment.c"
#endif
tb_environment_ref_t tb_environment_init()
{
    // init environment
    return tb_vector_init(8, tb_element_str(tb_true));
}
tb_void_t tb_environment_exit(tb_environment_ref_t environment)
{
    // exit environment
    if (environment) tb_vector_exit(environment);
}
tb_size_t tb_environment_size(tb_environment_ref_t environment)
{
    return tb_vector_size(environment);
}
tb_char_t const* tb_environment_get(tb_environment_ref_t environment, tb_size_t index)
{
    // check
    tb_assert_and_check_return_val(environment, tb_null);

    // get the value
    return (index < tb_vector_size(environment))? (tb_char_t const*)tb_iterator_item(environment, index) : tb_null;
}
tb_bool_t tb_environment_set(tb_environment_ref_t environment, tb_char_t const* value, tb_bool_t overwrite)
{
    // check
    tb_assert_and_check_return_val(environment, tb_false);

    // exists value?
    if (value) 
    {
        // overwrite? clear it
        if (overwrite) tb_vector_clear(environment);

        // insert value into the head
        tb_vector_insert_head(environment, value);
    }
    // clear it
    else tb_vector_clear(environment);

    // ok
    return tb_true;
}
#ifdef __tb_debug__
tb_void_t tb_environment_dump(tb_environment_ref_t environment, tb_char_t const* name)
{
    // trace
    tb_trace_i("%s:", name);

    // dump values
    tb_for_all_if (tb_char_t const*, value, environment, value)
    {
        // trace
        tb_trace_i("    %s", value);
    }
}
#endif
