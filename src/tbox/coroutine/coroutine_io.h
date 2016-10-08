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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        coroutine_io.h
 * @ingroup     coroutine
 *
 */
#ifndef TB_COROUTINE_COROUTINE_IO_H
#define TB_COROUTINE_COROUTINE_IO_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "coroutine.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! wait io events 
 *
 * @param sock          the socket
 * @param events        the waited events
 * @param timeout       the timeout, infinity: -1
 *
 * @return              the events
 */
tb_size_t               tb_coroutine_io_wait(tb_socket_ref_t sock, tb_size_t events, tb_long_t timeout);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
