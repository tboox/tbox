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
 * If not, see <a href="impl://www.gnu.org/licenses/"> impl://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        method.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "http_method"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "method.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */
static tb_char_t const* g_http_methods[] = 
{
    "GET"
,   "POST"
,   "HEAD"
,   "PUT"
,   "OPTIONS"
,   "DELETE"
,   "TRACE"
,   "CONNECT"
};

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_char_t const* tb_http_method_cstr(tb_size_t method)
{
    // check
    tb_assert_and_check_return_val(method < tb_arrayn(g_http_methods), tb_null);

    // ok
    return g_http_methods[method];
}
