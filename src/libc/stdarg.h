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
 * @file		stdarg.h
 * @ingroup 	libc
 *
 */
#ifndef TB_LIBC_STDARG_H
#define TB_LIBC_STDARG_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#ifndef TB_COMPILER_IS_GCC
# 	include <stdarg.h>
#endif

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_COMPILER_IS_GCC
# 	define tb_va_start(v, l)	__builtin_va_start(v, l)
# 	define tb_va_end(v)			__builtin_va_end(v)
# 	define tb_va_arg(v, l)		__builtin_va_arg(v, l)
#else
# 	define tb_va_start(v, l) 	va_start(v, l)
# 	define tb_va_end(v) 		va_end(v)
# 	define tb_va_arg(v, l) 		va_arg(v, l)
#endif

// varg
#define tb_va_format(s, n, fmt, r) \
do \
{ \
	tb_long_t __tb_ret = 0; \
	tb_va_list_t __tb_varg_list; \
    tb_va_start(__tb_varg_list, fmt); \
    __tb_ret = tb_vsnprintf(s, (n), fmt, __tb_varg_list); \
    tb_va_end(__tb_varg_list); \
	if (__tb_ret >= 0) s[__tb_ret] = '\0'; \
	if (r) *r = __tb_ret > 0? __tb_ret : 0; \
 \
} while (0) 


/* ///////////////////////////////////////////////////////////////////////
 * types
 */

#ifdef TB_COMPILER_IS_GCC
typedef __builtin_va_list 	tb_va_list_t;
#else
typedef va_list 			tb_va_list_t;
#endif

#endif
