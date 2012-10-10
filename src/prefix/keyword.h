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
 * @file		keyword.h
 *
 */
#ifndef TB_PREFIX_KEYWORD_H
#define TB_PREFIX_KEYWORD_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include "compiler.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#define __tb_register__ 			register
#define __tb_volatile__ 			volatile
#define __tb_asm__ 					__asm__
#define __tb_func__ 				__FUNCTION__
#define __tb_file__ 				__FILE__
#define __tb_line__ 				__LINE__

#if defined(TB_COMPILER_IS_MSVC)

# 	define __tb_inline__ 			__inline
# 	define __tb_inline_force__ 		__forceinline
# 	define __tb_cdecl__ 			__cdecl
# 	define __tb_stdcall__ 			__stdcall
# 	define __tb_fastcall__ 			__fastcall
# 	define __tb_thiscall__ 			__thiscall
# 	define __tb_packed__ 

#elif defined(TB_COMPILER_IS_GCC)

# 	define __tb_inline__ 			__inline__
# 	define __tb_inline_force__ 		__inline__ __attribute__((always_inline))
# 	define __tb_cdecl__ 			__attribute__((__cdecl__))
# 	define __tb_stdcall__ 			__attribute__((__stdcall__))
# 	define __tb_fastcall__ 			__attribute__((__fastcall__))
# 	define __tb_thiscall__ 			__attribute__((__thiscall__))
# 	define __tb_packed__ 			__attribute__((packed, aligned(1)))

#else

# 	define __tb_inline__ 			inline
# 	define __tb_inline_force__ 		inline
# 	define __tb_asm__ 				
# 	define __tb_func__ 				
# 	define __tb_file__ 				""
# 	define __tb_line__ 				(0)

# 	define __tb_cdecl__		
# 	define __tb_stdcall__		
# 	define __tb_fastcall__		
# 	define __tb_thiscall__
# 	define __tb_packed__ 

#endif

#if defined(TB_COMPILER_IS_GCC) && __GNUC__ > 2
# 	define __tb_likely__(x) 		__builtin_expect((x), 1)
# 	define __tb_unlikely__(x) 		__builtin_expect((x), 0)
#else
# 	define __tb_likely__(x) 		(x)
# 	define __tb_unlikely__(x) 		(x)
#endif


#endif


