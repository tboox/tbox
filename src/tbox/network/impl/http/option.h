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
 * @file        option.h
 * @ingroup     network
 *
 */
#ifndef TB_NETWORK_IMPL_HTTP_OPTION_H
#define TB_NETWORK_IMPL_HTTP_OPTION_H

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

/* init option
 *
 * @param option        the option
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_http_option_init(tb_http_option_t* option);

/* exit option
 *
 * @param option        the option
 */
tb_void_t               tb_http_option_exit(tb_http_option_t* option);

/* ctrl option
 *
 * @param option        the option
 * @param ctrl          the ctrl code
 * @param args          the ctrl args
 */
tb_bool_t               tb_http_option_ctrl(tb_http_option_t* option, tb_size_t code, tb_va_list_t args);

#ifdef __tb_debug__
/* dump option
 *
 * @param option        the option
 */
tb_void_t               tb_http_option_dump(tb_http_option_t* option);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

