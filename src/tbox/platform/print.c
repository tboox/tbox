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
 * @file        print.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "print.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#if defined(TB_CONFIG_OS_WINDOWS)
#   include "windows/print.c"
#elif defined(TB_CONFIG_OS_ANDROID)
#   include "android/print.c"
#elif defined(TB_CONFIG_OS_IOS)
#   include "mach/ios/print.c"
#elif defined(TB_CONFIG_LIBC_HAVE_FPUTS)
#   include "libc/print.c"
#else
tb_void_t tb_print(tb_char_t const* string)
{
    tb_trace_noimpl();
}
tb_void_t tb_printl(tb_char_t const* string)
{
    tb_trace_noimpl();
}
tb_void_t tb_print_sync()
{
    tb_trace_noimpl();
}
#endif
