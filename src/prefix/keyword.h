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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		keyword.h
 *
 */
#ifndef TB_PREFIX_KEYWORD_H
#define TB_PREFIX_KEYWORD_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

#define __tb_inline__ 				TB_CONFIG_KEYWORD_INLINE
#define __tb_register__ 			TB_CONFIG_KEYWORD_REGISTER
#define __tb_asm__ 					TB_CONFIG_KEYWORD_ASM
#define __tb_func__ 				TB_CONFIG_KEYWORD_FUNC
#define __tb_file__ 				TB_CONFIG_KEYWORD_FILE
#define __tb_line__ 				TB_CONFIG_KEYWORD_LINE
#define __tb_volatile__ 			volatile

#if defined(TB_COMPILER_IS_GCC) && __GNUC__ > 2
# 	define __tb_likely__(x) 		__builtin_expect((x), 1)
# 	define __tb_unlikely__(x) 		__builtin_expect((x), 0)
#else
# 	define __tb_likely__(x) 		(x)
# 	define __tb_unlikely__(x) 		(x)
#endif


#endif


