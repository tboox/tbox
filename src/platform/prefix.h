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
 *
 */
#ifndef TB_PLATFORM_PREFIX_H
#define TB_PLATFORM_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

#ifdef TB_CONFIG_OS_WINDOWS
/// the iovec type for WSASend, WSARecv using WSABUF
typedef struct __tb_iovec_t
{
    /// the size
    tb_size_t               size;

    /// the data
    tb_byte_t*              data;

}tb_iovec_t;
#else
/// the iovec type for readv, preadv, writv, pwritv, recvv, sendv
typedef struct __tb_iovec_t
{
    /// the data
    tb_byte_t*              data;

    /// the size
    tb_size_t               size;

}tb_iovec_t;
#endif


#endif
