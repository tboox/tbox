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
 * @file		print.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../print.h"
#include <windows.h>
#include <stdio.h>

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_print(tb_char_t const* string)
{
	// check
	tb_check_return(string);

	// print to the stdout
	fputs(string, stdout);

	// flush the stdout
	fflush(stdout);
}
tb_void_t tb_printl(tb_char_t const* string)
{
	// check
	tb_check_return(string);

	// print string to the stdout
	fputs(string, stdout);

	// print newline to the stdout
	fputs("\r\n", stdout);

	// flush the stdout
	fflush(stdout);
}

