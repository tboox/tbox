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
 * @file        exception.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "platform.h"
#include "impl/exception.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_EXCEPTION_ENABLE) && defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/exception.c"
#elif defined(TB_CONFIG_EXCEPTION_ENABLE) && defined(tb_signal)
#   include "libc/exception.c"
#else
tb_bool_t tb_exception_init_env()
{
    tb_trace_noimpl();
    return tb_true;
} 
tb_void_t tb_exception_exit_env()
{
    tb_trace_noimpl();
}
#endif

