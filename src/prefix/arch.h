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
 * @file		arch.h
 *
 */
#ifndef TB_PREFIX_ARCH_H
#define TB_PREFIX_ARCH_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

/* arch
 *
 * gcc builtin macros for gcc -dM -E - < /dev/null
 *
 * .e.g gcc -m64 -dM -E - < /dev/null | grep 64
 * .e.g gcc -m32 -dM -E - < /dev/null | grep 86
 * .e.g gcc -march=armv6 -dM -E - < /dev/null | grep ARM
 */
#if defined(__i386) \
	|| defined(__i686) \
	|| defined(__i386__) \
	|| defined(__i686__)
# 	define TB_ARCH_x86
#elif defined(__x86_64) \
	|| defined(__amd64__) \
	|| defined(__amd64)
# 	define TB_ARCH_x64
#elif defined(__arm__)
# 	define TB_ARCH_ARM
# 	if defined(__ARM_ARCH)
# 		define TB_ARCH_ARM_VERSION 		__ARM_ARCH
# 		if __ARM_ARCH >= 7
# 			define TB_ARCH_ARM_v7
# 		elif __ARM_ARCH >= 6
# 			define TB_ARCH_ARM_v6
# 		else
# 			define TB_ARCH_ARM_v5
# 		endif
# 	elif defined(__ARM_ARCH_7__)
# 		define TB_ARCH_ARM_VERSION 		(7)
# 		define TB_ARCH_ARM_v7
# 	elif defined(__ARM_ARCH_7A__)
# 		define TB_ARCH_ARM_VERSION 		(7)
# 		define TB_ARCH_ARM_v7A
# 	elif defined(__ARM_ARCH_6__)
# 		define TB_ARCH_ARM_VERSION 		(6)
# 		define TB_ARCH_ARM_v6
# 	elif defined(__ARM_ARCH_5__)
# 		define TB_ARCH_ARM_VERSION 		(5)
# 		define TB_ARCH_ARM_v5
# 	else 
# 		error unknown arm arch version
# 	endif
# 	if defined(__thumb__)
# 		define TB_ARCH_ARM_THUMB
# 	endif
# 	if defined(__ARM_NEON__)
# 		define TB_ARCH_ARM_NEON
# 	endif 
#elif defined(mips) \
	|| defined(_mips) \
	|| defined(__mips__)
# 	define TB_ARCH_MIPS
#else
//# 	define TB_ARCH_SPARC
//# 	define TB_ARCH_PPC
//# 	define TB_ARCH_SH4
# 	error unknown arch
#endif

// sse
#if defined(TB_ARCH_x86) || defined(TB_ARCH_x64)
# 	if defined(__SSE__)
# 		define TB_ARCH_SSE
# 	endif
# 	if defined(__SSE2__)
# 		define TB_ARCH_SSE2
# 	endif
#endif

// vfp
#if defined(__VFP_FP__)
# 	define TB_ARCH_VFP
#endif

#endif


