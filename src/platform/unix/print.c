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
 * @ingroup 	platform
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../print.h"
#include <unistd.h>
#include <stdio.h>
#if defined(TB_CONFIG_OS_ANDROID)
# 	include <android/log.h>     
#elif defined(TB_CONFIG_OS_IOS)
# 	include <asl.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_void_t tb_print(tb_char_t const* string)
{
	// check
	tb_check_return(string);

#if defined(TB_CONFIG_OS_ANDROID)
	// print to the android device log
	__android_log_print(ANDROID_LOG_DEBUG, TB_TRACE_PREFIX? TB_TRACE_PREFIX : "tbox", "%s", string);
#elif defined(TB_CONFIG_OS_IOS)
	// print to the ios device log
	asl_log(tb_null, tb_null, ASL_LEVEL_WARNING, string);
#endif

	// print to the stdout
	fputs(string, stdout);
}
tb_void_t tb_printl(tb_char_t const* string)
{
	// check
	tb_check_return(string);

#if defined(TB_CONFIG_OS_ANDROID)
	// print to the android device log
	__android_log_print(ANDROID_LOG_DEBUG, TB_TRACE_PREFIX? TB_TRACE_PREFIX : "tbox", "%s\n", string);
#elif defined(TB_CONFIG_OS_IOS)
	// print to the ios device log
	asl_log(tb_null, tb_null, ASL_LEVEL_WARNING, "%s\n", string);
#endif

	// print string to the stdout
	fputs(string, stdout);

	// print newline to the stdout
	fputs("\n", stdout);
}
tb_void_t tb_print_sync()
{
	// flush the stdout
	fflush(stdout);
}
