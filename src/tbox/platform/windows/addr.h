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
 * @file        addr.h
 */
#ifndef TB_PLATFORM_WINDOWS_ADDR_H
#define TB_PLATFORM_WINDOWS_ADDR_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* save the native address to address
 *
 * @param addr      the address
 * @param saddr     the native address
 *
 * @return          the native address size
 */
tb_size_t           tb_socket_addr_save(tb_addr_ref_t addr, struct sockaddr_storage const* saddr);

/* load the address to native address
 *
 * @param saddr     the native address
 * @param addr      the address
 *
 * @return          the native address size
 */
tb_size_t           tb_socket_addr_load(struct sockaddr_storage* saddr, tb_addr_ref_t addr);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
