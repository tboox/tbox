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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        dynamic.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "dynamic.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_OS_WINDOWS
#   include "windows/dynamic.c"
#elif defined(TB_CONFIG_POSIX_HAVE_DLOPEN)
#   include "posix/dynamic.c"
#else
tb_dynamic_ref_t tb_dynamic_init(tb_char_t const* name)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_void_t tb_dynamic_exit(tb_dynamic_ref_t dynamic)
{
    tb_trace_noimpl();
}
tb_pointer_t tb_dynamic_func(tb_dynamic_ref_t dynamic, tb_char_t const* name)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_pointer_t tb_dynamic_pvar(tb_dynamic_ref_t dynamic, tb_char_t const* name)
{
    tb_trace_noimpl();
    return tb_null;
}
#endif

