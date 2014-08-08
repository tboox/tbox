/*!The Treasure Box dynamic
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
 * @file        dynamic.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../dynamic.h"
#include <dlfcn.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_dynamic_ref_t tb_dynamic_init(tb_char_t const* name)
{
    // check
    tb_assert_and_check_return_val(name, tb_null);

    // clear error
    dlerror();

    // open
    tb_handle_t dynamic = dlopen(name, RTLD_LAZY);

    // error?
    if (dlerror()) 
    {
        if (dynamic) dlclose(dynamic);
        dynamic = tb_null;
    }

    // ok?
    return (tb_dynamic_ref_t)dynamic;
}
tb_void_t tb_dynamic_exit(tb_dynamic_ref_t dynamic)
{
    // check
    tb_assert_and_check_return(dynamic);

    // close it
    dlclose((tb_handle_t)dynamic);
    tb_assert(!dlerror());
}
tb_pointer_t tb_dynamic_func(tb_dynamic_ref_t dynamic, tb_char_t const* name)
{
    // check
    tb_assert_and_check_return_val(dynamic && name, tb_null);

    // the func
    return (tb_pointer_t)dlsym((tb_handle_t)dynamic, name);
}
tb_pointer_t tb_dynamic_pvar(tb_dynamic_ref_t dynamic, tb_char_t const* name)
{
    // check
    tb_assert_and_check_return_val(dynamic && name, tb_null);

    // the variable address
    return (tb_pointer_t)dlsym((tb_handle_t)dynamic, name);
}
