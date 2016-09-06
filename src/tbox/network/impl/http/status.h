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
 * @file        status.h
 */
#ifndef TB_NETWORK_IMPL_HTTP_STATUS_H
#define TB_NETWORK_IMPL_HTTP_STATUS_H

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

/* init status
 *
 * @param status        the status
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_http_status_init(tb_http_status_t* status);

/* exit status
 *
 * @param status        the status
 */
tb_void_t               tb_http_status_exit(tb_http_status_t* status);

/* clear status
 *
 * @param status        the status
 * @param host_changed  the host is changed
 */
tb_void_t               tb_http_status_cler(tb_http_status_t* status, tb_bool_t host_changed);

#ifdef __tb_debug__
/* dump status
 *
 * @param status        the status
 */
tb_void_t               tb_http_status_dump(tb_http_status_t* status);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

