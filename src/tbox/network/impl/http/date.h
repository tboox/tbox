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
 * @file        date.h
 */
#ifndef TB_NETWORK_IMPL_HTTP_DATE_H
#define TB_NETWORK_IMPL_HTTP_DATE_H

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

/* get the http date from the given cstring
 *
 * <pre>
 * supports format:
 *    Sun, 06 Nov 1994 08:49:37 GMT  ; RFC 822, updated by RFC 1123
 *    Sunday, 06-Nov-94 08:49:37 GMT ; RFC 850, obsoleted by RFC 1036
 *    Sun Nov 6 08:49:37 1994        ; ANSI C's asctime() format
 *
 * for cookies(RFC 822, RFC 850, RFC 1036, and RFC 1123):
 *    Sun, 06-Nov-1994 08:49:37 GMT
 *
 * </pre>
 *
 * @param cstr          the cstring
 * @param size          the cstring length
 *
 * @return              the date
 */
tb_time_t               tb_http_date_from_cstr(tb_char_t const* cstr, tb_size_t size);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

