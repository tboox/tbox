/*!The Treasure Arch Library
 * 
 * TArch is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TArch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TArch; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        wprintf.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "stdio.h"
#include "../libc.h"
#include "../../platform/platform.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_long_t tb_wprintf(tb_wchar_t const* format, ...)
{
    // check
    tb_check_return_val(format, 0);

    // format line
    tb_long_t   size = 0;
    tb_wchar_t  line[8192] = {0};
    tb_vswprintf_format(line, 8191, format, &size);
    if (size >= 0 && size < 8192) line[size] = L'\0';

    // wtoa
    tb_char_t text[8192] = {0};
    size = tb_wtoa(text, line, 8191);
    if (size >= 0 && size < 8192) text[size] = '\0';

    // print it
    tb_print(text);

    // ok?
    return size;
}
