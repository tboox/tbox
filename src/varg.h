/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		varg.h
 *
 */
#ifndef TB_VARG_H
#define TB_VARG_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <stdarg.h>

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */

#define TB_VARG_FORMAT(s, n, fmt, r) \
do \
{ \
	tb_int_t __tb_ret = 0; \
	va_list __tb_varg_list; \
    va_start(__tb_varg_list, fmt); \
    __tb_ret = vsnprintf(s, (n) - 1, fmt, __tb_varg_list); \
    va_end(__tb_varg_list); \
	if (__tb_ret >= 0) s[__tb_ret] = '\0'; \
	if (r) *r = __tb_ret > 0? __tb_ret : 0; \
 \
} while (0) 



/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */



#endif

