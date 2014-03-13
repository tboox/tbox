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
 * @author		ruki
 * @file		printf.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "printf.h"
#include "../libc/libc.h"

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_wprintf(tb_wchar_t const* format, ...)
{
	// format info
	tb_long_t 	size = 0;
	tb_wchar_t 	info[8192] = {0};
	tb_vswprintf_format(info, 8191, format, &size);
	if (size >= 0 && size < 8192) info[size] = L'\0';

	// wtoa
	tb_char_t text[8192] = {0};
	size = tb_wtoa(text, info, 8191);
	if (size >= 0 && size < 8192) text[size] = '\0';

	// printf
	tb_printf("%s", text);
}

