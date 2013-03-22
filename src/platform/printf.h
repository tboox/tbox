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
 * @file		printf.h
 * @ingroup 	platform
 *
 */
#ifndef TB_PLATFORM_PRINTF_H
#define TB_PLATFORM_PRINTF_H


/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

/// the printf mode enum
typedef enum __tb_printf_mode_e
{
	TB_PRINTF_MODE_NONE 	= 0
,	TB_PRINTF_MODE_FILE 	= 1
,	TB_PRINTF_MODE_STDOUT 	= 2
,	TB_PRINTF_MODE_STDERR 	= 3

}tb_printf_mode_e;

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init printf
 *
 * @param mode 	the printf mode
 * @param path 	the printf path
 *
 * @return 		tb_true or tb_false
 */
tb_bool_t 		tb_printf_init(tb_size_t mode, tb_char_t const* path);

/// exit the printf
tb_void_t 		tb_printf_exit();

/*! reset printf
 *
 * @param mode 	the printf mode
 * @param path 	the printf path
 *
 * @return 		tb_true or tb_false
 */
tb_bool_t 		tb_printf_reset(tb_size_t mode, tb_char_t const* path);

/// printf
tb_void_t 		tb_printf(tb_char_t const* format, ...);

#endif
