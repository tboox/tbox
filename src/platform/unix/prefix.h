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
 * @file		prefix.h
 * @ingroup 	platform
 */
#ifndef TB_PLATFORM_UNIX_PREFIX_H
#define TB_PLATFORM_UNIX_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../../libc/libc.h"
#include "../../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// fd to handle
#define tb_fd2handle(fd) 			((fd) >= 0? (tb_handle_t)((tb_long_t)(fd) + 1) : tb_null)

// handle to fd
#define tb_handle2fd(handle) 		(tb_int_t)((handle)? (((tb_long_t)(handle)) - 1) : -1)

#endif
