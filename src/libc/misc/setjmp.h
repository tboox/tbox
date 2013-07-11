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
 * @file		setjmp.h
 * @ingroup 	libc
 *
 */
#ifndef TB_LIBC_MISC_SETJMP_H
#define TB_LIBC_MISC_SETJMP_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../prefix.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#if defined(TB_COMPILER_IS_GCC)
# 	define tb_setjmp(buf) 			__builtin_setjmp(buf)
# 	define tb_longjmp(buf, val) 	__builtin_longjmp(buf, val)
#elif defined(TB_CONFIG_LIBC_HAVE_SETJMP)
# 	define tb_setjmp(buf) 			setjmp(buf)
# 	define tb_longjmp(buf, val) 	longjmp(buf, val)
#else
# 	undef tb_setjmp
# 	undef tb_longjmp
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the jmpbuf type
#if defined(tb_setjmp)
# 	if defined(TB_ARCH_x86)
	typedef tb_int_t tb_jmpbuf_t[6];
# 	elif defined(TB_ARCH_x64)
# 		if TB_CPU_BIT64
		typedef tb_int_t tb_jmpbuf_t[8];
# 		else
		typedef tb_int_t tb_jmpbuf_t[6];
# 		endif
# 	else
# 		undef tb_setjmp
# 		undef tb_longjmp
# 	endif
#endif

/* ///////////////////////////////////////////////////////////////////////
 * declaration
 */
#if defined(tb_setjmp) && defined(TB_CONFIG_LIBC_HAVE_SETJMP)
tb_int_t 	setjmp(tb_jmpbuf_t buf);
tb_void_t 	longjmp(tb_jmpbuf_t buf, tb_int_t val);
#endif

#endif
