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
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "directory.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_OS_WINDOWS
#   include "windows/directory.c"
#elif defined(TB_CONFIG_POSIX_HAVE_OPENDIR)
#   include "posix/directory.c"
#else
tb_bool_t tb_directory_create(tb_char_t const* path)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_directory_remove(tb_char_t const* path)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_size_t tb_directory_home(tb_char_t* path, tb_size_t maxn)
{
    tb_trace_noimpl();
    return 0;
}
tb_size_t tb_directory_current(tb_char_t* path, tb_size_t maxn)
{
    tb_trace_noimpl();
    return 0;
}
tb_bool_t tb_directory_current_set(tb_char_t const* path)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_size_t tb_directory_temporary(tb_char_t* path, tb_size_t maxn)
{
    tb_trace_noimpl();
    return 0;
}
tb_void_t tb_directory_walk(tb_char_t const* path, tb_bool_t recursion, tb_bool_t prefix, tb_directory_walk_func_t func, tb_cpointer_t priv)
{
    tb_trace_noimpl();
}
tb_bool_t tb_directory_copy(tb_char_t const* path, tb_char_t const* dest)
{
    tb_trace_noimpl();
    return tb_false;
}
#endif
