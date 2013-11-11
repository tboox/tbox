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
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../printf.h"
#include "../mutex.h"
#include "../../libc/libc.h"
#include <unistd.h>
#include <stdio.h>
#if defined(TB_CONFIG_OS_ANDROID)
# 	include <android/log.h>     
#elif defined(TB_CONFIG_OS_IOS)
# 	include <asl.h>
#endif

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
	FILE* 			file;

}tb_printf_t;

/* ///////////////////////////////////////////////////////////////////////
 * globals
 */

// the printf
static tb_printf_t 	g_printf = {TB_PRINTF_MODE_STDOUT, tb_null, tb_null};

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_printf_init(tb_size_t mode, tb_char_t const* path)
{
	// init mutex
	if (!g_printf.mutx) g_printf.mutx = tb_mutex_init();

	// ok?
	return tb_printf_reset(mode, path);
}
tb_void_t tb_printf_exit()
{
	// enter
	if (g_printf.mutx) tb_mutex_enter(g_printf.mutx);

	// exit file
	if (g_printf.mode == TB_PRINTF_MODE_FILE && g_printf.file)
		fclose(g_printf.file);
	g_printf.file = stdout;
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

	// init mode
	g_printf.mode = mode;

	// exit file
	if (g_printf.mode == TB_PRINTF_MODE_FILE && g_printf.file)
		fclose(g_printf.file);
	g_printf.file = tb_null;

	// init file
	switch (mode)
	{
	case TB_PRINTF_MODE_STDERR:
		g_printf.file = stderr;
		break;
	case TB_PRINTF_MODE_FILE:
		g_printf.file = fopen(path, "w");
		if (!g_printf.file)
		{
			g_printf.file = stdout;
			g_printf.mode = TB_PRINTF_MODE_STDOUT;
		}
		break;
	case TB_PRINTF_MODE_STDOUT:
	default:
		g_printf.file = stdout;
		break;
	}

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
	tb_vsnprintf_format(info, 8192, format, &size);
	if (size >= 0) info[size] = '\0';

	// print to file
	if (g_printf.file)
	{
#if defined(TB_CONFIG_OS_ANDROID)
		if (g_printf.mode != TB_PRINTF_MODE_FILE) __android_log_print(ANDROID_LOG_DEBUG, TB_PRINT_TAG, "%s", info);
#elif defined(TB_CONFIG_OS_IOS)
		/*
		 * ASL_LEVEL_EMERG   0
		 * ASL_LEVEL_ALERT   1
		 * ASL_LEVEL_CRIT    2
		 * ASL_LEVEL_ERR     3
		 * ASL_LEVEL_WARNING 4 //< for NSLog      
		 * ASL_LEVEL_NOTICE  5 //< show 
		 *
		 * ASL_LEVEL_INFO    6
		 * ASL_LEVEL_DEBUG   7
		 */
		if (g_printf.mode != TB_PRINTF_MODE_FILE) asl_log(tb_null, tb_null, ASL_LEVEL_WARNING, info);
#endif
		{
			fprintf(g_printf.file, "%s", info);
			fflush(g_printf.file);
		}
	}
	else 
	{
#if defined(TB_CONFIG_OS_ANDROID)
		__android_log_print(ANDROID_LOG_DEBUG, TB_PRINT_TAG, "%s", info);
#elif defined(TB_CONFIG_OS_IOS)
		asl_log(tb_null, tb_null, ASL_LEVEL_WARNING, info);
#endif

		FILE* file = (g_printf.mode == TB_PRINTF_MODE_STDERR)? stderr : stdout;
		fprintf(file, "%s", info);
		fflush(file);
	}

	// leave
	if (g_printf.mutx) tb_mutex_leave(g_printf.mutx);
}

