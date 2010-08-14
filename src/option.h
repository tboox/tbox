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
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		option.h
 *
 */
#ifndef TB_OPTION_H
#define TB_OPTION_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

// is debug?
#ifdef CONFIG_DEBUG
# 	define TB_DEBUG
#endif

// memory
#ifdef CONFIG_MEMORY_SMALL
# 	define TB_MEMORY_MODE_SMALL
#endif

#ifdef CONFIG_MEMORY_POOL_ENABLE
# 	define TB_MEMORY_POOL_ENABLE
# 	define TB_MEMORY_POOL_INDEX 	(EPLAT_POOL_PRIVATE_INDEX)
#endif

// platform
#ifdef CONFIG_PLATFORM_BIGENDIAN
# 	define TB_WORDS_BIGENDIAN
# 	define TB_FLOAT_BIGENDIAN
#endif

// math
#ifdef CONFIG_MATH_HAS_ROUND
# 	define TB_MATH_HAS_ROUND 		(CONFIG_MATH_HAS_ROUND)
#else
# 	define TB_MATH_HAS_ROUND 		(0)
#endif

// compiler
#ifndef CONFIG_CONPILER_NOT_SUPPORT_INLINE
# 	define __tb_inline__ 			inline
#else
# 	define __tb_inline__ 				
#endif

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
