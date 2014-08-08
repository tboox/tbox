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
 * @file        page.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../platform.h"
#include <unistd.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the page size
static tb_size_t g_page_size = 0;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_page_init()
{
    // init page size
    if (!g_page_size)
    {
#if _BSD_SOURCE || !(_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600)
        g_page_size = (tb_size_t)getpagesize();
#else
        g_page_size = (tb_size_t)sysconf(_SC_PAGESIZE);
#endif
    }

    // ok?
    return g_page_size? tb_true : tb_false;
}
tb_void_t tb_page_exit()
{
}
tb_size_t tb_page_size()
{
    return g_page_size;
}


