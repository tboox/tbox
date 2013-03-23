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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		printf.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../printf.h"
#include "../mutex.h"
#include "../file.h"
#include "../../stream/stream.h"
#include "../../libc/libc.h"
#include <windows.h>
#include <stdio.h>

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the printf type
typedef struct __tb_printf_t
{
	// the mode
	tb_size_t 		mode;

	// the mutex 
	tb_handle_t 	mutx;

	// the file
	tb_gstream_t* 	file;

}tb_printf_t;

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the printf
static tb_printf_t 	g_printf = {0};

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_printf_init(tb_size_t mode, tb_char_t const* path)
{
	// init mutex
	if (!g_printf.mutx) g_printf.mutx = tb_mutex_init(tb_null);

	// ok?
	return tb_printf_reset(mode, path);
}
tb_void_t tb_printf_exit()
{
	// enter
	if (g_printf.mutx) tb_mutex_enter(g_printf.mutx);

	// exit file
	if (g_printf.mode == TB_PRINTF_MODE_FILE && g_printf.file)
		tb_gstream_exit(g_printf.file);
	g_printf.file = tb_null;
	g_printf.mode = TB_PRINTF_MODE_STDOUT;

	// leave
	if (g_printf.mutx) tb_mutex_leave(g_printf.mutx);

	// exit the mutx
	if (g_printf.mutx) tb_mutex_exit(g_printf.mutx);
	g_printf.mutx = tb_null;
}
tb_bool_t tb_printf_reset(tb_size_t mode, tb_char_t const* path)
{
	// enter
	if (g_printf.mutx) tb_mutex_enter(g_printf.mutx);

	// exit file
	if (g_printf.mode == TB_PRINTF_MODE_FILE && g_printf.file)
		tb_gstream_exit(g_printf.file);
	g_printf.file = tb_null;

	// is file?
	if (mode == TB_PRINTF_MODE_FILE)
	{
		if (path)
		{
			// init file
			g_printf.file = tb_gstream_init_from_url(path);
			if (g_printf.file)
			{
				// ctrl
				if (tb_gstream_type(g_printf.file) == TB_GSTREAM_TYPE_FILE) 
					tb_gstream_ctrl(g_printf.file, TB_FSTREAM_CMD_SET_FLAGS, TB_FILE_RW | TB_FILE_CREAT | TB_FILE_TRUNC);
			
				// open 
				if (!tb_gstream_bopen(g_printf.file))
				{
					tb_gstream_exit(g_printf.file);
					g_printf.file = tb_null;
				}

				// init mode
				g_printf.mode = TB_PRINTF_MODE_FILE;
			}
		}
		else g_printf.mode = TB_PRINTF_MODE_STDOUT;
	}
	else g_printf.mode = mode;

	// leave
	if (g_printf.mutx) tb_mutex_leave(g_printf.mutx);

	// ok
	return tb_true;
}
tb_void_t tb_printf(tb_char_t const* format, ...)
{
	// enter
	if (g_printf.mutx) tb_mutex_enter(g_printf.mutx);

	// format info
	tb_long_t size = 0;
	tb_char_t info[8192] = {0};
	tb_va_format(info, 8192, format, &size);
	if (size >= 0) info[size] = '\0';

	// printf
	if (g_printf.file) tb_gstream_printf(g_printf.file, "%s", info);
	else 
	{
		FILE* file = (g_printf.mode == TB_PRINTF_MODE_STDERR)? stderr : stdout;
		fprintf(file, "%s", info);
		fflush(file);
	}

	// leave
	if (g_printf.mutx) tb_mutex_leave(g_printf.mutx);
}

