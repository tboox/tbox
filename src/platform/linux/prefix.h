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
 * @file        prefix.h
 * @ingroup     platform
 */
#ifndef TB_PLATFORM_LINUX_PREFIX_H
#define TB_PLATFORM_LINUX_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../../libc/libc.h"
#include "../../utils/utils.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// fd to file
#define tb_fd2file(fd)              ((fd) >= 0? (tb_file_ref_t)((tb_long_t)(fd) + 1) : tb_null)

// file to fd
#define tb_file2fd(file)            (tb_int_t)((file)? (((tb_long_t)(file)) - 1) : -1)

// fd to sock
#define tb_fd2sock(fd)              ((fd) >= 0? (tb_socket_ref_t)((tb_long_t)(fd) + 1) : tb_null)

// sock to fd
#define tb_sock2fd(sock)            (tb_int_t)((sock)? (((tb_long_t)(sock)) - 1) : -1)

#endif
