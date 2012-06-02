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
 * @file		stdio.h
 * @ingroup 	libc
 *
 */
#ifndef TB_LIBC_STDIO_H
#define TB_LIBC_STDIO_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * interfaces
 */

tb_long_t tb_sprintf(tb_char_t* s, tb_char_t const* fmt, ...);
tb_long_t tb_snprintf(tb_char_t* s, tb_size_t n, tb_char_t const* fmt, ...);
tb_long_t tb_vsnprintf(tb_char_t* s, tb_size_t n, tb_char_t const* fmt, tb_va_list_t args);

#endif
