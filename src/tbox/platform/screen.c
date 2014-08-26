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
 * @file        screen.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "screen"
#define TB_TRACE_MODULE_DEBUG           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
#include "screen.h"
#ifdef TB_CONFIG_OS_WINDOWS
#   include "windows/screen.c"
#elif defined(TB_CONFIG_OS_MAC)
//#   include "mach/mac/screen.m"
#elif defined(TB_CONFIG_OS_IOS)
//#   include "mach/ios/screen.m"
#else
tb_size_t tb_screen_width()
{
    tb_trace_noimpl();
    return 0;
}
tb_size_t tb_screen_height()
{
    tb_trace_noimpl();
    return 0;
}
#endif
