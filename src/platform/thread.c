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
 * @file        thread.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "thread.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#ifdef TB_CONFIG_OS_WINDOWS
#   include "windows/thread.c"
#elif defined(TB_CONFIG_API_HAVE_POSIX)
#   include "posix/thread.c"
#else
tb_handle_t tb_thread_init(tb_char_t const* name, tb_pointer_t (*func)(tb_pointer_t), tb_cpointer_t priv, tb_size_t stack)
{
    tb_trace_noimpl();
    return tb_null;
}
tb_void_t tb_thread_exit(tb_handle_t handle)
{
    tb_trace_noimpl();
}
tb_long_t tb_thread_wait(tb_handle_t handle, tb_long_t timeout)
{
    tb_trace_noimpl();
    return -1;
}
tb_void_t tb_thread_return(tb_pointer_t value)
{
    tb_trace_noimpl();
}
tb_bool_t tb_thread_suspend(tb_handle_t handle)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_bool_t tb_thread_resume(tb_handle_t handle)
{
    tb_trace_noimpl();
    return tb_false;
}
tb_size_t tb_thread_self()
{
    tb_trace_noimpl();
    return 0;
}
#endif
